/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DRIVERS_QTI_FUSEPROV_SHA256_H
#define DRIVERS_QTI_FUSEPROV_SHA256_H

#include <stddef.h>
#include <stdint.h>

void fuseprov_sha256(const uint8_t *data, size_t len, uint8_t *digest);

#endif /* DRIVERS_QTI_FUSEPROV_SHA256_H */
