/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "bme688.h"

void main(void)
{
	printf("Test sample main() - debug startup helper\n");
	k_sleep(K_MSEC(10000));
	printf("Test sample main()\n");

	const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bme688);
	if (!device_is_ready(dev)) {
		printk("sensor: device not ready.\n");
		return;
	}
	printf("Sensor device %p name is %s\n", dev, dev->name);

	bme688_init(dev);
	k_sleep(K_MSEC(3000));
	int count = 0;
	while (1) {
		printf("Test: %d\n",count);
		count++;

		sensor_sample_fetch(dev);
		struct sensor_value value;
		sensor_channel_get(dev, SENSOR_CHAN_ALL, &value);

		k_sleep(K_MSEC(20000));
	}
}
