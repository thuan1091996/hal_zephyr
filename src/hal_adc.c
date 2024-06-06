/*******************************************************************************
* Title                 :   ADC HAL for ZephyrRTOS
* Filename              :   hal_adc.c
* Origin Date           :   2023/09/15
* Version               :   0.0.0
* Compiler              :   Zephyr 2.7
* Target                :   nRF52 
* Notes                 :   None
*******************************************************************************/

/*************** MODULE REVISION LOG ******************************************
*
*    Date       Software Version	Initials	Description
*
*******************************************************************************/

/** \file hal_adc.c
 *  \brief This module contains the hardware abstraction layer for ADC.
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "hal.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "zephyr/drivers/adc.h"


#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_adc
#define MODULE_LOG_LEVEL	        LOG_LEVEL_INF
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#define ADC_TOTAL_CHANNELS              (8)
#define ADC_DEFAULT_OVERSAMPLING        (4)
#define ADC_DEFAULT_RESOLUTION          (12)
#define ADC_DEFAULT_ACQ_TIME            ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40) // 40us
#define ADC_DEFAULT_GAIN                ADC_GAIN_1_6
#define ADC_DEFAULT_REF_SOURCE          ADC_REF_INTERNAL

#define ADC_DT_NODE                     DT_NODELABEL(adc)
/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static const struct device* adc_device = DEVICE_DT_GET(ADC_DT_NODE);

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

int __InitADC(void)
{
    if (!device_is_ready(adc_device))
    {
        LOG_ERR("ADC device %s is not ready", adc_device->name);
        return FAILURE;
    }
    LOG_INF("ADC device %s is ready", adc_device->name);
    return SUCCESS;
}

int hal__ADCRead(uint8_t channel_num)
{
    param_check( (channel_num >= 0) && (channel_num <ADC_TOTAL_CHANNELS));
    const struct adc_channel_cfg cfg = {
        .gain             = ADC_DEFAULT_GAIN,
        .reference        = ADC_DEFAULT_REF_SOURCE,
        .acquisition_time = ADC_DEFAULT_ACQ_TIME,
        .channel_id       = channel_num,
        .input_positive   = channel_num + 1,
    };
    if ( adc_channel_setup(adc_device, &cfg) != 0)
    {
        LOG_ERR("Failed to setup ADC channel %d", channel_num);
        return FAILURE;
    }

    int16_t raw_adc = 0;
    struct adc_sequence sequence = {
        .buffer      = &raw_adc,
        .buffer_size = sizeof(raw_adc),
        .channels = BIT(channel_num),
        .resolution = ADC_DEFAULT_RESOLUTION,
        .oversampling = ADC_DEFAULT_OVERSAMPLING,
    };
    int err = adc_read(adc_device, &sequence);
    if (err != 0) {
        LOG_ERR("ADC channel %d read failed with error %d", channel_num, err);
        return FAILURE;
    }
    LOG_INF("ADC channel %d read success with value %d", channel_num, raw_adc);
    return raw_adc;
}

int hal__ADCReadMV(uint8_t channel_num)
{
    int adc_mv = hal__ADCRead(channel_num);
    if(adc_mv < 0)
    {
        LOG_WRN("Read ADC channel %d, reading value %d", channel_num, adc_mv);
    }
    
    int status = adc_raw_to_millivolts(adc_ref_internal(adc_device), ADC_DEFAULT_GAIN, ADC_DEFAULT_RESOLUTION, &adc_mv);
    if(status != 0)
    {
        LOG_ERR("Failed to convert ADC channel %d raw value to mV", channel_num);
        return FAILURE;
    }
    LOG_INF("ADC channel %d: %d (mV)", channel_num, adc_mv);
    return adc_mv;
}