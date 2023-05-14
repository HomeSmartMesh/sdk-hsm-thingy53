#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

zephyr_library_sources(bsec2.cpp)

zephyr_library_import(lib_bsec ${CMAKE_CURRENT_SOURCE_DIR}/Bosch-BSEC2-Library/src/cortex-m33/fpv5-sp-d16-hard/libalgobsec.a)
zephyr_include_directories(Bosch-BSEC2-Library/src/inc)
target_link_libraries(lib_bsec INTERFACE kernel)
target_link_libraries(app PRIVATE lib_bsec)
