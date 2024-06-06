//hal_ble.c
//Abstraction layer for BLE functions
#include <string.h>
#include "ble/bluetoothle.h"
#include "zephyr/bluetooth/gatt.h"
#include "hal.h"

#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_ble
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define BLE_CONFIG_ALWAYS_ADV               (1)     /* 1 -> Always advertise */

void* char1_read_cb(void* p_data, void* p_len)
{
    static char default_char_value[]="CHAR1_PAYLOAD";
    *(int*)p_len = strlen(default_char_value);
    return default_char_value;
}

void* char2_read_cb(void* p_data, void* p_len)
{
    static char default_char_value[]="CHAR2_PAYLOAD";
    *(int*)p_len = strlen(default_char_value);
    return default_char_value;
}

void* char3_read_cb(void* p_data, void* p_len)
{
    static char default_char_value[]="CHAR3_PAYLOAD";
    *(int*)p_len = strlen(default_char_value);
    return default_char_value;
}

void* char4_read_cb(void* p_data, void* p_len)
{
    static char default_char_value[]="CHAR4_PAYLOAD";
    *(int*)p_len = strlen(default_char_value);
    return default_char_value;
}

int char5_write_cb(void* p_data, void* p_len)
{
    // Dump receive data
    LOG_INF("char5_write_cb, recv %dB", *(int*)p_len);
    LOG_HEXDUMP_INF(p_data, *(int*)p_len, "raw_payload: ");
    return 0;
}

ble_custom_gatt_cb_t custom_service_app =
{
    .custom_char1_read_cb = &char1_read_cb,
    .custom_char2_read_cb = &char2_read_cb,
    .custom_char3_read_cb = &char3_read_cb,
    .custom_char4_read_cb = &char4_read_cb,
    .custom_char5_write_cb = &char5_write_cb,
};


#if (BLE_CONFIG_ALWAYS_ADV == 1)
void app_ble_disconnect_cb(void)
{
    LOG_INF("on ble_disconnect_cb");
    ble_adv_start();
}

ble_callback_t app_ble_callbacks = {
    .ble_disconnected_cb = &app_ble_disconnect_cb,
};

#endif /* End of (BLE_CONFIG_ALWAYS_ADV == 1) */

int __InitBLE(void)
{
    LOG_INF("============== BLE testing =====================");
#if (BLE_CONFIG_ALWAYS_ADV == 1)
    if ( ble_init(&app_ble_callbacks) != SUCCESS )
#else /* !(BLE_CONFIG_ALWAYS_ADV == 1) */
    if ( ble_init(NULL) != SUCCESS )
#endif /* End of (BLE_CONFIG_ALWAYS_ADV == 1) */
    {
        LOG_ERR("BLE init failed");
        return FAILURE;
    }
    ble_custom_service_init(&custom_service_app);

    if (ble_set_adv_name("MY_CUSTOMNAME") != SUCCESS)
    {
        LOG_ERR("BLE set adv name failed");
    }
    
    if (ble_adv_start() != SUCCESS)
    {
        LOG_ERR("BLE adv start failed");
        return FAILURE;
    }
    LOG_INF("============== BLE testing =====================");
    return(SUCCESS);
}