/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "bme68x.h"

void main(void)
{
	printf("Test sample\n");
	k_sleep(K_MSEC(5000));
	printf("Test sample\n");

	struct bme68x_dev dev;
	bme68x_init(&dev);

	int count = 0;
	while (1) {
		k_sleep(K_MSEC(3000));

		printf("Test: %d\n",count);
		count++;
	}
}
