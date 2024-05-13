/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/ztest.h>
#include "hal.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);

/* Test InitGPIO() */
ZTEST(framework_tests, test_InitGPIO_should_ReturnSUCCESS)
{
	int ret = __InitGPIO();
	zassert_equal(ret, SUCCESS, "__InitGPIO() should return SUCCESS");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE_when_pinNum_is_less_than_0)
{
	int ret = SUCCESS;
	ret = hal__setState(-1, 0);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with pin number -1");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE_when_pinNum_greater_than_or_equal_to_GPIO_PIN_NUMBER_ALL)
{
	int ret = SUCCESS;
	uint8_t pinNum = GPIO_PIN_NUMBER_ALL;
	ret = hal__setState(pinNum, 0);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with pin number equal to GPIO_PIN_NUMBER_ALL");

	ret = SUCCESS;
	pinNum = GPIO_PIN_NUMBER_ALL + 1;
	ret = hal__setState(pinNum, 0);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with pin number greater than GPIO_PIN_NUMBER_ALL");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE__when_state_is_less_than_0)
{
	int ret = SUCCESS;
	ret = hal__setState(0, -1);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with state -1");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE_when_state_is_greater_than_or_equal_to_3)
{
	int ret = SUCCESS;
	ret = hal__setState(0, 3);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with state 3");

	ret = SUCCESS;
	ret = hal__setState(0, 4);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with state 4");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnSUCCESS_when_validParams)
{
	int ret;

	// Valid pin number tests
	ret = hal__setState(0, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(47, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(24, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(26, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");

	// Valid pin state tests
	ret = hal__setState(0, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(0, 1);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(0, 2);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");

	ret = hal__setState(47, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(47, 1);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(47, 2);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");

	ret = hal__setState(22, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(22, 1);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
	ret = hal__setState(22, 2);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS");
}

ZTEST(framework_tests, test_hal__setState_should_set_pin_to_input_when_state_is_0)
{
	int ret;
	int ret = hal__setState(0, 1);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS with valid params");

	// Verify that the pin is set to input
	// gpio_port_pins_t * io_input=NULL;
	gpio_flags_t flags_get = 0;

	// gpio_port_pins_t * io_output=NULL;
	// ret = gpio_port_get_direction(gpio_devices[mPORT(0)], mPIN(0), io_input, io_output);
	// ret = gpio_pin_get_config(gpio_devices[mPORT(0)], mPIN(0), io_input, io_output);
	// ret = gpio_pin_get_config(gpio_devices[mPORT(0)], mPIN(0), &flags_get);
	// ret = gpio_pin_is_input(gpio_devices[mPORT(0)], mPIN(0));
	// ret = gpio_pin_is_output(gpio_devices[mPORT(0)], mPIN(0));
	zassert_equal(ret, 0, "gpio_port_get_direction() should return 0");
	zassert_equal(flags_get, GPIO_INPUT, "hal__setState() should set pin to input");
}


