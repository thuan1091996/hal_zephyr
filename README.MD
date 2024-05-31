# HAL for nRF52 with ZephyrRTOS

## Build commands
```
# nRF52840DK
west build -b nrf52840dk_nrf52840 -- -DEXTRA_CONF_FILE=usbd_cdc.conf

# BMD-330
west build -b ubx_bmd330eval_nrf52810
```

## Requirements
- [Setup for ZephyrRTOS@3.5.0](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html)


## Simulation with Renode-run
- [Install Renode-run](https://github.com/antmicro/renode-run)
- Simulate with Renode-run
```bash
renode-run demo --board nrf52840dk_nrf52840 build/zephyr/zephyr.elf
```

## Testing with ZTest
- 
