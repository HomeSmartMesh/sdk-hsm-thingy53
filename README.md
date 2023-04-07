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
