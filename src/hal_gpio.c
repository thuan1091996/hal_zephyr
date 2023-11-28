#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "zephyr/drivers/gpio.h"

#include "hal.h"

#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_io
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#if (CONFIG_SOC_NRF52840 != 0 )
/*
*   PORT    HW_PIN      SOFT_PIN
*   0	    0-31	    0-31
*   1	    0-15	    32-47 
*/
#define GPIO_PIN_NUMBER_ALL                 48
#define mPORT(pinNum)                       (pinNum/32)
#define mPIN(pinNum)                        (pinNum%32)
#endif /* End of (CONFIG_SOC_NRF52840 != 0 ) */


static const struct device * gpio_devices[] = {
    DEVICE_DT_GET(DT_NODELABEL(gpio0)),
    DEVICE_DT_GET(DT_NODELABEL(gpio1)) 
};

int __InitGPIO(void)
{
    for(uint8_t idx=0; idx < ARRAY_SIZE(gpio_devices); idx++)
    {
        if (device_is_ready(gpio_devices[idx]))
        {
            LOG_INF("GPIO device %d is ready", idx);
        }
        else
        {
            LOG_ERR("GPIO device %d is not ready", idx);
            return FAILURE;
        }
    }
    LOG_INF("Init GPIO success");
    return SUCCESS;
}

// (0,0) sets pin 0 as input, 
// (0,1) sets pin 0 as output, 
// (0,2) sets pin as high impedance. 
// Returns 0 on success, -1 on failure.
int hal__setState(uint8_t pinNum, uint8_t state)
{
    param_check( (pinNum >= 0) && (pinNum < GPIO_PIN_NUMBER_ALL));
    param_check( (state >= 0) && (state <3));

    gpio_flags_t pin_flags;
    if(state == 0)
        pin_flags = GPIO_INPUT;
    else if(state == 1)
        pin_flags = GPIO_OUTPUT;
    else if(state == 2)
        pin_flags = GPIO_DISCONNECTED;
    else
    {
        LOG_ERR("%d is an invalid pin state", state);
        return FAILURE;
    }

    if(gpio_pin_configure(gpio_devices[mPORT(pinNum)], mPIN(pinNum), pin_flags) != 0)
    {
        LOG_ERR("Failed to set pin %d to state %d", pinNum, state);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__setHigh(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum < GPIO_PIN_NUMBER_ALL));
    int status = gpio_pin_set_raw(gpio_devices[mPORT(pinNum)], mPIN(pinNum),1);
    if(status != 0)
    {
        LOG_ERR("Failed to set pin %d to high with error %d", pinNum, status);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__setLow(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum < GPIO_PIN_NUMBER_ALL));
    int status = gpio_pin_set_raw(gpio_devices[mPORT(pinNum)], mPIN(pinNum), 0);
    if(status != 0)
    {
        LOG_ERR("Failed to set pin %d to low with error %d", pinNum, status);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__read(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum < GPIO_PIN_NUMBER_ALL));
    int status = gpio_pin_get_raw(gpio_devices[mPORT(pinNum)], mPIN(pinNum));
    if(status < 0)
    {
        LOG_ERR("Failed to read pin %d with error %d", pinNum, status);
        return FAILURE;
    }
    return status;
}