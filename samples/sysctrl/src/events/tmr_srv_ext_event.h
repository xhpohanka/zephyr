/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#ifndef _TMR_SRV_EXT_EVENT_H_
#define _TMR_SRV_EXT_EVENT_H_

/**
 * @brief TMR_SRV_EXT Event
 * @defgroup tmr_srv_ext_event TMR_SRV_EXT Event
 * @{
 */

#include "event_manager.h"

#include <nrfs_phy.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tmr_srv_ext_event {
	struct event_header header;
	nrfs_phy_t *p_msg;
};

EVENT_TYPE_DECLARE(tmr_srv_ext_event);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _TMR_SRV_EXT_EVENT_H_ */
