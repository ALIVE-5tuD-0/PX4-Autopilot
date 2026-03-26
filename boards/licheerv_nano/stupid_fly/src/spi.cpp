#include <px4_arch/spi_hw_description.h>
#include <drivers/drv_sensor.h>
#include <nuttx/spi/spi.h>

constexpr px4_spi_bus_t px4_spi_buses[SPI_BUS_MAX_BUS_ITEMS] = {
    initSPIBusExternal(SPI::Bus::SPI6, {
        initSPIConfigExternal(SPI::DRDY{GPIO::PortD, GPIO::Pin11}),
    }),
};

static constexpr bool unused = validateSPIConfig(px4_spi_buses);
