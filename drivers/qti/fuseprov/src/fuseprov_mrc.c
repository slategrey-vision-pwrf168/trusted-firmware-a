/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include "fuseprov_port.h"
#include "fuseprov_mrc_cfg.h"

/* MRC activation/revocation list update
 * Stub implementation - MRC checks are skipped
 */
fuseprov_err_t fuseprov_mrc_update(const fuseprov_transport_t *t,
				   uint32_t mrc_activation_list,
				   uint32_t mrc_revocation_list)
{
	(void)t;
	(void)mrc_activation_list;
	(void)mrc_revocation_list;

	/* MRC checks skipped - return success */
	NOTICE("Fuseprov: MRC update skipped (not required for this build)\n");
	return FUSEPROV_OK;
}
