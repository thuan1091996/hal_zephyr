# HAL for nRF52 with ZephyrRTOS

## Build commands
```
# nRF52840DK
west build -p -b nrf52840dk_nrf52840 -- -DEXTRA_CONF_FILE="usbd_cdc.conf;ble.conf"

# BMD-330
west build -p -b ubx_bmd330eval_nrf52810 -- -DEXTRA_CONF_FILE="ble.conf"
```

## Requirements
- nRF connect SDK v2.6.1


## Simulation with Renode-run
- [Install Renode-run](https://github.com/antmicro/renode-run)
- Simulate with Renode-run
```bash
renode-run demo --board nrf52840dk_nrf52840 build/zephyr/zephyr.elf
```

## Testing with ZTest
- 
