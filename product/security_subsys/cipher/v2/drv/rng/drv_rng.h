// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef DRV_RNG_H
#define DRV_RNG_H

/* add include here */
#include "ot_drv_cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Macro Definition ******************************/
td_u32 drv_cipher_rand(td_void);
td_s32 drv_rng_init(td_void);
td_void drv_rng_deinit(td_void);

#ifdef __cplusplus
}
#endif

#endif /* DRV_RNG_H */
