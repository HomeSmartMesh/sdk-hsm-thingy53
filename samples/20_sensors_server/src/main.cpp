/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "json_endpoint.h"
#include "app_ot.h"
#include "app_led.h"
#include "app_battery.h"
#include "udp_broadcast.h"
#include <json.hpp>
using json = nlohmann::json;

LOG_MODULE_REGISTER(sensor_server_sample, CONFIG_SONSORS_SERVER_LOG_LEVEL);

json data;

void json_endpoint_handler(std::string &client, std::string &topic, json &request, json &response){
	LOG_INF("json_endpoint_handler()");
	response = request;
	response["result"] = "OK";
}

int main(void)
{
	LOG_INF(R"(
	main()
	sensors server sample
	)");

	app_ot_init();//logs joiner info and initializes reset buttons
	app_led_init();
	app_battery_init();
	set_endpoint_handler(json_endpoint_handler);

	app_led_blink_green(0.1,500,1000);

	int count = 0;
	while(1){
		data["alive"] = count++;

		int32_t voltage = app_battery_voltage_mv();
		bool is_charging = app_battery_charging();
		data["battery"]["mv"] = voltage;
		data["battery"]["charging"] = is_charging;

		std::string message = "thingy_53/"+data.dump();
		send_udp(message);

		LOG_INF("%s",data.dump(4).c_str());
		LOG_INF("sleeping 10 sec");
		k_sleep(K_MSEC(10000));
	}
}
