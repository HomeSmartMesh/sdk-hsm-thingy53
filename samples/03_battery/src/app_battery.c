/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * Edited for app_battery optimizations
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <hal/nrf_saadc.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "app_battery.h"
#include <stdio.h>

LOG_MODULE_DECLARE(app_battery);

#define VBATT DT_PATH(vbatt)
#define ZEPHYR_USER DT_PATH(zephyr_user)

static const struct gpio_dt_spec sPowerGpio = GPIO_DT_SPEC_GET(VBATT, power_gpios);
static const struct gpio_dt_spec sChargeGpio = GPIO_DT_SPEC_GET(ZEPHYR_USER, battery_charge_gpios);
static const uint32_t sFullOhms = DT_PROP(VBATT, full_ohms);
static const uint32_t sOutputOhms = DT_PROP(VBATT, output_ohms);
static const struct adc_dt_spec sAdc = ADC_DT_SPEC_GET(VBATT);

int16_t sAdcBuffer = 0;

struct adc_sequence sAdcSeq = {
	.buffer = &sAdcBuffer,
	.buffer_size = sizeof(sAdcBuffer),
	.calibrate = true,
};

int app_battery_init()
{
	int err = 0;

	if (!device_is_ready(sPowerGpio.port)) {
		LOG_ERR("Battery measurement GPIO device not ready");
		return -ENODEV;
	}

	err += gpio_pin_configure_dt(&sPowerGpio, GPIO_OUTPUT_INACTIVE);
	if (err != 0) {
		LOG_ERR("Failed to configure battery measurement GPIO %d", err);
		return err;
	}

	if (!device_is_ready(sAdc.dev)) {
		LOG_ERR("ADC controller not ready");
		return -ENODEV;
	}

	err += adc_channel_setup_dt(&sAdc);
	if (err) {
		LOG_ERR("Setting up the ADC channel failed");
		return err;
	}

	(void)adc_sequence_init_dt(&sAdc, &sAdcSeq);

    err += gpio_pin_set_dt(&sPowerGpio, 1);
    if (err != 0) {
        LOG_ERR("Failed to enable measurement pin %d", err);
    }

	if (!device_is_ready(sChargeGpio.port)) {
		LOG_ERR("Charge GPIO controller not ready");
		return -ENODEV;
	}

	err += gpio_pin_configure_dt(&sChargeGpio, GPIO_INPUT);
	if (err != 0) {
		LOG_ERR("Failed to configure battery charge GPIO %d", err);
		return err;
	}

	return err;
}

int32_t app_battery_voltage_mv()
{
	int32_t result = 0;
    int ret = adc_read(sAdc.dev, &sAdcSeq);
    if (ret == 0) {
        int32_t val = sAdcBuffer;
        adc_raw_to_millivolts_dt(&sAdc, &val);
        result = (int32_t)((int64_t)(val) * sFullOhms / sOutputOhms);
    }else{
        LOG_ERR("ADC Fail = %d\n",ret);
    }
	return result;
}

void app_battery_voltage_text(const char * text,int max_size){
	int32_t v = app_battery_voltage_mv();
	float vf = v;
	vf /=1000;
	sprintf_s(text,max_size,"%.3f",vf);
}

bool app_battery_charging()
{
	return (bool)gpio_pin_get_dt(&sChargeGpio);
}
