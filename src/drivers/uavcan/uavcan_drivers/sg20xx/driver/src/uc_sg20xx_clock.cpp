/*
 * Copyright (C) 2026 Author: 8_B!T0
 */

#include <uavcan_sg20xx/clock.hpp>
#include <uavcan_sg20xx/thread.hpp>
#include <px4_platform_common/px4_config.h>
#include "internal.hpp"

#if (UAVCAN_SG20XX_TIMER_NUMBER >= 4) && (UAVCAN_SG20XX_TIMER_NUMBER <= 7)

#include <cassert>
#include <math.h>

extern "C" bool TimerX_IRQHandler(unsigned int *context, FAR void *arg);

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
    struct timer_lowerhalf_s *lower = px4_timer_initialize(UAVCAN_SG20XX_TIMER_NUMBER);

    if (lower) {
        lower->ops->setcallback(lower, TimerX_IRQHandler, NULL);
    }

#endif

}

void setUtc(uavcan::UtcTime time) {
    MutexLocker mlocker(mutex);
    UAVCAN_ASSERT(initialized);

    {
        CriticalSectionLocker locker;
        time_utc = time.toUSec();
    }

    utc_set = true;
    utc_locked = false;
    utc_jump_cnt++;
    utc_prev_adj = 0;
    utc_rel_rate_ppm = 0;
}

static uavcan::uint64_t sampleUtcFromCriticalSection() {
# if UAVCAN_SG20XX_NUTTX

    UAVCAN_ASSERT(initialized);

    volatile uavcan::uint64_t time = time_utc;
    // volatile uavcan::uint32_t cnt = getreg16(TMR_REG(STM32_BTIM_CNT_OFFSET));

    // if (getreg16(TMR_REG(STM32_BTIM_SR_OFFSET)) & BTIM_SR_UIF) {
    //     cnt = getreg16(TMR_REG(STM32_BTIM_CNT_OFFSET));
    //     const uavcan::int32_t add = uavcan::int32_t(USecPerOverflow) +
    //                     (utc_accumulated_correction_nsec + utc_correction_nsec_per_overflow) / 1000;
    //     time = uavcan::uint64_t(uavcan::int64_t(time) + add);
    // }

    return time + cnt;

# endif
}

uavcan::uint64_t getUtcUSecFromCanInterrupt() {
    return utc_set ? sampleUtcFromCriticalSection() : 0;
}





}

}

/**
 * Timer interrupt handler
 */

extern "C"
bool TimerX_IRQHandler(unsigned int *context, FAR void *arg) {

	using namespace uavcan_stm32::clock;
	UAVCAN_ASSERT(initialized);

	time_mono += USecPerOverflow;

	if (utc_set) {
		time_utc += USecPerOverflow;
		utc_accumulated_correction_nsec += utc_correction_nsec_per_overflow;

		if (std::abs(utc_accumulated_correction_nsec) >= 1000) {
			time_utc = uavcan::uint64_t(uavcan::int64_t(time_utc) + utc_accumulated_correction_nsec / 1000);
			utc_accumulated_correction_nsec %= 1000;
		}

		// Correction decay - 1 nsec per 65536 usec
		if (utc_correction_nsec_per_overflow > 0) {
			utc_correction_nsec_per_overflow--;

		} else if (utc_correction_nsec_per_overflow < 0) {
			utc_correction_nsec_per_overflow++;

		}
	}

    return 0;
}

#endif

