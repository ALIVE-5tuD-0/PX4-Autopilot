/*
 * Copyright (C) 2026 Author: 8_B!T0
 */

#include <uavcan_sg20xx/clock.hpp>
#include <uavcan_sg20xx/thread.hpp>
#include "internal.hpp"

#if (UAVCAN_SG20XX_TIMER_NUMBER >= 4) && (UAVCAN_SG20XX_TIMER_NUMBER <= 6)

#include <cassert>
#include <math.h>

namespace uavcan_sg20xx {
namespace clock {
namespace {

    const uavcan::uint32_t USecPerOverflow = 65536;

    Mutex mutex;

    bool initialized = false;

    bool utc_set = false;
    bool utc_locked = false;
    uavcan::uint32_t utc_jump_cnt = 0;
    UtcSyncParams utc_sync_params;
    float utc_prev_adj = 0;
    float utc_rel_rate_ppm = 0;
    float utc_rel_rate_error_integral = 0;
    uavcan::int32_t utc_accumulated_correction_nsec = 0;
    uavcan::int32_t utc_correction_nsec_per_overflow = 0;
    uavcan::MonotonicTime prev_utc_adj_at;

    uavcan::uint64_t time_mono = 0;
    uavcan::uint64_t time_utc = 0;

}


void init() {
    CriticalSectionLocker lock;

    if (initialized) {
        return;
    }

    initialized = true;

#if UAVCAN_SG20XX_NUTTX

    // sg2002_timer_initialize(UAVCAN_SG20XX_TIMER_NUMBER);

#endif

}

}

}

#endif

