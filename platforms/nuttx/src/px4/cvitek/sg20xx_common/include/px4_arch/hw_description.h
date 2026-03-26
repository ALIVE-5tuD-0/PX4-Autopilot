
#pragma once

#include <stdint.h>

#include <px4_platform_common/constexpr_util.h>

/*
 * GPIO
 */

namespace GPIO {
enum Port = {
    PortA = 0,
    PortB,
    PortC,
    PortD
};

enum Pin {
    Pin0 = 0,
    Pin1,
    Pin2,
    Pin3,
    Pin4,
    Pin5,
    Pin6,
    Pin7,
    Pin8,
    Pin9,
    Pin10,
    Pin11,
    Pin12,
    Pin13,
    Pin14,
    Pin15,
    Pin16,
    Pin17,
    Pin18,
    Pin19,
    Pin20,
    Pin21,
    Pin22,
    Pin23,
    Pin24,
    Pin25,
    Pin26,
    Pin27,
    Pin28,
    Pin29,
    Pin30,
    Pin31,
};

struct GPIOPin {
    Port port;
    Pin pin;
};
}

static inline constexpr int8_t getGPIOPort(GPIO::Port port) {
    switch (port) {
        case GPIO::PortA: return 0;
        case GPIO::PortB: return 1;
        case GPIO::PortC: return 2;
        case GPIO::PortD: return 3;
        default: return -1;
    }

    return -1;
}

static inline constexpr int8_t getGPIOPin(GPIO::Pin pin) {
	switch (pin) {
        case GPIO::Pin0: return 0;
        case GPIO::Pin1: return 1;
        case GPIO::Pin2: return 2;
        case GPIO::Pin3: return 3;
        case GPIO::Pin4: return 4;
        case GPIO::Pin5: return 5;
        case GPIO::Pin6: return 6;
        case GPIO::Pin7: return 7;
        case GPIO::Pin8: return 8;
        case GPIO::Pin9: return 9;
        case GPIO::Pin10: return 10;
        case GPIO::Pin11: return 11;
        case GPIO::Pin12: return 12;
        case GPIO::Pin13: return 13;
        case GPIO::Pin14: return 14;
        case GPIO::Pin15: return 15;
        case GPIO::Pin16: return 16;
        case GPIO::Pin17: return 17;
        case GPIO::Pin18: return 18;
        case GPIO::Pin19: return 19;
        case GPIO::Pin20: return 20;
        case GPIO::Pin21: return 21;
        case GPIO::Pin22: return 22;
        case GPIO::Pin23: return 23;
        case GPIO::Pin24: return 24;
        case GPIO::Pin25: return 25;
        case GPIO::Pin26: return 26;
        case GPIO::Pin27: return 27;
        case GPIO::Pin28: return 28;
        case GPIO::Pin29: return 29;
        case GPIO::Pin30: return 30;
        case GPIO::Pin31: return 31;
        default: return -1;
    }

	return -1;
}

namespace SPI
{
enum class Bus {
	SPI2 = 2,
};

using DRDY = GPIO::GPIOPin; ///< data ready pin

struct bus_device_external_cfg_t {
	DRDY drdy_gpio;
};

} // namespace SPI
