// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __SEC_BOOT_H__
#define __SEC_BOOT_H__

#define MAX_IMAGE_NAME_LEN 64

typedef struct partition_info_tag {
	char partition_name[MAX_IMAGE_NAME_LEN];
	unsigned long offset;                       // partition offset in flash
	unsigned long load_addr;                    // after verification, partition will be loaded to the load_addr
} partition_info;

/**
 * Outer interface to start secure boot.
 *
 * Notice: vbmeta should be the first partition in the partition_info_list.
 */
int check_security_boot(const partition_info *partition_info_list, unsigned int partition_num,
	unsigned long data_load_addr);

#endif