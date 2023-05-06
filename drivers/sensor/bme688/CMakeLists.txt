# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources(bme688.c)

add_subdirectory(BME68x-Sensor-API)
zephyr_include_directories(BME68x-Sensor-API)
