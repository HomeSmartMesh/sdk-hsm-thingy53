/*
 * Copyright (c) 2018 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include <bme688.h>

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

		//bme688 API usage, sensor_sample_fetch and sensor_channel_get also available
		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data;
		if(bme688_data_get(dev, &data)){
			printf("Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm)\n");
			printf("%.2f, %.2f, %.2f, %.2f\n",
					data.temperature,
					data.pressure,
					data.humidity,
					data.gas_resistance);
			if(!(data.status&BME68X_NEW_DATA_MSK)){
				printf("no new data");
			}
			if(!(data.status&BME68X_GASM_VALID_MSK)){
				printf("Gas Measure not valid");
			}
			if(!(data.status&BME68X_HEAT_STAB_MSK)){
				printf("No Heat Stability");
			}
		}else{
			printf("No new data\n");
		}

		k_sleep(K_MSEC(20000));
	}
}
