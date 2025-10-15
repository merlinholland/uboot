// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "otp_user_hal.h"

#include <common.h>
#include "securec.h"

#define otp_hal_read_reg(addr, result)   (*(result) = *(volatile unsigned int *)(addr))
#define otp_hal_write_reg(addr, result)  (*(volatile unsigned int *)(addr) = (result))

typedef enum {
	OTP_READ_LOCK_STA_MODE,
	OTP_LOCK_CIPHER_KEY_MODE,
	OTP_WRITE_KEY_ID_OR_PASSWD_MODE,
	OTP_KEY_ID_OR_PASSWD_CRC_MODE,
	OTP_SET_FLAG_ENABLE_MODE,
	OTP_WRITE_USER_ROOM_MODE,
	OTP_READ_USER_ROOM_MODE,
	OTP_UNKNOWN_MODE,
} otp_user_work_mode_e;

#define MAX_TIMEOUT_CNT 10000
#define HASH_LEN 32

/**
 * wait until otp is idle.
 */
static int otp_hal_wait_free(void)
{
	unsigned int timeout_cnt = 0;

	while (1) {
		unsigned int reg_value = 0;
		otp_hal_read_reg(OTP_USER_CTRL_STA, &reg_value);
		if ((reg_value & 0x1) == 0)   // bit-0: otp_op_busy 0:idle, 1:busy
			return 0;
		timeout_cnt++;
		if (timeout_cnt >= MAX_TIMEOUT_CNT) {
			printf("error: otp_hal_wait_free timeout!\n");
			break;
		}
	}
	return -1;
}

/**
 * set otp work mode.
 */
static void otp_hal_set_mode(otp_user_work_mode_e otp_mode)
{
	unsigned int reg_value = otp_mode;
	otp_hal_write_reg(OTP_USER_WORK_MODE, reg_value);
}

/**
 * start otp operation.
 */
static void otp_hal_op_start(void)
{
	otp_hal_write_reg(OTP_USER_OP_START, OTP_USER_OP_START_CODE);
}

/**
 * wait until otp read operation done.
 */
static int otp_hal_wait_op_read_done(void)
{
	unsigned int timeout_cnt = 0;

	while (1) {
		unsigned int reg_value = 0;
		otp_hal_read_reg(OTP_USER_CTRL_STA, &reg_value);
		if (reg_value & 0x2)  // 0x2: bit-1 otp_user_cmd_finish 0: unfinished, 1: finished
			return 0;

		timeout_cnt++;
		if (timeout_cnt >= MAX_TIMEOUT_CNT) {
			printf("error: otp_hal_wait_op_read_done timeout!\n");
			break;
		}
	}
	return -1;
}

/**
 * wait until otp write operation done.
 */
static int otp_hal_wait_op_write_done(void)
{
	unsigned int timeout_cnt = 0;

	while (1) {
		unsigned int reg_value = 0;
		otp_hal_read_reg(OTP_USER_CTRL_STA, &reg_value);
		if (reg_value & 0x2) {  // 0x2: bit-1 otp_user_cmd_finish 0: unfinished, 1: finished
			if (reg_value & 0x4) {  // 0x4: bit-2 otp_user_lock_err 0: success, 1: locked
				printf("error: otp_hal_wait_op_write_done otp user room space is locked!\n");
				return -1;
			}
			return 0;
		}

		timeout_cnt++;
		if (timeout_cnt >= MAX_TIMEOUT_CNT) {
			printf("error: otp_hal_wait_op_write_done timeout!\n");
			break;
		}
	}
	return -1;
}

/**
 * set otp address.
 */
static void otp_hal_set_otp_addr(unsigned int addr)
{
	otp_hal_write_reg(OTP_USER_REV_ADDR, addr);
}

/**
 * judge if the otp_addr is valid.
 * return 0 - success, -1 - failed
 */
static int is_valid_otp_addr(unsigned int otp_addr)
{
	if (otp_addr >= OTP_MIN_ADDR && otp_addr <= OTP_MAX_ADDR)
		return 0;
	if (otp_addr >= OTP_VBMETA_KEY_HASH_START_ADDR && otp_addr <= OTP_VBMETA_KEY_HASH_END_ADDR)
		return 0;
	if (otp_addr == OTP_ROLLBACK_FLAG_ADDR)
		return 0;
	return -1;
}

/**
 * read otp content in otp_addr.
 */
int otp_user_hal_read(unsigned int otp_addr, unsigned int *out_data)
{
	int ret;

	// addr valid judge
	if (is_valid_otp_addr(otp_addr) != 0) {
		printf("error: otp_user_hal_read invalid addr\n");
		return -1;
	}

	if (out_data == NULL) {
		printf("error: otp_user_hal_read out_data == NULL\n");
		return -1;
	}

	ret = otp_hal_wait_free();
	if (ret != 0) {
		printf("error: otp_user_hal_read otp_hal_wait_free failed\n");
		return -1;
	}

	otp_hal_set_otp_addr(otp_addr);

	otp_hal_set_mode(OTP_READ_USER_ROOM_MODE);

	otp_hal_op_start();

	ret = otp_hal_wait_op_read_done();
	if (ret != 0) {
		printf("error: otp_user_hal_read otp_hal_wait_op_read_done failed\n");
		return -1;
	}

	otp_hal_read_reg(OTP_USER_REV_RDATA, out_data);

	return ret;
}

/**
 * write content to otp_addr.
 */
int otp_user_hal_write(unsigned int otp_addr, unsigned int in_data)
{
	int ret;

	// addr valid judge
	if (is_valid_otp_addr(otp_addr)) {
		printf("error: otp_user_hal_write invalid addr\n");
		return -1;
	}

	ret = otp_hal_wait_free();
	if (ret != 0) {
		printf("error: otp_user_hal_write otp_hal_wait_free failed\n");
		return -1;
	}

	otp_hal_set_otp_addr(otp_addr);

	otp_hal_write_reg(OTP_USER_REV_WDATA, in_data);

	otp_hal_set_mode(OTP_WRITE_USER_ROOM_MODE);

	otp_hal_op_start();

	ret = otp_hal_wait_op_write_done();
	if (ret != 0) {
		printf("error: otp_user_hal_write otp_hal_wait_op_write_done failed\n");
		return -1;
	}
	return ret;
}

int otp_user_hal_read_hash(unsigned char *out_hash, unsigned int hash_len)
{
	if (out_hash == NULL) {
		printf("error: otp_user_hal_read_hash out_hash == NULL\n");
		return -1;
	}
	if (hash_len != HASH_LEN) {
		printf("otp_user_hal_read_hash hash_len != %d\n", HASH_LEN);
		return -1;
	}
	int i;
	for (i = OTP_VBMETA_KEY_HASH_START_ADDR; i <= OTP_VBMETA_KEY_HASH_END_ADDR; i++) {
		unsigned int offset = i - OTP_VBMETA_KEY_HASH_START_ADDR;
		unsigned int out_data;
		int ret = otp_user_hal_read(i, &out_data);
		if (ret != 0) {
			printf("otp_user_hal_read_hash otp_user_hal_read at addr %d failed\n", i);
			return -1;
		}
		ret = memcpy_s(out_hash + offset * sizeof(unsigned int), sizeof(unsigned int),
			(unsigned char *)&out_data, sizeof(unsigned int));
		if (ret != EOK) {
			printf("otp_user_hal_read_hash memcpy failed\n");
			return -1;
		}
	}
	return 0;
}