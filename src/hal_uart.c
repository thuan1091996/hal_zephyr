/*******************************************************************************
* Title                 :   HAL UART for nRF52
* Filename              :   hal_uart.c
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

/** \file hal_uart.c
 *  \brief This module contains the source code for UART HAL driver
 */
/******************************************************************************
* Includes
*******************************************************************************/

#include "hal.h"
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/sys/util.h>

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal_uart
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define UART_MAX_INSTANCE   ARRAY_SIZE(uart_device)
#define UART0_NODE          DT_NODELABEL(uart0)
#define UART1_NODE          DT_NODELABEL(uart1)
#define USB_CDC_NODE        DT_NODELABEL(usbd_cdc_acm)

// UART param define
#define UART0_BAUDRATE      DT_PROP(UART0_NODE, current_speed)
#if DT_NODE_HAS_STATUS(UART1_NODE, okay)
#define UART1_BAUDRATE      DT_PROP(UART1_NODE, current_speed)
#endif

// USB CDC param define
#if DT_NODE_HAS_STATUS(USB_CDC_NODE, okay)
    #define USB_CDC_UART_ENABLE  (1)
#endif

#if (USB_CDC_UART_ENABLE == 1)
#include <zephyr/drivers/usb/udc.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/usb_device.h>
#endif /* End of (USB_CDC_UART_ENABLE == 1) */
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define UART_DEFAULT_RINGBUFFER_SIZE    64
#define UART_DEFAULT_CONF_BAUDRATE      UART0_BAUDRATE
#define UART_DEFAULT_CONF_PARITY        UART_CFG_PARITY_NONE
#define UART_DEAFULT_CONF_STOP_BITS     UART_CFG_STOP_BITS_1
#define UART_DEAFULT_CONF_DATA_BITS     UART_CFG_DATA_BITS_8
#define UART_DEAFULT_CONF_FLOW_CTRL     UART_CFG_FLOW_CTRL_NONE

/******************************************************************************
* Module Preprocessor Configs
*******************************************************************************/
#define UART_CONF_DUMP_RX_DATA          (0) // Set to 1 to dump UART RX data when received


/******************************************************************************
* Module Typedefs
*******************************************************************************/

typedef struct {
    struct ring_buf ringbuf;
    uint32_t buffer[UART_DEFAULT_RINGBUFFER_SIZE];
}uart_ringbuffer_t;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static const struct device *uart_device[] = {
#if DT_NODE_HAS_STATUS(UART0_NODE, okay)
	DEVICE_DT_GET(UART0_NODE),
#endif

#if DT_NODE_HAS_STATUS(UART1_NODE, okay)
    DEVICE_DT_GET(UART1_NODE),
#endif

#if (USB_CDC_UART_ENABLE == 1)
    DEVICE_DT_GET(USB_CDC_NODE),
#endif /* (USB_CDC_UART_ENABLE == 1) */

};

uart_ringbuffer_t uart_ringbuffer[UART_MAX_INSTANCE];
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
#if (UART_CONF_DUMP_RX_DATA == 1)
void ring_buf_dump_data(struct ring_buf *p_ringbuf)
{
    uint8_t* p_data = NULL;
    // Get ring buffer current size
    uint32_t size = ring_buf_size_get(p_ringbuf);
    if(size == 0)
    {
        LOG_INF("Ring buffer is empty");
        return;
    }
    // Allocate memory for storing ring buffer data, get data and put it back to ring buffer
    p_data = k_malloc(size);
    if(p_data == NULL)
    {
        LOG_ERR("Failed to allocate memory for ring buffer data");
        return;
    }
    uint32_t ret = ring_buf_get(p_ringbuf, p_data, size);
    if(ret != size)
    {
        LOG_ERR("Failed to get ring buffer data, err code %d", ret);
        k_free(p_data);
        return;
    }
    ret = ring_buf_put(p_ringbuf, p_data, size);
    if(ret != size)
    {
        LOG_ERR("Failed to put ring buffer data, err code %d", ret);
        k_free(p_data);
        return;
    }
    // Dump ring buffer data
    LOG_HEXDUMP_DBG(p_data, size, "Ring buffer data");
    k_free(p_data);
}
#endif /* End of (UART_CONF_DUMP_RX_DATA == 1) */

void uart_irq_callback(const struct device *dev, void *user_data)
{
    // Claim ring buffer memory
    uart_ringbuffer_t* p_uart_ringbuffer = (uart_ringbuffer_t*)user_data;
    int rx = 0;
    uint8_t *dst;
    uint32_t partial_size = 0;
    uint32_t total_size = 0;

    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) 
    {
        if (!partial_size) 
        {
            partial_size = ring_buf_put_claim(&p_uart_ringbuffer->ringbuf, &dst, UART_DEFAULT_RINGBUFFER_SIZE);
        }
        if (!partial_size) 
        {
            LOG_ERR("Ring buffer of UART device: %s doesn't have enough space", dev->name);
            break;
        }
        rx = uart_fifo_read(dev, dst, partial_size);
        if (rx <= 0) 
        {
            continue;
        }
        dst += rx;
        total_size += rx;
        partial_size -= rx;
    }
    ring_buf_put_finish(&p_uart_ringbuffer->ringbuf, total_size);
}

