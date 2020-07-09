/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/

#include "tmr_srv_ext.h"
#include <nrfx.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(EXT, LOG_LEVEL_INF);

#define REQUEST_GET(p_payload)    (timer_service_request_t*)p_payload->data
#define REQUEST_SIZE_GET          (sizeof(timer_service_request_t))

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
