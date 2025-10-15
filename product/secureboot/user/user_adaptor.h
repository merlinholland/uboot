// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __USER_ADAPTOR_H__
#define __USER_ADAPTOR_H__

/**
 * define rsa public key struct to verify image.
 */

// pre-define struct rsa_pub_key
struct rsa_pub_key_tag;
typedef struct rsa_pub_key_tag rsa_pub_key;

struct partition_info_tag;
typedef struct partition_info_tag partition_info;

/**
 * init.
 */
int user_init(const partition_info *vbmeta_info, unsigned char *load_addr);

/**
 * get rsa public key of partition.
 * Description: This function will return rsa_pub_key of partition.
 * Notice: call user_init() before call this function.
 */
rsa_pub_key *user_get_rsa_pub_key(const char *partition_name);

/**
 * verify image.
 * Notice: call user_init() before call this function.
 */
int user_rsa_verify_partition(const char *partition_name, unsigned long offset, unsigned char *load_addr,
	const rsa_pub_key *pub_key);

/**
 * get version from persistent area to support anti-rollback.
 */
int user_read_version_from_persistent_area(unsigned int *version);

/**
 * write version to persistent area to support anti-rollback.
 * Description: This function will update persistent version to version.
 * Notice: call this function carefully, as every time you call the function(if success),
 * the persistent area will reduce.
 */
int user_write_version_to_persistent_area(unsigned int version);

/**
 * get rollback support flag.
 * Description: This flag determine whether support version anti-rollback.
 * return: 0 - don't support anti-rollback, other - support anti-rollback.
 * Notice: call user_init() before call this function, otherwise it will return 0.
 */
unsigned int user_get_rollback_flag(void);

/**
 * get version update flag.
 * Description: This flag determine whether update the persistent version.
 * return: 0 - don't update persistent version, 1 - update persistent version.
 * Notice: call user_init() before call this function, otherwise it will return 0.
 */
unsigned int user_get_version_update_flag(void);

/**
 * get current version.
 * Description: This function return current version, notice that all the partitions are the same version.
 * Notice: call user_init() before call this function, otherwise it will return 0.
 */
unsigned int user_get_current_version(void);


/**
 * get persistent version.
 * Description: This function return persistent version storing at the persisitent area like otp.
 * Notice: call user_init() before call this function, otherwise it will return 0.
 */
unsigned int user_get_persistent_version(void);

/**
 * handle update version.
 * Description: This function will handle update persistent version operation, the process is as follows.
 * Only if the rollback_flag and version_update_flag is set, current_version > persistent_version
 * will the function update persistent_version to current_version.
 * Notice: call user_init() before call this function, otherwise nothing will be done.
 * Notice: if the persistent area is full, the function will print a warning info.
 */
int user_update_persistent_version(void);

/**
 * deinit.
 */
void user_deinit(void);

#endif