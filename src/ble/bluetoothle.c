/*******************************************************************************
* Title                 :   Bluetooth LE 
* Filename              :   bluetoothle.c
* Author                :   Itachi
* Origin Date           :   2022/09/04
* Version               :   0.0.0
* Compiler              :   NCS toolchain v2.0.0
* Target                :   nRF52840dk
* Notes                 :   None
*******************************************************************************/

/*************** MODULE REVISION LOG ******************************************
*
*    Date       Software Version	Initials	Description
*  2022/09/04       0.0.0	         Itachi      Module Created.
*
*******************************************************************************/

/** \file bluetoothle.c
 * \brief This module contains the
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include <zephyr/logging/log.h>
#include "bluetoothle.h"
#include "ble_gatt.h"
#include "../hal.h"
/******************************************************************************
* Module configs
*******************************************************************************/
/* Logging relative */
#define MODULE_NAME			        bluetoothle
#define MODULE_LOG_LEVEL	        LOG_LEVEL_INF
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define ADV_DEFAULT_DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define ADV_PACKET_MAX_LEN          (31)
#define ADV_NAME_MAX_LEN            (29) /* -1B ADV len - 1B ADV type*/


#define BLE_CONFIG_ADV_NAME         (1)     /* 1 -> Include ADV name in the ADC packet */
#define BLE_CONFIG_ENABLE_SCAN_REQ  (1)     /* 1 -> Support scan request/ scan response */

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
volatile static bool is_advertising=false;

#if (ADV_NAME_MAX_LEN < 0)
#warning "ADV_NAME_MAX_LEN should be greater than 0"
#else /* !(ADV_NAME_MAX_LEN < 0) */
static char ADV_NAME[ADV_NAME_MAX_LEN] = ADV_DEFAULT_DEVICE_NAME;
#endif /* End of (ADV_NAME_MAX_LEN < 0) */
static struct bt_data ADV_DATA[] = 
{
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),       /* General discoverable mode */
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,  BT_CUSTOM_SERV1_UUID),                  /* Custom service UUID */
};

static struct bt_data SCAN_RESP_DATA[] = 
{
    BT_DATA(BT_DATA_NAME_COMPLETE, ADV_NAME, sizeof(ADV_DEFAULT_DEVICE_NAME))   /* Device name */
};

/* Bluetooth applicatiton callbacks */
static ble_callback_t ble_cb_app = {
    .ble_connected_cb = NULL,
    .ble_disconnected_cb = NULL,
    .ble_adv_started_cb = NULL,
    .ble_adv_stopped_cb = NULL,
};

/******************************************************************************
* Static Function Definitions
*******************************************************************************/
static bool ble_is_advertising(void)
{
    return is_advertising;
}

static void ble_set_advertising(bool is_adv)
{
    is_advertising = is_adv;
}

static void on_ble_connect(struct bt_conn *conn, uint8_t err)
{
	if(err) 
    {
		LOG_ERR("BLE connection err: %d, re-advertising \n", err);
		return;
	}
    LOG_INF("BLE Connected.");

    if(ble_cb_app.ble_connected_cb != NULL)
    {
        ble_cb_app.ble_connected_cb();
    }
}

static void on_ble_disconnect(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("BLE Disconnected (reason: %d)", reason);
    if(ble_cb_app.ble_disconnected_cb != NULL)
    {
        ble_cb_app.ble_disconnected_cb();
    }
}

