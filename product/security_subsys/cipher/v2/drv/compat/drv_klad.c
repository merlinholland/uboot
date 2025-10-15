// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "drv_klad.h"
#include "cipher_adapt.h"
#include "hal_otp.h"

/* Define the union klad_ctrl */
typedef union {
    struct {
        td_u32    start                 : 1; /* [0]  */
        td_u32    decrypt               : 1; /* [1]  */
        td_u32    type                  : 2; /* [3..2]  */
        td_u32    high_low_128bit_flag  : 1; /* [4]  */
        td_u32    resv1                 : 11; /* [15..5]  */
        td_u32    klad2ci_addr          : 3; /* [18..16]  */
        td_u32    resv2                 : 13; /* [31..19]  */
    } bits;

    td_u32    u32;
} klad_ctrl;

#define KLAD_REG_BASE_ADDR                      g_klad_base
#define KLAD_REG_KLAD_CTRL                      (KLAD_REG_BASE_ADDR + 0x00)
#define KLAD_REG_DAT_IN                         (KLAD_REG_BASE_ADDR + 0x10)
#define KLAD_REG_ENC_OUT                        (KLAD_REG_BASE_ADDR + 0x20)
#define KLAD_KEY_LEN                            4
#define CIPHER_WAIT_IDEL_TIMES                  1000

static td_void *g_klad_base = TD_NULL;

static td_s32 hal_cipher_klad_config(td_u32 chn_id,
    td_u32 opt_id, ot_cipher_klad_target klad_target, td_bool is_decrypt)
{
    td_s32 ret;
    klad_ctrl ctrl;

    /* Load efuse or OTP key to KLAD */
    ret = hal_efuse_otp_load_cipher_key(chn_id, opt_id);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ctrl.u32 = 0;
    ctrl.bits.klad2ci_addr = chn_id;
    ctrl.bits.type = klad_target;
    ctrl.bits.decrypt = is_decrypt;
    ctrl.bits.start = 0;

    (td_void)hal_cipher_write_reg(KLAD_REG_KLAD_CTRL, ctrl.u32);

    return TD_SUCCESS;
}

static td_void hal_cipher_start_klad(td_u32 block_num)
{
    klad_ctrl ctrl;

    ctrl.u32 = 0;
    hal_cipher_read_reg(KLAD_REG_KLAD_CTRL, &ctrl.u32);

    /* High 128bits is just meaningful for loading cipher 256 bits key, and meaningless to rsa. */
    ctrl.bits.high_low_128bit_flag = block_num;

    /* start */
    ctrl.bits.start = 1;
    hal_cipher_write_reg(KLAD_REG_KLAD_CTRL, ctrl.u32);
}

static td_void hal_cipher_set_klad_data(const td_u32 *data_input, td_u32 data_len)
{
    td_u32 i;

    for (i = 0; i < data_len; i++) {
        hal_cipher_write_reg(KLAD_REG_DAT_IN + i * KLAD_KEY_LEN, data_input[i]);
    }
}

static td_void hal_cipher_get_klad_data(td_u32 *data_output)
{
    td_u32 i;

    for (i = 0; i < KLAD_KEY_LEN; i++) {
        hal_cipher_read_reg(KLAD_REG_ENC_OUT + i * KLAD_KEY_LEN, &data_output[i]);
    }
}

static td_s32 hal_cipher_wait_klad_done(void)
{
    td_u32 try_count = 0;
    td_u32 ctrl;

    do {
        hal_cipher_read_reg(KLAD_REG_KLAD_CTRL, &ctrl);
        if ((ctrl & 0x01) == 0x00) {
            return TD_SUCCESS;
        }
        try_count++;
    } while (try_count < CIPHER_WAIT_IDEL_TIMES);

    ot_err_cipher("Klad time out!\n");

    return TD_FAILURE;
}

static td_void hal_klad_init(td_void)
{
    td_u32 crg_value;
    td_u32 *sys_addr;

    sys_addr = cipher_ioremap_nocache(CIPHER_KLAD_CRG_ADDR_PHY, 0x100);
    if (sys_addr == TD_NULL) {
        ot_err_cipher("ERROR: sys_addr ioremap with nocache failed!!\n");
        return;
    }

    hal_cipher_read_reg(sys_addr, &crg_value);
    crg_value |= KLAD_CRG_RESET_BIT;   /* reset */
    crg_value |= KLAD_CRG_CLOCK_BIT;   /* set the bit 0, clock opened */
    hal_cipher_write_reg(sys_addr, crg_value);

    /* clock select and cancel reset 0x30100 */
    crg_value &= (~KLAD_CRG_RESET_BIT); /* cancel reset */
    crg_value |= KLAD_CRG_CLOCK_BIT;    /* set the bit 0, clock opened */
    hal_cipher_write_reg(sys_addr, crg_value);

    cipher_iounmap(sys_addr);
}

