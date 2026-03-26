#pragma once

#include <px4_arch/hw_description.h>
#include <px4_platform_common/spi.h>
#include <px4_arch/micro_hal.h>

static inline constexpr px4_spi_bus_device_t initSPIDevice(uint32_t devid, SPI::DRDY drdy_gpio = {}) {
    px4_spi_bus_device_t ret{};
    ret.drdy_gpio = (getGPIOPort(drdy_gpio.port) << 8) | getGPIOPin(drdy_gpio.pin);

    if (PX4_SPIDEVID_TYPE(devid) == 0) { // it's a PX4 device (internal or external)
        ret.devid = PX4_SPIDEV_ID(PX4_SPI_DEVICE_ID, devid);
    } else { // it's a NuttX device (e.g. SPIDEV_FLASH(0))
        ret.devid = devid;
    }

    ret.devtype_driver = PX4_SPI_DEV_ID(devid);
    return ret;
}

static inline constexpr px4_spi_bus_t initSPIBus(SPI::Bus bus, const px4_spi_bus_devices_t &devices, GPIO::GPIOPin power_enable = {}) {
    px4_spi_bus_t ret{};
    ret.requires_locking = false;

    for (int i = 0; i < SPI_BUS_MAX_DEVICES; ++i) {
        ret.devices[i] = devices.devices[i];


        if (ret.devices[i].cs_gpio != 0) {
            if (PX4_SPI_DEVICE_ID == PX4_SPIDEVID_TYPE(ret.devices[i].devid)) {
                int same_devices_count = 0;

                for (int j = 0; j < i; ++j) {
                    if (ret.devices[j].cs_gpio != 0) {
                        same_devices_count += (ret.devices[i].devid & 0xff) == (ret.devices[j].devid & 0xff);
                    }
                }

                // increment the 2. LSB byte to allow multiple devices of the same type
                ret.devices[i].devid |= same_devices_count << 8;

            } else {
                // A bus potentially requires locking if it is accessed by non-PX4 devices (i.e. NuttX drivers)
                ret.requires_locking = true;
            }
        }
    }

    ret.bus = (int)bus;
    ret.is_external = false;

    return ret;
}

// just a wrapper since we cannot pass brace-enclosed initialized arrays directly as arguments
struct bus_device_external_cfg_array_t {
    SPI::bus_device_external_cfg_t devices[SPI_BUS_MAX_DEVICES];
};

static inline constexpr px4_spi_bus_t initSPIBusExternal(SPI::Bus bus, const bus_device_external_cfg_array_t &devices) {
    px4_spi_bus_t ret{};

    for (int i = 0; i < SPI_BUS_MAX_DEVICES; ++i) {
        ret.devices[i] = initSPIDevice(i, devices.devices[i].drdy_gpio);
    }

    ret.bus = (int)bus;
    ret.is_external = true;
    ret.requires_locking = false; // external buses are never accessed by NuttX drivers
    return ret;
}

constexpr bool validateSPIConfig(const px4_spi_bus_t spi_buses_conf[SPI_BUS_MAX_BUS_ITEMS]);

