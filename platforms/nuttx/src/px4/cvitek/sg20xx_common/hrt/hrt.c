/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file hrt.c
 *
 * High-resolution timer callouts and timekeeping.
 *
 * RP2040's internal 64 bit timer can be used for this purpose.
 *
 * Note that really, this could use systick too, but that's
 * monopolised by NuttX and stealing it would just be awkward.
 *
 * We don't use the NuttX RP2040 driver per se; rather, we
 * claim the timer and then drive it directly.
 */

#include <px4_platform_common/px4_config.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include <sys/types.h>
#include <stdbool.h>

#include <assert.h>
#include <debug.h>
#include <time.h>
#include <queue.h>
#include <errno.h>
#include <string.h>

#include <board_config.h>
#include <drivers/drv_hrt.h>

#include <nuttx/timers/arch_alarm.h>
#include "hardware/sg2002_plic.h"

#include "riscv_internal.h"
#include "riscv_mtimer.h"

#define MTIMER_FREQ         25000000ul

/*
 * Queue of callout entries.
 */
static struct sq_queue_s    callout_queue;

/* latency baseline (last compare value applied) */
static uint16_t             latency_baseline;

/* timer count at interrupt (for latency purposes) */
static uint16_t             latency_actual;

void hrt_init(void) {
    struct oneshot_lowerhalf_s *lower;

    lower = riscv_mtimer_initialize(SG2002_CLINT_MTIME, SG2002_CLINT_MTIMECMP, RISCV_IRQ_MTIMER, MTIMER_FREQ);

    /* set hrt timer callback */
    // ((struct riscv_mtimer_lowerhalf_s *)lower)->callback = ;

    up_alarm_set_lowerhalt(lower);
}
