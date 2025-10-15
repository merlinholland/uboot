// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef DRV_KLAD_H
#define DRV_KLAD_H

#include "ot_type.h"
#include "common.h"
#include "drv_cipher_ioctl.h"

extern td_void *g_efuse_otp_reg_base;

td_s32 drv_klad_init(td_void);
td_void drv_klad_deinit(td_void);

td_s32 drv_cipher_klad_load_key(td_u32 chn_id, ot_cipher_ca_type root_key,
    ot_cipher_klad_target klad_target, const td_u8 *data_input, td_u32 key_len);

td_s32 drv_cipher_klad_encrypt_key(ot_cipher_ca_type root_key,
    ot_cipher_klad_target klad_target, td_u32 *clean_key, td_u32 *encrypt_key);

#endif /* DRV_KLAD_H */
