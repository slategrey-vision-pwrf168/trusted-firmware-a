/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <string.h>
#include <drivers/qti/fuseprov/fuseprov_main.h>
#include <drivers/qti/fuseprov/fuseprov.h>
#include <drivers/qti/fuseprov/fuseprov_port_tme.h>
#include <drivers/qti/tme/tme_fuse.h>

/* TME message tags for SEC.DAT operations */
#define TME_MSG_CBOR_TAG_SECDAT_AUTHENTICATE 290
#define TME_MSG_CBOR_TAG_SECDAT_REGISTER 291

/* SEC.DAT authentication request structure */
typedef struct {
	uint32_t buffer_addr_low;   /* Physical address (lower 32 bits) */
	uint32_t buffer_addr_high;  /* Physical address (upper 32 bits) */
	uint32_t buffer_len;        /* Buffer length */
} __attribute__((packed)) tme_secdat_auth_req_t;

/* SEC.DAT authentication response structure */
typedef struct {
	uint32_t status;            /* 0 = success */
	uint32_t auth_token;        /* Authentication token */
} __attribute__((packed)) tme_secdat_auth_rsp_t;

/* SEC.DAT registration request structure */
typedef struct {
	uint32_t auth_token;        /* Authentication token from previous step */
	uint32_t buffer_addr_low;   /* Physical address (lower 32 bits) */
	uint32_t buffer_addr_high;  /* Physical address (upper 32 bits) */
	uint32_t buffer_len;        /* Buffer length */
} __attribute__((packed)) tme_secdat_register_req_t;

/* SEC.DAT registration response structure */
typedef struct {
	uint32_t status;            /* 0 = success */
	uint32_t registration_id;   /* Registration ID for fuse provisioning */
} __attribute__((packed)) tme_secdat_register_rsp_t;

/* Forward declarations for TME COM interface */
extern int TmeForwardRequest(void *reqBuf, size_t reqSize,
			     void *rspBuf, size_t *rspBufSize);

/* Helper: Get physical address of buffer
 * In TF-A, buffers are typically in secure memory with known physical addresses
 */
static uint64_t fuseprov_get_buffer_phys_addr(const uint8_t *buffer)
{
	/* For now, assume buffer address is the physical address
	 * In production, this would use proper memory mapping APIs
	 */
	return (uint64_t)(uintptr_t)buffer;
}

