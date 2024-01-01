/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include "hal.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, test_InitGPIO_should_ReturnSUCCESS)
{
	int ret = __InitGPIO();
	zassert_equal(ret, SUCCESS, "__InitGPIO() should return SUCCESS");
}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE_when_invalidPinNum)
{
	int ret = SUCCESS;
	ret = hal__setState(49, 0);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with pin number 49");

	ret = SUCCESS;
	ret = hal__setState(-1, 0);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with pin number -1");

}

ZTEST(framework_tests, test_hal__setState_should_ReturnFAILURE_when_invalidState)
{
	int ret = SUCCESS;
	ret = hal__setState(0, 3);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with state 3");

	ret = SUCCESS;
	ret = hal__setState(0, -1);
	zassert_equal(ret, FAILURE, "hal__setState() should return FAILURE with state -1");
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

