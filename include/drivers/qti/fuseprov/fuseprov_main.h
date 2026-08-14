/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_FUSEPROV_FUSEPROV_MAIN_H
#define DRIVERS_QTI_FUSEPROV_FUSEPROV_MAIN_H

#include <stdint.h>
#include <stddef.h>

/* SEC.DAT provisioning status codes */
typedef enum {
	FUSEPROV_STATUS_SUCCESS = 0,
	FUSEPROV_STATUS_INVALID_BUFFER = -1,
	FUSEPROV_STATUS_AUTH_FAILED = -2,
	FUSEPROV_STATUS_REGISTRATION_FAILED = -3,
	FUSEPROV_STATUS_PARSING_FAILED = -4,
	FUSEPROV_STATUS_FUSE_BLOW_FAILED = -5,
	FUSEPROV_STATUS_DEVICE_RESET_FAILED = -6,
} fuseprov_status_t;

/* SEC.DAT provisioning context */
typedef struct {
	uint8_t *buffer;           /* SEC.DAT buffer in memory */
	size_t buffer_len;         /* SEC.DAT buffer length */
	uint64_t buffer_addr;      /* Physical address of SEC.DAT buffer */
	uint32_t auth_token;       /* Authentication token from TME */
	uint32_t registration_id;  /* Registration ID from TME */
	uint32_t fuses_blown;      /* Number of fuses blown */
	uint32_t errors;           /* Error count during provisioning */
} fuseprov_context_t;

/* Main entry point: Load, authenticate, register, parse, and blow fuses
 *
 * This function implements the complete end-to-end SEC.DAT provisioning flow:
 * 1. Validate SEC.DAT buffer
 * 2. Authenticate SEC.DAT with TME
 * 3. Register SEC.DAT with TME
 * 4. Parse SEC.DAT v3 format
 * 5. Blow fuses via TME transport
 * 6. Reset device on completion
 *
 * @param buffer: Pointer to SEC.DAT buffer in memory
 * @param len: Length of SEC.DAT buffer
 * @param ctx: Output context with provisioning details
 *
 * @return FUSEPROV_STATUS_SUCCESS on success, error code otherwise
 */
fuseprov_status_t fuseprov_load_authenticate_and_blow(
	const uint8_t *buffer,
	size_t len,
	fuseprov_context_t *ctx);

/* Step 1: Validate SEC.DAT buffer
 *
 * @param buffer: Pointer to SEC.DAT buffer
 * @param len: Length of SEC.DAT buffer
 *
 * @return FUSEPROV_STATUS_SUCCESS on success, error code otherwise
 */
fuseprov_status_t fuseprov_validate_buffer(const uint8_t *buffer, size_t len);

/* Step 2: Authenticate SEC.DAT with TME
 *
 * Sends SEC.DAT to TME for authentication and receives auth token
 *
 * @param buffer: Pointer to SEC.DAT buffer
 * @param len: Length of SEC.DAT buffer
 * @param auth_token: Output authentication token from TME
 *
 * @return FUSEPROV_STATUS_SUCCESS on success, error code otherwise
 */
fuseprov_status_t fuseprov_authenticate_secdat(
	const uint8_t *buffer,
	size_t len,
	uint32_t *auth_token);

/* Step 3: Register SEC.DAT with TME
 *
 * Registers authenticated SEC.DAT with TME and receives registration ID
 *
 * @param buffer: Pointer to SEC.DAT buffer
 * @param len: Length of SEC.DAT buffer
 * @param auth_token: Authentication token from previous step
 * @param registration_id: Output registration ID from TME
 *
 * @return FUSEPROV_STATUS_SUCCESS on success, error code otherwise
 */
fuseprov_status_t fuseprov_register_secdat(
	const uint8_t *buffer,
	size_t len,
	uint32_t auth_token,
	uint32_t *registration_id);

/* Step 4: Parse and blow fuses
 *
 * Parses SEC.DAT and blows fuses via TME transport
 *
 * @param buffer: Pointer to SEC.DAT buffer
 * @param len: Length of SEC.DAT buffer
 * @param registration_id: Registration ID from previous step
 * @param fuses_blown: Output count of fuses blown
 *
 * @return FUSEPROV_STATUS_SUCCESS on success, error code otherwise
 */
fuseprov_status_t fuseprov_parse_and_blow_fuses(
	const uint8_t *buffer,
	size_t len,
	uint32_t registration_id,
	uint32_t *fuses_blown);

/* Step 5: Reset device
 *
 * Triggers device reset after successful fuse provisioning
 *
 * @return Does not return on success; returns error code on failure
 */
fuseprov_status_t fuseprov_reset_device(void);

#endif /* DRIVERS_QTI_FUSEPROV_FUSEPROV_MAIN_H */
