/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "common.h"

void main(void)
{
	printf("Test sample\n");
	k_sleep(K_MSEC(5000));
	printf("Test sample\n");

	const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bme688);

	if (!device_is_ready(dev)) {
		printk("sensor: device not ready.\n");
		return;
	}

	printf("Device %p name is %s\n", dev, dev->name);



	bme68x_interface_init();

	int count = 0;
	while (1) {
		k_sleep(K_MSEC(3000));

		printf("Test: %d\n",count);
		count++;
	}
}
