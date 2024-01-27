/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>

#include "app_led.h"
#include "app_ot.h"


#define LED_PWM_RATIO 0.5
#define SLEEP_CYCLE_SEC 10U

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

const struct device *usb_dev;
int init_usb_uart()
{
	int ret;

	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return 0;
	}

	usb_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
	if (usb_dev == NULL) {
		LOG_ERR("Failed to find specific UART device");
		return 0;
	}

	LOG_INF("Waiting for host to be ready to communicate");

	k_sleep(K_MSEC(500));
	/* Data Carrier Detect Modem - mark connection as established */
	(void)uart_line_ctrl_set(usb_dev, UART_LINE_CTRL_DCD, 1);
	/* Data Set Ready - the NCP SoC is ready to communicate */
	(void)uart_line_ctrl_set(usb_dev, UART_LINE_CTRL_DSR, 1);

	return 0;
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
	app_led_init();
	app_led_blink_red(LED_PWM_RATIO,300,100);
	app_led_blink_blue(LED_PWM_RATIO,300,100);
	app_led_blink_green(LED_PWM_RATIO,300,100);

	app_ot_init();//short long press for reset and factory reset

	init_usb_uart();
	k_sleep(K_MSEC(1000));

	while(true){
		led_notify_ot_role();
		k_sleep(K_MSEC(SLEEP_CYCLE_SEC*1000));
	}

	return 0;
}
