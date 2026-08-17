/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/qti/tme/tme_fuse.h>
#include <drivers/qti/fuseprov/fuseprov_port_tme.h>

/* TME transport: read fuse row via TME COM */
static fuseprov_err_t tme_read_row(void *ctx, uint32_t addr,
				   fuseprov_addr_space_t space,
				   uint32_t out[2])
{
	(void)ctx;

	int ret = tme_fuse_read((tme_qfprom_addr_space_t)space, addr, out);
	if (ret != 0) {
		ERROR("Fuseprov: TME fuse read failed at addr 0x%x\n", addr);
		return FUSEPROV_ERR_TRANSPORT;
	}

	return FUSEPROV_OK;
}

/* TME transport: write fuse rows via TME COM */
static fuseprov_err_t tme_write_rows(void *ctx, const uint32_t addr[],
				     const uint64_t data[], uint32_t count,
				     uintptr_t *addr_err)
{
	(void)ctx;

	if (count == 0)
		return FUSEPROV_OK;

	if (count > 64) {
		ERROR("Fuseprov: too many fuses to write (%u > 64)\n", count);
		return FUSEPROV_ERR_ADDR_INVALID;
	}

	/* Convert to TME fuse array format */
	tme_fuse_t fuses[count];
	for (uint32_t i = 0; i < count; i++) {
		fuses[i].addr = addr[i];
		fuses[i].data[0] = (uint32_t)(data[i]);        /* LSB */
		fuses[i].data[1] = (uint32_t)(data[i] >> 32);  /* MSB */
	}

	int ret = tme_fuse_write_multiple(fuses, count);
	if (ret != 0) {
		ERROR("Fuseprov: TME fuse write failed\n");
		if (addr_err)
			*addr_err = addr[0];
		return FUSEPROV_ERR_TRANSPORT;
	}

	return FUSEPROV_OK;
}

/* Get the TME transport instance */
const fuseprov_transport_t *fuseprov_port_tme_get(void)
{
	static const fuseprov_transport_t tme_transport = {
		.read_row = tme_read_row,
		.write_rows = tme_write_rows,
		.ctx = NULL,
	};

	return &tme_transport;
}
