/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include "bme688_server.h"
#include <string>

json config = {
  {"mode", "parallel"},
  {"heater", true},
};

void bme688_handler(json &data){
	std::string text = data.dump();
	printf("bme688_handler> %s\n",text.c_str());
}

int main(void)
{
	printf("Test bme688 server app - early debug wait connection\n");
	k_sleep(K_MSEC(10000));
	printf("Test bme688 server app - start\n");

	set_bme688_config(config);

	set_bme688_handler(bme688_handler);
	uint32_t count = 0;
	while (1) {
		printf("loop %d main sleeping 10 sec\n",count++);
		k_sleep(K_MSEC(10000));
	}
	return 0;
}
