/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <drivers/qti/tme/tme.h>
#include <drivers/qti/fuseprov/fuseprov.h>
#include <drivers/qti/crypto/rng.h>

/* SEC.DAT v3 format constants */
#define FUSEPROV_SECDAT_MAGIC1 0x3B7251CA
#define FUSEPROV_SECDAT_MAGIC2 0x2A126F29
#define FUSEPROV_SECDAT_REV_V3 3
#define FUSEPROV_SECDAT_SEGMENT_EFUSE 0x0
#define FUSEPROV_SECDAT_SEGMENT_ENCKEY 0x1

/* Fuse region types */
typedef enum {
	FUSEPROV_REGION_TYPE_OEM_SEC_BOOT = 0x0,
	FUSEPROV_REGION_TYPE_OEM_PK_HASH = 0x1,
	FUSEPROV_REGION_TYPE_SEC_HW_KEY = 0x2,
	FUSEPROV_REGION_TYPE_OEM_CONFIG = 0x3,
	FUSEPROV_REGION_TYPE_READ_WRITE_PERM = 0x4,
	FUSEPROV_REGION_TYPE_SPARE_REG19 = 0x5,
	FUSEPROV_REGION_TYPE_GENERAL = 0x6,
	FUSEPROV_REGION_TYPE_FEC_EN = 0x7,
	FUSEPROV_REGION_TYPE_ANTI_ROLLBACK_2 = 0x8,
	FUSEPROV_REGION_TYPE_ANTI_ROLLBACK_3 = 0x9,
	FUSEPROV_REGION_TYPE_PK_HASH1 = 0xA,
	FUSEPROV_REGION_TYPE_IMAGE_ENCR_KEY1 = 0xB,
	FUSEPROV_REGION_TYPE_OEM_SECURE = 0xC,
	FUSEPROV_REGION_TYPE_MRC_2_0 = 0xD,
	FUSEPROV_REGION_TYPE_OEM_SPARE = 0xE,
	FUSEPROV_REGION_TYPE_TME_RW_PERM = 0xC,
	FUSEPROV_REGION_TYPE_TME_FEC_EN = 0xD,
	FUSEPROV_REGION_TYPE_TME_OEM = 0xE,
	FUSEPROV_REGION_TYPE_TME_SPARE = 0xF,
} fuseprov_region_type_t;

/* Fuse operation types */
typedef enum {
	FUSEPROV_OPERATION_TYPE_BLOW = 0x0,
	FUSEPROV_OPERATION_TYPE_VERIFYMASK0 = 0x1,
	FUSEPROV_OPERATION_TYPE_BLOW_RANDOM = 0x2,
} fuseprov_operation_type_t;

/* SEC.DAT header structure */
typedef struct {
	uint32_t magic1;
	uint32_t magic2;
	uint32_t revision;
	uint32_t size;
	uint8_t info[16];
	uint32_t segment_number;
	uint32_t reserved[3];
} fuseprov_secdat_hdr_t;

/* SEC.DAT segment header */
typedef struct {
	uint32_t offset;
	uint16_t type;
	uint16_t attribute;
} fuseprov_secdat_segment_hdr_t;

/* Fuse list header */
typedef struct {
	uint32_t revision;
	uint32_t size;
	uint32_t fuse_count;
	uint32_t reserved[4];
} fuseprov_qfuse_list_hdr_t;

/* Fuse entry */
typedef struct {
	uint32_t region_type;
	uint32_t raw_row_address;
	uint32_t lsb_val;
	uint32_t msb_val;
	uint32_t operation;
} fuseprov_qfuse_entry_t;

/* SEC.DAT footer */
typedef struct {
	uint8_t hash[32];
} fuseprov_secdat_footer_t;

/* Fuse blow categories */
typedef enum {
	FUSEPROV_CATEGORY_GENERAL = 0,
	FUSEPROV_CATEGORY_SHK = 1,
	FUSEPROV_CATEGORY_OEM_SPARE = 2,
	FUSEPROV_CATEGORY_OEM_CONFIG = 3,
	FUSEPROV_CATEGORY_SECBOOT = 4,
	FUSEPROV_CATEGORY_FEC_EN = 5,
	FUSEPROV_CATEGORY_RW_PERM = 6,
	FUSEPROV_CATEGORY_MRC = 7,
	FUSEPROV_CATEGORY_ENCKEY = 8,
} fuseprov_category_t;

