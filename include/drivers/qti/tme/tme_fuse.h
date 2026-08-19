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

/* QFPROM operation completed successfully (QFPROM_ERR_CODE in qsee_fuse.h). */
#define QFPROM_NO_ERR		0
/* Last entry of QFPROM_ERR_CODE in qsee_fuse.h; used when the real QFPROM
 * status of a failure is not known.
 */
#define QFPROM_ERR_UNKNOWN	0x7FFFFFFF

/* Generic status codes returned by TmeFuseRead()/TmeFuseWriteMultiple()/
 * TmeWriteConfigRegister() (IxErrnoType in IxErrno.h): 0 on success,
 * non-zero on failure. Declared locally since IxErrno.h belongs to the
 * TME COM/Interface library and is not part of this driver.
 */
#define E_SUCCESS		0
#define E_NOT_SUPPORTED		4

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

/* PIL (Peripheral Image Loader) software ID for the authenticated sec.elf
 * image, as recorded by TME during boot.
 */
#define SEC_ELF_SS_SWID		0x002bU

/* DDR region of a PIL image that TME authenticated during boot. */
typedef struct {
	uint32_t startAddr;
	uint32_t endAddr;
} tmePilRegion_t;

/* Ask TME for the DDR region(s) of already-authenticated PIL image(s).
 * @swIdCount: in: number of entries in swIds; out: unused
 * @swIds: input array of software IDs to query (e.g. SEC_ELF_SS_SWID)
 * @regionListCount: in: capacity of regionList; out: number of entries filled
 * @regionList: output array of DDR regions, one per matched swId
 * @return: 0 on success, non-zero on failure
 */
int TmeGetPilImageRegions(uint32_t *const swIdCount, uint32_t *const swIds,
			  uint32_t *const regionListCount,
			  tmePilRegion_t *const regionList);

#endif /* DRIVERS_QTI_TME_FUSE_H */
