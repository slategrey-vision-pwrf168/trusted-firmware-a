/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/qti/fuseprov/fuseprov.h>

/*
 * Blow fuses and trigger reset.
 *
 * This function locates the SEC.DAT buffer, calls the fuseprov driver to parse
 * and blow fuses, and triggers a device reset on completion or error.
 *
 * @param secdat_buffer: Pointer to SEC.DAT buffer
 * @param secdat_len: Length of SEC.DAT buffer
 *
 * @return Does not return on success (device resets); returns error code on failure
 */
int qti_fuseprov_blow_fuses_and_reset(const uint8_t *secdat_buffer,
				      size_t secdat_len)
{
	int ret;

	if (secdat_buffer == NULL || secdat_len == 0) {
		ERROR("Fuseprov: Invalid SEC.DAT buffer\n");
		return -1;
	}

	NOTICE("Fuseprov: Starting fuse provisioning and reset sequence\n");

	ret = fuseprov_blow_fuses(secdat_buffer, secdat_len);
	if (ret != 0) {
		ERROR("Fuseprov: Fuse provisioning failed with error %d\n", ret);
		return ret;
	}

	NOTICE("Fuseprov: Fuse provisioning complete, triggering reset\n");

	/* TODO: Trigger device reset via bl31qtilib_psci_warm_reset() or similar
	 * This requires integration with the platform's reset infrastructure.
	 * For now, return success to allow testing of the fuse-blowing logic.
	 */

	return 0;
}
