/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_MNGR_BACKEND_H__
#define TMR_MNGR_BACKEND_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup tmr_mngr Virtual RTC service module.
 * @{
 * @ingroup nrf_services
 * @brief Implementation of standard in and out functions for stdio library.
 */

void     tmr_back_init(void);
int      tmr_back_cc_sync_handler(uint64_t cc_value);
int      tmr_back_cc_handler(uint64_t cc_value);
void     tmr_back_start_handler(void);
void     tmr_back_stop_handler(void);
uint64_t tmr_back_cnt_get_handler(void);
bool     tmr_back_constr_check_handler(uint64_t cc_value);

void tmr_mngr_back_cc_irq(void);


/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // TMR_MNGR_BACKEND_H__
