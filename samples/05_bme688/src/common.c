/**
 * Copyright (C) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bme68x.h"
#include "common.h"

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
static struct bme68x_dev bme_api_dev;

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
    else{
        printf("\nbme68x_i2c_read success @0x%0x: ",reg_addr);
        for(int i=0;i<len;i++){
            printf("%0x ",reg_data[i]);
        }
        printf("\n");
    }
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

int bme68x_interface_init(const struct device *dev)
{
    int8_t rslt = BME68X_OK;

    bme_api_dev.read = bme68x_i2c_read;
    bme_api_dev.write = bme68x_i2c_write;
    bme_api_dev.intf = BME68X_I2C_INTF;
    bme_api_dev.delay_us = bme68x_delay_us;
    bme_api_dev.intf_ptr = dev;
    bme_api_dev.amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */

    bme68x_init(&bme_api_dev);

    return (int)rslt;
}

static int bme680_sample_fetch(const struct device *dev,enum sensor_channel chan)
{
	return 0;
}

static int bme680_channel_get(const struct device *dev,enum sensor_channel chan,struct sensor_value *val)
{
	return 0;
}


static const struct sensor_driver_api bme688_api_funcs = {
	.sample_fetch = bme680_sample_fetch,
	.channel_get = bme680_channel_get,
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
			 bme68x_interface_init,					                                    \
			 NULL,						                                                \
			 &bme68x_data_##inst,				                                        \
			 &bme688_config_##inst,				                                        \
			 POST_KERNEL,					                                            \
			 CONFIG_SENSOR_INIT_PRIORITY,			                                    \
			 &bme688_api_funcs);

/* Create the struct device for every status "okay" node in the devicetree. */
DT_INST_FOREACH_STATUS_OKAY(BME688_DEFINE)
