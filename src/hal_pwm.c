#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>

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
#if DT_NODE_HAS_STATUS(DT_NODELABEL(pwm0), okay)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm0_pins), pwms, PWM_DT_SPEC)
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(pwm1), okay)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm1_pins), pwms, PWM_DT_SPEC)
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(pwm2), okay)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm2_pins), pwms, PWM_DT_SPEC)
#endif

#if DT_NODE_HAS_STATUS(DT_NODELABEL(pwm3), okay)
    DT_FOREACH_PROP_ELEM(DT_NODELABEL(pwm3_pins), pwms, PWM_DT_SPEC)
#endif

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