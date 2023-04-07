# sdk-hsm-thingy53
Home Automation Software-Development-Kit from Home-Smart-Mesh for the Nordic Thingy53 dev kit

# usage
```bash
mkdir thingy53
cd thingy53
>west init -m https://github.com/HomeSmartMesh/sdk-hsm-thingy53 --mr main
>west update
cd thingy53/hsm/samples/bme680
>west build -b thingy53_nrf5340_cpuapp
```
* connect to USB
* power on while holding SW2 down, see details on [updating thingy53 through USB](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/thingy53_gs.html#updating-through-usb)
* with nRFConnect Programmer flash `hsm\samples\bme680\build\zephyr\dfu_application.zip`

# deveopment guide
This repository is a [Zephyr workspace application](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/application/index.html#zephyr-workspace-application) that contains the samples source code only therefore lightweight to clone and manage, yet it tracks an exact reference of all dependencies that get deployed once initialized with `west init`

![Dependencies](./design/dependencies.drawio.svg)

## how was this repo created
* step one is to get familiar with Zephyr a good reference is https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/index.html
* second is to focus on the subsection for west https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/develop/west/index.html

This repository is targetting an nRF dev kit, thereforeit is safer to derive it from nRF's fork of Zephyr and other dependencies. the steps are :
* west.yml dependencies taken from https://github.com/nrfconnect/sdk-nrf/blob/main/west.yml
* the default remote is hsm instead of ncs, therefore in the `projects:` the remote ncs needs to be added where applicable
* in the application `zephyr`, the `name-allowlist` helps reduce the dependencies from Zephyr
* note also some Zephyr dependencies can be replaced with top level projects such as mbedtls which is then taken from nRF fork and not from Zephyr
* in case of Kconfig wanrings, it is necessary to compare with the original repo (ncs/nrf) and find the directory (dependency) where that flag is used e.g. missing config for `NRF_MODEM_LIB_SHMEM_CTRL_SIZE` which default is `NRF_MODEM_SHMEM_CTRL_SIZE` defined in `nrfxlib\nrf_modem\Kconfig` that shows a dependency from `nrfconnect/nrf` to `nrfconnect/nrfxlib`

