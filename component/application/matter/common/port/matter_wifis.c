/************************** 
* Matter WiFi Related 
**************************/
#include "FreeRTOS.h"
#include "platform_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "string.h"
#include "wifi_conf.h"
#include "wifi_auto_reconnect.h"
#include "chip_porting.h"
#include "os_wrapper.h"

u32 apNum = 0; // no of total AP scanned
static rtw_scan_result_t matter_userdata[65] = {0};
static char *matter_ssid;
// struct task_struct matter_wifi_autoreconnect_task;
struct matter_wifi_autoreconnect_param {
    rtw_security_t security_type;
    char *ssid;
    int ssid_len;
    char *password;
    int password_len;
    int key_id;
};

rtw_mac_t ap_bssid;
rtw_security_t sta_security_type;
int error_flag = RTW_UNKNOWN;

enum rtw_mode_type wifi_mode = RTW_MODE_NONE;

chip_connmgr_callback chip_connmgr_callback_func = NULL;
void *chip_connmgr_callback_data = NULL;
void chip_connmgr_set_callback_func(chip_connmgr_callback p, void *data)
{
    chip_connmgr_callback_func = p;
    chip_connmgr_callback_data = data;
}

void matter_initiate_wifi_and_connect(rtw_network_info_t* connect_param)
{
    sta_security_type = -1;
    memset(&ap_bssid, 0, ETH_ALEN);
    error_flag = wifi_connect(connect_param, 1);

    if (error_flag == RTW_SUCCESS)
    {
        sta_security_type = connect_param->security_type;
        memcpy(&ap_bssid, connect_param->bssid.octet, ETH_ALEN);
#if CONFIG_AUTO_RECONNECT
        rtw_reconn_new_conn(connect_param);
#endif
    }
    else if(rtw_join_status == RTW_JOINSTATUS_FAIL)
    {
        wifi_indication(WIFI_EVENT_DISCONNECT, NULL, 0, 0);
    }
}

void print_matter_scan_result( rtw_scan_result_t* record )
{
    RTW_API_INFO("%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra");
    RTW_API_INFO(MAC_FMT, MAC_ARG(record->BSSID.octet));
    RTW_API_INFO(" %d\t ", record->signal_strength);
    RTW_API_INFO(" %d\t  ", record->channel);
    RTW_API_INFO(" %d\t  ", record->wps_type);
    RTW_API_INFO("%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                                 ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                                 ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
                                 ( record->security == RTW_SECURITY_WPA_MIXED_PSK ) ? "WPA Mixed" :
                                 ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
                                 ( record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
                                 "Unknown");

    RTW_API_INFO(" %s ", record->SSID.val);
    RTW_API_INFO("\r\n");
}

static rtw_result_t matter_scan_result_handler(unsigned int scanned_AP_num, void *user_data)
{
    static int total_ap_num = 0;
    int ap_num_on_single_channel = scanned_AP_num;
    rtw_scan_result_t *scanned_AP_info;
    char *scan_buf = NULL;
    int ret = RTW_ERROR;

    if (ap_num_on_single_channel > 0) {
        scan_buf = (char *)rtos_mem_zmalloc(ap_num_on_single_channel * sizeof(rtw_scan_result_t));
        if (scan_buf == NULL) {
            DiagPrintf("malloc scan buf fail for scan mcc\n");
            ret = RTW_ERROR;
            goto exit;
        }
        if (wifi_get_scan_records(&ap_num_on_single_channel, scan_buf) < 0) {
            rtos_mem_free(scan_buf);
            ret = RTW_ERROR;
            goto exit;
        }
        //print each scan result on one channel
        for (int i = 0; i < ap_num_on_single_channel; i++) {
            scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));

            RTW_API_INFO("%d\t ", ++apNum);
            memcpy(&matter_userdata[i], scanned_AP_info, sizeof(rtw_scan_result_t));
            print_matter_scan_result(&matter_userdata[i]);
        }
        rtos_mem_free(scan_buf);
        ret = RTW_SUCCESS;
    }

    if (chip_connmgr_callback_func && chip_connmgr_callback_data)
    {
        // inform matter
        chip_connmgr_callback_func(chip_connmgr_callback_data);
        ret = RTW_SUCCESS;
    }
    else
    {
        DiagPrintf("chip_connmgr_callback_func is NULL\r\n");
        apNum = 0;
        ret = RTW_ERROR;
    }

exit:
    return ret;
}

