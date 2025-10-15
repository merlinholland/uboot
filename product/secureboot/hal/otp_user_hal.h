// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __OTP_USER_HAL_H__
#define __OTP_USER_HAL_H__

#define OTP_USER_OP_START_CODE      (0x1acce551)

#define OTP_USER_IF_BASE            (SECUREBOOT_OTP_REG_BASE_ADDR_PHY)
#define OTP_USER_WORK_MODE          (OTP_USER_IF_BASE + 0x0000)
#define OTP_USER_OP_START           (OTP_USER_IF_BASE + 0x0004)
#define OTP_USER_KEY_INDEX          (OTP_USER_IF_BASE + 0x0008)
#define OTP_USER_KEY_DATA0          (OTP_USER_IF_BASE + 0x000c)
#define OTP_USER_KEY_DATA1          (OTP_USER_IF_BASE + 0x0010)
#define OTP_USER_KEY_DATA2          (OTP_USER_IF_BASE + 0x0014)
#define OTP_USER_KEY_DATA3          (OTP_USER_IF_BASE + 0x0018)
#define OTP_USER_KEY_DATA4          (OTP_USER_IF_BASE + 0x001c)
#define OTP_USER_KEY_DATA5          (OTP_USER_IF_BASE + 0x0020)
#define OTP_USER_KEY_DATA6          (OTP_USER_IF_BASE + 0x0024)
#define OTP_USER_KEY_DATA7          (OTP_USER_IF_BASE + 0x0028)
#define OTP_USER_KEY_DATA8          (OTP_USER_IF_BASE + 0x002c)
#define OTP_USER_FLAG_VALUE         (OTP_USER_IF_BASE + 0x0030)
#define OTP_USER_FLAG_INDEX         (OTP_USER_IF_BASE + 0x0034)
#define OTP_USER_REV_ADDR           (OTP_USER_IF_BASE + 0x0038)
#define OTP_USER_REV_WDATA          (OTP_USER_IF_BASE + 0x003c)
#define OTP_USER_REV_RDATA          (OTP_USER_IF_BASE + 0x0040)
#define OTP_USER_LOCK_STA0          (OTP_USER_IF_BASE + 0x0044)
#define OTP_USER_LOCK_STA1          (OTP_USER_IF_BASE + 0x0048)
#define OTP_USER_CTRL_STA           (OTP_USER_IF_BASE + 0x004c)

// the otp layout

/**
 * offset(otp_slot_addr):  0                      0x8                  0x9             0xa            0xb           0x9 + OTP_ANTI_ROLLBACK_SIZE
 * content:                | VBMETA_PUB_KEY_HASH | ANTI_ROLLBACK_FLAG | OTP_VERSION_1 | OTP_VERSION_ | ... | OTP_VERSION_N |
 * ----------------------------------------------------------------------------------------------------------------------------
 */

// the following addr is the otp slot addr, and a otp slot contents 4 bytes.
// define otp scope for vbmeta pub key hash
#define OTP_VBMETA_KEY_HASH_START_ADDR	(0x0)
#define OTP_VBMETA_KEY_HASH_END_ADDR		(OTP_VBMETA_KEY_HASH_START_ADDR + 0x8 - 1)

// define otp scope for anti-rollback
#define OTP_ANTI_ROLLBACK_SIZE		15
#define OTP_ROLLBACK_FLAG_ADDR      (OTP_VBMETA_KEY_HASH_START_ADDR + 0x8)
#define OTP_MIN_ADDR                (OTP_ROLLBACK_FLAG_ADDR + 0x1)
#define OTP_MAX_ADDR                (OTP_MIN_ADDR + OTP_ANTI_ROLLBACK_SIZE - 1)

/**
 * read otp content in otp_addr.
 */
int otp_user_hal_read(unsigned int otp_addr, unsigned int *out_data);

/**
 * write content to otp_addr.
 */
int otp_user_hal_write(unsigned int otp_addr, unsigned int in_data);

/**
 * read hash in otp.
 */
int otp_user_hal_read_hash(unsigned char *out_hash, unsigned int hash_len);

#endif