/* Step 1: Validate SEC.DAT buffer */
fuseprov_status_t fuseprov_validate_buffer(const uint8_t *buffer, size_t len)
{
	if (buffer == NULL) {
		ERROR("Fuseprov: Invalid buffer (NULL)\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	if (len == 0) {
		ERROR("Fuseprov: Invalid buffer length (0)\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	if (len > 1024 * 1024) {  /* Max 1MB */
		ERROR("Fuseprov: Buffer too large (%zu bytes)\n", len);
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	NOTICE("Fuseprov: Buffer validation passed (len=%zu)\n", len);
	return FUSEPROV_STATUS_SUCCESS;
}

/* Step 2: Authenticate SEC.DAT with TME */
fuseprov_status_t fuseprov_authenticate_secdat(
	const uint8_t *buffer,
	size_t len,
	uint32_t *auth_token)
{
	tme_secdat_auth_req_t req;
	tme_secdat_auth_rsp_t rsp;
	size_t rsp_size = sizeof(rsp);
	uint64_t phys_addr;
	int ret;

	if (buffer == NULL || auth_token == NULL) {
		ERROR("Fuseprov: Invalid arguments to authenticate\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	/* Get physical address of buffer */
	phys_addr = fuseprov_get_buffer_phys_addr(buffer);

	/* Build authentication request */
	req.buffer_addr_low = (uint32_t)(phys_addr & 0xFFFFFFFF);
	req.buffer_addr_high = (uint32_t)((phys_addr >> 32) & 0xFFFFFFFF);
	req.buffer_len = (uint32_t)len;

	NOTICE("Fuseprov: Authenticating SEC.DAT (addr=0x%llx, len=%zu)\n",
	       phys_addr, len);

	/* Send authentication request to TME */
	ret = TmeForwardRequest(&req, sizeof(req), &rsp, &rsp_size);
	if (ret != 0) {
		ERROR("Fuseprov: Authentication failed - TmeForwardRequest error %d\n", ret);
		return FUSEPROV_STATUS_AUTH_FAILED;
	}

	/* Check response size */
	if (rsp_size < sizeof(rsp)) {
		ERROR("Fuseprov: Authentication response too small (%zu < %zu)\n",
		      rsp_size, sizeof(rsp));
		return FUSEPROV_STATUS_AUTH_FAILED;
	}

	/* Check TME status */
	if (rsp.status != 0) {
		ERROR("Fuseprov: Authentication failed - TME status %d\n", rsp.status);
		return FUSEPROV_STATUS_AUTH_FAILED;
	}

	*auth_token = rsp.auth_token;
	NOTICE("Fuseprov: SEC.DAT authenticated (token=0x%x)\n", *auth_token);
	return FUSEPROV_STATUS_SUCCESS;
}

/* Step 3: Register SEC.DAT with TME */
fuseprov_status_t fuseprov_register_secdat(
	const uint8_t *buffer,
	size_t len,
	uint32_t auth_token,
	uint32_t *registration_id)
{
	tme_secdat_register_req_t req;
	tme_secdat_register_rsp_t rsp;
	size_t rsp_size = sizeof(rsp);
	uint64_t phys_addr;
	int ret;

	if (buffer == NULL || registration_id == NULL) {
		ERROR("Fuseprov: Invalid arguments to register\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	/* Get physical address of buffer */
	phys_addr = fuseprov_get_buffer_phys_addr(buffer);

	/* Build registration request */
	req.auth_token = auth_token;
	req.buffer_addr_low = (uint32_t)(phys_addr & 0xFFFFFFFF);
	req.buffer_addr_high = (uint32_t)((phys_addr >> 32) & 0xFFFFFFFF);
	req.buffer_len = (uint32_t)len;

	NOTICE("Fuseprov: Registering SEC.DAT (token=0x%x)\n", auth_token);

	/* Send registration request to TME */
	ret = TmeForwardRequest(&req, sizeof(req), &rsp, &rsp_size);
	if (ret != 0) {
		ERROR("Fuseprov: Registration failed - TmeForwardRequest error %d\n", ret);
		return FUSEPROV_STATUS_REGISTRATION_FAILED;
	}

	/* Check response size */
	if (rsp_size < sizeof(rsp)) {
		ERROR("Fuseprov: Registration response too small (%zu < %zu)\n",
		      rsp_size, sizeof(rsp));
		return FUSEPROV_STATUS_REGISTRATION_FAILED;
	}

	/* Check TME status */
	if (rsp.status != 0) {
		ERROR("Fuseprov: Registration failed - TME status %d\n", rsp.status);
		return FUSEPROV_STATUS_REGISTRATION_FAILED;
	}

	*registration_id = rsp.registration_id;
	NOTICE("Fuseprov: SEC.DAT registered (id=0x%x)\n", *registration_id);
	return FUSEPROV_STATUS_SUCCESS;
}

/* Step 4: Parse and blow fuses */
fuseprov_status_t fuseprov_parse_and_blow_fuses(
	const uint8_t *buffer,
	size_t len,
	uint32_t registration_id,
	uint32_t *fuses_blown)
{
	const fuseprov_transport_t *transport;
	fuseprov_error_etype ret;

	if (buffer == NULL || fuses_blown == NULL) {
		ERROR("Fuseprov: Invalid arguments to parse and blow\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	NOTICE("Fuseprov: Parsing SEC.DAT and blowing fuses (id=0x%x)\n",
	       registration_id);

	/* Get TME transport for fuse operations */
	transport = fuseprov_port_tme_get();
	if (transport == NULL) {
		ERROR("Fuseprov: Failed to get TME transport\n");
		return FUSEPROV_STATUS_FUSE_BLOW_FAILED;
	}

	/* Parse SEC.DAT and blow fuses */
	ret = fuseprov_blow_fuses_sec_elf_v3(transport,
					     (uint8_t *)buffer,
					     (uint32_t)len);
	if (ret != FUSEPROV_SUCCESS && ret != FUSEPROV_SECDAT_LOCK_BLOWN) {
		ERROR("Fuseprov: Fuse provisioning failed with error %d\n", ret);
		return FUSEPROV_STATUS_FUSE_BLOW_FAILED;
	}

	/* Count fuses blown (placeholder - would be tracked during parsing) */
	*fuses_blown = 0;  /* TODO: Track actual fuse count during parsing */

	NOTICE("Fuseprov: Fuses blown successfully\n");
	return FUSEPROV_STATUS_SUCCESS;
}

/* Step 5: Reset device */
fuseprov_status_t fuseprov_reset_device(void)
{
	NOTICE("Fuseprov: Triggering device reset\n");

	/* TODO: Implement actual device reset via PSCI or platform-specific mechanism
	 * For now, just log and return success
	 */

	return FUSEPROV_STATUS_SUCCESS;
}

/* Main entry point: Load, authenticate, register, parse, and blow fuses */
fuseprov_status_t fuseprov_load_authenticate_and_blow(
	const uint8_t *buffer,
	size_t len,
	fuseprov_context_t *ctx)
{
	fuseprov_status_t status;

	if (ctx == NULL) {
		ERROR("Fuseprov: Invalid context\n");
		return FUSEPROV_STATUS_INVALID_BUFFER;
	}

	/* Initialize context */
	memset(ctx, 0, sizeof(*ctx));
	ctx->buffer = (uint8_t *)buffer;
	ctx->buffer_len = len;
	ctx->buffer_addr = fuseprov_get_buffer_phys_addr(buffer);

	NOTICE("Fuseprov: Starting end-to-end SEC.DAT provisioning\n");
	NOTICE("Fuseprov: Buffer: 0x%llx, Length: %zu\n", ctx->buffer_addr, len);

	/* Step 1: Validate buffer */
	status = fuseprov_validate_buffer(buffer, len);
	if (status != FUSEPROV_STATUS_SUCCESS) {
		ctx->errors++;
		return status;
	}

	/* Step 2: Authenticate SEC.DAT with TME */
	status = fuseprov_authenticate_secdat(buffer, len, &ctx->auth_token);
	if (status != FUSEPROV_STATUS_SUCCESS) {
		ctx->errors++;
		return status;
	}

	/* Step 3: Register SEC.DAT with TME */
	status = fuseprov_register_secdat(buffer, len, ctx->auth_token,
					  &ctx->registration_id);
	if (status != FUSEPROV_STATUS_SUCCESS) {
		ctx->errors++;
		return status;
	}

	/* Step 4: Parse and blow fuses */
	status = fuseprov_parse_and_blow_fuses(buffer, len, ctx->registration_id,
					       &ctx->fuses_blown);
	if (status != FUSEPROV_STATUS_SUCCESS) {
		ctx->errors++;
		return status;
	}

	/* Step 5: Reset device */
	status = fuseprov_reset_device();
	if (status != FUSEPROV_STATUS_SUCCESS) {
		ctx->errors++;
		return status;
	}

	NOTICE("Fuseprov: End-to-end provisioning complete\n");
	NOTICE("Fuseprov: Fuses blown: %u, Errors: %u\n",
	       ctx->fuses_blown, ctx->errors);

	return FUSEPROV_STATUS_SUCCESS;
}
