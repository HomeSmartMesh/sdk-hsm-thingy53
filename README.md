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
* connect to USB
* power on while holding SW2 down, see details on [updating thingy53 through USB](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/thingy53_gs.html#updating-through-usb)
* with nRFConnect Programmer flash `hsm\samples\bme680\build\zephyr\dfu_application.zip`

# Samples
for convenience and given that this repo is providing samples for `thingy53_nrf5340_cpuapp` board, it has been configured in the CMakeLists.txt to be taken as default board, it is still possbile to override it with -b option.
## 01_BME680
* This is the default [BME680 Zephyr sample](https://docs.zephyrproject.org/latest/samples/sensor/bme680/README.html) from Nordic's fork [nRF BME680 Zephyr sample](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/samples/sensor/bme680/README.html)
* this samples runs as is without modifications on the thingy53 board thanks to the proper device drivers declaration `bosch,bme680` in the thingy53 dts file `thingy53_nrf5340_common.dts`
* the sensor used is the [Bosch-sensortec BME680](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/) used for
    * Temperature
    * Air pressure
    * Humidity
    * Air quality Gas Sensor

build
```shell
cd thingy53/hsm/samples/01_bme680
>west build
```

output log
```
Device 0x20002b74 name is BME680
T: 23.988877; P: 97.648568; H: 53.689533; G: 1035.211466
T: 24.168500; P: 97.648866; H: 53.565966; G: 1046.677896
```
## 11_openthread_shell
* provides a shell on the USB UART interface that allows to manually configure the openthread stack

build
```shell
cd thingy53/hsm/samples/11_openthread_shell
>west build
```
## 12_openthread_alive
* automatically configures openthread credentials 
* loops sending alive messages as thread udp packets
* optionally takes an `overlay-logging.conf` for openthread and loop info
* default is using joiner PSKD `ABCDE2`
* using `prj-fixed-credentials.conf` allows to hard-code network credentials for testing only (not suited for deployment), even when used for local deployments it is unpractical as the device needs to be flashed everytime the network parameters change

build
```shell
cd thingy53/hsm/samples/12_openthread_shell
>west build -- -DOVERLAY_CONFIG="overlay-logging.conf"
```
Note on joining:
* the `eui64` can be known by first flashing the shell sample
* when flashing this sample only, the `eui64` is considered to be unknown and joining would have to use '*' as `eui64` parameter for the commissionner

# Updates
## Thread robustness
* join retry timeout
* join retry with restart or function call
* watchdog restart
* factory reset on long button press
* restart on button press

# Deveopment guide
This repository is a [Zephyr workspace application](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/application/index.html#zephyr-workspace-application) that contains the samples source code only therefore lightweight to clone and manage, yet it tracks an exact reference of all dependencies that get deployed once initialized with `west init`

![Dependencies](./design/dependencies.drawio.svg)

## How was this repo created
* step one is to get familiar with Zephyr a good reference is https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/index.html
* second is to focus on the subsection for west https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/west/index.html

This repository is targetting an nRF dev kit, thereforeit is safer to derive it from nRF's fork of Zephyr and other dependencies. the steps are :
* west.yml dependencies taken from https://github.com/nrfconnect/sdk-nrf/blob/main/west.yml
* the default remote is hsm instead of ncs, therefore in the `projects:` the remote ncs needs to be added where applicable
* in the application `zephyr`, the `name-allowlist` helps reduce the dependencies from Zephyr
* note also some Zephyr dependencies can be replaced with top level projects such as mbedtls which is then taken from nRF fork and not from Zephyr
* in case of Kconfig wanrings, it is necessary to compare with the original repo (ncs/nrf) and find the directory (dependency) where that flag is used e.g. missing config for `NRF_MODEM_LIB_SHMEM_CTRL_SIZE` which default is `NRF_MODEM_SHMEM_CTRL_SIZE` defined in `nrfxlib\nrf_modem\Kconfig` that shows a dependency from `nrfconnect/nrf` to `nrfconnect/nrfxlib`
