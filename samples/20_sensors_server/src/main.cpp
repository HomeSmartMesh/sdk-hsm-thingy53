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
#include "udp_broadcast.h"

LOG_MODULE_REGISTER(sensor_server_sample, CONFIG_SONSORS_SERVER_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"main()\n\r" \
	"sesnors server sample\n\r"

void json_endpoint_handler(std::string &client, std::string &topic, json &request, json &response){
	LOG_INF("json_endpoint_handler()");
	response = request;
	response["result"] = "OK";
}

int main(void)
{
	LOG_INF(WELLCOME_TEXT);

	app_ot_init();//logs joiner info and initializes reset buttons
	app_led_init();
	set_endpoint_handler(json_endpoint_handler);

	app_led_blink_green(0.1,500,1000);

	int count = 0;
	while(1){
		uint8_t message[250];
		int size = sprintf((char*)message,"thread_thingy_53/{\"alive\":%d}",count);
		send_udp(message, size);
		count++;

		LOG_INF("sleeping 10 sec count = %d",count);
		k_sleep(K_MSEC(10000));
	}
}
