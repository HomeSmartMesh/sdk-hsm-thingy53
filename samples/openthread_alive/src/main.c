/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>

LOG_MODULE_REGISTER(cli_sample, CONFIG_OT_COMMAND_LINE_INTERFACE_LOG_LEVEL);

#define WELLCOME_TEXT \
	"\n\r"\
	"\n\r"\
	"OpenThread main()\n\r" \
	"cli ready\n\r"

void main(void)
{
	LOG_INF(WELLCOME_TEXT);

	while(1){
		LOG_INF("sleeping 10 sec");
		k_sleep(K_MSEC(10000));
	}

}
