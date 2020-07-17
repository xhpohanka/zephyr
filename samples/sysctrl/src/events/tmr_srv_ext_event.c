/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stdio.h>

#include "tmr_srv_ext_event.h"


static void profile_tmr_srv_ext_event(struct log_event_buf *buf,
				      const struct event_header *eh)
{
	struct tmr_srv_ext_event *event = cast_tmr_srv_ext_event(eh);

	profiler_log_encode_u32(buf, event->p_msg->domain_id);
}

static int log_tmr_srv_ext_event(const struct event_header *eh, char *buf,
				 size_t buf_len)
{
	struct tmr_srv_ext_event *event = cast_tmr_srv_ext_event(eh);

	return snprintf(buf, buf_len, "dom=%u,size=%u",
			event->p_msg->domain_id, event->p_msg->size);
}

EVENT_INFO_DEFINE(tmr_srv_ext_event,
		  ENCODE(PROFILER_ARG_U8),
		  ENCODE("domain"),
		  profile_tmr_srv_ext_event);

EVENT_TYPE_DEFINE(tmr_srv_ext_event,
		  true,
		  log_tmr_srv_ext_event,
		  &tmr_srv_ext_event_info);
