/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr.h>

/*$$$LICENCE_NORDIC_STANDARD<2018>$$$*/

/**
 * @defgroup empty_example_main Empty application
 * @{
 * @ingroup nrfx_examples
 *
 * @brief Empty example Application main file.
 *
 * This file contains the source code for a sample application with logger subsystem.
 *
 * Please connect development kit and configure following options when opening COM port:
 *  - Baudrate:   115200
 *  - Parity:     None
 *  - Stop bits:  1
 *  - Data bits:  8
 *
 * @}
 */
#include <stdlib.h>
#include <stdint.h>
//#include <nrfx.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_INF);

#include <tmr_mngr_backend.h>

#include <tmr_srv_ext.h>

extern uint64_t tmr_mngr_cnt_get(void);

static volatile uint64_t curr_time;

static volatile bool cb_done = true;

static void cb(void * context)
{
    curr_time = tmr_mngr_cnt_get();
    tmr_srv_int_context_t * ctx = (tmr_srv_int_context_t*)&context;
    LOG_INF("Callback from id %u at %u", ctx->id, (uint32_t)curr_time);

    cb_done = true;
}

static  uint32_t i = 200000;

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    LOG_INF("tmr_srv_ext_sample %u", sizeof(tmr_srv_int_context_t));
    timer_service_request_t request =
    {
        .timeout_value = 32768 * 5,
        .settings_mask = 0
    };
    sysctl_request_payload_t req_pload =
    {
        .type = MSG_TYPE_TIMER_SERVICE,
        .app_ctx = 0,
        .data = (void*)&request,
        .data_size = sizeof(timer_service_request_t)
    };
    prism_dispatcher_msg_t msg = {
        .domain_id = PRISM_DOMAIN_NET,
        .ept_id = 1,
        .payload = (void*)&req_pload,
        .size = sizeof(sysctl_request_payload_t)
    };
    if (tmr_srv_ext_init(cb) < 0)
    {
        LOG_ERR("tmr_srv_ext_init() returned error");
    }
    int ret;

    while (1)
    {
        k_sleep(K_MSEC(1000));
        curr_time = tmr_mngr_cnt_get();
        request.timeout_value = i;
        if (cb_done)
        {
            LOG_INF("Setting timer to %u at %u", i, (uint32_t)curr_time);
        if ((ret = tmr_srv_ext_set_timeout(&msg)) < 0)
        {
            LOG_ERR("tmr_srv_set_timeout returned %d while setting to %u", ret, i);
        }
            i += 120000;
            cb_done = false;
        }
        else
        {
            LOG_INF("%u", (uint32_t)curr_time);
        }
    }
    return 0;
}
