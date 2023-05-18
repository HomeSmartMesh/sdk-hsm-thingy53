/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include "json_endpoint.h"
#include "app_ot.h"
#include "app_led.h"
#include "app_battery.h"
#include "udp_broadcast.h"
#include <json.hpp>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include "bme688_server.h"


using json = nlohmann::json;

LOG_MODULE_REGISTER(sensor_server_sample, CONFIG_SONSORS_SERVER_LOG_LEVEL);

const struct device *sensor_dev_bh1749;
const struct device *sensor_dev_bme688;

void send_data(std::string topic,json &data){
	std::string message = topic+data.dump();
	send_udp(message);
	LOG_INF("send_data> %s",message.c_str());
}

void app_bh1749_init(){
	sensor_dev_bh1749 = DEVICE_DT_GET_ONE(rohm_bh1749);
	if (!device_is_ready(sensor_dev_bh1749)) {
		printk("Sensor device bh1749 not ready\n");
		return;
	}
}

void bh1749_get_values(int32_t &r,int32_t &g,int32_t &b,int32_t &ir){
	int ret;
	struct sensor_value temp_val;
	ret = sensor_sample_fetch_chan(sensor_dev_bh1749, SENSOR_CHAN_ALL);
	/* The sensor does only support fetching SENSOR_CHAN_ALL */
	if (ret) {
		printk("sensor_sample_fetch failed ret %d\n", ret);
		return;
	}

	ret = sensor_channel_get(sensor_dev_bh1749, SENSOR_CHAN_RED, &temp_val);
	if (ret) {
		printk("sensor_channel_get failed ret %d\n", ret);
		return;
	}
	r = temp_val.val1;
	ret = sensor_channel_get(sensor_dev_bh1749, SENSOR_CHAN_GREEN, &temp_val);
	if (ret) {
		printk("sensor_channel_get failed ret %d\n", ret);
		return;
	}
	g = temp_val.val1;

	ret = sensor_channel_get(sensor_dev_bh1749, SENSOR_CHAN_BLUE, &temp_val);
	if (ret) {
		printk("sensor_channel_get failed ret %d\n", ret);
		return;
	}
	b = temp_val.val1;

	ret = sensor_channel_get(sensor_dev_bh1749, SENSOR_CHAN_IR, &temp_val);
	if (ret) {
		printk("sensor_channel_get failed ret %d\n", ret);
		return;
	}
	ir = temp_val.val1;
}

void json_endpoint_handler(std::string &client, std::string &topic, json &request, json &response){
	LOG_INF("json_endpoint_handler()");
	response = request;
	response["result"] = "OK";
}

void bme688_handler(json &data){
	send_data("thingy_53/tester/env",data);
}

int main(void)
{
	LOG_INF(R"(
	main()
	sensors server sample
	)");

	app_ot_init();//logs joiner info and initializes reset buttons
	app_led_init();
	app_battery_init();
	app_bh1749_init();
	set_endpoint_handler(json_endpoint_handler);
	start_bme688(bme688_handler);

	app_led_blink_green(0.1,500,1000);

	int count = 0;
	while(1){
		json data;
		
		//state
		data["alive"] = count++;
		int32_t voltage = app_battery_voltage_mv();
		bool is_charging = app_battery_charging();
		data["voltage"] = voltage;
		data["charging"] = is_charging;
		send_data("thingy_53/tester/state",data);
		data = {};
		k_sleep(K_MSEC(5000));

		//light
		int32_t r,g,b,ir;
		bh1749_get_values(r,g,b,ir);
		data["light_red"] = r;
		data["light_green"] = g;
		data["light_blue"] = b;
		data["light_ir"] = ir;
		send_data("thingy_53/tester/light",data);
		data = {};

		app_led_blink_blue(0.08,100,0);

		k_sleep(K_MSEC(5000));
	}
}
