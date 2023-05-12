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
	//bme688_set_mode_single();
	bme688_set_mode_multi();
	k_sleep(K_MSEC(3000));
	int sample_count = 1;
	while (1) {
		printf("Sample: %d : \n",sample_count);
		//bme688 API usage, sensor_sample_fetch and sensor_channel_get also available
		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data[10];
		uint8_t n_fields = bme688_data_get(dev, data);
		if(n_fields != 0){
			printf(" n=%d : meas index, gas index, Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm)\n",n_fields);
			for(uint8_t i = 0; i < n_fields; i++){
				if(!(data[i].status&BME68X_NEW_DATA_MSK)){
					printf("no new data ");
				}
				if(!(data[i].status&BME68X_GASM_VALID_MSK)){
					printf("Gas Measure not valid ");
				}
				if(!(data[i].status&BME68X_HEAT_STAB_MSK)){
					printf("No Heat Stability ");
				}
				if (data[i].status == BME68X_VALID_DATA){
					printf("%u, %d, %.2f, %.2f, %.2f, %.2f\n",
						data[i].meas_index,
						data[i].gas_index,
						data[i].temperature,
						data[i].pressure,
						data[i].humidity,
						data[i].gas_resistance);
				}
			}
		}else{
			printf("No new data\n");
		}

		sample_count++;
		k_sleep(K_MSEC(20000));
	}
}
