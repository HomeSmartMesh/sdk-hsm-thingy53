#
# Copyright (c) 2018 - 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

add_subdirectory_ifdef(CONFIG_APP_BATTERY   app_battery)
add_subdirectory_ifdef(CONFIG_APP_LED	app_led)
add_subdirectory_ifdef(CONFIG_APP_OT app_ot)
add_subdirectory_ifdef(CONFIG_JSON_ENDPOINT json_endpoint)
add_subdirectory_ifdef(CONFIG_UDP_BROADCAST udp_broadcast)
add_subdirectory_ifdef(CONFIG_BME688_SERVER bme688_server)