td_s32 drv_klad_init(td_void)
{
    td_s32 ret;

    g_klad_base = cipher_ioremap_nocache(CIPHER_KLAD_REG_BASE_ADDR_PHY, 0x100);
    if (g_klad_base == TD_NULL) {
        ot_err_cipher("ERROR: osal_ioremap_nocache for KLAD failed!!\n");
        return TD_FAILURE;
    }

    ret = hal_efuse_otp_init();
    if (ret != TD_SUCCESS) {
        cipher_iounmap(g_klad_base);
        return ret;
    }

    hal_klad_init();

    return TD_SUCCESS;
}

td_void drv_klad_deinit(td_void)
{
    if (g_klad_base != TD_NULL) {
        cipher_iounmap(g_klad_base);
        g_klad_base = TD_NULL;
    }
    if (g_efuse_otp_reg_base != TD_NULL) {
        cipher_iounmap(g_efuse_otp_reg_base);
        g_efuse_otp_reg_base = TD_NULL;
    }
    return;
}

static td_void drv_cipher_invbuf(td_u8 *buf, td_u32 u32len)
{
    td_u32 i;
    td_u8 ch;

    for (i = 0; i < u32len / 2; i++) { /* 2 */
        ch = buf[i];
        buf[i] = buf[u32len - i - 1];
        buf[u32len - i - 1] = ch;
    }
}

td_s32 drv_cipher_klad_load_key(td_u32 chn_id, ot_cipher_ca_type root_key,
    ot_cipher_klad_target klad_target, const td_u8 *data_input, td_u32 key_len)
{
    td_s32 ret;
    td_u32 i, opt_id;
    td_u32 key[KLAD_KEY_LEN] = {0};

    if ((root_key < OT_CIPHER_KEY_SRC_KLAD_1) ||
        (root_key > OT_CIPHER_KEY_SRC_KLAD_3)) {
        ot_err_cipher("Error: Invalid Root Key src 0x%x!\n", root_key);
        return TD_FAILURE;
    }

    if (((key_len % 16) != 0) || (key_len == 0)) { /* key 16 align */
        ot_err_cipher("Error: Invalid key len 0x%x!\n", key_len);
        return TD_FAILURE;
    }

    opt_id = root_key - OT_CIPHER_KEY_SRC_KLAD_1 + 1;

    ret = hal_cipher_klad_config(chn_id, opt_id, klad_target, TD_TRUE);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Error: cipher klad config failed!\n");
        return TD_FAILURE;
    }

    for (i = 0; i < key_len / 16; i++) { /* key 16 align */
        ret = memcpy_s(key, sizeof(key), data_input + i * 16, 16); /* key 16 align */
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        hal_cipher_set_klad_data(key, KLAD_KEY_LEN);
        hal_cipher_start_klad(i);
        ret = hal_cipher_wait_klad_done();
        if (ret != TD_SUCCESS) {
            ot_err_cipher("Error: cipher klad wait done failed!\n");
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

td_s32 drv_cipher_klad_encrypt_key(ot_cipher_ca_type root_key,
    ot_cipher_klad_target klad_target, td_u32 *clean_key, td_u32 *encrypt_key)
{
    td_s32 ret;
    td_u32 opt_id;

    if ((root_key < OT_CIPHER_KEY_SRC_KLAD_1) ||
        (root_key >= OT_CIPHER_KEY_SRC_BUTT)) {
        ot_err_cipher("Error: Invalid Root Key src 0x%x!\n", root_key);
        return TD_FAILURE;
    }

    if ((clean_key == TD_NULL) || (encrypt_key == TD_NULL)) {
        ot_err_cipher("Clean key or encrypt key is null.\n");
        return TD_FAILURE;
    }

    opt_id = root_key - OT_CIPHER_KEY_SRC_KLAD_1 + 1;

    ret = hal_cipher_klad_config(0, opt_id, OT_CIPHER_KLAD_TARGET_AES, TD_FALSE);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Error: cipher klad config failed!\n");
        return TD_FAILURE;
    }

    if (klad_target == OT_CIPHER_KLAD_TARGET_RSA)
        drv_cipher_invbuf((td_u8*)clean_key, 16); /* 16 clean key len */

    hal_cipher_set_klad_data(clean_key, KLAD_KEY_LEN);
    hal_cipher_start_klad(0);
    ret = hal_cipher_wait_klad_done();
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Error: cipher klad wait done failed!\n");
        return TD_FAILURE;
    }
    hal_cipher_get_klad_data(encrypt_key);

    return TD_SUCCESS;
}

