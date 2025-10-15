// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef OTP_REG_BASE_H
#define OTP_REG_BASE_H

#define OTP_REG_BASE_ADDR_PHY           0x10200000
#define OTP_REG_BASE_ADDR_SIZE          0x1000

#define REG_SYS_OTP_CLK_ADDR_PHY        0x11013240
#define OTP_CRG_CLOCK_BIT               (0x01 << 4)

#define OTP_USER_WORK_MODE          0x0000
#define OTP_USER_OP_START           0x0004
#define OTP_USER_KEY_INDEX          0x0008
#define OTP_USER_KEY_DATA0          0x000c
#define OTP_USER_KEY_DATA1          0x0010
#define OTP_USER_KEY_DATA2          0x0014
#define OTP_USER_KEY_DATA3          0x0018
#define OTP_USER_KEY_DATA4          0x001c
#define OTP_USER_KEY_DATA5          0x0020
#define OTP_USER_KEY_DATA6          0x0024
#define OTP_USER_KEY_DATA7          0x0028
#define OTP_USER_KEY_DATA8          0x002c
#define OTP_USER_FLAG_VALUE         0x0030
#define OTP_USER_FLAG_INDEX         0x0034
#define OTP_USER_REV_ADDR           0x0038
#define OTP_USER_REV_WDATA          0x003c
#define OTP_USER_REV_RDATA          0x0040
#define OTP_USER_LOCK_STA0          0x0044
#define OTP_USER_LOCK_STA1          0x0048
#define OTP_USER_CTRL_STA           0x004c
#define OTP_OP_START_VAL            0x1acce551

#endif /* OTP_REG_BASE_H */
