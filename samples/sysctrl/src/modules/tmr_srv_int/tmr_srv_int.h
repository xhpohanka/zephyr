/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_SRV_INT_H__
#define TMR_SRV_INT_H__

#include <stdint.h>
#include <tmr_srv_int_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Internal Timer Service module.
 * @{
 * @ingroup nrf_services
 * @brief Implementation of system controller internal timer module.
 */

/**
 * @brief groups of request sources.
 */
typedef enum
{
#if TIM_SRV_ENABLE_PM_SRC
    TMR_SRV_PM_REQ_SRC,    //< requests from Power Management.
#endif
#if TIM_SRV_ENABLE_EXT_SRC
    TMR_SRV_EXT_REQ_SRC,   //< Requests from Timer External Module.
#endif
    TMR_SRV_NB_OF_REQ_SRCS //< Total number of request sources.
} tmr_srv_int_src_t;

/**
 * @brief struct of data passed with timer requests.
 */
typedef struct {
    tmr_srv_int_src_t source;
    uint8_t id;
} tmr_srv_int_context_t;

/** @brief Timer internal service handler type. */
typedef void (*tmr_srv_int_callback_t)(tmr_srv_int_context_t * context);

/**
 * @brief Function for initializing internal timer service module.
 *
 * @retval 0        Operation finished successfully.
 */
int tmr_srv_int_init(void);

/**
 * @brief Function for registering callback which is called when timer event occurs
 *
 * @param src       id of source
 * @param callback pointer to the callback function
 *
 * @retval 0        Operation finished successfully
 */
int tmr_srv_int_callback_register(tmr_srv_int_src_t src,
                                  tmr_srv_int_callback_t callback);

/**
 * @brief Function for setting absolute timeout value.
 *
 * @param timestamp absolute timestamp value.
 * @param ctx        context
 *
 * @retval 0        Operation finished successfully.
 * @retval -EPERM   Timer instance is already started.
 * @retval -EEFAULT Requested @ref value is too close to set.
 * @retval -E2BIG   @ref value is too big to work with timer.
 */
int tmr_srv_set_timeout(uint64_t timeout, uint32_t ctx);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // TMR_SRV_INT_H__
