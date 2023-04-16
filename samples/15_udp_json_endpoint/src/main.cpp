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

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread main()\n\r" \
	"udp echo server\n\r"

void json_endpoint_handler(std::string &client, std::string &topic, json &request, json &response){
	LOG_INF("json_endpoint_handler()");
	response = request;
	response["result"] = "OK";
}

int main(void)
{
	LOG_INF(WELLCOME_TEXT);

	app_ot_init();//logs joiner info and initializes reset buttons

	set_endpoint_handler(json_endpoint_handler);

	int count = 0;
	while(1){
		LOG_INF("sleeping 10 sec count = %d",count);
		count++;
		k_sleep(K_MSEC(10000));
	}
}
