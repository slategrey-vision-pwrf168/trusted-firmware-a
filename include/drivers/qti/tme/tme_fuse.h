/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_TME_FUSE_H
#define DRIVERS_QTI_TME_FUSE_H

#include <stddef.h>
#include <stdint.h>

/*
 * This header intentionally mirrors the naming of the reference TME
 * interface (TmeInterfaces.h / TmeInterfacesDefs.h) rather than this
 * project's usual lower_snake_case convention, so that fuseprov_port_tme.c
 * matches the reference transport port line-for-line.
 */

/* QFPROM address space selector. */
typedef enum {
	TME_QFPROM_ADDR_SPACE_RAW = 0,
	TME_QFPROM_ADDR_SPACE_CORR = 1,
	TME_QFPROM_ADDR_SPACE_MAX = 0x7FFFFFFF,
} TMEQFPROMAddrSpace_t;

/* Single fuse row: address plus 64-bit value split as [LSB, MSB]. */
typedef struct {
	uint32_t addr;
	uint32_t data[2];
} TMEFuse_t;

/* Maximum number of fuse rows in one TmeFuseWriteMultiple() request. */
#define TME_MAX_FUSE_WRITE_REQ 64

/* QFPROM configuration register identifiers. */
typedef enum {
	QFPROM_BIST_CTRL = 1,
	QFPROM_WRITE_DISABLE_STICKY_BIT0,
	QFPROM_WRITE_DISABLE_STICKY_BIT1,
	TME_WRITE_CONFIG_REGISTER_MAX = 0xFF,
} tmeConfigRegisterId_e;

/* QFPROM operation completed successfully. */
#define QFPROM_NO_ERR 0

/* Read a single fuse row via TME.
 * @addrType: raw or corrected address space
 * @fuseAddr: fuse row address
 * @fuseData: output buffer for row data [LSB, MSB]
 * @qfpromApiStatus: output QFPROM status code (QFPROM_NO_ERR on success)
 * @return: 0 on success, non-zero on failure
 */
int TmeFuseRead(TMEQFPROMAddrSpace_t addrType, uint32_t fuseAddr,
		uint32_t *const fuseData, uint32_t *const qfpromApiStatus);

/* Write multiple fuse rows via TME.
 * @fuseArray: array of fuse rows to write
 * @fuseArrayLen: number of entries in fuseArray (<= TME_MAX_FUSE_WRITE_REQ)
 * @qfpromApiStatus: output QFPROM status code (QFPROM_NO_ERR on success)
 * @return: 0 on success, non-zero on failure
 */
int TmeFuseWriteMultiple(TMEFuse_t *fuseArray, size_t fuseArrayLen,
			 uint32_t *const qfpromApiStatus);

/* Write a QFPROM configuration register via TME.
 * @registerId: configuration register identifier
 * @value: value to write
 * @return: 0 on success, non-zero on failure
 */
int TmeWriteConfigRegister(tmeConfigRegisterId_e registerId, uint32_t value);

#endif /* DRIVERS_QTI_TME_FUSE_H */
