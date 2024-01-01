# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(integration)

# Generate compile_commands.json 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Get parent directory
set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
message(STATUS "APP_SRC_DIR: ${APP_SRC_DIR}")

# Include ${APP_SRC_DIR} directory
target_include_directories(app PRIVATE ${APP_SRC_DIR})
# Add test source file
target_sources(app PRIVATE ${APP_SRC_DIR}/hal_gpio.c)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