/* Simple SHA256 implementation (minimal, for SEC.DAT hash verification) */
#include "fuseprov_sha256.h"

/*
 * Validate SEC.DAT header and extract segment information.
 */
static int fuseprov_parse_secdat_hdr(const uint8_t *buffer, size_t buffer_len,
				     fuseprov_secdat_hdr_t *hdr,
				     fuseprov_secdat_segment_hdr_t **segments,
				     uint32_t *segment_count)
{
	const uint8_t *data_end;
	uint32_t data_size;
	uint8_t computed_hash[32];
	const fuseprov_secdat_footer_t *footer;

	if (buffer == NULL || hdr == NULL || segments == NULL ||
	    segment_count == NULL) {
		return FUSEPROV_INVALID_ARG;
	}

	if (buffer_len < sizeof(fuseprov_secdat_hdr_t) +
			 sizeof(fuseprov_secdat_footer_t)) {
		return FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
	}

	memcpy(hdr, buffer, sizeof(fuseprov_secdat_hdr_t));

	if (hdr->magic1 != FUSEPROV_SECDAT_MAGIC1 ||
	    hdr->magic2 != FUSEPROV_SECDAT_MAGIC2) {
		ERROR("Fuseprov: SEC.DAT magic mismatch\n");
		return FUSEPROV_SECDAT_MAGIC_MISMATCH;
	}

	if (hdr->revision != FUSEPROV_SECDAT_REV_V3) {
		ERROR("Fuseprov: SEC.DAT revision %u not supported\n",
		      hdr->revision);
		return FUSEPROV_SECDAT_REV_NOT_SUPPORTED;
	}

	if (hdr->segment_number > 32) {
		ERROR("Fuseprov: Too many segments (%u)\n", hdr->segment_number);
		return FUSEPROV_SECDAT_SEGMENT_NUM_NOT_SUPPORTED;
	}

	data_size = sizeof(fuseprov_secdat_hdr_t) +
		    (hdr->segment_number * sizeof(fuseprov_secdat_segment_hdr_t)) +
		    hdr->size;

	if (data_size + sizeof(fuseprov_secdat_footer_t) > buffer_len) {
		ERROR("Fuseprov: Buffer too small for SEC.DAT\n");
		return FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
	}

	/* Verify SHA256 hash */
	fuseprov_sha256(buffer, data_size, computed_hash);
	footer = (const fuseprov_secdat_footer_t *)(buffer + data_size);

	if (memcmp(computed_hash, footer->hash, 32) != 0) {
		ERROR("Fuseprov: SEC.DAT hash mismatch\n");
		return FUSEPROV_INVALID_HASH;
	}

	*segments = (fuseprov_secdat_segment_hdr_t *)(buffer +
						      sizeof(fuseprov_secdat_hdr_t));
	*segment_count = hdr->segment_number;

	return FUSEPROV_SUCCESS;
}

/*
 * Find a specific segment by type.
 */
static int fuseprov_find_segment(const fuseprov_secdat_segment_hdr_t *segments,
				 uint32_t segment_count, uint16_t segment_type,
				 const uint8_t *buffer, size_t buffer_len,
				 const uint8_t **segment_data)
{
	uint32_t i;

	if (segments == NULL || segment_data == NULL) {
		return FUSEPROV_INVALID_ARG;
	}

	for (i = 0; i < segment_count; i++) {
		if (segments[i].type == segment_type) {
			if (segments[i].offset >= buffer_len) {
				return FUSEPROV_SEGMENT_NOT_FOUND;
			}
			*segment_data = buffer + segments[i].offset;
			return FUSEPROV_SUCCESS;
		}
	}

	return FUSEPROV_SEGMENT_NOT_FOUND;
}

/*
 * Parse fuse list header and extract fuse entries.
 */
