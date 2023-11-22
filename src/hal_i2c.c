/*******************************************************************************
* Title                 :   I2C HAL layer for ZephyrRTOS
* Filename              :   hal_i2c.c
* Origin Date           :   2023/09/15
* Version               :   0.0.0
* Compiler              :   nRF connect SDK 2.3.0
* Target                :   nRF52 
* Notes                 :   None
*******************************************************************************/


/** \file hal_i2c.c
 *  \brief This module contains the
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "hal.h"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>


/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_i2c
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define I2C0_NODE           DT_NODELABEL(i2c0)
#define I2C1_NODE           DT_NODELABEL(i2c1)
#define I2C_MAX_INSTANCE    ARRAY_SIZE(i2c_device)

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/


/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static const struct device *i2c_device[] = {
#if DT_NODE_HAS_STATUS(I2C0_NODE, okay)
    DEVICE_DT_GET(I2C0_NODE),
#endif

#if DT_NODE_HAS_STATUS(I2C1_NODE, okay)
    DEVICE_DT_GET(I2C1_NODE),
#endif
};
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
int hal__I2CINIT(uint8_t i2cNum)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    if( !device_is_ready(i2c_device[i2cNum]) )
    {
        LOG_ERR("I2C %d device is not ready!", i2cNum);
        return FAILURE;
    }
    LOG_INF("Init I2C %d success", i2cNum);
    return SUCCESS;
}

bool hal__I2CEXISTS(uint8_t i2cNum, uint8_t ADDR)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    //Check if I2C device exists at ADDR. Returns true if it does, false if not.
    struct i2c_msg msgs;
    uint8_t dst;

    /* Send the address to read from */
    msgs.buf = &dst;
    msgs.len = 0U;
    msgs.flags = I2C_MSG_WRITE | I2C_MSG_STOP;
    if (i2c_transfer(i2c_device[i2cNum], &msgs, 1, ADDR) == 0) 
    {
        LOG_INF("I2C device exists at address 0x%02X", ADDR);
        return true;
    }
    else 
    {
        LOG_INF("I2C device does not exist at address 0x%02X", ADDR);
        return false;
    }
}

int hal__I2CREAD_uint8(uint8_t i2cNum, uint8_t ADDR, uint8_t REG, uint8_t *data)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    param_check(data != NULL);
    
    // Read data from I2C device at ADDR, register REG. Returns 0 on success, -1 on failure.
    if (0 != i2c_reg_read_byte(i2c_device[i2cNum], ADDR, REG, data))
    {
        LOG_ERR("I2C read failed at address 0x%02X, register 0x%02X", ADDR, REG);
        return FAILURE;
    }
    else
    {
        LOG_INF("I2C read at address 0x%02X, register 0x%02X, data 0x%02X", ADDR, REG, *data);
        return SUCCESS;
    }
}

//Read data from I2C device at ADDR, register REG. Returns number of bytes read on success, -1 on failure.
int hal__I2CREAD(uint8_t i2cNum, uint8_t ADDR, uint8_t REG, uint8_t *data, uint16_t len)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    param_check(data != NULL);

    // Read data from I2C device at ADDR, register REG. Returns number of bytes read on success, -1 on failure.
    if (0 == i2c_burst_read(i2c_device[i2cNum], ADDR, REG, data, len))
    {
        LOG_INF("I2C read at address 0x%02X, register 0x%02X", ADDR, REG);
        LOG_HEXDUMP_INF(data, len, "data:");
        return len;
    }
    else
    {
        // Read failed, try to read each byte individually till failure
        uint8_t read_buf[len];
        int16_t read_idx;
        memset(read_buf, 0, len);
        for(read_idx=0; read_idx<len; read_idx++)
        {
            if (SUCCESS != hal__I2CREAD_uint8(i2cNum, ADDR, REG + read_idx, &read_buf[read_idx]))
            {
                if(read_idx > 0) // Able to read some bytes
                    break;
                else
                {
                    LOG_ERR("I2C read failed at address 0x%02X, register 0x%02X", ADDR, REG);
                    return FAILURE;
                }
            }
        }
        memcpy(data, read_buf, read_idx);
        LOG_WRN("I2C read %d/%dB at address 0x%02X, register 0x%02X", read_idx, len, ADDR, REG);
        LOG_HEXDUMP_INF(data, read_idx, "data:");
        return read_idx;
    }
}

int hal__I2CWRITE_uint8(uint8_t i2cNum, uint8_t ADDR, uint8_t REG, uint8_t data)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    //Write data to I2C device at ADDR, register REG. Returns 0 on success, -1 on failure.
    if (0 != i2c_reg_write_byte(i2c_device[i2cNum], ADDR, REG, data))
    {
        LOG_ERR("I2C write failed at address 0x%02X, register 0x%02X, data 0x%02X", ADDR, REG, data);
        return FAILURE;
    }
    else
    {
        LOG_INF("I2C write at address 0x%02X, register 0x%02X, data 0x%02X", ADDR, REG, data);
        return SUCCESS;
    }
}

//Write data to I2C device at ADDR, register REG. Returns number of bytes written on success, -1 on failure.
int hal__I2CWRITE(uint8_t i2cNum, uint8_t ADDR, uint8_t REG, uint8_t *data, uint16_t len)
{
    param_check((i2cNum >= 0) && (i2cNum < I2C_MAX_INSTANCE));
    param_check(data != NULL);
    //Write data to I2C device at ADDR, register REG. Returns number of bytes written on success, -1 on failure.
    if (0 == i2c_burst_write(i2c_device[i2cNum], ADDR, REG, data, len))
    {
        LOG_INF("I2C write at address 0x%02X, register 0x%02X", ADDR, REG);
        LOG_HEXDUMP_INF(data, len, "data:");
        return len;
    }
    else
    {
        // Write failed, try to write each byte individually till failure
        int16_t write_idx;
        for(write_idx=0; write_idx<len; write_idx++)
        {
            if (SUCCESS != hal__I2CWRITE_uint8(i2cNum, ADDR, REG + write_idx, data[write_idx]))
            {
                if(write_idx > 0) // Able to write some bytes
                    break;
                else
                {
                    LOG_ERR("I2C write failed at address 0x%02X, register 0x%02X", ADDR, REG);
                    return FAILURE;
                }
            }
        }
        LOG_WRN("I2C write %d/%dB at address 0x%02X, register 0x%02X", write_idx, len, ADDR, REG);
        LOG_HEXDUMP_INF(data, write_idx, "data:");
        return write_idx;
    }
}