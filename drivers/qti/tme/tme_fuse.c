/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <string.h>
#include "tme_fuse.h"

/* TME message tags from TmeMessagesTags.h */
#define TME_MSG_CBOR_TAG_FUSE_READ 301
#define TME_MSG_CBOR_TAG_FUSE_WRITE_MULTIPLE 316
#define TME_MSG_CBOR_TAG_WRITE_CONFIG_REGISTER 334

/* Error codes */
#define E_SUCCESS 0
#define E_FAILURE -1

/* Forward declarations for TME COM interface */
extern int TmeForwardRequest(void *reqBuf, size_t reqSize,
			     void *rspBuf, size_t *rspBufSize);

/* TME fuse read request structure */
typedef struct {
	uint32_t addr_space;  /* 0 = raw, 1 = corrected */
	uint32_t addr;
} __attribute__((packed)) tme_fuse_read_req_t;

/* TME fuse read response structure */
typedef struct {
	uint32_t status;
	uint32_t data[2];  /* LSB and MSB */
} __attribute__((packed)) tme_fuse_read_rsp_t;

/* TME fuse write entry */
typedef struct {
	uint32_t addr;
	uint32_t data[2];  /* LSB and MSB */
} __attribute__((packed)) tme_fuse_write_entry_t;

/* TME fuse write multiple request structure */
typedef struct {
	uint32_t count;
	tme_fuse_write_entry_t fuses[64];  /* Max 64 fuses per request */
} __attribute__((packed)) tme_fuse_write_multiple_req_t;

/* TME fuse write multiple response structure */
typedef struct {
	uint32_t status;
	uint32_t failed_index;  /* Index of first failed fuse, or 0xFFFFFFFF if all succeeded */
} __attribute__((packed)) tme_fuse_write_multiple_rsp_t;

/* TME write config register request structure */
typedef struct {
	uint32_t reg_id;
	uint32_t value;
} __attribute__((packed)) tme_write_config_register_req_t;

/* TME write config register response structure */
typedef struct {
	uint32_t status;
} __attribute__((packed)) tme_write_config_register_rsp_t;

/* TME fuse read operation */
int tme_fuse_read(tme_qfprom_addr_space_t space, uint32_t addr, uint32_t out[2])
{
	tme_fuse_read_req_t req;
	tme_fuse_read_rsp_t rsp;
	size_t rsp_size = sizeof(rsp);
	int ret;

	if (out == NULL) {
		ERROR("TME: fuse read - invalid output buffer\n");
		return E_FAILURE;
	}

	/* Build request */
	req.addr_space = (uint32_t)space;
	req.addr = addr;

	/* Send request to TME via TmeForwardRequest */
	ret = TmeForwardRequest(&req, sizeof(req), &rsp, &rsp_size);
	if (ret != E_SUCCESS) {
		ERROR("TME: fuse read failed - TmeForwardRequest error %d\n", ret);
		return ret;
	}

	/* Check response size */
	if (rsp_size < sizeof(rsp)) {
		ERROR("TME: fuse read - response too small (%zu < %zu)\n",
		      rsp_size, sizeof(rsp));
		return E_FAILURE;
	}

	/* Check TME status */
	if (rsp.status != E_SUCCESS) {
		ERROR("TME: fuse read failed - TME status %d\n", rsp.status);
		return rsp.status;
	}

	/* Copy fuse data to output */
	out[0] = rsp.data[0];
	out[1] = rsp.data[1];

	return E_SUCCESS;
}

/* TME fuse write multiple operation */
int tme_fuse_write_multiple(const tme_fuse_t *fuses, uint32_t count)
{
	tme_fuse_write_multiple_req_t req;
	tme_fuse_write_multiple_rsp_t rsp;
	size_t rsp_size = sizeof(rsp);
	uint32_t i;
	int ret;

	if (fuses == NULL || count == 0) {
		ERROR("TME: fuse write - invalid input\n");
		return E_FAILURE;
	}

	if (count > 64) {
		ERROR("TME: fuse write - too many fuses (%u > 64)\n", count);
		return E_FAILURE;
	}

	/* Build request */
	req.count = count;
	for (i = 0; i < count; i++) {
		req.fuses[i].addr = fuses[i].addr;
		req.fuses[i].data[0] = fuses[i].data[0];
		req.fuses[i].data[1] = fuses[i].data[1];
	}

	/* Send request to TME via TmeForwardRequest */
	ret = TmeForwardRequest(&req, sizeof(req), &rsp, &rsp_size);
	if (ret != E_SUCCESS) {
		ERROR("TME: fuse write failed - TmeForwardRequest error %d\n", ret);
		return ret;
	}

	/* Check response size */
	if (rsp_size < sizeof(rsp)) {
		ERROR("TME: fuse write - response too small (%zu < %zu)\n",
		      rsp_size, sizeof(rsp));
		return E_FAILURE;
	}

	/* Check TME status */
	if (rsp.status != E_SUCCESS) {
		ERROR("TME: fuse write failed - TME status %d (failed at index %u)\n",
		      rsp.status, rsp.failed_index);
		return rsp.status;
	}

	return E_SUCCESS;
}

/* TME write config register operation */
int tme_write_config_register(uint32_t reg_id, uint32_t value)
{
	tme_write_config_register_req_t req;
	tme_write_config_register_rsp_t rsp;
	size_t rsp_size = sizeof(rsp);
	int ret;

	/* Build request */
	req.reg_id = reg_id;
	req.value = value;

	/* Send request to TME via TmeForwardRequest */
	ret = TmeForwardRequest(&req, sizeof(req), &rsp, &rsp_size);
	if (ret != E_SUCCESS) {
		ERROR("TME: write config register failed - TmeForwardRequest error %d\n", ret);
		return ret;
	}

	/* Check response size */
	if (rsp_size < sizeof(rsp)) {
		ERROR("TME: write config register - response too small (%zu < %zu)\n",
		      rsp_size, sizeof(rsp));
		return E_FAILURE;
	}

	/* Check TME status */
	if (rsp.status != E_SUCCESS) {
		ERROR("TME: write config register failed - TME status %d\n", rsp.status);
		return rsp.status;
	}

	return E_SUCCESS;
}
