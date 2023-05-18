/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include "bme688_server.h"
#include <string>

json config =  R"(
  {
    "temperatures": [320, 100, 100, 100, 200, 200, 200, 320, 320, 320],
    "durations": [100, 100, 100, 100, 100, 100, 100, 100, 100, 100]
  }
)"_json;

std::string text;

void bme688_handler(json &data){
	text = data.dump(4);
	printf(" - bme688_handler> %s\n",text.c_str());
}

int main(void)
{
	printf(" - Test bme688 server app - early debug wait connection\n");
	k_sleep(K_MSEC(10000));
	printf(" - Test bme688 server app - start\n");
	printf(" - setting heater config: ");
	text = config.dump(4);
	printf("%s\n",text.c_str());

	set_bme688_config(config);
	start_bme688(bme688_handler);
	k_sleep(K_MSEC(3000));

	uint32_t count = 0;
	while (1) {
		printf(" - loop %d main sleeping 30 sec\n",count++);
		k_sleep(K_MSEC(30000));
	}
	return 0;
}
