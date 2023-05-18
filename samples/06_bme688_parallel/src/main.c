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
	bme688_mode_t mode = bme688_mode_parallel;//bme688_mode_forced, bme688_mode_parallel, bme688_mode_sequencial
	bme688_set_mode_default_conf(mode);
	printf("bme688 set to Parallel mode\n");
	k_sleep(K_MSEC(3000));
	int measure_count = 0;
	//#define BME68X_NEW_DATA_MSK                       UINT8_C(0x80)
	//#define BME68X_GASM_VALID_MSK                     UINT8_C(0x20)
	//#define BME68X_HEAT_STAB_MSK                      UINT8_C(0x10)
	printf("0xA0 => nhs: No Heat Stability\n");
	printf("sample:count (meas index/gas index) : Temperature°, Pressure Pa, Humidity %%, 'Gas resistance ohm'\n");
	uint8_t last_index = 1;
	while (1) {
		//bme688 API usage, sensor_sample_fetch and sensor_channel_get also available
		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data[10];
		uint8_t n_fields = bme688_data_get(dev, data);
		if(n_fields != 0){
			for(uint8_t i = 0; i < n_fields; i++){
				if(data[i].gas_index < last_index){
					printf("---\n");//this is the time to broadcast the previous group
					measure_count++;
				}
				last_index = data[i].gas_index;
				if ((data[i].status & BME68X_NEW_DATA_MSK) &&
					(data[i].status & BME68X_GASM_VALID_MSK)
					){
						printf("sample:%d (%u/%d) : %.2f° , %.2f Pa , %.2f %% , %s %.2f ohm\n",
							measure_count,
							data[i].meas_index,
							data[i].gas_index,
							data[i].temperature,
							data[i].pressure,
							data[i].humidity,
							(data[i].status & BME68X_HEAT_STAB_MSK)?"":"(nhs)",
							data[i].gas_resistance);
				}
			}
		}else{
			printf("n_fields=%d;\n",n_fields);
		}
	}
		//no sleep as sample fetch do the sleep
}