void matter_scan_networks(void)
{
    volatile int ret = RTW_SUCCESS;
    rtw_scan_param_t scan_param;
	memset(&scan_param, 0, sizeof(scan_param));

    scan_param.scan_user_callback = matter_scan_result_handler;
    // scan_param.scan_user_data = NULL;
    apNum = 0; // reset counter at the start of scan
    memset(matter_userdata, 0, sizeof(matter_userdata));

    ret = wifi_scan_networks(&scan_param, 0);
    if(ret != RTW_SUCCESS)
    {
        DiagPrintf("ERROR: wifi scan failed\n\r");
    }
}

void matter_scan_networks_with_ssid(const unsigned char *ssid, size_t length)
{
    volatile int ret = RTW_SUCCESS;
    rtw_scan_param_t scan_param;
	memset(&scan_param, 0, sizeof(scan_param));

    scan_param.scan_user_callback = matter_scan_result_handler;
    // scan_param.scan_user_data = NULL;
    apNum = 0; // reset counter at the start of scan
    memset(matter_userdata, 0, sizeof(matter_userdata));

    matter_ssid = (char*) pvPortMalloc(length+1);
    memset(matter_ssid, 0, length+1);
    memcpy(matter_ssid, ssid, length);
    matter_ssid[length] = '\0';
    scan_param.ssid = matter_ssid;

    ret = wifi_scan_networks(&scan_param, 0);
    if(ret != RTW_SUCCESS)
    {
        DiagPrintf("ERROR: wifi scan failed\n\r");
    }

    if (matter_ssid)
        vPortFree(matter_ssid);
}

rtw_scan_result_t *matter_get_scan_results()
{
    return matter_userdata;
}

static int matter_find_ap_from_scan_buf(char*buf, int buflen, char *target_ssid, void *user_data)
{
    rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
    int plen = 0;

    while(plen < buflen){
        u8 len, ssid_len, security_mode;
        char *ssid;

        // len offset = 0
        len = (int)*(buf + plen);
        // check end
        if(len == 0) break;
        // ssid offset = 14
        ssid_len = len - 14;
        ssid = buf + plen + 14 ;
        if((ssid_len == strlen(target_ssid))
            && (!memcmp(ssid, target_ssid, ssid_len)))
        {
            strncpy((char*)pwifi->ssid, target_ssid, 33);
            // channel offset = 13
            pwifi->channel = *(buf + plen + 13);
            // security_mode offset = 11
            security_mode = (u8)*(buf + plen + 11);
            if(security_mode == IW_ENCODE_ALG_NONE)
                pwifi->security_type = RTW_SECURITY_OPEN;
            else if(security_mode == IW_ENCODE_ALG_WEP)
                pwifi->security_type = RTW_SECURITY_WEP_PSK;
            else if(security_mode == IW_ENCODE_ALG_CCMP)
                pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
            break;
        }
        plen += len;
    }
    return 0;
}

static int matter_get_ap_security_mode(IN char * ssid, OUT rtw_security_t *security_mode, OUT u8 * channel, OUT u8 * bssid)
{
    volatile int ret = RTW_SUCCESS;
    rtw_scan_param_t scan_param;
    
	memset(&scan_param, 0, sizeof(scan_param));
    scan_param.ssid = ssid;
    scan_param.scan_user_callback = matter_scan_result_handler;
    // scan_param.scan_user_data = NULL;
    apNum = 0; // reset counter at the start of scan
    memset(matter_userdata, 0, sizeof(matter_userdata));
    ret = wifi_scan_networks(&scan_param, 0);
    if(ret != RTW_SUCCESS)
    {
        DiagPrintf("ERROR: wifi scan failed\n\r");
        return 0;
    } else {
        int wait_wifi_scan_count = 0;
        DiagPrintf("Getting AP Security Mode\n\r");
        while(!*matter_userdata[0].SSID.val)
        {
            vTaskDelay(1000); //Wait until the scanning is finished
            if(++wait_wifi_scan_count >= 12) // 12 seconds for AmebaD+
                break;
        }
    }

    for (int i = 0; i < (sizeof(matter_userdata)/sizeof(matter_userdata[0])); i++){
        if(strcmp(matter_userdata[i].SSID.val, ssid) == 0){
            *security_mode = matter_userdata[i].security;
            *channel = matter_userdata[i].channel;
            memcpy(bssid, matter_userdata[i].BSSID.octet, ETH_ALEN); 
            return 1;
        }
    }
    return 0;
}

