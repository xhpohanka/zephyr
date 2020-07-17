/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/

#include "tmr_srv_int.h"
#include "tmr_mngr.h"

#include <nrfx.h>
#include <pot.h>

#include "tmr_mngr_config.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(INT, LOG_LEVEL_INF);

extern pot_element_t vrtc_instances[TMR_MNGR_NUM];

static tmr_srv_int_callback_t callbacks[TMR_SRV_NB_OF_REQ_SRCS];

static void tmr_srv_int_handler(uint32_t context)
{
    tmr_srv_int_context_t * ctx = (tmr_srv_int_context_t*)&context;
    NRF_ASSERT(callbacks[ctx->source]);
    callbacks[ctx->source](ctx);
}

int tmr_srv_int_init(void)
{
    static bool already_initialized = false;
    int ret;

    if (already_initialized)
    {
        //in case when another source initialized the module, we can continue, so this is not an error
        return EEXIST;
    }
    if ((ret = tmr_mngr_init(tmr_srv_int_handler)) < 0)
    {
        return ret;
    }
    already_initialized = true;
    return 0;
}

int tmr_srv_int_callback_register(tmr_srv_int_src_t src,
                                  tmr_srv_int_callback_t callback)
{
    NRF_ASSERT((src < TMR_SRV_NB_OF_REQ_SRCS) && (callback != NULL));
    if (callbacks[src] == NULL)
    {

        callbacks[src] = callback;
        return 0;
    }
    LOG_ERR("Reg callback failed");

    // Otherwise - callback already exist
    return -EEXIST;
}


int tmr_srv_set_timeout(uint64_t timeout, uint32_t ctx)
{
    tmr_srv_int_src_t src = ((tmr_srv_int_context_t*)&ctx)->source;
    if (src >= TMR_SRV_NB_OF_REQ_SRCS)
    {

        return -EAGAIN;
    }
    return tmr_mngr_start(TMR_MNGR_MODE_ONE_SHOT, timeout, ctx);

}
