/*
 * Copyright (c) 2026, Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/qti/tme/tme_fuse.h>

/*
 * STUB IMPLEMENTATION -- placeholder for the real TME COM/Interface library.
 *
 * TmeFuseRead(), TmeFuseWriteMultiple(), TmeWriteConfigRegister() and
 * TmeGetPilImageRegions() are an external interface (see the reference
 * TmeInterfaces.h / TmeInterfacesDefs.h): callers such as
 * fuseprov_port_tme.c and qti_fuseprov_init() call them directly, the same
 * way they would call into a real TME COM/Interface library. Encoding
 * requests, talking to TME hardware, and decoding responses is that
 * library's job, not this driver's -- fuseprov only needs these symbols to
 * exist so its callers can call them.
 *
 * No such library is linked into TF-A yet, so these bodies just fail. This
 * file is intentionally isolated so a real TME COM/Interface implementation
 * can replace it later without touching fuseprov_port_tme.c or any
 * fuseprov logic above it.
 */

int TmeFuseRead(TMEQFPROMAddrSpace_t addrType, uint32_t fuseAddr,
		uint32_t *const fuseData, uint32_t *const qfpromApiStatus)
{
	(void)addrType;
	(void)fuseAddr;

	if (fuseData == NULL || qfpromApiStatus == NULL)
		return E_NOT_SUPPORTED;

	WARN("TME: TmeFuseRead not implemented (stub)\n");
	*qfpromApiStatus = QFPROM_ERR_UNKNOWN;
	return E_NOT_SUPPORTED;
}

int TmeFuseWriteMultiple(TMEFuse_t *fuseArray, size_t fuseArrayLen,
			 uint32_t *const qfpromApiStatus)
{
	(void)fuseArray;
	(void)fuseArrayLen;

	if (qfpromApiStatus == NULL)
		return E_NOT_SUPPORTED;

	WARN("TME: TmeFuseWriteMultiple not implemented (stub)\n");
	*qfpromApiStatus = QFPROM_ERR_UNKNOWN;
	return E_NOT_SUPPORTED;
}

int TmeWriteConfigRegister(tmeConfigRegisterId_e registerId, uint32_t value)
{
	(void)registerId;
	(void)value;

	WARN("TME: TmeWriteConfigRegister not implemented (stub)\n");
	return E_NOT_SUPPORTED;
}

int TmeGetPilImageRegions(uint32_t *const swIdCount, uint32_t *const swIds,
			  uint32_t *const regionListCount,
			  tmePilRegion_t *const regionList)
{
	(void)swIds;

	if (swIdCount == NULL || regionListCount == NULL || regionList == NULL)
		return E_NOT_SUPPORTED;

	WARN("TME: TmeGetPilImageRegions not implemented (stub)\n");
	*regionListCount = 0;
	return E_NOT_SUPPORTED;
}
