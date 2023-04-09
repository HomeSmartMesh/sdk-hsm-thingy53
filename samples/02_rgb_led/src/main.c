#include <zephyr/kernel.h>
#include <stdio.h>
# include "app_leds.h"

void main(void)
{
	uint32_t count = 0;

	app_leds_init();

	printf("Hello from led counter\n");
	while (1) {
		printf("red\n");

		//blink with basic API
		app_led_set_red(0.1);
		k_msleep(300);
		app_led_off();
		k_msleep(1000);

		//blink with blink API
		app_led_blink_red(0.1,300,1000);

		printf("green\n");
		app_led_blink_green(0.1,300,1000);
		printf("blue\n");
		app_led_blink_blue(0.1,300,1000);
		printf("color\n");
		app_led_blink_color(0.001,0.2,0.3,300,3000);
		printf("led counter : %" PRIu32 "\n",count);
		count++;
	}
}
