# Features
* Full Thread Device : Thread router, range extender
* Auto joiner on startup with PSKD "ABCDE2"
* Power Tx +8 dbM
* RGBLed notifications
    * startup : Red, Blue, green
    * cyclic every 20 sec notifies Openthread State
        * Disabled : x3 Red
        * Detached : x2 Blue
        * Child : x1 Green
        * Router : x1 Cyan
        * Leader : x2 Violet
* User button press
    * short press < 1sec : Reset
    * Long press > 3 sec : Openthread factory reset
* CLI : when connected to a USB Host, a command line interface is available with "ot" commands and others

This repo provides a binary for dfu flashing

```shell
nrfutil dfu usb-serial -pkg flash/zephyr.zip -p COM9
```

# developer guide
This sample is a config only that should be added as overlay to `nrf\samples\openthread\cli\` then built with the following command

```shell
west build -p always -b nrf52840dongle_nrf52840 -- "-DOVERLAY_CONFIG=overlay-usb.conf;overlay-joiner.conf" -DDTC_OVERLAY_FILE="usb.overlay"
nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 build/zephyr/zephyr.zip
nrfutil dfu usb-serial -pkg build/zephyr/zephyr.zip -p COM9
```

then allow it to join on the boarder router
```shell
sudo ot-ctl
commissioner start
commissioner joiner add * ABCDE2
```

or on a cli dongle as explained in https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/protocols/thread/overview/commissioning.html#setting-up-the-commissioner

```shell
ot dataset init new
ot dataset channel 16
ot dataset networkkey 00112233445566778899aabbccddeeff
ot dataset commit active
ot ifconfig up
ot thread start
```