int matter_wifi_connect(
    char              *ssid,
    rtw_security_t    security_type,
    char              *password,
    int               ssid_len,
    int               password_len,
    int               key_id,
    void              *semaphore)
{
    rtw_network_info_t connect_param = {0};
    u8 connect_channel = 0;
    int security_retry_count = 0;

    if(strlen((const char *) password) != 0) {
        security_type = RTW_SECURITY_WPA_WPA2_MIXED_PSK;
    }
    else {
        security_type = RTW_SECURITY_OPEN;
    }

    if(security_type == RTW_SECURITY_WPA_WPA2_MIXED_PSK) {
        while (1) {
            if (matter_get_ap_security_mode((char*)ssid, &security_type, &connect_channel, connect_param.bssid.octet)) {
                break;
            }
            security_retry_count++;
            if(security_retry_count >= 3) {
                DiagPrintf("Can't get AP security mode and channel. Use RTW_SECURITY_WPA_WPA2_MIXED\n");
                security_type = RTW_SECURITY_WPA_WPA2_MIXED;
                break;
            }
        }
        /* Don't set WEP Key ID, default use key_id = 0 for connection
         * If connection fails, use onnetwork (connect to AP with AT Command) instead of ble-wifi
         * This behavior matches other devices behavior as phone and laptop is unable to connect with key_id > 0
         * */
    }

    matter_set_autoreconnect(1);
    strncpy(connect_param.ssid.val, ssid, sizeof(connect_param.ssid.val) - 1);
    connect_param.ssid.len = ssid_len;
    connect_param.password = password;
    connect_param.password_len = password_len;
    connect_param.security_type = security_type;
    connect_param.key_id = key_id;
    connect_param.channel = connect_channel;
    connect_param.pscan_option = 0;
    connect_param.joinstatus_user_callback = NULL;

    matter_initiate_wifi_and_connect(&connect_param);

    return RTW_SUCCESS;
}

int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting)
{
    return wifi_get_setting(WLAN0_IDX, pSetting);
}

int matter_wifi_disconnect(void)
{
    int ret = wifi_disconnect();
    wifi_indication(WIFI_EVENT_DISCONNECT, NULL, 0, 0);
    return ret;
}

int matter_wifi_on(rtw_mode_t mode)
{
    int ret = -1;
    if (wifi_mode == mode)
    {
        ret = RTW_SUCCESS;
    }
    else
    {
        ret = wifi_on(mode);
    }

    if (ret == RTW_SUCCESS)
    {
        wifi_mode = mode;
#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 1
#if LWIP_IPV6
        if (mode == RTW_MODE_STA)
        {
            netif_create_ip6_linklocal_address(&xnetif[0], 1);
        }
#endif
#endif
    }

    return ret;
}

int matter_wifi_set_mode(rtw_mode_t mode)
{
    return matter_wifi_on(mode);
}

int matter_wifi_is_connected_to_ap(void)
{
    return wifi_is_connected_to_ap();
}

int matter_wifi_is_ready_to_transceive(enum rtw_interface_type interface)
{
    switch (interface)
    {
        case RTW_AP_INTERFACE:
            return wifi_is_running(SOFTAP_WLAN_INDEX);
        case RTW_STA_INTERFACE:
            switch (error_flag)
            {
                case RTW_NO_ERROR:
                    return RTW_SUCCESS;
                default:
                    return RTW_ERROR;
            }
        default:
            return RTW_ERROR;
    }
}

int matter_wifi_is_up(enum rtw_interface_type interface)
{
    if (interface == RTW_STA_INTERFACE)
    {
        return wifi_is_running(STA_WLAN_INDEX);
    }
    else
    {
        return wifi_is_running(SOFTAP_WLAN_INDEX);
    }
}

int matter_wifi_is_open_security (void)
{
    if(sta_security_type == RTW_SECURITY_OPEN)
    {
        return 1;
    }
    return 0;
}

void matter_lwip_dhcp()
{
    netif_set_link_up(&xnetif[0]);
    matter_set_autoreconnect(0);

    LwIP_DHCP(0, DHCP_START);
}

void matter_lwip_dhcp6(void)
{
    LwIP_DHCP6(0, DHCP6_START);
}

void matter_lwip_releaseip(void)
{
    LwIP_ReleaseIP(0);
}

int matter_wifi_get_ap_bssid(unsigned char *bssid)
{
    if( (int) RTW_SUCCESS == matter_wifi_is_ready_to_transceive(RTW_STA_INTERFACE)){
        memcpy(bssid, ap_bssid.octet, ETH_ALEN);
        return RTW_SUCCESS;
    }
    return RTW_ERROR;
}

int matter_wifi_get_network_mode(void)
{
    return wifi_mode;
}

int matter_wifi_get_security_type(uint8_t wlan_idx, uint16_t *alg, uint8_t *key_idx, uint8_t *passphrase)
{
    rtw_wifi_setting_t setting = {0};

    if (wifi_get_setting(wlan_idx, &setting) < 0){
        return RTW_ERROR;
    }

    memcpy(alg, &setting.alg, sizeof(setting.alg));
    memcpy(key_idx, &setting.key_idx, sizeof(setting.key_idx));
    memcpy(passphrase, &setting.password, sizeof(setting.password));

    return RTW_SUCCESS;
}

