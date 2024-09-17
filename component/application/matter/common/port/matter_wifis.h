/**
 * @brief High resolution sleep.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html
 *
 * @note rmtp is ignored, as signals are not implemented.
 */
#ifndef MATTER_WIFIS_H_
#define MATTER_WIFIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wifi_conf.h>
#include <lwip_netconf.h>
#include <rtw_wifi_defs.h>

#define JOIN_HANDSHAKE_DONE (uint32_t)(1 << 7)

#define RTW_SECURITY_WPA_WPA2_MIXED RTW_SECURITY_WPA_WPA2_AES_PSK

/* struct iw_encode_ext ->alg */
#define IW_ENCODE_ALG_NONE	0
#define IW_ENCODE_ALG_WEP	1
#define IW_ENCODE_ALG_TKIP	2
#define IW_ENCODE_ALG_CCMP	3
#define IW_ENCODE_ALG_PMK	4
#define IW_ENCODE_ALG_AES_CMAC	5 //IGTK

/* connect result */
/**
  * @brief  The enumeration lists the disconnect reasons.
  */
enum rtw_connect_error_flag_t {
	RTW_NO_ERROR,        /**< no error */
	RTW_NONE_NETWORK,   /**< none network */
	RTW_AUTH_FAIL,            /**< auth fail */
	RTW_ASSOC_FAIL,          /**< assocation fail */
	RTW_WRONG_PASSWORD, /**< wrong password */
	RTW_4WAY_HANDSHAKE_TIMEOUT, /**< 4 way handshake timeout*/
	RTW_CONNECT_FAIL,  /**< connect fail*/
	RTW_DHCP_FAIL,        /**< dhcp fail*/
	RTW_UNKNOWN,         /**< unknown*/
};

typedef enum{
    MATTER_WIFI_EVENT_CONNECT = WIFI_EVENT_CONNECT,
    MATTER_WIFI_EVENT_FOURWAY_HANDSHAKE_DONE = WIFI_EVENT_WPA_STA_4WAY_RECV,
    MATTER_WIFI_EVENT_DISCONNECT = WIFI_EVENT_DISCONNECT,
    MATTER_WIFI_EVENT_DHCP6_DONE = WIFI_EVENT_DHCP6_DONE,
} matter_wifi_event;

/**
  * @brief  The structure is used to describe the setting when configure the network.
  */
typedef struct rtw_wifi_config {
    unsigned int		boot_mode;
    unsigned char 		ssid[32];
    unsigned char		ssid_len;
    unsigned char		security_type;
    unsigned char		password[RTW_MAX_PSK_LEN+1];
    unsigned char		password_len;
    unsigned char		channel;
} rtw_wifi_config_t;

extern uint32_t rtw_join_status;
extern rtw_mode_t wifi_mode;

extern int CHIP_SetWiFiConfig(rtw_wifi_setting_t *config);
extern int CHIP_GetWiFiConfig(rtw_wifi_setting_t *config);

extern u32 apNum;
typedef int (*chip_connmgr_callback)(void *object);
void chip_connmgr_set_callback_func(chip_connmgr_callback p, void *data);
void matter_scan_networks(void);
void matter_scan_networks_with_ssid(const unsigned char *ssid, size_t length);
rtw_scan_result_t *matter_get_scan_results(void);
void matter_wifi_autoreconnect_hdl(
    rtw_security_t security_type,
    char *ssid, int ssid_len, char* bssid,
    char *password, int password_len,
    int key_id, char is_wps_trigger);
void matter_set_autoreconnect(u8 mode);
int matter_wifi_connect(
    char              *ssid,
    rtw_security_t    security_type,
    char              *password,
    int               ssid_len,
    int               password_len,
    int               key_id,
    void              *semaphore);
int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting);
int matter_wifi_disconnect(void);
int matter_wifi_on(rtw_mode_t mode);
int matter_wifi_set_mode(rtw_mode_t mode);
int matter_wifi_is_connected_to_ap(void);
int matter_wifi_is_open_security (void);
int matter_wifi_is_ready_to_transceive(enum rtw_interface_type interface);
int matter_wifi_is_up(enum rtw_interface_type interface);
void matter_lwip_dhcp(void);
void matter_lwip_dhcp6(void);
void matter_lwip_releaseip(void);
int matter_wifi_get_ap_bssid(unsigned char *bssid);
int matter_wifi_get_network_mode(void);
int matter_wifi_get_security_type(uint8_t wlan_idx, uint16_t *alg, uint8_t *key_idx, uint8_t *passphrase);
int matter_wifi_get_wifi_channel_number(uint8_t wlan_idx, uint8_t *ch);
int matter_wifi_get_rssi(int *prssi);
int matter_wifi_get_mac_address(char *mac);
int matter_wifi_get_last_error(void);
uint8_t *matter_LwIP_GetIPv6_linklocal(uint8_t idx);
uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx);
unsigned char *matter_LwIP_GetIP(uint8_t idx);
unsigned char *matter_LwIP_GetGW(uint8_t idx);
uint8_t *matter_LwIP_GetMASK(uint8_t idx);
int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting);
void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data);

#ifdef __cplusplus
}
#endif

#endif //MATTER_WIFIS_H_