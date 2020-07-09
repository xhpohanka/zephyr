/*$$$LICENCE_NORDIC_STANDARD<2020>$$$*/
#ifndef EXT_SERVICE_COMMON_H__
#define EXT_SERVICE_COMMON_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup External services common data.
 * @{
 * @ingroup nrf_services
 * @brief Macros, types and structures definitions for all external services.
 */

#define EXT_SERVICE_PAYLOAD_GET(msg)    msg->payload


//TODO remove below
/** @brief Prism Dispatcher domains. */
typedef enum
{
    PRISM_DOMAIN_APP,     ///< Application domain.
    PRISM_DOMAIN_NET,     ///< Network domain.
    PRISM_DOMAIN_SECURE,  ///< Secure domain.
    PRISM_DOMAIN_MODEM,   ///< Modem domain.
    PRISM_DOMAIN_SYSCTRL, ///< System Controller domain.
} prism_domain_t;

/** @brief Prism Dispatcher message structure. */
typedef struct
{
    prism_domain_t domain_id; ///< Domain ID.
    uint8_t        ept_id;    ///< Endpoint ID.
    void *         payload;   ///< Pointer to the payload.
    size_t         size;      ///< Payload size in bytes.
} prism_dispatcher_msg_t;

/** @brief External service ID. */
typedef enum
{
    MSG_TYPE_TIMER_SERVICE,
} msg_type_t;

/** @brief Payload of system controller message for external system services. */
typedef struct
{
    msg_type_t type;
    uint32_t   app_ctx;
    void *     data;
    size_t     data_size;

} sysctl_request_payload_t;

/** @brief External services handler type. */
typedef void (*srv_ext_callback_t)(void * context);


#ifdef __cplusplus
}
#endif


#endif /* EXT_SERVICE_COMMON_H__ */
