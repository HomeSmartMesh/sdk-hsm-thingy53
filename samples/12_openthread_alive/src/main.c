/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <zephyr/sys/reboot.h>
#include <openthread/thread.h>
#include <openthread/instance.h>
#include "udp_client.h"
#include "app_button.h"

otInstance* instance;

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread main()\n\r" \
	"cli ready\n\r"

void click(void){
	printk("button - click - rebooting warm\n");
	sys_reboot(SYS_REBOOT_WARM);
}

void long_press(){
	printk("button - long press - OpenThread Factory Reset\n");
	otInstanceFactoryReset(instance);
	sys_reboot(SYS_REBOOT_COLD);
}

void main(void)
{
	instance = otInstanceInitSingle();
	LOG_INF(WELLCOME_TEXT);

	app_button_init();
	app_button_set_short_callback(click);
	app_button_set_long_callback(long_press);
	app_button_set_short_timeout(1000);
	app_button_set_long_timeout(4000);

	int count = 0;
	while(1){
		char message[250];
		int size = sprintf(message,"thread_thingy_53/{\"alive\":%d}",count);
		send_udp(message, size);
		count++;

		LOG_INF("sleeping 10 sec");
		k_sleep(K_MSEC(10000));
	}
}
