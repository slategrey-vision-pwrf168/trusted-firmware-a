#
# Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
#

FUSEPROV_SOURCES	:= drivers/qti/fuseprov/fuseprov.c \
			   drivers/qti/fuseprov/fuseprov_sha256.c

$(eval $(call add_sources,FUSEPROV_SOURCES))
