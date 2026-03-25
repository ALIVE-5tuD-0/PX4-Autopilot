#pragma once

#include <px4_platform/micro_hal.h>

__BEGIN_DECLS

#include <sg2002_spi.h>
#include <sg2002_timer.h>
#include <sg2002_gpio.h>

#define PX4_CPU_UUID_BYTE_LENGTH                    	12
#define PX4_CPU_UUID_WORD32_LENGTH                  	(PX4_CPU_UUID_BYTE_LENGTH / sizeof(uint32_t))
#define PX4_CPU_MFGUID_BYTE_LENGTH                  	PX4_CPU_UUID_BYTE_LENGTH

#define px4_spibus_initialize(bus_num_1based)       	sg2002_spibus_initialize(bus_num_1based)

#define px4_timer_initialize(timer_index)           	sg2002_timer_initialize(timer_index)

#define px4_arch_configgpio(pinset)
#define px4_arch_gpioread(pinset)                   	sg2002_gpio_read(pinset)
#define px4_arch_gpiowrite(pinset, value)           	sg2002_gpioPwrite(pinset, value)
#define px4_arch_gpiosetevent(pinset,r,f,e,fp,a)    	sg2002_gpio_set_event(pinset,r,f,e,fp,a)

#define px4_cache_aligned_data()
#define px4_cache_aligned_alloc				malloc

__END_DECLS
