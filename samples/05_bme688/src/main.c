/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <bme688.h>
#include <stdio.h>

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
	printf("default bme688 mode is Forced Mode\n");
	k_sleep(K_MSEC(3000));
	int sample_count = 1;
	printf("Sample count, Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm)\n");
	while (1) {
		//bme688 API usage, sensor_sample_fetch and sensor_channel_get also available
		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data;
		uint8_t n_fields = bme688_data_get(dev, &data);
		if(n_fields){//only 1 expected in Forced mode
			if (data.status == BME68X_VALID_DATA){
				printf("%d, %.2f, %.2f, %.2f, %.2f\n",
					sample_count,
					data.temperature,
					data.pressure,
					data.humidity,
					data.gas_resistance);
				sample_count++;
			}
		}
		k_sleep(K_MSEC(10000));
	}
}
