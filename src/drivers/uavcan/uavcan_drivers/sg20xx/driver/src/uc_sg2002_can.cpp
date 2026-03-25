#include <cassert>
#include <cstring>
#include <uavcan_sg20xx/can.hpp>
#include <uavcan_sg20xx/clock.hpp>
#include "internal.hpp"

#if UAVCAN_STM32_NUTTX
# include <nuttx/arch.h>
# include <nuttx/irq.h>
# include <arch/board/board.h>
#else
# error "Unknown OS"
#endif

namespace uavcan_sg20xx {
    namespace {

    }
}

