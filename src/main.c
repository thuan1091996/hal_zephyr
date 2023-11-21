/*******************************************************************************
* Title                 :   HAL Template
* Filename              :   main.c
* Origin Date           :   2023/11/21
* Version               :   0.0.0
* Compiler              :   nRF connect SDK 2.3.0
* Target                :   nRF52840DK
* Notes                 :   None
*******************************************************************************/


/******************************************************************************
* Includes
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include <sys/_intsup.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include "hal.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#include "zephyr/logging/log.h"
#define MODULE_NAME			        main
#define MODULE_LOG_LEVEL	        LOG_LEVEL_INF
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);

#define APP_TEST_UART 				(1)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Static Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
#if (APP_TEST_UART != 0)
void uart_test_echo(int uart_num)
{
	int size = hal__UARTAvailable(uart_num);
	if(size > 0)
	{
		uint8_t data[128]={0};
		hal__UARTRead(uart_num, data, size);
		LOG_INF("UART%d recv %dB", uart_num, size);

		// Echo back
		hal__UARTWrite(uart_num, data, size);
		LOG_HEXDUMP_INF(data, size, "Data: ");
	}
}


void uart_test_task(void* p_param)
{
#define NUMB_UART		(2)
    while(1)
    {
		for(uint8_t i=0; i<=NUMB_UART; i++)
		{
			uart_test_echo(i);
        	k_msleep(100);
		}
    }
}
#endif /* End of (APP_TEST_UART != 0) */

int main(void)
{

	if (hal__init() != 0)
	{
		LOG_ERR("HAL init failed");
		return -1;
	}

#if (APP_TEST_UART != 0)
	uart_test_task(NULL);	
#endif /* End of (APP_TEST_UART != 0) */

}
