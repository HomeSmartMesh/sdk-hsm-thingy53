#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>

#include "flash_settings_storage.h"

#define SETTINGS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(settings_storage)
#define SETTINGS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(settings_storage)
#define FLASH_PAGE_SIZE 4096

// SETTINGS_PARTITION_OFFSET 0xF0000 is using ~ 310 bytes for thread credentials
// last block 0xFF000 is not used
#define SETTINGS_BSEC2_OFFSET (SETTINGS_PARTITION_OFFSET + 0xF000)

static const struct device *flash_dev = SETTINGS_PARTITION_DEVICE;

void fss_write_data(uint8_t *data,uint16_t size){
	if (flash_erase(flash_dev, SETTINGS_BSEC2_OFFSET, FLASH_PAGE_SIZE) != 0) {
		printf("   Flash erase failed!\n");
	} else {
		printf(" * Succeeded Flash erase partition 'settings_storage' @0x%x 4KB Page!\n",SETTINGS_BSEC2_OFFSET);
	}
    off_t offset = SETTINGS_BSEC2_OFFSET;
    if (flash_write(flash_dev, offset, data,size) != 0) {
        printf("   Flash write failed!\n");
        return;
    }
}

void fss_read_data(uint8_t *data,uint16_t size){
	if (!device_is_ready(flash_dev)) {
		printf("Flash device not ready\n");
		return;
	}

    const off_t offset = SETTINGS_BSEC2_OFFSET;
    printf(" * reading from 'settings_storage' 0x%lx",offset);
    if (flash_read(flash_dev, offset, data,size) != 0){
        printf("   Flash read failed!\n");
        return;
    }
}