static int fuseprov_parse_qfuse_list_hdr(const uint8_t *segment_data,
					 size_t segment_len,
					 fuseprov_qfuse_list_hdr_t *list_hdr,
					 const fuseprov_qfuse_entry_t **entries,
					 uint32_t *entry_count)
{
	if (segment_data == NULL || list_hdr == NULL || entries == NULL ||
	    entry_count == NULL) {
		return FUSEPROV_INVALID_ARG;
	}

	if (segment_len < sizeof(fuseprov_qfuse_list_hdr_t)) {
		return FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
	}

	memcpy(list_hdr, segment_data, sizeof(fuseprov_qfuse_list_hdr_t));

	if (list_hdr->revision != 2 && list_hdr->revision != 3) {
		ERROR("Fuseprov: Qfuse list revision %u not supported\n",
		      list_hdr->revision);
		return FUSEPROV_QFUSE_REV_NOT_SUPPORTED;
	}

	if (list_hdr->fuse_count == 0 || list_hdr->fuse_count > 1024) {
		ERROR("Fuseprov: Invalid fuse count %u\n", list_hdr->fuse_count);
		return FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
	}

	if (list_hdr->size !=
	    list_hdr->fuse_count * sizeof(fuseprov_qfuse_entry_t)) {
		ERROR("Fuseprov: Fuse list size mismatch\n");
		return FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
	}

	*entries = (const fuseprov_qfuse_entry_t *)(segment_data +
						    sizeof(fuseprov_qfuse_list_hdr_t));
	*entry_count = list_hdr->fuse_count;

	return FUSEPROV_SUCCESS;
}

/*
 * Blow fuses in a specific category.
 */
static int fuseprov_blow_fuseregion(const fuseprov_qfuse_entry_t *entries,
				    uint32_t entry_count,
				    fuseprov_category_t category)
{
	uint32_t i;
	uint32_t fuse_data[2];
	uint32_t qfprom_status;
	int ret;
	tme_fuse_t fuse_write;

	for (i = 0; i < entry_count; i++) {
		if (entries[i].region_type != category) {
			continue;
		}

		if (entries[i].operation != FUSEPROV_OPERATION_TYPE_BLOW) {
			continue;
		}

		if (entries[i].lsb_val == 0 && entries[i].msb_val == 0) {
			continue;
		}

		/* Read current fuse value */
		ret = tme_fuse_read(TME_QFPROM_ADDR_SPACE_CORR,
				    entries[i].raw_row_address, fuse_data,
				    &qfprom_status);
		if (ret != 0) {
			ERROR("Fuseprov: Failed to read fuse at 0x%x\n",
			      entries[i].raw_row_address);
			return FUSEPROV_QFPROM_READ_ERROR;
		}

		/* Write new fuse value */
		fuse_write.addr = entries[i].raw_row_address;
		fuse_write.data[0] = entries[i].lsb_val;
		fuse_write.data[1] = entries[i].msb_val;

		ret = tme_fuse_write_multiple(&fuse_write, 1, &qfprom_status);
		if (ret != 0) {
			ERROR("Fuseprov: Failed to write fuse at 0x%x\n",
			      entries[i].raw_row_address);
			return FUSEPROV_QFPROM_WRITE_ERROR;
		}

		NOTICE("Fuseprov: Blew fuse at 0x%x (LSB=0x%x, MSB=0x%x)\n",
		       entries[i].raw_row_address, entries[i].lsb_val,
		       entries[i].msb_val);
	}

	return FUSEPROV_SUCCESS;
}

/*
 * Provision SHK (Secondary Hardware Key) with random data.
 */
static int fuseprov_provision_shk(const fuseprov_qfuse_entry_t *entries,
				  uint32_t entry_count)
{
	uint8_t random_data[40];
	uint32_t i;
	int ret;

	/* Generate random data for SHK (5 fuse rows * 8 bytes) */
	ret = qti_rng_get_data(random_data, sizeof(random_data));
	if (ret != 0) {
		ERROR("Fuseprov: Failed to generate random data for SHK\n");
		return FUSEPROV_SHK_GENERATION_FAILED;
	}

	/* Blow SHK fuses with random data */
	for (i = 0; i < entry_count; i++) {
		if (entries[i].region_type == FUSEPROV_REGION_TYPE_SEC_HW_KEY &&
		    entries[i].operation == FUSEPROV_OPERATION_TYPE_BLOW_RANDOM) {
			tme_fuse_t fuse_write;
			uint32_t qfprom_status;

			fuse_write.addr = entries[i].raw_row_address;
			memcpy(&fuse_write.data[0], &random_data[i * 8], 8);

			ret = tme_fuse_write_multiple(&fuse_write, 1,
						     &qfprom_status);
			if (ret != 0) {
				ERROR("Fuseprov: Failed to write SHK fuse\n");
				return FUSEPROV_QFPROM_WRITE_ERROR;
			}
		}
	}

	return FUSEPROV_SUCCESS;
}

/*
 * Provision OEM spare data with random values.
 */
