This sample is a config only that should be added as overlay to `nrf\samples\openthread\cli\` then built with the following command

```shell
west build -p always -b nrf52840dongle_nrf52840 -- "-DOVERLAY_CONFIG=overlay-usb.conf;overlay-joiner-rounter.conf" -DDTC_OVERLAY_FILE="usb.overlay"
nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 build/zephyr/zephyr.zip
nrfutil dfu usb-serial -pkg build/zephyr/zephyr.zip -p COM9
```

then allow it to join on the boarder router
```shell
sudo ot-ctl
commissioner start
commissioner joiner add * ABCDE2
```