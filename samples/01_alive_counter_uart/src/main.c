#include <zephyr/kernel.h>
#include <stdio.h>

void main(void)
{
	uint32_t count = 0;

	printf("Hello from counter\n");

	while (1) {
		k_sleep(K_MSEC(3000));

		printf("alive counter : %" PRIu32 "\n",count);
		count++;
	}
}
