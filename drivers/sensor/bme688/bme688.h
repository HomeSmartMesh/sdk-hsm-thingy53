/**
 * Copyright (C) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include "bme68x.h"
#include "bme68x_defs.h"
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#define BME68X_VALID_DATA (BME68X_NEW_DATA_MSK|BME68X_GASM_VALID_MSK|BME68X_HEAT_STAB_MSK)
typedef enum{
	bme688_mode_sleep = 0x00,
	bme688_mode_forced = 0x01,
	bme688_mode_parallel = 0x02,
	bme688_mode_sequencial = 0x03
} bme688_mode_t;

struct bme688_config {
	struct i2c_dt_spec i2c;
};

typedef struct{
    uint16_t heater_temperature;
    uint16_t heater_duration;
    uint16_t *heater_temperature_profile;
    uint16_t *heater_duration_profile;
    uint8_t heater_profile_len;
    uint8_t op_mode;
}bme688_heater_config_t;

/*!
 *  @brief Function to select the interface between SPI and I2C.
 *
 *  @param[in] bme      : Structure instance of bme68x_dev
 *  @param[in] intf     : Interface selection parameter
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int bme688_init(const struct device * dev);

void bme688_set_mode(bme688_mode_t v_mode);
void bme688_set_oversampling(uint8_t osTemp, uint8_t osPres, uint8_t osHum);
void bme688_set_heater_config(bme688_heater_config_t *heater_config);
void bme688_set_mode_default_conf(bme688_mode_t v_mode);

void bme688_wait_for_measure();
int bme688_sample_fetch(const struct device *dev,enum sensor_channel chan);
uint8_t bme688_data_get(const struct device *dev, struct bme68x_data *data);

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] len          : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose value is to be written.
 *  @param[in] len          : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] len          : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose data has to be written.
 *  @param[in] len          : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 * APIs.
 *
 *  @param[in] period       : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 *
 */
void bme68x_delay_us(uint32_t period, void *intf_ptr);

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : Name of the API whose execution status has to be printed.
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bme68x_check_rslt(const char api_name[], int8_t rslt);

#ifdef __cplusplus
}
#endif /*__cplusplus */
