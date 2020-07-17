/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/

#include "tmr_srv_ext.h"
#include <nrfx.h>

#include <logging/log.h>
#include <tmr_mngr_backend.h>
#include <nrfs_timer.h>

LOG_MODULE_REGISTER(EXT, LOG_LEVEL_INF);

#include "status_event.h"
#include "tmr_srv_ext_event.h"
#include "prism_event.h"
#include "ncm.h"

#define REQUEST_GET(p_payload)    (timer_service_request_t*)p_payload->data
#define REQUEST_SIZE_GET          (sizeof(timer_service_request_t))

extern uint64_t tmr_mngr_cnt_get(void);

int tmr_srv_ext_set_timeout(prism_dispatcher_msg_t *msg)
{
    NRFX_ASSERT(msg);
    sysctl_request_payload_t *req_pload = EXT_SERVICE_PAYLOAD_GET(msg);
    timer_service_request_t *tim_req = REQUEST_GET(req_pload);
    tmr_srv_int_context_t msg_ctx = {
            .source = TMR_SRV_EXT_REQ_SRC,
            .id = msg->domain_id
    };
    return tmr_srv_set_timeout(tim_req->timeout_value, *((uint32_t*)&msg_ctx));
}

int tmr_srv_ext_init(srv_ext_callback_t callback)
{
    int ret;
    if ((ret = tmr_srv_int_init()) < 0)
    {
        return ret;
    }
    return tmr_srv_int_callback_register(TMR_SRV_EXT_REQ_SRC, (tmr_srv_int_callback_t)callback);
}

static void cb(void * context)
{
	LOG_ERR("CB_TIMER: tick");
}

static void tmr_get_time_handle(struct tmr_srv_ext_event *evt)
{
	static struct ncm_ctx ctx;

	u32_t time = (u32_t)tmr_mngr_cnt_get();

	LOG_ERR("czas = %d", time);
	ncm_fill(&ctx, evt->p_msg);

	struct prism_event *prism_evt = new_prism_event();
	prism_evt->p_msg = evt->p_msg;
	prism_evt->status = PRISM_MSG_STATUS_RX_RELEASED;
	EVENT_SUBMIT(prism_evt);

	nrfs_timer_srv_t srv = {
		.type = NRFS_TIMER_SRV_GET_TIME,
		.data = time
	};

	void *p_buffer = ncm_alloc(sizeof(nrfs_timer_srv_t));
	(void)memcpy(p_buffer, &srv, sizeof(srv));
	ncm_notify(&ctx, p_buffer, sizeof(srv));
}

static enum nrfs_timer_service_type dispatch_srv_type(const nrfs_phy_t *p_msg)
{
	const nrfs_timer_t *p_timer = (nrfs_timer_t *)p_msg->p_buffer;

	return p_timer->srv.type;
}

static bool event_handler(const struct event_header *eh)
{
	if (is_status_event(eh)) {
		struct status_event *evt = cast_status_event(eh);
		if (evt->status == STATUS_INIT) {
			if (tmr_srv_ext_init(cb)) {
				LOG_ERR("tmr_srv_ext_init() returned error");
			} else {
				LOG_ERR("tmr_srv_ext_init [OK]");
			}
		}
		return false;
	}

	if (is_tmr_srv_ext_event(eh)) {
		struct tmr_srv_ext_event *evt = cast_tmr_srv_ext_event(eh);

		if (dispatch_srv_type(evt->p_msg) == NRFS_TIMER_SRV_GET_TIME) {
			tmr_get_time_handle(evt);
			return true;
		}
	}

	__ASSERT_NO_MSG(false);
	return false;
}

EVENT_LISTENER(TIMER_SERVICE, event_handler);
EVENT_SUBSCRIBE(TIMER_SERVICE, status_event);
EVENT_SUBSCRIBE(TIMER_SERVICE, tmr_srv_ext_event);
