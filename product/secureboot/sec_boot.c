// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "sec_boot.h"

#include <common.h>

#include "security_boot_struct.h"
#include "user_adaptor.h"
#include "vbmeta.h"
#include "rsa_public.h"
#include "macro_utils.h"
#include "secureboot_log.h"

#define VBMETA_IMAGE_NAME "vbmeta"
#define VBMETA_IMAGE_LOAD_ADDR (0)
#define KERNEL_IMAGE_NAME "kernel"
#define KERNEL_IMAGE_OFFSET (2 * 1024 * 1024)
#define KERNEL_IMAGE_LOAD_ADDR (0x82000000)
#define PARTITION_MAX_NUM 20

const static partition_info *local_partition_info_list = NULL;
static unsigned int local_partition_num = 0;
static uintptr_t local_load_addr = 0;

#if defined(CONFIG_SECUREBOOT_TIMER_ENABLE)
static unsigned long g_start = 0;
static unsigned long g_cost = 0;
#define timer_start() do {		\
	g_start = get_timer(0);		\
} while (0)
#define timer_end(fmt, ...) do {	\
	g_cost = get_timer(g_start);	\
	se_log(fmt " cost %lu ms\n", ##__VA_ARGS__, g_cost);	\
} while (0)
#else
#define timer_start()
#define timer_end(fmt, ...)
#endif


static int security_boot_startup(void)
{
	int ret;
	unsigned int i;
	rsa_pub_key *pub_key = NULL;

	if (local_partition_num == 0) {
		printf("security_boot_startup local_partition_num == 0\n");
		return -1;
	}

	// 1. init
	ret = user_init(&local_partition_info_list[0], (unsigned char *)local_load_addr);
	if (ret != 0) {
		printf("security_boot_startup user_init failed\n");
		return -1;
	}

	// 2. verify all partitions in vbmeta
	if (local_partition_num > PARTITION_MAX_NUM) {
		printf("security_boot_startup local_partition_num is bigger than PARTITION_MAX_NUM:%d\n", PARTITION_MAX_NUM);
		ret = -1;
		goto __EXIT_FAILURE__;
	}
	for (i = 1; i < local_partition_num; i++) {
		// 1) get rsa pub key
		pub_key = user_get_rsa_pub_key(local_partition_info_list[i].partition_name);
		if (pub_key == NULL) {
		printf("security_boot_startup user_get_rsa_pub_key failed for %s\n",
			local_partition_info_list[i].partition_name);
		ret = -1;
		goto __EXIT_FAILURE__;
		}

		// 2) verify image
		ret = user_rsa_verify_partition(local_partition_info_list[i].partition_name, local_partition_info_list[i].offset,
			(unsigned char *)(local_partition_info_list[i].load_addr), pub_key);
		if (ret != 0) {
		printf("security_boot_startup user_rsa_verify_partition failed for %s\n",
			local_partition_info_list[i].partition_name);
		goto __EXIT_FAILURE__;
		}
	}

	// 3. update otp version
	ret = user_update_persistent_version();
	if (ret != 0) {
		printf("security_boot_startup user_update_persistent_version failed\n");
		goto __EXIT_FAILURE__;
	}
__EXIT_FAILURE__:
	user_deinit();
	return ret;
}

int check_security_boot(const partition_info *partition_info_list, unsigned int partition_num,
	unsigned long data_load_addr)
{
	int ret;
	var_is_null_return(partition_info_list, -1);
	local_partition_info_list = partition_info_list;
	local_partition_num = partition_num;
	local_load_addr = (uintptr_t)data_load_addr;

	timer_start();
	ret = security_boot_startup();
	timer_end("secureboot");
	if (ret != 0) {
		printf("check_security_boot security_boot_startup failed\n");
		return -1;
	}
	return 0;
}