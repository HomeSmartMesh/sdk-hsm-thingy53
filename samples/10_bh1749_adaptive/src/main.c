/*
 * Copyright (c) 2019 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <bh1749.h>
#include <stdio.h>

void main(void)
{
	const struct device *dev = DEVICE_DT_GET_ONE(rohm_bh1749);

	if (!device_is_ready(dev)) {
		printf("Sensor device BH1749 not ready\n");
		return;
	}

	while(true){
		float r,g,b,ir;
		bh1749_get_light(dev,&r,&g,&b,&ir);
		printf("light r=%.2f, g=%.2f, b=%.2f, ir=%.2f\n", r,g,b,ir);
		k_sleep(K_MSEC(10000));
	}
}
