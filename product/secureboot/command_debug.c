// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */
#include <common.h>

#include "sec_boot.h"
#include "macro_utils.h"

#include "securec.h"

#define PARTITION_MAX_NUM 20
#define HEX_BASE 16

#define PART_NAME_IDX 1
#define FLASH_OFFSET_IDX 2
#define DST_DDR_ADDR_IDX 3
#define TMP_DDR_ADDR_IDX 4

#define CMD_VBMETA_ARGV_NUM	5
#define CMD_SECIMAGE_ARGV_NUM 4
#define CMD_SECVERIFY_ARGV_NUM 1

static partition_info g_partition_info_list[PARTITION_MAX_NUM] = {0};
static unsigned int g_partition_num = 0;
static unsigned g_load_addr = 0x0;

static int reg_vbmeta_info_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	printf("in comd reg_vbmeta_info_cmd.\n");
    /* Check Input Args Count : four arguments needed */
	if (argc != CMD_VBMETA_ARGV_NUM)
		goto usage;

	var_is_null_return(argv, -1);
	unsigned flash_offset  = simple_strtoul(argv[FLASH_OFFSET_IDX], NULL, HEX_BASE);
	unsigned dst_ddr_address = simple_strtoul(argv[DST_DDR_ADDR_IDX], NULL, HEX_BASE);
	unsigned tmp_ddr_address = simple_strtoul(argv[TMP_DDR_ADDR_IDX], NULL, HEX_BASE);

	printf("name[%s], flash_offset[%x], dst_ddr_address[%x], tmp_ddr_address[%x].\n",
		argv[1], flash_offset, dst_ddr_address, tmp_ddr_address);

	if (g_partition_num == 0) {
		unsigned int part_name_len = strlen(argv[PART_NAME_IDX]);
		if (part_name_len >= (MAX_IMAGE_NAME_LEN - 1)) {
			printf("partition name must less than %d char\n", MAX_IMAGE_NAME_LEN - 1);
			return -1;
		}
		ret = memcpy_s(g_partition_info_list[g_partition_num].partition_name, MAX_IMAGE_NAME_LEN,
			argv[PART_NAME_IDX], part_name_len);
		if (ret != EOK) {
			printf("reg_vbmeta_info_cmd memcpy failed\n");
			return -1;
		}
		g_partition_info_list[g_partition_num].partition_name[part_name_len] = '\0';
		g_partition_info_list[g_partition_num].offset = flash_offset;
		g_partition_info_list[g_partition_num].load_addr = dst_ddr_address;
		g_load_addr = tmp_ddr_address;
		g_partition_num++;
	} else {
		printf("partition sequence err.\n");
		return -1;
	}
	return 0;
usage:
	cmd_usage(cmdtp);
	return 1;
}

static int reg_image_info_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	printf("in comd reg_image_info_cmd.\n");

	/* Check Input Args Count : four arguments needed */
	if (argc != CMD_SECIMAGE_ARGV_NUM)
			goto usage;
	var_is_null_return(argv, -1);
	unsigned flash_offset  = simple_strtoul(argv[FLASH_OFFSET_IDX], NULL, HEX_BASE);
	unsigned dst_ddr_address = simple_strtoul(argv[DST_DDR_ADDR_IDX], NULL, HEX_BASE);

	printf("name[%s], flash_offset[%x], dst_ddr_address[%x].\n",
		argv[1], flash_offset, dst_ddr_address);

	if (g_partition_num != 0) {
		if (g_partition_num >= PARTITION_MAX_NUM) {
			printf("partition num is bigger than PARTITION_MAX_NUM\n");
			return -1;
		}
		unsigned int part_name_len = strlen(argv[PART_NAME_IDX]);
		if (part_name_len >= (MAX_IMAGE_NAME_LEN - 1)) {
			printf("partition name must less than %d char\n", MAX_IMAGE_NAME_LEN - 1);
			return -1;
		}
		ret = memcpy_s(g_partition_info_list[g_partition_num].partition_name,
			MAX_IMAGE_NAME_LEN, argv[PART_NAME_IDX], part_name_len);
		if (ret != EOK) {
			printf("reg_image_info_cmd memcpy failed\n");
			return -1;
		}
		g_partition_info_list[g_partition_num].partition_name[part_name_len] = '\0';
		g_partition_info_list[g_partition_num].offset = flash_offset;
		g_partition_info_list[g_partition_num].load_addr = dst_ddr_address;
		g_partition_num++;
	} else {
		printf("partition sequence err.\n");
		return -1;
	}
	return 0;
usage:
	cmd_usage(cmdtp);
	return 1;
}

static int secure_verify_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("in comd secure_verify_cmd.\n");
	/* Check Input Args Count : four arguments needed */
	if (argc != CMD_SECVERIFY_ARGV_NUM)
		goto usage;

	var_is_null_return(argv, -1);

	int ret = check_security_boot(g_partition_info_list, g_partition_num, g_load_addr);
	if (ret != 0) {
		printf("check_security_boot failed\n");
		do_reset(NULL, 0, 0, NULL);
	}

	return 0;
usage:
	cmd_usage(cmdtp);
	return 1;
}


U_BOOT_CMD(vbmeta, CMD_VBMETA_ARGV_NUM, 1, reg_vbmeta_info_cmd,
	"reg vbmeta info for secureboot, 'vbmeta <partition name> <flash offset>  <dst ddr address> <tmp ddr address>'",
	"regvbmeta\n"
	"regvbmeta");

U_BOOT_CMD(secimage, CMD_SECIMAGE_ARGV_NUM, 1, reg_image_info_cmd,
	"reg image info for secureboot, 'secimage <partition name> <flash offset>  <dst ddr address>'",
	"regvbmeta\n"
	"regvbmeta");

U_BOOT_CMD(secverify, CMD_SECVERIFY_ARGV_NUM, 1, secure_verify_cmd,
	"reg image info for secureboot",
	"regvbmeta\n"
	"regvbmeta");