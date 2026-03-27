#include <board_config.h>
#include <systemlib/px4_macros.h>
#include <px4_platform_common/spi.h>
#include <px4_arch/micro_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <debug.h>
#include <unistd.h>

#include <nuttx/spi/spi.h>
#include <arch/board/board.h>
#include <chip.h>

static const px4_spi_bus_t *_spi_bus2 = NULL;

__EXPORT void sg2002_spiinitialize() {
    px4_set_spi_buses_from_hw_version();

    for (int i = 0; i < SPI_BUS_MAX_BUS_ITEMS; ++i) {
        switch (px4_spi_buses[i].bus) {
            case PX4_BUS_NUMBER_TO_PX4(2): _spi_bus2 = &px4_spi_buses[i]; break;
            default: return;
        }
    }
}


