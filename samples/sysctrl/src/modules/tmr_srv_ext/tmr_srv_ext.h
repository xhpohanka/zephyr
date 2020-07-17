/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_SRV_EXT_H__
#define TMR_SRV_EXT_H__

#include <ext_service_common.h>
#include <tmr_srv_int.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint64_t timeout_value;
    uint8_t  settings_mask;
} timer_service_request_t;


/**
 * @defgroup External Timer Service module.
 * @{
 * @ingroup nrf_services
 * @brief This module includes timer external service which is one of
 *        available services passed by the filter module
 */



/**
 * @brief Function fetches filtered message from filter module, isolates
 *        timeout value and pass it to the timer backend module.
 */
int tmr_srv_ext_set_timeout(prism_dispatcher_msg_t *msg);

/**
 * @brief Function for initializing external timer module
 *
 * @param callback Pointer to callback function which is called
 *                 when timer has expired.
 *
 * @retval 0       Operation finished successfully
 */
int tmr_srv_ext_init(srv_ext_callback_t callback);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // TMR_SRV_EXT_H__
