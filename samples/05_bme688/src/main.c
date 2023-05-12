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
	bme688_mode_t mode = parallel;//single, parallel, sequencial
	bme688_set_mode(mode);
	k_sleep(K_MSEC(3000));
	int sample_count = 1;
	printf("Sample count, meas index, gas index, Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm)\n");
	while (1) {
		//bme688 API usage, sensor_sample_fetch and sensor_channel_get also available
		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data[10];
		uint8_t n_fields = bme688_data_get(dev, data);
		if(n_fields != 0){
			for(uint8_t i = 0; i < n_fields; i++){
				if (data[i].status == BME68X_VALID_DATA){
					printf("%d, %u, %d, %.2f, %.2f, %.2f, %.2f\n",
						sample_count,
						data[i].meas_index,
						data[i].gas_index,
						data[i].temperature,
						data[i].pressure,
						data[i].humidity,
						data[i].gas_resistance);
					sample_count++;
				}
			}
		}
		//no sleep as sample fetch do the sleep
	}
}
