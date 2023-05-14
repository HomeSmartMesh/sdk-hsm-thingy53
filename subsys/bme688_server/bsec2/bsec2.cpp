#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "bsec2.h"
#include "bsec_interface.h"
#include "bsec_datatypes.h"

LOG_MODULE_REGISTER(bme688_bsec2, LOG_LEVEL_INF);

void bsec2_start(){
	LOG_INF("bsec2_start()");

    bsec_library_return_t res = bsec_init();
	LOG_INF("bsec_init() => %d",res);
}
