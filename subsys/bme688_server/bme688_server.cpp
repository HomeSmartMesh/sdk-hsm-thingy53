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

LOG_MODULE_REGISTER(bme688_server, LOG_LEVEL_DBG);

#define BME688_SERVICE_STACK_SIZE 8192
#define BME688_SERVICE_PRIORITY 30
#define BME688_SERVICE_START_DELAY_MS 100

json user_data = nullptr;
int measure_count = 0;
uint8_t last_index = 1;

#if defined(CONFIG_BME688_BSEC2)
iaq_output_t iaq_output;
void bme688_service_bsec2();
K_THREAD_DEFINE(	bme688_thread, BME688_SERVICE_STACK_SIZE, bme688_service_bsec2, NULL, NULL, NULL,
					BME688_SERVICE_PRIORITY, 0, BME688_SERVICE_START_DELAY_MS);
#else
void bme688_service();
K_THREAD_DEFINE(	bme688_thread, BME688_SERVICE_STACK_SIZE, bme688_service, NULL, NULL, NULL,
					BME688_SERVICE_PRIORITY, 0, BME688_SERVICE_START_DELAY_MS);
#endif




const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bme688);
static bme688_handler_t m_bme688_server_handler = NULL;
static bool started = false;
bme688_mode_t bme688_mode = bme688_mode_sleep;//bme688_mode_forced, bme688_mode_parallel, bme688_mode_sequencial

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

void start_bme688(bme688_handler_t handler){
	LOG_INF("start_bme688()");

	if (!device_is_ready(dev)) {
		printk("sensor: device not ready.\n");
		return;
	}
	LOG_INF("Sensor device %p name is %s\n", dev, dev->name);

	bme688_init(dev);

	#if defined(CONFIG_BME688_BSEC2)
	bsec2_start();
	#else
	bme688_mode_t mode = bme688_mode_parallel;//bme688_mode_forced, bme688_mode_parallel, bme688_mode_sequencial
	bme688_set_mode(mode);
	LOG_INF("bme688 set to Parallel mode\n");
	#endif

	LOG_INF("set_bme688_handler()");
	m_bme688_server_handler = handler;

	started = true;
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

#if defined(CONFIG_BME688_BSEC2)

void set_mode(bsec_bme_settings_t &conf){
	switch (conf.op_mode)
	{
	case BME68X_FORCED_MODE:
		bme688_mode = bme688_mode_forced;
		LOG_INF("bme688_set_mode(Forced)");
		break;
	case BME68X_PARALLEL_MODE:
		if(bme688_mode != bme688_mode_parallel){
			bme688_mode = bme688_mode_parallel;
			LOG_INF("bme688_set_mode(Parallel)");
		}
		break;
	case BME68X_SLEEP_MODE:
		if(bme688_mode != bme688_mode_sleep){
			bme688_mode = bme688_mode_sleep;
			LOG_INF("bme688_set_mode(Sleep)");
		}
		break;
	default:
		LOG_INF("bme688_set_mode(Other) %u",bme688_mode);
		break;
	}
	bme688_set_mode(bme688_mode);
}

void update_user_data(const struct bme68x_data &data){
	user_data["sample"] = measure_count;
	user_data["temperature"] = data.temperature;
	user_data["humidity"] = data.humidity;
	user_data["pressure"] = data.pressure;

	std::string gas_index_name = "gas";
	gas_index_name += std::to_string(data.gas_index);
	if(!(data.status & BME68X_HEAT_STAB_MSK)){
		gas_index_name += "_nhs";
	}
	user_data[gas_index_name.c_str()] = data.gas_resistance;

	LOG_DBG("sample:%d (%u/%d) : %.2f° , %.2f Pa , %.2f %% , %s %.2f ohm",
		measure_count,
		data.meas_index,
		data.gas_index,
		data.temperature,
		data.pressure,
		data.humidity,
		(data.status & BME68X_HEAT_STAB_MSK)?"":"(nhs)",
		data.gas_resistance);

}

void measure_process(bsec_bme_settings_t &conf){
	struct bme68x_data data[3];

	bme688_wait_for_measure();
	const int64_t time_stamp = k_ticks_to_ns_near64(k_uptime_ticks());
	uint8_t n_fields = bme688_data_get(dev, data);
	LOG_INF("n_fields=%d;",n_fields);
	if(n_fields != 0){
		for(uint8_t i = 0; i < n_fields; i++){
			LOG_INF("   data %u ; status 0x%x ; index %d",i,data[i].status,data[i].gas_index);
			if(data[i].gas_index == 0){
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
					update_user_data(data[i]);
					if(processData(data[i],iaq_output,time_stamp)){
						user_data["iaq"] = iaq_output.iaq;
						user_data["iaq_accuracy"] = iaq_output.iaq_accuracy;
						user_data["co2_eq"] = iaq_output.co2_eq;
						user_data["breath_voc"] = iaq_output.breath_voc;
						user_data["stabilization"] = iaq_output.stabilization;
						user_data["runin"] = iaq_output.runin_status;
					}
			}
		}
	}
}

void bme688_service_bsec2(){
	LOG_INF("hello from bme688_service_bsec2()");
	//#define BME68X_NEW_DATA_MSK                       UINT8_C(0x80)
	//#define BME68X_GASM_VALID_MSK                     UINT8_C(0x20)
	//#define BME68X_HEAT_STAB_MSK                      UINT8_C(0x10)
	LOG_DBG("0xA0 => nhs: No Heat Stability");
	LOG_DBG("sample:count (meas index/gas index) : Temperature°, Pressure Pa, Humidity %%, 'Gas resistance ohm'");
	while(!started){
			k_sleep(K_MSEC(100));
	}
	bsec_bme_settings_t conf;
	bsec2_get_conf(conf);
	while(true){
		//LOG_INF("   ---   bme688_service_bsec2() loop   ---");
		set_mode(conf);

        if (conf.trigger_measurement && conf.op_mode != BME68X_SLEEP_MODE)
        {
			measure_process(conf);
		}
		//sleep until next_call
		const int64_t currTimeNs = k_ticks_to_ns_near64(k_uptime_ticks());
		if(currTimeNs < conf.next_call){
			const int64_t sleep_time_us = (conf.next_call - currTimeNs) / 1000;
	    	LOG_INF("next call in %" PRId64 " seconds",(sleep_time_us/1000000));
			k_usleep(sleep_time_us);
		}
		bsec2_get_conf(conf);
	}

}

#endif /* CONFIG_BME688_BSEC2 */