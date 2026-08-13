/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Hoya platform fuseprov stub.
 * Fuseprov is not currently enabled for Hoya; this stub is provided for
 * consistency with the project's convention that all families get a platform
 * service file.
 */

#include <stddef.h>
#include <stdint.h>

int qti_fuseprov_blow_fuses_and_reset(const uint8_t *secdat_buffer,
				      size_t secdat_len)
{
	(void)secdat_buffer;
	(void)secdat_len;
	return -1;
}
