#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "tmr_mngr_backend.h"

#include <hal/nrf_rtc.h>

#if defined(CACHE_PRESENT)
#include <hal/nrf_cache.h>
#endif

int tmr_back_cc_sync_handler(uint64_t cc_value)
{
    int ret_code = 0;
    uint64_t cnt = (uint64_t)nrf_rtc_counter_get(NRF_RTC0);
    // if cc_value is lower than actual counter then error should be raised
    if ((cc_value < cnt) || ((cc_value - cnt) < 3))
    {
        ret_code = -EPERM;
    } else
    {
        nrf_rtc_cc_set(NRF_RTC0, 0, (uint32_t)cc_value);
        nrf_rtc_int_enable(NRF_RTC0, NRF_RTC_INT_COMPARE0_MASK);
        nrf_rtc_event_enable(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0);
    }
    return ret_code;
}

int tmr_back_cc_handler(uint64_t cc_value)
{
    int ret_code = 0;
    uint64_t cnt = (uint64_t)nrf_rtc_counter_get(NRF_RTC0);
    if ((cc_value < cnt) || ((cc_value - cnt) < 1))
    {
        ret_code = -EPERM;
    } else
    {
        nrf_rtc_cc_set(NRF_RTC0, 0, (uint32_t)cc_value);
        nrf_rtc_int_enable(NRF_RTC0, NRF_RTC_INT_COMPARE0_MASK);
        nrf_rtc_event_enable(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0);
    }

    return ret_code;
}

void tmr_back_start_handler(void)
{
}

void tmr_back_stop_handler(void)
{
    nrf_rtc_task_trigger(NRF_RTC0, NRF_RTC_TASK_STOP);;
}

uint64_t tmr_back_cnt_get_handler(void)
{
    return (uint64_t)nrf_rtc_counter_get(NRF_RTC0);
}

bool tmr_back_constr_check_handler(uint64_t cc_value)
{
    bool ret_val = true;
    uint32_t cnt = nrf_rtc_counter_get(NRF_RTC0);
    if ((cc_value < cnt) || ((cc_value - cnt) <= 3))
    {
        ret_val = false;
    }
    return ret_val;
}

void RTC0_IRQHandler(void)
{
    if (nrf_rtc_event_check(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0))
    {
        nrf_rtc_event_clear(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0);
        tmr_mngr_back_cc_irq();
    }
}

void tmr_back_init(void)
{
#if defined(CACHE_PRESENT)
    nrf_cache_enable(NRF_CACHE);
#endif

    IRQ_CONNECT(DT_IRQN(DT_NODELABEL(rtc0)),
        DT_IRQ(DT_NODELABEL(rtc0), priority),
        nrfx_isr, RTC0_IRQHandler, 0);
    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(NRF_RTC0), 0);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(NRF_RTC0));
    nrf_rtc_prescaler_set(NRF_RTC0, 0);
    nrf_rtc_task_trigger(NRF_RTC0, NRF_RTC_TASK_START);

}
