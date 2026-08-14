/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/qti/fuseprov/fuseprov_main.h>

/* SMC function IDs for fuse provisioning */
#define QTI_FUSEPROV_SMC_LOAD_AND_BLOW 0x02000100

/* SMC handler for fuse provisioning
 *
 * This handler receives SEC.DAT buffer details via SMC and orchestrates
 * the complete end-to-end provisioning flow:
 * 1. Validate SEC.DAT buffer
 * 2. Authenticate SEC.DAT with TME
 * 3. Register SEC.DAT with TME
 * 4. Parse SEC.DAT v3 format
 * 5. Blow fuses via TME transport
 * 6. Reset device on completion
 *
 * SMC Arguments:
 *   x1: SEC.DAT buffer address (lower 32 bits)
 *   x2: SEC.DAT buffer address (upper 32 bits)
 *   x3: SEC.DAT buffer length
 *
 * SMC Return:
 *   x0: Status code (0 = success, negative = error)
 *   x1: Number of fuses blown
 *   x2: Error count
 */
uint64_t qti_fuseprov_smc_handler(uint32_t smc_fid,
				  uint64_t x1, uint64_t x2, uint64_t x3,
				  void *cookie,
				  void *handle,
				  uint64_t flags)
{
	uint64_t buffer_addr;
	size_t buffer_len;
	const uint8_t *buffer;
	fuseprov_context_t ctx;
	fuseprov_status_t status;

	(void)cookie;
	(void)handle;
	(void)flags;

	switch (smc_fid) {
	case QTI_FUSEPROV_SMC_LOAD_AND_BLOW:
		/* Extract buffer address and length from SMC arguments */
		buffer_addr = ((uint64_t)x2 << 32) | (x1 & 0xFFFFFFFF);
		buffer_len = (size_t)x3;
		buffer = (const uint8_t *)(uintptr_t)buffer_addr;

		NOTICE("Fuseprov SMC: Load and blow fuses\n");
		NOTICE("Fuseprov SMC: Buffer: 0x%llx, Length: %zu\n",
		       buffer_addr, buffer_len);

		/* Execute end-to-end provisioning flow */
		status = fuseprov_load_authenticate_and_blow(buffer, buffer_len, &ctx);

		/* Return results to caller */
		SMC_RET3(handle, status, ctx.fuses_blown, ctx.errors);
		break;

	default:
		WARN("Fuseprov SMC: Unknown function ID 0x%x\n", smc_fid);
		SMC_RET1(handle, -1);
		break;
	}

	return 0;
}
