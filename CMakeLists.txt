# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hal_nrf52)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_sources(app PRIVATE 
            src/main.c
            src/hal.c
            src/hal_uart.c
            src/hal_i2c.c
            )
