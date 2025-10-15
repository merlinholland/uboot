// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef CIPHER_OSAL_H
#define CIPHER_OSAL_H

#ifndef OT_MINIBOOT_SUPPORT
#include <common.h>
#else
#include "string.h"
#include "stdio.h"
#endif
#include "common.h"
#include "malloc.h"
#include "ot_type.h"
#include "ot_drv_cipher.h"
#include "drv_cipher_ioctl.h"
#include "ot_mpi_cipher.h"
#include "securec.h"

#define CIPHER_KLAD_SUPPORT
#define HASH_BLOCK_SIZE                     128
#define HASH_RESULT_MAX_LEN                 64
#define HASH_RESULT_MAX_LEN_IN_WORD         16
#define HASH_MAX_BLOCK_SIZE                 256
#define HASH_CHANNAL_MAX_NUM                8

#define OT_ERR_CIPHER_NOT_INIT              (td_s32)(0x804D0001)
#define OT_ERR_CIPHER_INVALID_HANDLE        (td_s32)(0x804D0002)
#define OT_ERR_CIPHER_INVALID_POINT         (td_s32)(0x804D0003)
#define OT_ERR_CIPHER_INVALID_PARAM         (td_s32)(0x804D0004)
#define OT_ERR_CIPHER_FAILED_INIT           (td_s32)(0x804D0005)
#define OT_ERR_CIPHER_FAILED_GETHANDLE      (td_s32)(0x804D0006)
#define OT_ERR_CIPHER_FAILED_RELEASEHANDLE  (td_s32)(0x804D0007)
#define OT_ERR_CIPHER_FAILED_CONFIGAES      (td_s32)(0x804D0008)
#define OT_ERR_CIPHER_FAILED_CONFIGDES      (td_s32)(0x804D0009)
#define OT_ERR_CIPHER_FAILED_ENCRYPT        (td_s32)(0x804D000A)
#define OT_ERR_CIPHER_FAILED_DECRYPT        (td_s32)(0x804D000B)
#define OT_ERR_CIPHER_BUSY                  (td_s32)(0x804D000C)
#define OT_ERR_CIPHER_NO_AVAILABLE_RNG      (td_s32)(0x804D000D)
#define OT_ERR_CIPHER_FAILED_SEC_FUNC       (td_s32)(0x804D000F)

#define cipher_cpu_to_be16(v)               (((v) << 8) | ((v) >> 8))

#define cipher_cpu_to_be32(v)                                   \
    ((((td_u32)(v)) >> 24) | ((((td_u32)(v)) >> 8) & 0xff00) |  \
    ((((td_u32)(v)) << 8) & 0xff0000) | (((td_u32)(v)) << 24))

#define cipher_cpu_to_be64(x)                                           \
    ((((td_u64)(x) & (td_u64)0x00000000000000ffULL) << 56) |            \
     (((td_u64)(x) & (td_u64)0x000000000000ff00ULL) << 40) |            \
     (((td_u64)(x) & (td_u64)0x0000000000ff0000ULL) << 24) |            \
     (((td_u64)(x) & (td_u64)0x00000000ff000000ULL) << 8)  |            \
     (((td_u64)(x) & (td_u64)0x000000ff00000000ULL) >> 8)  |            \
     (((td_u64)(x) & (td_u64)0x0000ff0000000000ULL) >> 24) |            \
     (((td_u64)(x) & (td_u64)0x00ff000000000000ULL) >> 40) |            \
     (((td_u64)(x) & (td_u64)0xff00000000000000ULL) >> 56))

#define cipher_min(a, b) ((a) < (b) ? (a) : (b))

#define UMAP_DEVNAME_CIPHER         "cipher"
#define UMAP_CIPHER_MINOR_BASE      50

#define get_ulong_low(dw)           (unsigned int)(dw)
#define get_ulong_high(dw)          0
#define make_ulong(low, high)       (low)

#define OT_PRINT                    printf
#define ot_err_cipher(fmt, ...)       OT_PRINT("ERROR [%s-%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ot_info_cipher(fmt, ...)      // OT_PRINT("INFO [%s-%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define CIPHER_MUTEX                td_void *
#define cipher_mutex_init(x)
#define cipher_mutex_lock(x)        0
#define cipher_mutex_unlock(x)
#define cipher_mutex_destroy(x)

#define cipher_open(a, b, c)            (cipher_module_init(), 1)
#define cipher_close(x)                 (cipher_module_exit())
#define cipher_ioctl(dev, cmd, argp)    drv_cipher_ioctl(cmd, argp, NULL)

#define cipher_malloc(x) malloc(x)
#define cipher_free(x)     \
    do {                   \
        if ((x) != NULL) { \
            free(x);       \
            x = NULL;      \
        }                  \
    } while (0)

td_s32 mpi_cipher_get_random_number(td_u32 *random_number, td_u32 time_out_us);

extern td_s32 g_cipher_dev_fd;
extern td_s32 g_cipher_init_counter;

typedef struct {
    td_handle handle;
    ot_cipher_hash_type sha_type;

    td_u8 is_used;
    td_u8 block_size;
    td_u8 sha_len;
    td_u8 last_block_size;

    td_u32 total_data_len;
    td_u32 sha_val[HASH_RESULT_MAX_LEN_IN_WORD];
    td_u8 last_block[HASH_MAX_BLOCK_SIZE];
    td_u8 mac[HASH_MAX_BLOCK_SIZE];
} hash_info_s;

/* params check api */
#define chk_u32_data_fail_return(real, expect)                                  \
    do {                                                                        \
        if ((real) != (expect)) {                                               \
            ot_err_cipher("%s expect 0x%x real 0x%x\n", #real, expect, real);   \
            return OT_ERR_CIPHER_INVALID_PARAM;                                 \
        }                                                                       \
    } while (0)

#define chk_ptr_null_return(p)                          \
    do {                                                \
        if ((p) == TD_NULL) {                           \
            ot_err_cipher("%s is null\n", #p);          \
            return OT_ERR_CIPHER_INVALID_POINT;         \
        }                                               \
    } while (0)

#define chk_func_fail_return(formula, ret, func)        \
    do {                                                \
        if (formula) {                                  \
            ot_err_cipher("call %s failed\n", #func);   \
            return ret;                                 \
        }                                               \
    } while (0)

#define chk_formula_fail_return(formula)                \
    do {                                                \
        if (formula) {                                  \
            ot_err_cipher("%s is invalid\n", #formula); \
            return OT_ERR_CIPHER_INVALID_PARAM;         \
        }                                               \
    } while (0)

#define chk_dev_open_fail_return()                      \
    do {                                                \
        if (g_cipher_init_counter < 0) {                \
            ot_err_cipher("CIPHER is not open.\n");     \
            return OT_ERR_CIPHER_NOT_INIT;              \
        }                                               \
    } while (0)

typedef struct {
    td_void *start_vir_addr;
    td_size_t start_phy_addr;
    td_u32 mmz_size;
} cipher_mmz_buf_t;

#endif /* CIPHER_OSAL_H */
