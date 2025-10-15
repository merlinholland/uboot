// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "ot_drv_compat.h"
#include "drv_klad.h"

td_s32 ot_drv_compat_init(void)
{
    td_s32 ret;

    ret = drv_klad_init();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 ot_drv_compat_deinit(void)
{
    drv_klad_deinit();

    return TD_SUCCESS;
}

