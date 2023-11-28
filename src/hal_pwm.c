#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include "zephyr/drivers/gpio.h"

#include "hal.h"

#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_pwm
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define PWM_DT_SPEC(n, p, i) PWM_DT_SPEC_GET_BY_IDX(n, i),
#define PWM_MAX_CHANNELS     ARRAY_SIZE(pwm_devices)

/*
* | PWM Module    | PWM Channels | PWM Pins |
* |---------------|--------------|----------|
* | PWM0          | 0            | P0.04    |
* | PWM0          | 1            | P0.13    |
* | PWM0          | 2            | P0.14    |
* | PWM0          | 3            | P0.15    |
* | PWM1          | 4            | P0.17    |
* | PWM1          | 5            | P0.24    |
* | PWM1          | 6            | P0.25    |
* | PWM1          | 7            | P0.27    |
* | PWM2          | 8            | P0.28    |
* | PWM2          | 9            | P0.29    |
* | PWM2          | 10           | P0.31    |
* | PWM2          | 11           | P1.02    |
* | PWM3          | 12           | P1.04    |
* | PWM3          | 13           | P1.07    |
* | PWM3          | 14           | P1.10    |
* | PWM3          | 15           | P1.11    |
 */
static struct pwm_dt_spec pwm_devices[] = {
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm0_pins), pwms, PWM_DT_SPEC)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm1_pins), pwms, PWM_DT_SPEC)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm2_pins), pwms, PWM_DT_SPEC)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm3_pins), pwms, PWM_DT_SPEC)
} ;

int __InitPWM()
{
    for(uint8_t idx=0; idx < ARRAY_SIZE(pwm_devices); idx++)
    {
        if (device_is_ready(pwm_devices[idx].dev))
        {
            LOG_INF("PWM channel %d is ready", idx);
        }
        else
        {
            LOG_ERR("PWM channel %d is not ready", idx);
            return FAILURE;
        }
    }
    LOG_INF("Init PWM success");
    return SUCCESS;
}

int hal__setDutyCycle(uint8_t channel_num, uint16_t dutyCycle_tenth)
{
    param_check( (channel_num >= 0) && (channel_num < PWM_MAX_CHANNELS));
    param_check(dutyCycle_tenth <= 1000 && dutyCycle_tenth >= 0);
    uint64_t pulse_dt = (uint64_t)pwm_devices[channel_num].period * dutyCycle_tenth / 1000;
    pwm_set_pulse_dt(&pwm_devices[channel_num], pulse_dt);
    return 0;
}

static const struct device *gpio0_device = DEVICE_DT_GET(DT_NODELABEL(gpio0));
int __InitGPIO(void)
{
    if (device_is_ready(gpio0_device))
    {
        LOG_INF("GPIO0 is ready");
    }
    else
    {
        LOG_ERR("GPIO0 is not ready");
        return FAILURE;
    }
    LOG_INF("Init GPIO0 success");
    return SUCCESS;
}

// (0,0) sets pin 0 as input, 
// (0,1) sets pin 0 as output, 
// (0,2) sets pin as high impedance. 
// Returns 0 on success, -1 on failure.
int hal__setState(uint8_t pinNum, uint8_t state)
{
    param_check( (pinNum >= 0) && (pinNum <32));
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
    if(gpio_pin_configure(gpio0_device, pinNum, pin_flags) != 0)
    {
        LOG_ERR("Failed to set pin %d to state %d", pinNum, state);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__setHigh(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum <32));
    int status = gpio_pin_set_raw(gpio0_device, pinNum, 1);
    if(status != 0)
    {
        LOG_ERR("Failed to set pin %d to high with error %d", pinNum, status);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__setLow(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum <32));
    int status = gpio_pin_set_raw(gpio0_device, pinNum, 0);
    if(status != 0)
    {
        LOG_ERR("Failed to set pin %d to low with error %d", pinNum, status);
        return FAILURE;
    }
    return SUCCESS;
}

int hal__read(uint8_t pinNum)
{
    param_check( (pinNum >= 0) && (pinNum <32));
    int status = gpio_pin_get_raw(gpio0_device, pinNum);
    if(status < 0)
    {
        LOG_ERR("Failed to read pin %d with error %d", pinNum, status);
        return FAILURE;
    }
    return status;
}