int matter_wifi_get_wifi_channel_number(uint8_t wlan_idx, uint8_t *ch)
{
    if(wifi_get_channel(wlan_idx, ch) < 0)
    {
        return RTW_ERROR;
    }
    return RTW_SUCCESS;
}

int matter_wifi_get_rssi(int *prssi)
{
    int ret;
    rtw_phy_statistics_t phy_statistics;
    ret = wifi_fetch_phy_statistic(&phy_statistics);
    if (ret >= 0)
    {
        *prssi = phy_statistics.rssi;
    }
    return ret;
}

int matter_wifi_get_mac_address(char *mac)
{
    rtw_mac_t mac_struct = {0};
    int ret = wifi_get_mac_address(WLAN0_IDX, &mac_struct, 0);
    DiagSnPrintf(mac, 32, "%02X:%02X:%02X:%02X:%02X:%02X", mac_struct.octet[0], mac_struct.octet[1],
        mac_struct.octet[2], mac_struct.octet[3], mac_struct.octet[4], mac_struct.octet[5]);
    return ret;
}

int matter_wifi_get_last_error()
{
    return error_flag;
}

#if CONFIG_AUTO_RECONNECT
extern struct rtw_auto_reconn_t rtw_reconn;
void matter_set_autoreconnect(u8 mode)
{
    size_t ssidLen = 0;
    unsigned char buf[32];
    const char kWiFiSSIDKeyName[] = "wifi-ssid";

    //if wifi-ssid exist in KVS, it has been commissioned before, CHIP will do autoreconnection
    s32 ret = getPref_bin_new(kWiFiSSIDKeyName, kWiFiSSIDKeyName, buf, sizeof(buf), &ssidLen);
    if ((ret == DCT_SUCCESS) && (mode != 0))
    {
        wifi_config_autoreconnect(mode);
    }
    return;
}

void matter_reconn_task_hdl(void *param)
{
	(void) param;
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

    matter_initiate_wifi_and_connect(&rtw_reconn.conn_param);

    // matter_wifi_autoreconnect_task.task = 0;
    rtos_task_delete(NULL);
}

// void matter_wifi_autoreconnect_hdl(rtw_security_t security_type,
//                             char *ssid, int ssid_len, char* bssid,
//                             char *password, int password_len,
//                             int key_id, char is_wps_trigger)
// {
//     static struct matter_wifi_autoreconnect_param param;
//     matter_param_indicator = &param;
//     param.security_type = security_type;
//     param.ssid = ssid;
//     param.ssid_len = ssid_len;
//     param.password = password;
//     param.password_len = password_len;
//     param.key_id = key_id;

//     if (matter_wifi_autoreconnect_task.task != NULL) {
// #if CONFIG_LWIP_LAYER
//         netifapi_dhcp_stop(&xnetif[0]);
// #endif
//         u32 start_tick = rtos_time_get_current_system_time_ms();
//         while (1) {
//             rtos_time_delay_ms(2);
//             u32 passing_tick = rtos_time_get_current_system_time_ms() - start_tick;
//             if (passing_tick >= 2*1000) {
//                 RTW_API_INFO("\r\n Create matter_wifi_autoreconnect_task timeout \r\n");
//                 return;
//             }

//             if (matter_wifi_autoreconnect_task.task == NULL) {
//                 break;
//             }
//         }
//     }

//     rtos_task_create(&matter_wifi_autoreconnect_task.task, (const char *)"matter_wifi_autoreconnect", matter_wifi_autoreconnect_thread, &param, 2048, 1);
// }
#endif

uint8_t *matter_LwIP_GetIPv6_linklocal(uint8_t idx)
{
    return LwIP_GetIPv6_linklocal(idx);
}

uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx)
{
    return LwIP_GetIPv6_global(idx);
}

unsigned char *matter_LwIP_GetIP(uint8_t idx)
{
    return LwIP_GetIP(idx);
}

unsigned char *matter_LwIP_GetGW(uint8_t idx)
{
    return LwIP_GetGW(idx);
}

uint8_t *matter_LwIP_GetMASK(uint8_t idx)
{
    return LwIP_GetMASK(idx);
}

int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting)
{
    return wifi_get_setting(wlan_idx, psetting);
}

void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data)
{
    wifi_reg_event_handler(event_cmds, handler_func, handler_user_data);
}

#ifdef __cplusplus
}
#endif
