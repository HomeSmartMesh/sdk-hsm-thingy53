#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

#default board can be overriden by -b
set(BOARD thingy53_nrf5340_cpuapp)
set(DTC_OVERLAY_FILE app.overlay)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(openthread_cli)

FILE(GLOB app_sources src/*.c src/*.cpp)
target_sources(app PRIVATE ${app_sources})
