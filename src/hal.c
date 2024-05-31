/*******************************************************************************
* Title                 :   HAL for nRF52
* Filename              :   hal.c
* Origin Date           :   2023/09/20
* Version               :   0.0.0
* Compiler              :   Zephyr 2.7
* Target                :   nRF52832
* Notes                 :   None
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/
#include "hal.h"

#include "nrf.h"
#include "system_nrf52.h"


/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#include "zephyr/logging/log.h"
#define MODULE_NAME			        hal
#define MODULE_LOG_LEVEL	        LOG_LEVEL_DBG
LOG_MODULE_REGISTER(MODULE_NAME, MODULE_LOG_LEVEL);
/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
uint8_t io_input_pins[] = {3, 4, 5, 12, 13, 16};

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
int __InitUART()
{
    // Init UART0
    if(hal__UARTInit(1) != SUCCESS)
    {
        LOG_ERR("UART1 init failed");
        return FAILURE;
    }
    return SUCCESS;
}

int __InitI2C()
{
    // Init I2C0
    if(hal__I2CINIT(0) != SUCCESS)
    {
        LOG_ERR("I2C0 init failed");
        return FAILURE;
    }
    return SUCCESS;
}

// Enable nRESET of nRF52 by changing PSELRESET register
void System_EnableResetPin(void)
{
    // Log data of PSELRESET register
	LOG_INF("Current PSELRESET[0] = 0x%x\n", NRF_UICR->PSELRESET[0]);
	LOG_INF("Current PSELRESET[1] = 0x%x\n", NRF_UICR->PSELRESET[1]);
    if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) || 
		((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)))
	{
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_UICR->PSELRESET[0] = CONFIG_GPIO_NRESET_PIN;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_UICR->PSELRESET[1] = CONFIG_GPIO_NRESET_PIN;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        // Log data of PSELRESET register
        LOG_INF("Updated PSELRESET[0] = 0x%x\n", NRF_UICR->PSELRESET[0]);
        LOG_INF("Updated PSELRESET[1] = 0x%x\n", NRF_UICR->PSELRESET[1]);
        LOG_INF("nRESET pin is enabled\n");
        LOG_INF("System resetting...\n");
        NVIC_SystemReset();
	}
    LOG_INF("nRESET pin is already enabled\n");
}

// Disable nRESET of nRF52
void System_DisableResetPin(void)
{
    // Log data of PSELRESET register
    LOG_INF("Current PSELRESET[0] = 0x%x\n", NRF_UICR->PSELRESET[0]);
    LOG_INF("Current PSELRESET[1] = 0x%x\n", NRF_UICR->PSELRESET[1]);

	if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Disconnected << UICR_PSELRESET_CONNECT_Pos)) || 
		((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Disconnected << UICR_PSELRESET_CONNECT_Pos)))
	{
        // Do an ERASEUICR instead of changing PSELRESET register
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_NVMC->ERASEUICR = NVMC_ERASEUICR_ERASEUICR_Erase << NVMC_ERASEUICR_ERASEUICR_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        // Log data of PSELRESET register
        LOG_INF("Updated PSELRESET[0] = 0x%x\n", NRF_UICR->PSELRESET[0]);
        LOG_INF("Updated PSELRESET[1] = 0x%x\n", NRF_UICR->PSELRESET[1]);
        LOG_INF("nRESET pin is disabled\n");
        LOG_INF("System resetting...\n");
        NVIC_SystemReset();
	}
    LOG_INF("nRESET pin is already disabled\n");
}

int hal__init(void)
{
    // Init input pins
    if (__InitGPIO() != SUCCESS)
    {
        LOG_ERR("GPIO init failed");
        return FAILURE;
    }
    // TODO - TMT: Implement
    // for (int i = 0; i < ARRAY_SIZE(io_input_pins); i++)
    // {
    //     if ( hal__setState(io_input_pins[i], 0) != SUCCESS)
    //     {
    //         LOG_ERR("Setting pin %d as input failed", io_input_pins[i]);
    //         return FAILURE;
    //     }      
    // }

    // Init PWM
    if (__InitPWM() != SUCCESS)
    {
        LOG_ERR("PWM init failed");
        return FAILURE;
    }

    // // Init ADC
    // if (__InitADC() != SUCCESS)
    // {
    //     LOG_ERR("ADC init failed");
    //     return FAILURE;
    // }

    // Init UART
    if (__InitUART() != SUCCESS)
    {
        LOG_ERR("UART init failed");
        return FAILURE;
    }

    // Init I2C
    if (__InitI2C() != SUCCESS)
    {
        LOG_ERR("I2C init failed");
        return FAILURE;
    }

    return SUCCESS;

}