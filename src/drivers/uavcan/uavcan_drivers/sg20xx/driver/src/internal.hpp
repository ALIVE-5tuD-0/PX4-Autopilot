#pragma once

#include "board_config.h"

#if UAVCAN_SG20XX_NUTTX
# include <nuttx/arch.h>
# include <arch/board/board.h>
# include <sg2002_timer.h>
# include <syslog.h>
#else
# error "Unknown OS"
#endif

namespace uavcan_sg20xx {
#if UAVCAN_SG20XX_NUTTX

struct CriticalSectionLocker {
    const irqstate_t flags_;

    CriticalSectionLocker() : flags_(enter_critical_section()) { }

    ~CriticalSectionLocker() { leave_critical_section(flags_); }
};

#endif

namespace clock {
    uavcan::uint64_t getUtcUSecFromCanInterrupt();
}

}

