/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr.h>

/*$$$LICENCE_NORDIC_STANDARD<2018>$$$*/

/**
 * @defgroup empty_example_main Empty application
 * @{
 * @ingroup nrfx_examples
 *
 * @brief Empty example Application main file.
 *
 * This file contains the source code for a sample application with logger subsystem.
 *
 * Please connect development kit and configure following options when opening COM port:
 *  - Baudrate:   115200
 *  - Parity:     None
 *  - Stop bits:  1
 *  - Data bits:  8
 *
 * @}
 */
#include <stdlib.h>
#include <stdint.h>

#include <logging/log.h>
#include <event_manager.h>
#include <status_event.h>

LOG_MODULE_REGISTER(MAIN, LOG_LEVEL_INF);

/**
 * @brief Function for application main entry.
 */
int main(void)
{
	LOG_ERR("Start");
	if (event_manager_init()) {
		LOG_ERR("Event Manager not initialized");
	} else {
		struct status_event *event = new_status_event();
		event->status = STATUS_INIT;
		EVENT_SUBMIT(event);
	}

	return 0;
}
