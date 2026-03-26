#pragma once

#include "../../../sg20xx_common/include/px4_arch/spi_hw_description.h"

constexpr bool validateSPIConfig(const px4_spi_bus_t spi_busses_conf[SPI_BUS_MAX_BUS_ITEMS]) {
	const bool nuttx_enabled_spi_buses[] = {
#ifdef CONFIG_SG2002_SPI2
		true,
#else
		false,
#endif
	};

	for (unsigned i = 0; i < sizeof(nuttx_enabled_spi_buses) / sizeof(nuttx_enabled_spi_buses[0]); ++i) {
		bool found_bus = false;

		for (int j = 0; j < SPI_BUS_MAX_BUS_ITEMS; ++j) {
			if (spi_busses_conf[j].bus == (int)i + 1) {
				found_bus = true;
			}
		}

		// Either the bus is enabled in NuttX and configured in spi_busses_conf, or disabled and not configured
		constexpr_assert(found_bus == nuttx_enabled_spi_buses[i], "SPI bus config mismatch (CONFIG_RP2040_SPIx)");
	}

	return false;
}
