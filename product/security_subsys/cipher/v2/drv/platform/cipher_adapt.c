// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "cipher_adapt.h"

td_s32 cipher_mmz_alloc_remap(td_char *name, cipher_mmz_buf_t *cipher_mmz)
{
    if (cipher_mmz->mmz_size == 0) {
        ot_err_cipher("Error: length of malloc is invalid!\n");
        return TD_FAILURE;
    }

    cipher_mmz->start_phy_addr = (td_size_t)(uintptr_t)memalign(ARCH_DMA_MINALIGN, cipher_mmz->mmz_size);

    if (cipher_mmz->start_phy_addr == 0) {
        ot_err_cipher("Error: Get phyaddr for cipher input failed!\n");
        return TD_FAILURE;
    }
    cipher_mmz->start_vir_addr = (td_u8 *)(uintptr_t)cipher_mmz->start_phy_addr;

    return TD_SUCCESS;
}

td_void cipher_mmz_release_unmap(cipher_mmz_buf_t *cipher_mmz)
{
    if (cipher_mmz->start_phy_addr > 0) {
        free(cipher_mmz->start_vir_addr);
        cipher_mmz->start_phy_addr = 0;
        cipher_mmz->start_vir_addr = NULL;
    }
}

td_s32 cipher_mmz_map(cipher_mmz_buf_t *cipher_mmz)
{
    cipher_mmz->start_vir_addr = (td_u8 *)(uintptr_t)cipher_mmz->start_phy_addr;

    return TD_SUCCESS;
}

td_void cipher_mmz_unmap(const cipher_mmz_buf_t *cipher_mmz)
{
}

/************************* SYSTEM API ************************/
void hex2str(char buf[2], td_u8 val) /* 2 buf size */
{
    td_u8 high, low;

    high = (val >> 4) & 0x0F; /* 4 */
    low = val & 0x0F;

    if (high <= 9) /* 9 */
        buf[0] = high + '0';
    else
        buf[0] = (high - 0x0A) + 'A';

    if (low <= 9) /* 9 */
        buf[1] = low + '0';
    else
        buf[1] = (low - 0x0A) + 'A';
}
