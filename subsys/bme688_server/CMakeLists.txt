#
# Copyright (c) 2019 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_BME688_SERVER bme688_server.cpp)

zephyr_include_directories_ifdef(CONFIG_BME688_BSEC2 bsec2)
add_subdirectory_ifdef(CONFIG_BME688_BSEC2 bsec2)
