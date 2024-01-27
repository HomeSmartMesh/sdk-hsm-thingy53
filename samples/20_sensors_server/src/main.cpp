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

#define LED_PWM_RATIO 0.5
#define SLEEP_DELAY_SEC 20

const struct device *sensor_dev_bh1749;
const struct device *sensor_dev_bme688;
char uid_text[20];


void init_uid(char*text){
	long unsigned int id0 = NRF_FICR->INFO.DEVICEID[0];//just for type casting and readable printing
	long unsigned int id1 = NRF_FICR->INFO.DEVICEID[1];
	sprintf(text,"%08lX%08lX",id0,id1);
	LOG_INF("Device ID:%s",text);
}

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
	static int subsample_count = 0;
	subsample_count++;
	if((subsample_count%10) == 0){
		std::string topic = "thread_tags/"+std::string(uid_text)+"/env";
		send_data(topic.c_str(),data);
	}
}

void led_notify_ot_role(){
		otDeviceRole role = ot_app_role();
		if(role == OT_DEVICE_ROLE_DISABLED){
			app_led_blink_red(LED_PWM_RATIO,300,900);
			app_led_blink_red(LED_PWM_RATIO,300,900);
			app_led_blink_red(LED_PWM_RATIO,300,900);
			LOG_INF("role: Disabled");
		}else if(role == OT_DEVICE_ROLE_DETACHED){
			app_led_blink_blue(LED_PWM_RATIO,300,600);
			app_led_blink_blue(LED_PWM_RATIO,300,600);
			LOG_INF("role: Detached");
		}else if(role == OT_DEVICE_ROLE_CHILD){
			app_led_blink_green(LED_PWM_RATIO,400,600);
			LOG_INF("role: Child");
		}else if(role == OT_DEVICE_ROLE_ROUTER){
			app_led_blink_color(0.01,0.89,0.87,400,600);//#03E2DD
			LOG_INF("role: Router");
		}else if(role == OT_DEVICE_ROLE_LEADER){
			app_led_blink_color(0.49,0.46,0.97,400,600);//#7E77F8
			LOG_INF("role: Leader");
		}else{
			app_led_blink_red(LED_PWM_RATIO,100,100);
			app_led_blink_red(LED_PWM_RATIO,100,100);
			app_led_blink_red(LED_PWM_RATIO,100,100);
			LOG_INF("role: Unknown");
		}
}

int main(void)
{
	LOG_INF(R"(
	main(1)
	sensors server sample
	)");

	init_uid(uid_text);
	app_ot_init();//logs joiner info and initializes reset buttons
	app_led_init();
	app_battery_init();
	app_bh1749_init();
	set_endpoint_handler(json_endpoint_handler);
	start_bme688(bme688_handler);

	app_led_blink_red(LED_PWM_RATIO,300,100);
	app_led_blink_blue(LED_PWM_RATIO,300,100);
	app_led_blink_green(LED_PWM_RATIO,300,100);

	int count = 0;
	std::string topic;
	while(1){
		json data;
		
		//state
		data["alive"] = count++;
		char voltage[10];
		app_battery_voltage_text(voltage);
		bool is_charging = app_battery_charging();
		data["voltage"] = voltage;
		data["charging"] = is_charging;
		topic = "thread_tags/"+std::string(uid_text)+"/state";
		send_data(topic.c_str(),data);
		data = {};
		k_sleep(K_MSEC(5000));

		//light
		int32_t r,g,b,ir;
		bh1749_get_values(r,g,b,ir);
		data["light_red"] = r;
		data["light_green"] = g;
		data["light_blue"] = b;
		data["light_ir"] = ir;
		topic = "thread_tags/"+std::string(uid_text)+"/light";
		send_data(topic.c_str(),data);
		data = {};

		led_notify_ot_role();
		k_sleep(K_MSEC(SLEEP_DELAY_SEC*1000));
	}
}
