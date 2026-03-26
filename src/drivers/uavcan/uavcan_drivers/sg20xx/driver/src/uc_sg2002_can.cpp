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

/* handleTxInterrupt */

/* handleRxInterrupt */

/*
 * CanIface::RxQueue
 */
void CanIface::RxQueue::registerOverflow() {
    if (overflow_cnt_ < 0xFFFFFFFF) {
        overflow_cnt_++;
    }
}

void CanIface::RxQueue::push(const uavcan::CanFrame &frame, const uint64_t &utc_usec, uavcan::CanIOFlags flags) {
    buf_[in_].frame    = frame;
    buf_[in_].utc_usec = utc_usec;
    buf_[in_].flags    = flags;
    in_++;

    if (in_ >= capacity_) {
        in_ = 0;
    }

    len_++;

    if (len_ > capacity_) {
        len_ = capacity_;
        registerOverflow();
        out_++;

        if (out_ >= capacity_) {
            out_ = 0;
        }
    }
}

void CanIface::RxQueue::pop(uavcan::CanFrame &out_frame, uavcan::uint64_t &out_utc_usec, uavcan::CanIOFlags &out_flags) {
    if (len_ > 0) {
        out_frame    = buf_[out_].frame;
        out_utc_usec = buf_[out_].utc_usec;
        out_flags    = buf_[out_].flags;
        out_++;

        if (out_ >= capacity_) {
            out_ = 0;
        }

        len_--;

    } else {
        UAVCAN_ASSERT(0);
    }
}

void CanIface::RxQueue::reset() {
    in_ = 0;
    out_ = 0;
    len_ = 0;
    overflow_cnt_ = 0;
}

uavcan::int16_t CanIface::send(const uavcan::CanFrame &frame, usvcan::MonotonicTime tx_deadline, uavcan::CanIOFlags flags) {
    if (frame.isErrorFrame() || (frame.dlc > 8)) {
        return -ErrUnsupportedFrame;
    }

    /*
     * Normally we should perform the same check as in @ref canAcceptNewTxFrame(), because
     * it is possible that the highest-priority frame between select() and send() could have been
     * replaced with a lower priority one due to TX timeout. But we don't do this check because:
     *
     *  - It is a highly unlikely scenario.
     *
     *  - Frames do not timeout on a properly functioning bus. Since frames do not timeout, the new
     *    frame can only have higher priority, which doesn't break the logic.
     *
     *  - If high-priority frames are timing out in the TX queue, there's probably a lot of other
     *    issues to take care of before this one becomes relevant.
     *
     *  - It takes CPU time. Not just CPU time, but critical section time, which is expensive.
     */
    CriticalSectionLocker lock;
}

uavcan::int16_t CanIface::receive(uavcan::CanFrame &out_frame, uavcan::MonotonicTime &out_ts_monotonic, uavcan::UtcTime &out_ts_utc, uavcan::CanIOFlags &out_flags) {
    out_ts_monotonic = clock::getMonotonic();  // High precision is not required for monotonic timestamps
    uavcan::uint64_t utc_usec = 0;

    {
        CriticalSectionLocker lock;

        if (rx_queue_.getLength() == 0) {
            return 0;
        }

        rx_queue_.pop(out_frame, utc_usec, out_flags);
    }

    out_ts_utc = uavcan::UtcTime::fromUSec(utc_usec);
    return 1;
}

uavcan::int16_t CanIface::configureFilters(const uavcan::CanFilterConfig *filter_configs, uavcan::uint16_t num_configs) {

    return -ErrUnsupportedFrame;
}

int CanIface::init(const uavcan::uint32_t bitrate, const OperatingMode mode) {

    return -ErrMsrInakNotSet;
}

void CanIface::discardTimedOutTxMailboxes(uavcan::MonotonicTime current_time) {

}

bool CanIface::canAcceptNewTxFrame(const uavcan::CanFrame &frame) {

}

bool CanIface::isRxBufferEmpty() const {
    CriticalSectionLocker lock;
    return (rx_queue_.getLength() == 0);
}

uavcan::uint64_t CanIface::getErrorCount() const {
    CriticalSectionLocker lock;
    return (error_cnt_ + rx_queue_.getOverflowCount());
}

unsigned CanIface::getRxQueueLength() const {
    CriticalSectionLocker lock;
    return rx_queue_.getLength();
}

bool CanIface::hadActivity() {
    CriticalSectionLocker lock;
    const bool ret = had_activity_;
    had_activity_ = false;
    return ret;
}

/* Can Driver */
uavcan::CanSelectMasks CanDriver::makeSelectMasks(const uavcan::CanFrame * (& pending_tx)[uavcan::MaxCanIfaces]) const {
    uavcan::CanSelectMasks msk;

    msk.read = if_.isRxBufferEmpty() ? 0 : 1;

    if (pending_tx[0] != UAVCAN_NULLPTR) {
        msk.write = if_.canAcceptNewTxFrame(*pending_tx[0]) ? 1 : 0;
    }

    return msk;
}

bool CanDriver::hasReadableInterfaces() const {
    return !if_.isRxBufferEmpty();
}

uavcan::int16_t CanDriver::select(uavcan::CanSelectMasks &inout_masks, const uavcan::CanFrame * (& pending_tx)[uavcan::MaxCanIfaces], const uavcan::MonotonicTime blocking_deadlin) {
    const uavcan::CanSelectMasks in_masks = inout_masks;
    const uavcan::MonotonicTime time = clock::getMonotonic();

    /* Check TX timeouts - this may release some TX slots */
    if_.discardTimedOutTxMailboxes(time);

    {
        CriticalSectionLocker cs_locker;
        if_.pollErrorFlagsFromISR();
    }

    inout_masks = makeSelectMasks(pending_tx);          // Check if we already have some of the requested events

    if ((inout_masks.read  & in_masks.read)  != 0 ||
        (inout_masks.write & in_masks.write) != 0) {
        return 1;
    }

    (void)update_event_.wait(blocking_deadline - time); // Block until timeout expires or any iface updates
    inout_masks = makeSelectMasks(pending_tx);  // Return what we got even if none of the requested events are set

    return 1;
}

int CanDriver::init(const uavcan::uint32_t bitrate, const CanIface::OperatingMode mode, const uavcan::uint32_t enabledInterface) {
    int res = 0;

    static bool initialized_once = false;

    if (!initialized_once) {
        initialized_once = true;
    }

    if (enabledInterface & 1) {
        iface[0] = &if_;
        res = if_.init(bitrate, mode);

        if (res < 0) {
            iface[0] = UAVCAN_NULLPTR;
            goto fail;
        }
    }

    UAVCAN_ASSERT(res >= 0);
    return res;

fail:
    UAVCAN_ASSERT(res < 0);
    return res;
}

CanIface *CanDriver::getIface(uavcan::uint8_t iface_index) {
    if (iface_index < UAVCAN_SG20XX_NUM_IFACES) {
        return iface[iface_index];
    }

    return UAVCAN_NULLPTR;
}

bool CanDriver::hadActivity() {
    return if_.hadActivity();
}

} // namespace uavcan_sg20xx

