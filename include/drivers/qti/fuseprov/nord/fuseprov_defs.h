/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Nord chipset-specific fuseprov definitions.
 * All addresses marked NEEDSWORK must be confirmed via Enablement_MCP/IPCatalog.
 */

#ifndef DRIVERS_QTI_FUSEPROV_NORD_DEFS_H
#define DRIVERS_QTI_FUSEPROV_NORD_DEFS_H

/* NEEDSWORK: SEC.DAT buffer location and size (from IPCatalog) */
#define QTI_FUSEPROV_SECDAT_BUFFER_BASE 0x0 /* NEEDSWORK */
#define QTI_FUSEPROV_SECDAT_BUFFER_SIZE 0x4000 /* NEEDSWORK */

/* NEEDSWORK: QFPROM write permission register address */
#define QTI_FUSEPROV_WR_PERM_ADDR 0x0 /* NEEDSWORK */

/* NEEDSWORK: QFPROM read permission register address */
#define QTI_FUSEPROV_RD_PERM_ADDR 0x0 /* NEEDSWORK */

/* NEEDSWORK: BIST control register address */
#define QTI_FUSEPROV_BIST_CTRL_ADDR 0x0 /* NEEDSWORK */

/* NEEDSWORK: BIST error register address */
#define QTI_FUSEPROV_BIST_ERROR_ADDR 0x0 /* NEEDSWORK */

/* NEEDSWORK: OEM_SEC_BOOT write permission disable mask */
#define QTI_FUSEPROV_SECBOOT_WR_PERM_DISABLE_MASK 0x0 /* NEEDSWORK */

/* NEEDSWORK: OEM_CONFIG write permission disable mask */
#define QTI_FUSEPROV_OEM_CONFIG_WR_PERM_DISABLE_MASK 0x0 /* NEEDSWORK */

/* NEEDSWORK: SHK write permission disable mask */
#define QTI_FUSEPROV_SHK_WR_PERM_DISABLE_MASK 0x0 /* NEEDSWORK */

/* NEEDSWORK: FEC mask for MSB (bits to clear for FEC calculation) */
#define QTI_FUSEPROV_FEC_ROW_MSB_MASK 0x00FFFFFF /* NEEDSWORK */

/* NEEDSWORK: MRC sticky bit register ID (TME config register) */
#define QTI_FUSEPROV_MRC_STICKY_BIT_REG_ID 0x0 /* NEEDSWORK */

/* NEEDSWORK: MRC write disable sticky bit mask */
#define QTI_FUSEPROV_MRC_WRITE_DISABLE_STICKY_BIT_MASK 0x0 /* NEEDSWORK */

/* Reset type: warm or cold */
#define QTI_FUSEPROV_REQUIRES_WARM_RESET 0

#endif /* DRIVERS_QTI_FUSEPROV_NORD_DEFS_H */
