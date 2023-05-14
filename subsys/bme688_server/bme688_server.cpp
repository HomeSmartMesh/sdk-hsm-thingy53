//zephyr\samples\net\sockets\echo_client\src\udp.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <bme688.h>
#include <stdio.h>
#include <string>

#include "bme688_server.h"

#if defined(CONFIG_BME688_BSEC2)
#include "bsec2.h"
#endif

LOG_MODULE_REGISTER(bme688_server, LOG_LEVEL_INF);

#define BME688_SERVICE_STACK_SIZE 8192
#define BME688_SERVICE_PRIORITY 30
#define BME688_SERVICE_START_DELAY_MS 100

json user_data = nullptr;
iaq_output_t iaq_output;

void bme688_service();

K_THREAD_DEFINE(	bme688_thread, BME688_SERVICE_STACK_SIZE, bme688_service, NULL, NULL, NULL,
					BME688_SERVICE_PRIORITY, 0, BME688_SERVICE_START_DELAY_MS);


const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bme688);
static bme688_handler_t m_bme688_server_handler = NULL;
static bool started = false;

void set_bme688_config(json &config){
	LOG_INF("set_bme688_config()");
	std::string text = config.dump(4);
	LOG_INF("%s",text.c_str());
	uint8_t nb_steps = config["temperatures"].size();
	uint16_t temperatures[10];
	uint16_t durations[10];
	for(uint8_t i=0;i<nb_steps;i++){
		temperatures[i] = config["temperatures"][i];
		durations[i] = config["durations"][i];
	}
	bme688_set_heater_config(temperatures,durations,nb_steps);
}

void start_bme688(){
	LOG_INF("start_bme688()");

	if (!device_is_ready(dev)) {
		printk("sensor: device not ready.\n");
		return;
	}
	LOG_INF("Sensor device %p name is %s\n", dev, dev->name);

	bme688_init(dev);
	bme688_mode_t mode = bme688_mode_parallel;//bme688_mode_forced, bme688_mode_parallel, bme688_mode_sequencial
	bme688_set_mode(mode);
	LOG_INF("bme688 set to Parallel mode\n");

	#if defined(CONFIG_BME688_BSEC2)
	bsec2_start();
	#endif

	started = true;
}

void set_bme688_handler(bme688_handler_t handler)
{
	LOG_INF("set_bme688_handler()");
	m_bme688_server_handler = handler;
	if(!started){
		start_bme688();
	}
}

void bme688_service(){

	LOG_INF("hello from bme688_service()");
	int measure_count = 0;
	//#define BME68X_NEW_DATA_MSK                       UINT8_C(0x80)
	//#define BME68X_GASM_VALID_MSK                     UINT8_C(0x20)
	//#define BME68X_HEAT_STAB_MSK                      UINT8_C(0x10)
	LOG_DBG("0xA0 => nhs: No Heat Stability");
	LOG_DBG("sample:count (meas index/gas index) : Temperature°, Pressure Pa, Humidity %%, 'Gas resistance ohm'");
	uint8_t last_index = 1;
	while(!started){
			k_sleep(K_MSEC(100));
	}
	while(true){

		bme688_sample_fetch(dev,SENSOR_CHAN_ALL);
		struct bme68x_data data[3];
		uint8_t n_fields = bme688_data_get(dev, data);
		if(n_fields != 0){
			for(uint8_t i = 0; i < n_fields; i++){
				if(data[i].gas_index < last_index){
					if(m_bme688_server_handler != NULL){
						if(user_data != nullptr){
							m_bme688_server_handler(user_data);
						}
						user_data = nullptr;
					}
					LOG_DBG("---");
					measure_count++;
				}
				last_index = data[i].gas_index;
				if ((data[i].status & BME68X_NEW_DATA_MSK) &&
					(data[i].status & BME68X_GASM_VALID_MSK)
					){
						user_data["sample"] = measure_count;
						user_data["temperature"] = data[i].temperature;
						user_data["humidity"] = data[i].humidity;
						user_data["pressure"] = data[i].pressure;
						#if defined(CONFIG_BME688_BSEC2)
						if(processData(data[i],iaq_output)){
							user_data["iaq"] = iaq_output.iaq;
							user_data["iaq_accuracy"] = iaq_output.iaq_accuracy;
							user_data["stabilization"] = iaq_output.stabilization;
							user_data["runin"] = iaq_output.runin_status;
						}
						#endif

						std::string gas_index_name = "gas";
						gas_index_name += std::to_string(data[i].gas_index);
						if(!(data[i].status & BME68X_HEAT_STAB_MSK)){
							gas_index_name += "_nhs";
						}
						user_data[gas_index_name.c_str()] = data[i].gas_resistance;

						LOG_DBG("sample:%d (%u/%d) : %.2f° , %.2f Pa , %.2f %% , %s %.2f ohm",
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
			LOG_INF("n_fields=%d;",n_fields);
		}
	}
}