/******************************************************************************
* Function Definitions
*******************************************************************************/
int ble_adv_start(void)
{
    if(ble_is_advertising())
    {
        LOG_INF("Advertising already started\n");
        return SUCCESS;
    }

#if (BLE_CONFIG_ENABLE_SCAN_REQ != 0)
    int errorcode = bt_le_adv_start(BT_LE_ADV_CONN, ADV_DATA, ARRAY_SIZE(ADV_DATA), SCAN_RESP_DATA, ARRAY_SIZE(SCAN_RESP_DATA));
#else /* !(BLE_CONFIG_ENABLE_SCAN_REQ != 0) */
	int errorcode = bt_le_adv_start(BT_LE_ADV_CONN, ADV_DATA, ARRAY_SIZE(ADV_DATA), NULL, 0);
#endif /* End of (BLE_CONFIG_ENABLE_SCAN_REQ != 0) */
    if (errorcode) {
        LOG_ERR("Couldn't start advertising (err = %d)", errorcode);
        return errorcode;
    }
    ble_set_advertising(true);
    LOG_INF("Advertising successfully started\n");
    if(ble_cb_app.ble_adv_started_cb != NULL)
    {
        ble_cb_app.ble_adv_started_cb();
    }
    return SUCCESS;
}

int ble_adv_stop(void)
{
    if(!ble_is_advertising())
    {
        LOG_INF("Advertising already stopped\n");
        return SUCCESS;
    }
    int errorcode = bt_le_adv_stop();
    if (errorcode) {
        LOG_ERR("Couldn't stop advertising (err = %d)", errorcode);
        return errorcode;
    }
    ble_set_advertising(false);
    LOG_INF("Advertising successfully stopped\n");
    if(ble_cb_app.ble_adv_stopped_cb != NULL)
    {
        ble_cb_app.ble_adv_stopped_cb();
    }
    return SUCCESS;
}

int ble_init(ble_callback_t* p_app_cb)
{
    int errorcode= 0;

    /* Bluetooth zephyr internal callbacks */
    static struct bt_conn_cb ble_cb = {
        .connected 		= &on_ble_connect,
        .disconnected 	= &on_ble_disconnect,

    };

    LOG_INF("BLE initializing \n\r");

    /* Initialize the application callbacks */
    if(p_app_cb != NULL)
    {
        ble_cb_app = *p_app_cb;
    }
    /* Assign callbacks for connection events */
    bt_conn_cb_register(&ble_cb);

    /* BLE initialization */
    errorcode = bt_enable(NULL);
    if(errorcode)
    {
        LOG_ERR("bt_enable return err %d \r\n", errorcode);
        return errorcode;
    }
    LOG_INF("BLE init succesfully");
    return SUCCESS;
}

int ble_set_adv_name(char* p_name)
{
    param_check(p_name != NULL);
    if(strlen(p_name) > ADV_NAME_MAX_LEN)
    {
        LOG_ERR("BLE name too long, %d/%d", strlen(p_name), ADV_NAME_MAX_LEN);
    }
    // Find index of BT_DATA_NAME_COMPLETE in SCAN_RESP_DATA[]
    for(int index = 0; index < ARRAY_SIZE(SCAN_RESP_DATA); index++)
    {
        if(SCAN_RESP_DATA[index].type == BT_DATA_NAME_COMPLETE)
        {
            memset((void *)SCAN_RESP_DATA[index].data, 0, SCAN_RESP_DATA[index].data_len);
            SCAN_RESP_DATA[index].data_len = strlen(p_name);
            memcpy((void *)SCAN_RESP_DATA[index].data, p_name, strlen(p_name));
            LOG_INF("BLE name set to %s", p_name);
            if(ble_is_advertising())
            {
                #if (BLE_CONFIG_ENABLE_SCAN_REQ != 0)
                return bt_le_adv_update_data(ADV_DATA, ARRAY_SIZE(ADV_DATA), SCAN_RESP_DATA, ARRAY_SIZE(SCAN_RESP_DATA));
                #else /* !(BLE_CONFIG_ENABLE_SCAN_REQ != 0) */
                return bt_le_adv_update_data(ADV_DATA, ARRAY_SIZE(ADV_DATA), NULL, 0);
                #endif /* End of (BLE_CONFIG_ENABLE_SCAN_REQ != 0) */
            }
            else
            {
                return SUCCESS;
            }
        }
    }
    LOG_ERR("Couldn't find BT_DATA_NAME_COMPLETE in SCAN_RESP_DATA");
    return FAILURE;
}