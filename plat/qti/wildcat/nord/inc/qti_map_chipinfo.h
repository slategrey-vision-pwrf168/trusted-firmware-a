/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef QTI_MAP_CHIPINFO_H
#define QTI_MAP_CHIPINFO_H

#include <stdint.h>

#include <qti_plat.h>

/*
 * NEEDSWORK: every identifier below requires confirmation from IPCatalog for
 * this chipset. The JTAG ID register address, the SoC revision register
 * address, and the JTAG-to-chipinfo ID mapping are all chipset specific.
 */
#define QTI_JTAG_ID_REG                         0x0 /* NEEDSWORK */
#define QTI_JTAG_ID_SHIFT                       12
#define QTI_JTAG_ID_NORD                        U(0x0000) /* NEEDSWORK */
#define QTI_CHIPINFO_ID_NORD                    U(0x0000) /* NEEDSWORK */
#define QTI_DEFAULT_CHIPINFO_ID                 U(0xFFFF)

/*----------------------------------------------------------------------------*/
/* SOC hw version register */
/*----------------------------------------------------------------------------*/
#define QTI_SOC_VERSION_MASK			U(0xFFFF)
#define QTI_SOC_REVISION_REG			0x0 /* NEEDSWORK */
#define QTI_SOC_REVISION_MASK			U(0xFFFF)

/*
 * NEEDSWORK: placeholder mapping. The chipinfo catalog in
 * include/drivers/qti/chipinfo/chipinfo.h carries a CHIPINFO_ID_SMP_NORDAU
 * entry, but it has not been confirmed that it corresponds to this target, so
 * it is deliberately not referenced here.
 */
static const chip_id_info_t g_map_jtag_chipinfo_id[] = {
	{QTI_JTAG_ID_NORD, QTI_CHIPINFO_ID_NORD},
};
#endif /* QTI_MAP_CHIPINFO_H */
