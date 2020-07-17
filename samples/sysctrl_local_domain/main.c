/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr/types.h>
#include <zephyr.h>
#include <logging/log.h>

#include <device.h>
#include <soc.h>

#include <stdio.h>
#include <string.h>
#include <init.h>

#include <prism_dispatcher.h>

#include <nrfs_led.h>
#include <nrfs_pm.h>
#include <nrfs_timer.h>

LOG_MODULE_REGISTER(nrf53net, CONFIG_LOG_DEFAULT_LEVEL);

void led_timer_expiry_fn(struct k_timer *dummy);

static struct k_thread m_txrx_thread_cb;
static K_THREAD_STACK_DEFINE(m_txrx_thread_stack, 1024);
static K_TIMER_DEFINE(m_led_timer, led_timer_expiry_fn, NULL);
static K_SEM_DEFINE(m_sem_irq, 0, 255);
static K_SEM_DEFINE(m_sem_tx, 0, 1);

void led_timer_expiry_fn(struct k_timer *dummy)
{
	uint32_t delay = sys_rand32_get() % 1000;

	k_sem_give(&m_sem_tx);

	LOG_INF("Setting led timer delay to %d ms", delay);
	k_timer_start(&m_led_timer, K_MSEC(delay), K_NO_WAIT);
}

void prism_irq_handler(void)
{
	LOG_INF("IPC IRQ handler invoked.");
	k_sem_give(&m_sem_irq);
}

static bool is_timer_srv_response(const void *payload)
{
	const nrfs_timer_t *p_req = (const nrfs_timer_t *)payload;

	return p_req->hdr.req == NRFS_TIMER_REQ_ACTION;
}

static void eptx_handler(void)
{
	prism_dispatcher_msg_t msg;

	prism_dispatcher_recv(&msg);

	if (is_timer_srv_response(msg.payload)) {
		const nrfs_timer_t *p_req = (const nrfs_timer_t *)msg.payload;

		LOG_ERR("Time = %d", (uint32_t)p_req->srv.data);
	} else {
		LOG_INF("[Domain: %d | Ept: %u] handler invoked.", msg.domain_id, msg.ept_id);
		LOG_HEXDUMP_INF(msg.payload, msg.size, "Payload:");
	}

	prism_dispatcher_free(&msg);
}

static const prism_dispatcher_ept_t m_epts[] =
{
	{ PRISM_DOMAIN_SYSCTRL, 0, eptx_handler },
	{ PRISM_DOMAIN_SYSCTRL, 1, eptx_handler },
	{ PRISM_DOMAIN_SYSCTRL, 2, eptx_handler },
	{ PRISM_DOMAIN_SYSCTRL, 3, eptx_handler },
};

static void led_service_req_generate(nrfs_led_t *p_req)
{
	NRFS_SERVICE_HDR_FILL(p_req, NRFS_LED_REQ_CHANGE_STATE);
	p_req->data.op = NRFS_LED_OP_TOGGLE;
	p_req->data.led_idx = sys_rand32_get() % 4;
}

static void pm_service_sleep_req_generate(nrfs_pm_sleep_t *p_req)
{
	NRFS_SERVICE_HDR_FILL(p_req, NRFS_PM_REQ_SLEEP);
	p_req->data.time = (sys_rand32_get() | ((uint64_t)sys_rand32_get() << 32));
	p_req->data.latency = sys_rand32_get();
	p_req->data.state = NRFS_PM_SLEEP_SYSTEM_OFF;
}

static void timer_srv_req_generate(nrfs_timer_t *p_req)
{
	NRFS_SERVICE_HDR_FILL(p_req, NRFS_TIMER_REQ_ACTION);
	p_req->srv.type = NRFS_TIMER_SRV_GET_TIME;
}

void txrx_thread(void *arg1, void *arg2, void *arg3)
{

	while (1) {
		if (k_sem_take(&m_sem_irq, K_USEC(100)) == 0) {
			prism_dispatcher_err_t status = prism_dispatcher_process(NULL);
			if (status != PRISM_DISPATCHER_OK && status != PRISM_DISPATCHER_ERR_NO_MSG) {
				LOG_ERR("Process failed: %d", status);
			}
		}

		if (k_sem_take(&m_sem_tx, K_NO_WAIT) == 0) {
			uint8_t ept_id = sys_rand32_get() % PRISM_DISPATCHER_EPT_COUNT;
			prism_dispatcher_msg_t msg =
			{
				.domain_id = PRISM_DOMAIN_SYSCTRL,
				.ept_id = ept_id,
			};

			nrfs_led_t 	led_req;
			nrfs_pm_sleep_t pm_sleep_req;
			nrfs_timer_t	timer_req;
			switch (sys_rand32_get() % 3) {
			case 0:
			{
				pm_service_sleep_req_generate(&pm_sleep_req);
				msg.payload = &pm_sleep_req;
				msg.size = sizeof(pm_sleep_req);
			}
			break;
			case 1:
			{
				led_service_req_generate(&led_req);
				msg.payload = &led_req;
				msg.size = sizeof(led_req);
			}
			break;
			case 2:
			{
				timer_srv_req_generate(&timer_req);
				msg.payload = &timer_req;
				msg.size = sizeof(timer_req);
			}
			break;
			default:
				break;
			}

			prism_dispatcher_err_t status = prism_dispatcher_send(&msg);
			if (status != PRISM_DISPATCHER_OK) {
				LOG_ERR("Sending failed: %d", status);
			}
		}
	}
}

int main(void)
{
	LOG_INF("Application domain.");

	prism_dispatcher_err_t status;
	status = prism_dispatcher_init(prism_irq_handler, m_epts, ARRAY_SIZE(m_epts));
	if (status != PRISM_DISPATCHER_OK) {
		LOG_ERR("Dispatcher init failed: %d", status);
	}

	k_thread_create(&m_txrx_thread_cb,
			m_txrx_thread_stack,
			K_THREAD_STACK_SIZEOF(m_txrx_thread_stack),
			txrx_thread,
			NULL, NULL, NULL,
			0, 0, K_NO_WAIT);

	k_timer_start(&m_led_timer, K_MSEC(10), K_NO_WAIT);

	return 0;
}
