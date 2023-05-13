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

LOG_MODULE_REGISTER(bme688_server, LOG_LEVEL_INF);

#define BME688_SERVICE_STACK_SIZE 8192
#define BME688_SERVICE_PRIORITY 30
#define BME688_SERVICE_START_DELAY_MS 100

json data;

void bme688_service();

K_THREAD_DEFINE(	bme688_thread, BME688_SERVICE_STACK_SIZE, bme688_service, NULL, NULL, NULL,
					BME688_SERVICE_PRIORITY, 0, BME688_SERVICE_START_DELAY_MS);

static bme688_handler_t m_bme688_server_handler = NULL;

void set_bme688_config(json &config){
	std::string text = config.dump(4);
	LOG_INF("set_bme688_config() OK");
	LOG_INF("%s",text.c_str());
}

void set_bme688_handler(bme688_handler_t handler)
{
	m_bme688_server_handler = handler;
}

void start_bme688(){
	LOG_INF("start_bme688()");
}

void bme688_service(){

	LOG_INF("hello from bme688_service()");
    start_bme688();

	while(true){
		LOG_INF("bme688_service>loop");
		data["test"] = "OK";
		if(m_bme688_server_handler != NULL){
			m_bme688_server_handler(data);
		}
		k_sleep(K_MSEC(10000));
	}
}
