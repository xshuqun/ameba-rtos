#if defined(CONFIG_MATTER_BLEMGR_ADAPTER) && CONFIG_MATTER_BLEMGR_ADAPTER
/*============================================================================*
 *                              Header
 *============================================================================*/
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
#include "gap.h"
#include "gap_le.h"
#include "os_mem.h"
#include "gap_le.h"
#include "gap_adv.h"
#include "gap_conn_le.h"
#include "platform_stdlib.h"
#include "os_sync.h"
#include "os_sched.h"
#include "os_timer.h"
#include "ble_matter_adapter_app_main.h"
#include "ble_matter_adapter_app_flags.h"
#include "ble_matter_adapter_app.h"
#include "os_sched.h"
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
#include "string.h"
#include "matter_blemgr_common.h"
#endif

/*============================================================================*
 *                              Constants
 *============================================================================*/
#define BLE_MATTER_DEVICE_NAME_LEN	39

uint16_t matter_adv_interval = 0;
uint16_t matter_adv_int_min = 0x20;
uint16_t matter_adv_int_max = 0x20;
uint8_t matter_adv_data_length = 0;
uint8_t matter_adv_data[31] = {0};

matter_blemgr_callback matter_blemgr_callback_func = NULL;
void *matter_blemgr_callback_data = NULL;

#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
#define MAX_ADV_NUMBER 2
uint8_t matter_adv_id = MAX_ADV_NUMBER;

uint8_t customer_adv_data[] = {0x02, 0x01, 0x05, 0x03, 0x03, 0x0A, 0xA0, 0x0D,0x09, 'B', 'L', 'E', '_', 'C', 'U', 'S', 'T', 'O', 'M', 'E', 'R'};
uint8_t customer_rsp_data[] = {0x03, 0x19, 0x00,0x00};
uint8_t customer_adv_data_length = sizeof(customer_adv_data);
uint8_t customer_rsp_data_length = sizeof(customer_rsp_data);
extern T_MULTI_ADV_CONCURRENT matter_multi_adapter; //app.c
extern int ble_matter_adapter_peripheral_app_max_links; //app.c
extern uint8_t customer_adv_id;
extern T_SERVER_ID ble_matter_adapter_service_id;
#endif

/*============================================================================*
 *                              Functions
 *============================================================================*/
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
extern void ble_matter_adapter_multi_adv_init();
extern bool matter_multi_adv_start_by_id(uint8_t *adv_id, uint8_t *adv_data, uint16_t adv_len, uint8_t *rsp_data, uint16_t rsp_len, uint8_t type);
extern bool matter_multi_adv_stop_by_id(uint8_t *adv_id);

#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
extern int ble_matter_adapter_peripheral_main(uint8_t enable);
extern int ble_matter_adapter_start_adv(void);
extern int ble_matter_adapter_stop_adv(void);
extern int ble_matter_adapter_config_adv(uint8_t *adv_data, uint8_t adv_data_length);
extern uint16_t ble_matter_adapter_get_mtu(uint8_t conn_id);
extern int ble_matter_adapter_disconnect(uint8_t connect_id);
extern int ble_matter_adapter_send_indication(uint8_t connect_id, uint8_t *data, uint16_t data_length);
#endif

int matter_blemgr_init(void) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	ble_matter_adapter_app_init();
	ble_matter_adapter_multi_adv_init();
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	ble_matter_adapter_peripheral_main(1);
#endif

	return 0;
}

void matter_blemgr_set_callback_func(matter_blemgr_callback p, void *data) {
	matter_blemgr_callback_func = p;
	matter_blemgr_callback_data = data;
}

int matter_blemgr_start_adv(void) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	bool result = 0;
	result = matter_multi_adv_start_by_id(&matter_adv_id, matter_adv_data, matter_adv_data_length, NULL, 0, 1); // the last parameter 0: Matter 1: Customer
	if (result == 1)
		return 1;
#if CONFIG_BLE_MATTER_MULTI_ADV
	result = matter_multi_adv_start_by_id(&customer_adv_id, customer_adv_data, customer_adv_data_length, customer_rsp_data, customer_rsp_data_length, 2);
	if (result == 1)
		return 1;
#endif
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	ble_matter_adapter_start_adv();
#endif

	return 0;
}

int matter_blemgr_stop_adv(void) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
#if CONFIG_BLE_MATTER_MULTI_ADV
	bool result = 0;
	if (matter_multi_adapter.matter_sta_sto_flag != false) {
		DiagPrintf("[%s]adv already stop...\r\n", __func__);
		return 1;
	}

	result = matter_multi_adv_stop_by_id(&matter_adv_id);
	if (result == 1)
		return 1;
	matter_multi_adapter.matter_sta_sto_flag = true;
