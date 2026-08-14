/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FUSEPROV_MRC_CFG_H
#define FUSEPROV_MRC_CFG_H

#include <stdint.h>

/* FEC range entry for rows that require FEC calculation */
typedef struct {
	uint32_t start_addr;
	uint32_t end_addr;
} fuseprov_fec_range_t;

/* Portable MRC configuration defaults
 * Chipset-specific overrides should be in nord/fuseprov_defs.h
 */

/* MRC fuse constants - chipset-specific, fill in via fuseprov_defs.h */
#ifndef FUSEPROV_MRC_ACTIVATION_LIST_ADDR
#define FUSEPROV_MRC_ACTIVATION_LIST_ADDR    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_ACTIVATION_LIST_BMSK
#define FUSEPROV_MRC_ACTIVATION_LIST_BMSK    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_ACTIVATION_LIST_SHFT
#define FUSEPROV_MRC_ACTIVATION_LIST_SHFT    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_REVOCATION_LIST_ADDR
#define FUSEPROV_MRC_REVOCATION_LIST_ADDR    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_REVOCATION_LIST_BMSK
#define FUSEPROV_MRC_REVOCATION_LIST_BMSK    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_REVOCATION_LIST_SHFT
#define FUSEPROV_MRC_REVOCATION_LIST_SHFT    0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_STICKY_BIT_HWIOSYM
#define FUSEPROV_MRC_STICKY_BIT_HWIOSYM      0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_MRC_WRITE_DISABLE_STICKY_BIT_BMSK
#define FUSEPROV_MRC_WRITE_DISABLE_STICKY_BIT_BMSK  0  /* NEEDSWORK: IPCatalog */
#endif

/* Permission fuse constants - chipset-specific, fill in via fuseprov_defs.h */
#ifndef FUSEPROV_WR_PERM_HWIO_ADDR
#define FUSEPROV_WR_PERM_HWIO_ADDR          0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_SECBOOT_WR_PERM_DISABLE_BMSK
#define FUSEPROV_SECBOOT_WR_PERM_DISABLE_BMSK  0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_RD_PERM_HWIO_ADDR
#define FUSEPROV_RD_PERM_HWIO_ADDR          0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_SECBOOT_RD_PERM_DISABLE_BMSK
#define FUSEPROV_SECBOOT_RD_PERM_DISABLE_BMSK  0  /* NEEDSWORK: IPCatalog */
#endif

/* FEC range table - chipset-specific, fill in via fuseprov_defs.h */
#ifndef FUSEPROV_FEC_RANGES_DEFINED
static const fuseprov_fec_range_t fuseprov_fec_ranges[] = {
	{ 0x0, 0x0 }  /* sentinel - NEEDSWORK: add real ranges */
};
#endif

/* SEC.DAT buffer configuration - chipset-specific, fill in via fuseprov_defs.h */
#ifndef FUSEPROV_SECDAT_BUFFER_BASE
#define FUSEPROV_SECDAT_BUFFER_BASE         0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_SECDAT_BUFFER_SIZE
#define FUSEPROV_SECDAT_BUFFER_SIZE         0  /* NEEDSWORK: IPCatalog */
#endif

/* BIST control register addresses - chipset-specific */
#ifndef FUSEPROV_BIST_CTRL_ADDR
#define FUSEPROV_BIST_CTRL_ADDR             0  /* NEEDSWORK: IPCatalog */
#endif

#ifndef FUSEPROV_BIST_ERROR_ADDR
#define FUSEPROV_BIST_ERROR_ADDR            0  /* NEEDSWORK: IPCatalog */
#endif

/* Reset type for platform-specific reset handling */
#ifndef FUSEPROV_RESET_TYPE
#define FUSEPROV_RESET_TYPE                 0  /* NEEDSWORK: warm or cold reset */
#endif

#endif /* FUSEPROV_MRC_CFG_H */
