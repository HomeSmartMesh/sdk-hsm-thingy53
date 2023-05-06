/**
 * Copyright (C) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bme68x.h"
#include "bme688.h"

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>

/******************************************************************************/
/*!                 Macro definitions                                         */

/******************************************************************************/
/*!                Static variable definition                                 */

/*                  !!! Single instance device driver !!!                     */

static struct bme68x_dev bme_api_dev;
static struct bme68x_conf conf;
static struct bme68x_heatr_conf heatr_conf;
static struct bme68x_data data;
static uint8_t n_fields;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    const struct device *const sensor_dev = (struct device*)intf_ptr;
    const struct bme688_config *config = (struct bme688_config*)sensor_dev->config;
	const struct i2c_dt_spec *i2c_dev = &config->i2c;

	if (i2c_burst_read_dt(i2c_dev, reg_addr, reg_data, len)) {
        printf("bme68x_i2c_read timeout\n");
		return 1;
	}
    //else{
    //    printf("\nbme68x_i2c_read success @0x%0x: ",reg_addr);
    //    for(int i=0;i<len;i++){
    //        printf("%0x ",reg_data[i]);
    //    }
    //    printf("\n");
    //}
    return BME68X_INTF_RET_SUCCESS;
}

/*!
 * I2C write function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    const struct device *const sensor_dev = (struct device*)intf_ptr;
    const struct bme688_config *config = (struct bme688_config*)sensor_dev->config;
	const struct i2c_dt_spec *i2c_dev = &config->i2c;

    if(i2c_burst_write_dt(i2c_dev, reg_addr, reg_data, len)){
        printf("i2c_burst_write_dt timeout\n");
        return 1;
    }
    return BME68X_INTF_RET_SUCCESS;
}

/*!
 * Delay function map to COINES platform
 */
void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    k_sleep(K_USEC(period));
}

void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:

            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}

int bme688_init(const struct device *dev)
{
    int8_t rslt = BME68X_OK;

    bme_api_dev.read = bme68x_i2c_read;
    bme_api_dev.write = bme68x_i2c_write;
    bme_api_dev.intf = BME68X_I2C_INTF;
    bme_api_dev.delay_us = bme68x_delay_us;
    bme_api_dev.intf_ptr = dev;
    bme_api_dev.amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */

    rslt = bme68x_init(&bme_api_dev);
    bme68x_check_rslt("bme68x_init",rslt);

    return (int)rslt;
}

int bme688_sample_fetch(const struct device *dev,enum sensor_channel chan)
{
    int8_t rslt = BME68X_OK;

    conf.filter = BME68X_FILTER_OFF;
    conf.odr = BME68X_ODR_NONE;
    conf.os_hum = BME68X_OS_16X;
    conf.os_pres = BME68X_OS_1X;
    conf.os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(&conf,&bme_api_dev);
    bme68x_check_rslt("bme68x_set_conf",rslt);

    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp = 300;
    heatr_conf.heatr_dur = 100;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf,&bme_api_dev);
    bme68x_check_rslt("bme68x_set_heatr_conf",rslt);

    rslt = bme68x_set_op_mode(BME68X_FORCED_MODE,&bme_api_dev);
    bme68x_check_rslt("bme68x_set_op_mode",rslt);

    const uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme_api_dev) + (heatr_conf.heatr_dur * 1000);
    //printf("del_period = %u\n",del_period);
    k_sleep(K_USEC(del_period));

    rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme_api_dev);
    bme68x_check_rslt("bme68x_get_data",rslt);

	return 0;
}

bool bme688_data_get(const struct device *dev, struct bme68x_data *p_data){
    (*p_data) = data;
    return (n_fields != 0);
}

static int bme688_channel_get(const struct device *dev,enum sensor_channel chan,struct sensor_value *val)
{
    if (n_fields == 0)
    {
        return EINPROGRESS;
    }

	switch (chan) {
	case SENSOR_CHAN_AMBIENT_TEMP:
		/*
		 * data.temperature has a resolution of 0.01 degC.
		 * So 5123 equals 51.23 degC.
		 */
		val->val1 = (int32_t)(data.temperature / 100);
		val->val2 = (data.temperature - val->val1 * 100) * 10000;
		break;
	case SENSOR_CHAN_PRESS:
		/*
		 * data.pressure has a resolution of 1 Pa.
		 * So 96321 equals 96.321 kPa.
		 */
		val->val1 = (int32_t)(data.pressure / 1000);
		val->val2 = (data.pressure - val->val1 * 1000) * 1000;
		break;
	case SENSOR_CHAN_HUMIDITY:
		/*
		 * data.humidity has a resolution of 0.001 %RH.
		 * So 46333 equals 46.333 %RH.
		 */
		val->val1 = (int32_t)(data.humidity / 1000);
		val->val2 = (data.humidity - val->val1 * 1000) * 1000;
		break;
	case SENSOR_CHAN_GAS_RES:
		/*
		 * data.gas_resistance has a resolution of 1 ohm.
		 * So 100000 equals 100000 ohms.
		 */
		val->val1 = data.gas_resistance;
		val->val2 = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}


static const struct sensor_driver_api bme688_api_funcs = {
	.sample_fetch = bme688_sample_fetch,
	.channel_get = bme688_channel_get,
};

#define DT_DRV_COMPAT bosch_bme688

#define BME688_CONFIG_I2C(inst)			       \
	{					       \
		.i2c = I2C_DT_SPEC_INST_GET(inst), \
	}

//bme68x_data unused

#define BME688_DEFINE(inst)						                                        \
	static struct bme68x_data bme68x_data_##inst;			                            \
	static const struct bme688_config bme688_config_##inst = BME688_CONFIG_I2C(inst);   \
	SENSOR_DEVICE_DT_INST_DEFINE(inst,				                                    \
			 bme688_init,					                                    \
			 NULL,						                                                \
			 &bme68x_data_##inst,				                                        \
			 &bme688_config_##inst,				                                        \
			 POST_KERNEL,					                                            \
			 CONFIG_SENSOR_INIT_PRIORITY,			                                    \
			 &bme688_api_funcs);

/* Create the struct device for every status "okay" node in the devicetree. */
DT_INST_FOREACH_STATUS_OKAY(BME688_DEFINE)