#endif
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	ble_matter_adapter_stop_adv();
#endif

	return 0;
}

int matter_blemgr_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *adv_data, uint8_t adv_data_length) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	matter_adv_interval = adv_int_max;
	matter_adv_int_min = adv_int_min;
	matter_adv_int_max = adv_int_max;
	matter_adv_data_length = adv_data_length;
	memcpy(matter_adv_data, adv_data, adv_data_length);
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	matter_adv_data_length = adv_data_length;
	memcpy(matter_adv_data, adv_data, adv_data_length);
	ble_matter_adapter_config_adv(matter_adv_data, matter_adv_data_length);
#endif

	return 0;
}

uint16_t matter_blemgr_get_mtu(uint8_t connect_id) {
	int ret;
	uint16_t mtu_size;
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	if (ble_matter_adapter_peripheral_app_max_links == 0) {
		DiagPrintf("[%s]matter as slave, no connection\r\n", __func__);
		return 1;
	}
	ret = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, connect_id);
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	mtu_size = ble_matter_adapter_get_mtu(connect_id);
	if (mtu_size != 0xFFFF)
	{
		DiagPrintf("[%s]printing MTU size, %d\r\n", __func__, mtu_size);
		return mtu_size;
	}
	else
		return 0;
#endif

	return 0;
}

int matter_blemgr_set_device_name(char *device_name, uint8_t device_name_length) {
	if (device_name == NULL || device_name_length > BLE_MATTER_DEVICE_NAME_LEN) {
		DiagPrintf("[%s]:invalid name or len:name 0x%x,len %d\r\n",__func__, device_name, device_name_length);
		return 1;
	}
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	le_set_gap_param(GAP_PARAM_DEVICE_NAME, device_name_length, device_name);
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	int ret = rtk_bt_le_gap_set_device_name((const uint8_t *)device_name);
	if (ret) {
		DiagPrintf("[%s]:set device name failed! err: 0x%x",__func__, ret);
		return -1;
	}

	DiagPrintf("[%s] set device name success",__func__);
#endif

	return 0;
}

int matter_blemgr_disconnect(uint8_t connect_id) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	if (connect_id >= BLE_MATTER_ADAPTER_APP_MAX_LINKS) {
		DiagPrintf("[%s]:invalid conn_hdl[%d]\r\n", __func__, connect_id);
		return 1;
	}
	T_GAP_DEV_STATE new_state;
	uint8_t *conn_id = (uint8_t *)os_mem_alloc(0, sizeof(uint8_t));
	*conn_id = connect_id;

	if ((ble_matter_adapter_app_send_api_msg(5, conn_id)) == false) {
		DiagPrintf("[%s] send msg fail\r\n", __func__);
		os_mem_free(conn_id);
		return 1;
	}
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	ble_matter_adapter_disconnect(connect_id);
#endif

	return 0;
}

int matter_blemgr_send_indication(uint8_t connect_id, uint8_t *data, uint16_t data_length) {
#if defined(BLE_MATTER_EXAMPLE_SET1) && BLE_MATTER_EXAMPLE_SET1
	if (connect_id >= BLE_MATTER_ADAPTER_APP_MAX_LINKS || data == NULL || data_length == 0) {
		DiagPrintf("[%s]:invalid param:conn_hdl %d,data 0x%x data_length 0x%x\r\n",__func__, connect_id, data, data_length);
		return 1;
	}
	BT_MATTER_SERVER_SEND_DATA *indication_param = (BT_MATTER_SERVER_SEND_DATA *)os_mem_alloc(0, sizeof(BT_MATTER_SERVER_SEND_DATA));
	if (indication_param)
    	{
        	indication_param->conn_id = connect_id;
		indication_param->service_id = ble_matter_adapter_service_id;
        	indication_param->attrib_index = BT_MATTER_ADAPTER_SERVICE_CHAR_TX_INDEX;
		indication_param->data_len = data_length;
		indication_param->type = GATT_PDU_TYPE_INDICATION;
        	if (indication_param->data_len != 0)
        	{
            		indication_param->p_data = os_mem_alloc(0, indication_param->data_len);
            		memcpy(indication_param->p_data, data, indication_param->data_len);
        	}
        	if (ble_matter_adapter_app_send_api_msg(4, indication_param) == false)
        	{
            		DiagPrintf("[%s] os_mem_free\r\n");
            		os_mem_free(indication_param->p_data);
            		os_mem_free(indication_param);
            		return 1;
        	}
    	}
#endif

#if defined(BLE_MATTER_EXAMPLE_SET2) && BLE_MATTER_EXAMPLE_SET2
	ble_matter_adapter_send_indication(connect_id, data, data_length);
#endif

	return 0;
}

#endif
