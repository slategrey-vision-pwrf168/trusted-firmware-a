/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_TME_FUSE_H
#define DRIVERS_QTI_TME_FUSE_H

#include <stdint.h>
#include <stddef.h>

/* TME QFPROM address space types */
typedef enum {
	TME_QFPROM_ADDR_SPACE_RAW = 0,
	TME_QFPROM_ADDR_SPACE_CORRECTED = 1,
} tme_qfprom_addr_space_t;

/* TME fuse data structure for write operations */
typedef struct {
	uint32_t addr;
	uint32_t data[2];  /* LSB and MSB */
} tme_fuse_t;

/* TME fuse read operation
 * Reads a single fuse row from QFPROM via TME COM
 *
 * @param space: Address space (raw or corrected)
 * @param addr: Fuse row address
 * @param out: Output buffer for fuse data (2 x uint32_t for LSB/MSB)
 *
 * @return 0 on success, error code otherwise
 */
int tme_fuse_read(tme_qfprom_addr_space_t space, uint32_t addr, uint32_t out[2]);

/* TME fuse write multiple operation
 * Writes multiple fuse rows to QFPROM via TME COM
 *
 * @param fuses: Array of fuse structures to write
 * @param count: Number of fuses to write
 *
 * @return 0 on success, error code otherwise
 */
int tme_fuse_write_multiple(const tme_fuse_t *fuses, uint32_t count);

/* TME write config register operation
 * Writes to a QFPROM configuration register via TME COM
 *
 * @param reg_id: Configuration register ID
 * @param value: Value to write
 *
 * @return 0 on success, error code otherwise
 */
int tme_write_config_register(uint32_t reg_id, uint32_t value);

#endif /* DRIVERS_QTI_TME_FUSE_H */
