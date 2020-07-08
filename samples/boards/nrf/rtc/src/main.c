/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define CONFIG_USE_NRFX_FOR_TIMER_BACKEND 0


#if CONFIG_USE_NRFX_FOR_TIMER_BACKEND
#include <zephyr.h>

#include <nrfx_rtc.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(nrfx_sample, LOG_LEVEL_INF);

static volatile bool interrupt_handled = false;
static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(2);  /**< RTC instance. */

static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
	interrupt_handled = true;
	LOG_INF("RTC event callback with type %u", (uint32_t) int_type);
}

static nrfx_err_t tmr_back_init(void)
{
	nrfx_rtc_config_t cfg = NRFX_RTC_DEFAULT_CONFIG;
	return nrfx_rtc_init(&rtc, &cfg, rtc_handler);

}

void main(void)
{
	LOG_INF("nrfx_rtc sample on %s", CONFIG_BOARD);

	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(rtc0)),
		    DT_IRQ(DT_NODELABEL(rtc0), priority),
		    nrfx_isr, nrfx_rtc_2_irq_handler, 0);
	nrfx_err_t ret = tmr_back_init();
	if (ret) {

	}

	nrfx_rtc_int_enable(&rtc, RTC_INTENSET_COMPARE0_Msk);
	nrfx_rtc_cc_set(&rtc, 0, 100000, true);
	nrfx_rtc_enable(&rtc);

	LOG_INF("inited");
	uint32_t cnt;

	while (1)
	{
		k_sleep(K_MSEC(1000));
		cnt = nrfx_rtc_counter_get(&rtc);
		LOG_INF("%u", cnt);
		if (interrupt_handled) {
			interrupt_handled = false;
			nrfx_rtc_cc_set(&rtc, 0, cnt + 100000, true);
		}
	}
}

#else
#include <zephyr.h>

#include <hal/nrf_rtc.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(nrfx_sample, LOG_LEVEL_INF);

static void rtc_handler(void)
{
	    if (nrf_rtc_event_check(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0))
	    {
	        nrf_rtc_event_clear(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0);
	    }
	LOG_INF("RTC event callback with type");
}


void main(void)
{
	LOG_INF("nrf_rtc sample on %s", CONFIG_BOARD);
	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(rtc0)),
	    DT_IRQ(DT_NODELABEL(rtc0), priority),
	    nrfx_isr, rtc_handler, 0);
	NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(NRF_RTC0), 0);
	NRFX_IRQ_ENABLE(nrfx_get_irq_number(NRF_RTC0));
	nrf_rtc_prescaler_set(NRF_RTC0, 0);


	nrf_rtc_cc_set(NRF_RTC0, 0, 500000);
	nrf_rtc_int_enable(NRF_RTC0, NRF_RTC_INT_COMPARE0_MASK);
	nrf_rtc_event_enable(NRF_RTC0, NRF_RTC_EVENT_COMPARE_0);
	nrf_rtc_task_trigger(NRF_RTC0, NRF_RTC_TASK_START);

	LOG_INF("Initialized");
	uint32_t cnt;
	while (1)
	{
		k_sleep(K_MSEC(1000));
		cnt = nrf_rtc_counter_get(NRF_RTC0);
		LOG_INF("%u", cnt);
	}

}

#endif
