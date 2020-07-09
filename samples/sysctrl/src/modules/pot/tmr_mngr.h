/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef TMR_MNGR_H__
#define TMR_MNGR_H__

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup tmr_mngr Timer manager module.
 * @{
 * @ingroup nrf_services
 * @brief Implementation of ordinary timeout fucntionalities.
 */

/** @brief Mode config type. */
typedef enum {
    TMR_MNGR_MODE_ONE_SHOT, //< Handler firing when reach given time and stop.
    TMR_MNGR_MODE_PERIODIC, //< Handler firing on each ticks and still running.
} tmr_mngr_mode_t;

/** @brief Timer manager module instance handler type. */
typedef void (*tmr_mngr_handler_t)(uint32_t context);

/**
 * @brief Function for initializing timer manager module.
 *
 * @retval 0      Operation finished successfully.
 * @retval -EPERM Handler is NULL.
 */
int tmr_mngr_init(tmr_mngr_handler_t p_handler);

/**
 * @brief Function for starting the timer with given configuration.
 *
 * @param mode     Timer mode.
 * @param value    Value of the timer mode. @ref tmr_mngr_mode_t
 *
 * @return          Index of the allocated and started timer.
 * @retval -EPERM   No free instance to start.
 * @retval -EEFAULT Requested @ref value is too close to set.
 * @retval -E2BIG   @ref value is too big to work with timer.
 */
int tmr_mngr_start(tmr_mngr_mode_t mode,
                   uint64_t        value,
                   uint32_t        context);

/**
 * @brief Function for stopping the timer.
 *
 * @param instance Timer instance.
 *
 * @retval 0            Operation finished successfully.
 * @retval -EPERM       Timer is not started.
 * @retval -EINPROGRESS Timer cannot be stopped now. It is scheduled and will be done in next 30.5us.
 * @retval -EFAULT      Timer was stopped from another context.
 */
int tmr_mngr_stop(uint8_t instance);

/**
 * @brief Function for getting current counter value.
 *
 * @return Current counter value.
 */
uint64_t tmr_mngr_cnt_get(void);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // TMR_MNGR_H__
