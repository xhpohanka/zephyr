/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_MNGR_CONFIG_H__
#define TMR_MNGR_CONFIG_H__

#include <nrf.h>

#define NRF_ASSERT(a) (void)(a)

#define HW_RTC_LEN_MASK 0x00FFFFFFuL

#define TMR_MNGR_NUM    8

#define TMR_MNGR_AVAL_TIMERS \
    {                        \
        0x000000FF,          \
    }

#define TMR_MNGR_LOCK()   __disable_irq()
#define TMR_MNGR_UNLOCK() __enable_irq()


#endif // TMR_MNGR_CONFIG_H__
