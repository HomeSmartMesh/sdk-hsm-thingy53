/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include "bme688_server.h"
#include <string>

void bme688_handler(json &data){
	std::string text = data.dump(4);
	printf(" - bme688_handler> %s\n",text.c_str());
}

int main(void)
{
	printf(" - Test bme688 bsec2 app - early debug wait connection\n");
	k_sleep(K_MSEC(10000));
	printf(" - Test bme688 bsec2 app - start\n");

	start_bme688(bme688_handler);
	k_sleep(K_MSEC(3000));

	uint32_t count = 0;
	while (1) {
		printf(" - loop %d main sleeping 30 sec\n",count++);
		k_sleep(K_MSEC(30000));
	}
	return 0;
}
