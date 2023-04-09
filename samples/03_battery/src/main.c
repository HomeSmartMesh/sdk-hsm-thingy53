#include <zephyr/kernel.h>
#include <stdio.h>
#include "app_battery.h"

void main(void)
{
	uint32_t count = 0;

	printf("Hello from battery sample\n");
	app_battery_init();

	while (1) {
		k_msleep(5000);
		int32_t voltage = app_battery_voltage_mv();
		bool is_charging = app_battery_charging();
		printf("battery voltage = %" PRIi32 " mV ; %s\n",voltage,is_charging?"charging":"not charging");
		printf("battery counter : %" PRIu32 "\n",count);
		count++;
	}
}
