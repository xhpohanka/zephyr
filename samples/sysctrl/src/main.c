/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr.h>

#include <hal/nrf_rtc.h>
#include <tmr_mngr.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_INF);

static volatile uint32_t tick;
static volatile uint32_t inst0_tick;
static volatile uint32_t inst1_tick;
static volatile uint32_t inst2_tick;

static volatile bool oneshot_fired = false;

void vrtc_handler(uint8_t instance)
{

    if (instance == 0)
    {
        inst0_tick++;
    }
    if (instance == 1)
    {
        LOG_INF("x:%u", instance);
        inst1_tick++;
    }
    if (instance == 2)
    {
        LOG_INF("x:%u", instance);
        inst2_tick++;
    }
    if (instance == 3)
    {
        LOG_INF("x:%u", instance);
        oneshot_fired = true;
    }
}


extern void rtc_handler(void);

void main(void)
{
    LOG_INF("pot-rtc sample on %s", CONFIG_BOARD);
        tmr_mngr_init(vrtc_handler);

        int idx;

        idx = tmr_mngr_start(TMR_MNGR_MODE_PERIODIC, 1000, 0);
        LOG_INF("Starting timer %d", idx);
        if (0 > idx)
        {
            LOG_INF("Error when started timer %d", idx);
        }

        idx = tmr_mngr_start(TMR_MNGR_MODE_PERIODIC, 50000, 0);
        LOG_INF("Starting timer %d", idx);
        if (0 > idx)
        {
            LOG_INF("Error when started timer %d", idx);
        }

        idx = tmr_mngr_start(TMR_MNGR_MODE_PERIODIC, 100, 0);
        LOG_INF("Starting timer %d", idx);
        if (0 > idx)
        {
            LOG_INF("Error when started timer %d", idx);
        }

        LOG_INF("Starting oneshot timer %d", idx);
        idx = tmr_mngr_start(TMR_MNGR_MODE_ONE_SHOT, 100000, 0);
        if (0 > idx)
        {
            LOG_INF("Error when started timer %d", idx);
        }

        while (1)
        {
            LOG_INF("ticks inst 0: %d| ticks inst 1: %d | ticks inst 2: %d | CNT: %d | CC: %d",
                          inst0_tick, inst1_tick, inst2_tick,
                          (uint32_t)tmr_mngr_cnt_get(),
                          NRF_RTC0->CC[0]);
            if (oneshot_fired)
            {
                oneshot_fired = !oneshot_fired;
                LOG_INF("One shot timer fired");
            }
            k_sleep(K_MSEC(1000));
        }
}

