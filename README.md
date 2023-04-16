# sdk-hsm-thingy53
Home Automation Software-Development-Kit from Home-Smart-Mesh for the Nordic Thingy53 dev kit

![USB Attachments](./design/thingy53-usb-attachments.webp)

Hardware
* Nordic's [Thingy53](https://www.nordicsemi.com/Products/Development-hardware/Nordic-Thingy-53) IoT Prototyping platform
* Segger's [j-Link Edu mini](https://www.segger.com/products/debug-probes/j-link/models/j-link-edu-mini/) (optional)


# Usage
```bash
mkdir thingy53
cd thingy53
>west init -m https://github.com/HomeSmartMesh/sdk-hsm-thingy53 --mr main
>west update
```
## building a sample
```bash
>cd hsm/samples/12_openthread_alive
>west build
```
## flashing
flashing using an attached debugger
```
>west flash
```

flashing manually
* connect to USB
* power on while holding SW2 down, see details on [updating thingy53 through USB](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/thingy53_gs.html#updating-through-usb)
* with nRFConnect Programmer flash `hsm\samples\12_openthread_alive\build\zephyr\dfu_application.zip`

Serial Port
* USB Serial Port : with the config `CONFIG_STDOUT_CONSOLE=y` this board creates a UAB virtual COM port of stdout. Note using the nRFSDK Connect Serial Terminal allows auto detection and reconnect of serial ports, very useful to recover automatically after flashing
* RTT Debugger Serial Port : using `CONFIG_USE_SEGGER_RTT=y` it is possible to have logs with the attached debugger and without using the board USB device, this needs rebuild the sample differently though.

# Samples
for convenience and given that this repo is providing samples for `thingy53_nrf5340_cpuapp` board, it has been configured in the CMakeLists.txt to be taken as default board, it is still possbile to override it with -b option.
## 01_alive_counter_uart
simplest program for checking UART with a live counter
## 02_rgb_led
controlling the colors of the Red Green and Blue LEDs with pwm
* function for setting x3 float colors
* function for blinking a color

## 03_battery

* Power Management Integrated Circuit specification [nPM1100_PS_v1.3.pdf](https://infocenter.nordicsemi.com/pdf/nPM1100_PS_v1.3.pdf)
* Thingy53 schematics `PCA20053_Schematic_And_PCB.pdf`

used pios
* Battery Measure ENABLE P0.16 `vbatt.power-gpio` in `thingy53_nrf5340_common.dts`
* Battery Measure BAT_MEAS P0.06/AIN2 `vbatt.io-channels` in `thingy53_nrf5340_common.dts`
* Battery charging Indicator CHG PMIC_STATUS P1.00 `battery-charge-gpios` in `app.overlay`

## 04_BME680
* Although the board is equipped with BM688, this sample only uses the common features with the default [BME680 Zephyr sample](https://docs.zephyrproject.org/latest/samples/sensor/bme680/README.html) from Nordic's fork [nRF BME680 Zephyr sample](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/samples/sensor/bme680/README.html)
* this samples runs as is without modifications on the thingy53 board thanks to the proper device drivers declaration `bosch,bme680` in the thingy53 dts file `thingy53_nrf5340_common.dts`
* the sensor used is the [Bosch-sensortec BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/) used for
    * Temperature
    * Air pressure
    * Humidity
    * Air quality Gas Sensor

output log
```
Device 0x20002b74 name is BME680
T: 23.988877; P: 97.648568; H: 53.689533; G: 1035.211466
T: 24.168500; P: 97.648866; H: 53.565966; G: 1046.677896
```

## 05_bh1749
* Color sensor BH1749NUC
    * [BH1749NUC Datasheet](https://fscdn.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1749nuc-e.pdf)
    * Measures Red, Green, Blue and IR
    * Illuminance Detection Range 80 klx (0.0125 lux/count)



## 11_openthread_shell
* provides a shell on the USB UART interface that allows to manually configure the openthread stack

build
```shell
cd thingy53/hsm/samples/11_openthread_shell
>west build
```

## 12_ot_udp
* using a fixed openthread network config allows to hard-code network credentials for testing only (not suited for deployment), even when used for local deployments it is unpractical as the device needs to be flashed everytime the network parameters change
* loops sending alive counter messages as thread udp packets
* `overlay-logging.conf` uses RTT and USB log for openthread state and loop count

build options
```shell
>west build
>west build -- -DOVERLAY_CONFIG="overlay-logging.conf"
```

## 13_ot_joiner
* Commissioning with a joiner PSKd (Pre-Shared Key for the Device) `ABCDE2`
    * needs the commissioner to be ready for this device
* short SW2 button press < 1 sec : soft reset `SYS_REBOOT_WARM`
    * will retry joining if not attached
* long SW2 button press > 1 sec : OpenThread Farctory reset (delete credentials) and `SYS_REBOOT_COLD`
    * will try joining a new network
* `overlay-logging.conf` uses RTT and USB log and prints the following on startup
    * Joiner `eui64`
    * Joiner `pskd` built with in the provided config
    * the qrcode text containing the `eui64` and `pskd` as parameters
    * a url to a generated qrcode image to be used for joining
* loops sending alive counter messages as thread udp packets

build options
```shell
>west build
>west build -- -DOVERLAY_CONFIG="overlay-logging.conf"
```

Note on joining:
* the `eui64` can be known by first flashing the logging version with `overlay-logging.conf`
* without knowing the `eui64` it is also possible to commission with '*' as `eui64` parameter

# 14_ot_udp_echo_server
* separate `udp_rx_handler` thread
* binds to port 4242 and echoes back received characters (printed as text)


# Updates
* MQTT publish sample
* reliable ot tx rx, e.g. tcp, websocket, session,... for packet request response
* main cpp and json for structured request response
* adopt url pattern (instead of mqtt topic) for endpoints
* Python MQTT Translator for request response with json bodies (no CoAP)
* RGB Led notification service
* sensors json logger service (configurable sensors and rates)
* ot lifecycle, if not connected, restart after timeout, which will retry join
* system lifecycle, watchdog restart
* power measurements
* microhpone streaming
* microphone local

# Deveopment guide
This repository is a [Zephyr workspace application](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/application/index.html#zephyr-workspace-application) that contains the samples source code only therefore lightweight to clone and manage, yet it tracks an exact reference of all dependencies that get deployed once initialized with `west init`

![Dependencies](./design/dependencies.drawio.svg)

## Hints and Tricks
* k_sleep in interrupt functions might lead to os crash, usage of LOG effect unclear so to be avoided
* Fifo, Lifo, Queue take pointers only, user allocates the data but there's also `k_fifo_alloc_put`
* Message Queues copy the data provided in a ring buffer, (overwrite oldest)
* Work queues are used to delay functions execution or order multiple functions execution


## How was this repo created
* step one is to get familiar with Zephyr a good reference is https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/index.html
* second is to focus on the subsection for west https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/west/index.html

This repository is targetting an nRF dev kit, thereforeit is safer to derive it from nRF's fork of Zephyr and other dependencies. the steps are :
* west.yml dependencies taken from https://github.com/nrfconnect/sdk-nrf/blob/main/west.yml
* the default remote is hsm instead of ncs, therefore in the `projects:` the remote ncs needs to be added where applicable
* in the application `zephyr`, the `name-allowlist` helps reduce the dependencies from Zephyr
* note also some Zephyr dependencies can be replaced with top level projects such as mbedtls which is then taken from nRF fork and not from Zephyr
* in case of Kconfig wanrings, it is necessary to compare with the original repo (ncs/nrf) and find the directory (dependency) where that flag is used e.g. missing config for `NRF_MODEM_LIB_SHMEM_CTRL_SIZE` which default is `NRF_MODEM_SHMEM_CTRL_SIZE` defined in `nrfxlib\nrf_modem\Kconfig` that shows a dependency from `nrfconnect/nrf` to `nrfconnect/nrfxlib`

## Zephyr references
* Kernel services : https://docs.zephyrproject.org/latest/kernel/services/index.html
