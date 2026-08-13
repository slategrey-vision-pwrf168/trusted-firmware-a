/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_FUSEPROV_FUSEPROV_H
#define DRIVERS_QTI_FUSEPROV_FUSEPROV_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fuseprov error codes */
typedef enum {
	FUSEPROV_SUCCESS = 0,
	FUSEPROV_FAILURE = 1,
	FUSEPROV_INVALID_ARG = 2,
	FUSEPROV_NO_MEMORY = 3,
	FUSEPROV_SECDAT_MAGIC_MISMATCH = 4,
	FUSEPROV_SECDAT_REV_NOT_SUPPORTED = 5,
	FUSEPROV_SECDAT_SIZE_LEN_MISMATCH = 6,
	FUSEPROV_QFPROM_READ_ERROR = 7,
	FUSEPROV_QFUSE_REV_NOT_SUPPORTED = 8,
	FUSEPROV_INVALID_HASH = 9,
	FUSEPROV_SECDAT_LOCK_BLOWN = 10,
	FUSEPROV_QFPROM_WRITE_ERROR = 11,
	FUSEPROV_SHK_RD_WR_DISABLE_BLOWN = 12,
	FUSEPROV_SHK_GENERATION_FAILED = 13,
	FUSEPROV_SHK_ALRDY_BLOWN = 14,
	FUSEPROV_SHK_RD_WR_MISMATCH = 15,
	FUSEPROV_SECDAT_DEFAULT_NOFUSES = 16,
	FUSEPROV_SEGMENT_NOT_FOUND = 17,
	FUSEPROV_SECDAT_SEGMENT_NUM_NOT_SUPPORTED = 18,
	FUSEPROV_SECDAT_HASH_SIZE_MISMATCH = 19,
	FUSEPROV_OEM_SPARE_ALRDY_BLOWN = 20,
	FUSEPROV_OEM_SPARE_RAND_GEN_FAILED = 21,
	FUSEPROV_INVALID_REGION_TYPE = 22,
	FUSEPROV_INVALID_OPERATION_TYPE = 23,
} fuseprov_error_t;

/*
 * Parse and blow fuses from a SEC.DAT buffer.
 *
 * @param secdat_buffer: Pointer to SEC.DAT buffer
 * @param secdat_len: Length of SEC.DAT buffer
 *
 * @return FUSEPROV_SUCCESS on success, error code otherwise
 */
int fuseprov_blow_fuses(const uint8_t *secdat_buffer, size_t secdat_len);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_QTI_FUSEPROV_FUSEPROV_H */