#if (USB_CDC_UART_ENABLE == 1)
int usb_init(const struct device *usb_dev)
{
	int ret;

	if (!device_is_ready(usb_dev)) {
		LOG_ERR("USB CDC ACM device not ready");
		return FAILURE;
	}
	
	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB CDC ACM");
		return FAILURE;
	}
	return SUCCESS;
}
#endif /* End of (USB_CDC_UART_ENABLE == 1) */

int hal__UARTInit(uint8_t uartNum)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE));
    int status;
    if( !device_is_ready(uart_device[uartNum]) )
    {
        LOG_ERR("UART %d device is not ready!", uartNum);
        return FAILURE;
    }
#if (USB_CDC_UART_ENABLE == 1)
    if (uartNum == (UART_MAX_INSTANCE -1)) // USB device
    {
        if (usb_init(uart_device[uartNum]) != 0)
        {
            LOG_ERR("USB CDC init failed");
            return FAILURE;
        }
        LOG_INF("Init USB CDC success");
    }
    else 
#endif /* End of (USB_CDC_UART_ENABLE == 1) */
    {
        const struct uart_config uart_cfg = {
            .baudrate = UART_DEFAULT_CONF_BAUDRATE,
            .parity = UART_DEFAULT_CONF_PARITY,
            .stop_bits = UART_DEAFULT_CONF_STOP_BITS,
            .data_bits = UART_DEAFULT_CONF_DATA_BITS,
            .flow_ctrl = UART_DEAFULT_CONF_FLOW_CTRL
        };
        status = uart_configure(uart_device[uartNum], &uart_cfg);
        if(status != 0)
        {
            LOG_ERR("Failed to configure UART %d with error code %d", uartNum, status);
            return status;
        }
        LOG_INF("Init UART %d success", uartNum);
    }
    // UART interrupt config
    ring_buf_init(&uart_ringbuffer[uartNum].ringbuf, UART_DEFAULT_RINGBUFFER_SIZE, uart_ringbuffer[uartNum].buffer);
    status = uart_irq_callback_user_data_set(uart_device[uartNum], &uart_irq_callback, &uart_ringbuffer[uartNum]);
    if(status != 0)
    {
        LOG_ERR("Failed to set UART %d IRQ callback with error code %d", uartNum, status);
    }
    uart_irq_rx_enable(uart_device[uartNum]);
    return SUCCESS;
}

//NOTE: if API limitations only allow for knowing IF data is available, return 1 if data is available.
//Returns number of bytes available to read from UART. Returns 0 on success, -1 on failure.  
int hal__UARTAvailable(uint8_t uartNum)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE));
    int cur_size = ring_buf_size_get(&uart_ringbuffer[uartNum].ringbuf);
    if(cur_size > 0)
    {
        LOG_DBG("UART %d has %d bytes available", uartNum, cur_size);
        #if ( UART_CONF_DUMP_RX_DATA != 0)
        ring_buf_dump_data(&uart_ringbuffer[uartNum].ringbuf);
        #endif /* End of ( UART_CONF_DUMP_RX_DATA != 0) */    
    }
    return cur_size;
}

//Write data to UART. Returns 0 on success, -1 on failure.
int hal__UARTWrite_uint8(uint8_t uartNum, uint8_t data)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE));
    uart_poll_out(uart_device[uartNum], data);
    return SUCCESS;
}

//Write data to UART. Returns number of bytes written on success, -1 on failure.
int hal__UARTWrite(uint8_t uartNum, uint8_t *data, uint16_t len)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE)  && (data != NULL));
    for(uint16_t i = 0; i < len; i++)
    {
        uart_poll_out(uart_device[uartNum], data[i]);
    }
    return SUCCESS;
} 

 //Read data from UART. Returns 0 on success, -1 on failure.
int hal__UARTRead_uint8(uint8_t uartNum, uint8_t *data)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE)  && (data != NULL));
    int ret = ring_buf_get(&uart_ringbuffer[uartNum].ringbuf, data, 1);
    if(ret != 1)
    {
        LOG_ERR("hal__UARTRead_uint8() read failed on UART %d, err code %d", uartNum, ret);
        return FAILURE;
    }
    return SUCCESS;
}

 //Read data from UART. Returns number of bytes read on success, -1 on failure.
int hal__UARTRead(uint8_t uartNum, uint8_t *data, uint16_t len)
{
    param_check((uartNum >= 0) && (uartNum < UART_MAX_INSTANCE)  && (data != NULL));
    int ret = ring_buf_get(&uart_ringbuffer[uartNum].ringbuf, data, len);
    if(ret != len)
    {
        LOG_ERR("hal__UARTRead() read failed on UART %d, err code %d", uartNum, ret);
        return FAILURE;
    }
    return SUCCESS;
}
