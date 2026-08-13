/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/qti/tme/tme.h>

/*
 * STUB IMPLEMENTATION — Placeholder for real TME SMC/mailbox transport.
 *
 * This stub returns failure for all TME operations. The actual implementation
 * will establish an SMC/mailbox channel to TME firmware and route fuse read/write
 * requests through that transport. This separation allows fuseprov logic to be
 * ported independently of the TME transport layer, which is a separate large task.
 *
 * TODO: Implement real TME COM channel (SMC/mailbox) and replace these stubs.
 */

int tme_fuse_read(tme_qfprom_addr_space_t addr_type, uint32_t fuse_addr,
		  uint32_t *fuse_data, uint32_t *qfprom_api_status)
{
	if (fuse_data == NULL || qfprom_api_status == NULL) {
		return -1;
	}

	WARN("TME fuse read not implemented (stub)\n");
	*qfprom_api_status = 0xFFFFFFFF;
	return -1;
}

int tme_fuse_write_multiple(tme_fuse_t *fuse_array, size_t fuse_array_len,
			    uint32_t *qfprom_api_status)
{
	if (fuse_array == NULL || fuse_array_len == 0 ||
	    qfprom_api_status == NULL) {
		return -1;
	}

	WARN("TME fuse write not implemented (stub)\n");
	*qfprom_api_status = 0xFFFFFFFF;
	return -1;
}

int tme_write_config_register(tme_config_register_id_t register_id,
			      uint32_t value)
{
	WARN("TME config register write not implemented (stub)\n");
	return -1;
}
