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
 * TmeFuseRead(), TmeFuseWriteMultiple() and TmeWriteConfigRegister() are an
 * external interface (see the reference TmeInterfaces.h /
 * TmeInterfacesDefs.h): callers such as fuseprov_port_tme.c call them
 * directly, the same way they would call into a real TME COM/Interface
 * library. Encoding requests, talking to TME hardware, and decoding
 * responses is that library's job, not this driver's -- fuseprov only needs
 * these three symbols to exist so the transport port can call them.
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
		return -1;

	WARN("TME: TmeFuseRead not implemented (stub)\n");
	*qfpromApiStatus = 0x7FFFFFFF; /* QFPROM_ERR_UNKNOWN */
	return -1;
}

int TmeFuseWriteMultiple(TMEFuse_t *fuseArray, size_t fuseArrayLen,
			 uint32_t *const qfpromApiStatus)
{
	(void)fuseArray;
	(void)fuseArrayLen;

	if (qfpromApiStatus == NULL)
		return -1;

	WARN("TME: TmeFuseWriteMultiple not implemented (stub)\n");
	*qfpromApiStatus = 0x7FFFFFFF; /* QFPROM_ERR_UNKNOWN */
	return -1;
}

int TmeWriteConfigRegister(tmeConfigRegisterId_e registerId, uint32_t value)
{
	(void)registerId;
	(void)value;

	WARN("TME: TmeWriteConfigRegister not implemented (stub)\n");
	return -1;
}
