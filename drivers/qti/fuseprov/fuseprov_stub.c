/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>
#include <drivers/qti/fuseprov/fuseprov.h>

int fuseprov_blow_fuses(const uint8_t *secdat_buffer, size_t secdat_len)
{
	(void)secdat_buffer;
	(void)secdat_len;
	return FUSEPROV_SUCCESS;
}