static int fuseprov_provision_oem_spare(const fuseprov_qfuse_entry_t *entries,
					uint32_t entry_count)
{
	uint8_t random_data[40];
	uint32_t i;
	int ret;

	ret = qti_rng_get_data(random_data, sizeof(random_data));
	if (ret != 0) {
		ERROR("Fuseprov: Failed to generate random data for OEM spare\n");
		return FUSEPROV_OEM_SPARE_RAND_GEN_FAILED;
	}

	for (i = 0; i < entry_count; i++) {
		if (entries[i].region_type == FUSEPROV_REGION_TYPE_OEM_SPARE &&
		    entries[i].operation == FUSEPROV_OPERATION_TYPE_BLOW_RANDOM) {
			tme_fuse_t fuse_write;
			uint32_t qfprom_status;

			fuse_write.addr = entries[i].raw_row_address;
			memcpy(&fuse_write.data[0], &random_data[i * 8], 8);

			ret = tme_fuse_write_multiple(&fuse_write, 1,
						     &qfprom_status);
			if (ret != 0) {
				ERROR("Fuseprov: Failed to write OEM spare fuse\n");
				return FUSEPROV_QFPROM_WRITE_ERROR;
			}
		}
	}

	return FUSEPROV_SUCCESS;
}

/*
 * Main entry point: parse and blow fuses from SEC.DAT.
 */
int fuseprov_blow_fuses(const uint8_t *secdat_buffer, size_t secdat_len)
{
	fuseprov_secdat_hdr_t hdr;
	fuseprov_secdat_segment_hdr_t *segments;
	uint32_t segment_count;
	const uint8_t *efuse_segment;
	const uint8_t *enckey_segment;
	fuseprov_qfuse_list_hdr_t list_hdr;
	const fuseprov_qfuse_entry_t *entries;
	uint32_t entry_count;
	int ret;

	if (secdat_buffer == NULL || secdat_len == 0) {
		return FUSEPROV_INVALID_ARG;
	}

	NOTICE("Fuseprov: Starting fuse provisioning\n");

	/* Parse SEC.DAT header */
	ret = fuseprov_parse_secdat_hdr(secdat_buffer, secdat_len, &hdr,
				       &segments, &segment_count);
	if (ret != FUSEPROV_SUCCESS) {
		ERROR("Fuseprov: Failed to parse SEC.DAT header\n");
		return ret;
	}

	/* Process EFUSE segment */
	ret = fuseprov_find_segment(segments, segment_count,
				   FUSEPROV_SECDAT_SEGMENT_EFUSE, secdat_buffer,
				   secdat_len, &efuse_segment);
	if (ret == FUSEPROV_SUCCESS) {
		ret = fuseprov_parse_qfuse_list_hdr(efuse_segment, secdat_len,
						   &list_hdr, &entries,
						   &entry_count);
		if (ret != FUSEPROV_SUCCESS) {
			ERROR("Fuseprov: Failed to parse EFUSE segment\n");
			return ret;
		}

		/* Blow fuses in order: GENERAL -> SHK -> OEM_SPARE -> OEM_CONFIG
		 * -> SECBOOT -> FEC_EN -> RW_PERM (last, locks everything)
		 */
		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_GENERAL);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_provision_shk(entries, entry_count);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_provision_oem_spare(entries, entry_count);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_OEM_CONFIG);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_OEM_SEC_BOOT);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_FEC_EN);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_READ_WRITE_PERM);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}
	}

	/* Process ENCKEY segment (boot image encryption keys) */
	ret = fuseprov_find_segment(segments, segment_count,
				   FUSEPROV_SECDAT_SEGMENT_ENCKEY, secdat_buffer,
				   secdat_len, &enckey_segment);
	if (ret == FUSEPROV_SUCCESS) {
		ret = fuseprov_parse_qfuse_list_hdr(enckey_segment, secdat_len,
						   &list_hdr, &entries,
						   &entry_count);
		if (ret != FUSEPROV_SUCCESS) {
			ERROR("Fuseprov: Failed to parse ENCKEY segment\n");
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_GENERAL);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_OEM_CONFIG);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_FEC_EN);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}

		ret = fuseprov_blow_fuseregion(entries, entry_count,
					      FUSEPROV_REGION_TYPE_READ_WRITE_PERM);
		if (ret != FUSEPROV_SUCCESS) {
			return ret;
		}
	}

	NOTICE("Fuseprov: Fuse provisioning complete\n");
	return FUSEPROV_SUCCESS;
}
