// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "otp_data.h"

otp_data_item g_otp_data_item[] = {
    /* Key operation, support burn, verify, load */
    {"aes_key0",            0, 256, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY | OTP_ATTR_LOAD_KEY},
    {"aes_key1",            1, 256, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY | OTP_ATTR_LOAD_KEY},
    {"aes_key2",            2, 256, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY | OTP_ATTR_LOAD_KEY},
    {"aes_key3",            3, 256, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY | OTP_ATTR_LOAD_KEY},
    {"jtag_pw_id",          4, 64,  TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY},
    {"jtag_pw",             5, 128, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY},
    {"root_key_sha256",     6, 256, TD_TRUE, OTP_ATTR_BURN_KEY | OTP_ATTR_VERIFY_KEY},

    /* Specify flag operation, support write, not read */
    {"secure_boot_en",      0, 1, TD_TRUE, OTP_ATTR_SPECIFY_FLAG},
    {"ddr_ca_en",           1, 1, TD_TRUE, OTP_ATTR_SPECIFY_FLAG},
    {"jtag_ca_en",          2, 1, TD_TRUE, OTP_ATTR_SPECIFY_FLAG},
    {"jtag_prt_mode",       3, 1, TD_TRUE, OTP_ATTR_SPECIFY_FLAG},
    {"uboot_redundance",    5, 1, TD_TRUE, OTP_ATTR_SPECIFY_FLAG},

    /* offset: 0b, 0word; bit_width: 28672b, 896word */
    {"user_reserved_data",  0, 28672, TD_TRUE, OTP_ATTR_REE_USER_DATA},
};

otp_data_item *otp_get_data_item(td_void)
{
    return g_otp_data_item;
}

td_u32 otp_get_data_item_num(td_void)
{
    return sizeof(g_otp_data_item) / sizeof(g_otp_data_item[0]);
}

