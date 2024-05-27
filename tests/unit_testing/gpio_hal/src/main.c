/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/ztest.h>
#include "hal.h"
#include "zephyr/ztest_assert.h"

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
	// Set pin 0 to input state
	int ret = hal__setState(0, 0);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS with valid params");

	// Verify that the pin is set to input
	gpio_port_pins_t io_input = 0;
	gpio_port_pins_t io_output= 0;
	ret = gpio_port_get_direction(gpio_devices[mPORT(0)], 1U << mPIN(0), &io_input, &io_output);
	zassert_equal(ret, 0, "gpio_port_get_direction() should return 0");
	zassert_equal(io_output, 0, "io_output should remain 0 when pin is set to input");
	zassert_not_equal(io_input, 0, "io_input should not be 0 when pin is set to input");
}

ZTEST(framework_tests, test_hal__setState_should_set_pin_to_output_when_state_is_1)
{
	// Set pin 0 to output state
	int ret = hal__setState(1, 1);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS with valid params");

	// Verify that the pin is set to output
	gpio_port_pins_t io_input = 0;
	gpio_port_pins_t io_output= 0;
	ret = gpio_port_get_direction(gpio_devices[mPORT(0)], 1U << mPIN(1), &io_input, &io_output);
	zassert_equal(ret, 0, "gpio_port_get_direction() should return 0");
	zassert_equal(io_input, 0, "io_input should remain 0 when pin is set to output");
	zassert_not_equal(io_output, 0, "io_output should not be 0 when pin is set to output");
}

ZTEST(framework_tests, test_hal__setState_should_set_pin_to_high_impedance_when_state_is_2)
{
	// Set pin 0 to high state
	int ret = hal__setState(2, 2);
	zassert_equal(ret, SUCCESS, "hal__setState() should return SUCCESS with valid params");

	// Verify that the pin is set to high
	gpio_port_pins_t io_input = 0;
	gpio_port_pins_t io_output= 0;
	ret = gpio_port_get_direction(gpio_devices[mPORT(0)], 1U << mPIN(2), &io_input, &io_output);
	zassert_equal(ret, 0, "gpio_port_get_direction() should return 0");
	zassert_equal(io_output, 0, "io_output should remain 0 when pin is set to high impedance");
	zassert_equal(io_input, 0, "io_input should remain 0 when pin is set to high impedance");
}


