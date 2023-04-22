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
using json = nlohmann::json;

LOG_MODULE_REGISTER(sensor_server_sample, CONFIG_SONSORS_SERVER_LOG_LEVEL);

json data;
const struct device *sensor_dev_bh1749;

void bh1749_init(){
	sensor_dev_bh1749 = DEVICE_DT_GET_ONE(rohm_bh1749);
	if (!device_is_ready(sensor_dev_bh1749)) {
		printk("Sensor device not ready\n");
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

int main(void)
{
	LOG_INF(R"(
	main()
	sensors server sample
	)");

	app_ot_init();//logs joiner info and initializes reset buttons
	app_led_init();
	app_battery_init();
	bh1749_init();
	set_endpoint_handler(json_endpoint_handler);

	app_led_blink_green(0.1,500,1000);

	int count = 0;
	while(1){
		data["alive"] = count++;

		//battery
		int32_t voltage = app_battery_voltage_mv();
		bool is_charging = app_battery_charging();
		data["battery"]["mv"] = voltage;
		data["battery"]["charging"] = is_charging;

		//light
		int32_t r,g,b,ir;
		bh1749_get_values(r,g,b,ir);
		data["light"]["r"] = r;
		data["light"]["g"] = g;
		data["light"]["b"] = b;
		data["light"]["ir"] = ir;

		std::string message = "thingy_53/"+data.dump();
		send_udp(message);

		LOG_INF("%s",message.c_str());
		LOG_INF("sleeping 10 sec");
		k_sleep(K_MSEC(10000));
	}
}
