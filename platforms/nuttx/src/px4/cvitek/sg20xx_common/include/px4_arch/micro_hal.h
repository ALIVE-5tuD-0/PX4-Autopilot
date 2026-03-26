#pragma once

#include <px4_platform/micro_hal.h>

__BEGIN_DECLS

#include <sg2002_spi.h>
#include <sg2002_timer.h>
#include <sg2002_gpio.h>

#define PX4_CPU_UUID_BYTE_LENGTH                    	12
#define PX4_CPU_UUID_WORD32_LENGTH                  	(PX4_CPU_UUID_BYTE_LENGTH / sizeof(uint32_t))
#define PX4_CPU_MFGUID_BYTE_LENGTH                  	PX4_CPU_UUID_BYTE_LENGTH

/* By not defining PX4_CPU_UUID_CORRECT_CORRELATION the following maintains the legacy incorrect order
 * used for selection of significant digits of the UUID in the PX4 code base.
 * This is done to avoid the ripple effects changing the IDs used on STM32 base platforms
 */
#if defined(PX4_CPU_UUID_CORRECT_CORRELATION)
# define PX4_CPU_UUID_WORD32_UNIQUE_H            	0 /* Least significant digits change the most */
# define PX4_CPU_UUID_WORD32_UNIQUE_M            	1 /* Middle significant digits */
# define PX4_CPU_UUID_WORD32_UNIQUE_L            	2 /* Most significant digits change the least */
#else
/* Legacy incorrect ordering */
# define PX4_CPU_UUID_WORD32_UNIQUE_H            	2 /* Most significant digits change the least */
# define PX4_CPU_UUID_WORD32_UNIQUE_M            	1 /* Middle significant digits */
# define PX4_CPU_UUID_WORD32_UNIQUE_L            	0 /* Least significant digits change the most */
#endif

#define PX4_BUS_OFFSET					0

#define PX4_SOC_ARCH_ID					PX4_SOC_ARCH_ID_UNUSED

#define px4_spibus_initialize(bus_num_1based)       	sg2002_spibus_initialize(bus_num_1based)

#define px4_timer_initialize(timer_index)           	sg2002_timer_initialize(timer_index)

#define px4_arch_configgpio(pinset)
#define px4_arch_gpioread(pinset)                   	sg2002_gpio_read(pinset)
#define px4_arch_gpiowrite(pinset, value)           	sg2002_gpioPwrite(pinset, value)
#define px4_arch_gpiosetevent(pinset,r,f,e,fp,a)    	sg2002_gpio_set_event(pinset,r,f,e,fp,a)

#define px4_cache_aligned_data()
#define px4_cache_aligned_alloc				malloc

__END_DECLS
