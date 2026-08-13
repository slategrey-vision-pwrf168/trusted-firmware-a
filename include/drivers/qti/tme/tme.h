/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_TME_TME_H
#define DRIVERS_QTI_TME_TME_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TME fuse address space types */
typedef enum {
	TME_QFPROM_ADDR_SPACE_RAW = 0,
	TME_QFPROM_ADDR_SPACE_CORR = 1,
	TME_QFPROM_ADDR_SPACE_MAX = 0x7FFFFFFF
} tme_qfprom_addr_space_t;

/* TME fuse write request structure */
typedef struct {
	uint32_t addr;
	uint32_t data[2];
} tme_fuse_t;

/* TME config register IDs */
typedef enum {
	TME_CONFIG_REG_QFPROM_BIST_CTRL = 1,
	TME_CONFIG_REG_QFPROM_WRITE_DISABLE_STICKY_BIT0 = 2,
	TME_CONFIG_REG_QFPROM_WRITE_DISABLE_STICKY_BIT1 = 3,
	TME_CONFIG_REG_MAX = 0xFF
} tme_config_register_id_t;

/*
 * Read a fuse row from QFPROM.
 *
 * @param addr_type: Address space type (raw or corrected)
 * @param fuse_addr: Fuse row address
 * @param fuse_data: Output buffer for fuse data (must hold 2 uint32_t)
 * @param qfprom_api_status: Output status from QFPROM API
 *
 * @return 0 on success, non-zero on failure
 */
int tme_fuse_read(tme_qfprom_addr_space_t addr_type, uint32_t fuse_addr,
		  uint32_t *fuse_data, uint32_t *qfprom_api_status);

/*
 * Write multiple fuse rows to QFPROM.
 *
 * @param fuse_array: Array of fuse write requests
 * @param fuse_array_len: Number of elements in fuse_array
 * @param qfprom_api_status: Output status from QFPROM API
 *
 * @return 0 on success, non-zero on failure
 */
int tme_fuse_write_multiple(tme_fuse_t *fuse_array, size_t fuse_array_len,
			    uint32_t *qfprom_api_status);

/*
 * Write a TME config register.
 *
 * @param register_id: Config register ID
 * @param value: Value to write
 *
 * @return 0 on success, non-zero on failure
 */
int tme_write_config_register(tme_config_register_id_t register_id,
			      uint32_t value);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_QTI_TME_TME_H */
