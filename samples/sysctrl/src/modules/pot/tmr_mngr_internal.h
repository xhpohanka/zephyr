/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_MNGR_INTERNAL_H__
#define TMR_MNGR_INTERNAL_H__

#include <stdbool.h>
#include <stdint.h>
#include "tmr_mngr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup tmr_mngr Virtual RTC service module.
 * @{
 * @ingroup nrf_services
 * @brief Implementation of standard in and out functions for stdio library.
 */

typedef enum {
    TMR_MNGR_STATE_IDLE,    //< VRTC is not started.
    TMR_MNGR_STATE_STOPPED, //< VRTC is started and armed in the tree.
    TMR_MNGR_STATE_ARMED,   //< VRTC is started and armed in the tree.
} tmr_mngr_state_t;


/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // TMR_MNGR_H__
