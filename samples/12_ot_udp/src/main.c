/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include "udp_client.h"

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n"\
	"Hello from 'openthread_udp' sample\n"\
	"OpenThread main()\n" \
	"cli ready\n"

void main(void)
{
	LOG_INF(WELLCOME_TEXT);

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
