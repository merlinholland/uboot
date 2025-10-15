// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "cipher_osal.h"
#include "cipher_ext.h"

#define HASH_MAX_BLOCK_SIZE     256
#define SHA1_RESULT_SIZE        20
#define SHA224_RESULT_SIZE      28
#define SHA256_RESULT_SIZE      32
#define SHA384_RESULT_SIZE      48
#define SHA512_RESULT_SIZE      64

#define SPACC_HASH_CHN          1

#define SHA1_H0                 0x67452301
#define SHA1_H1                 0xefcdab89
#define SHA1_H2                 0x98badcfe
#define SHA1_H3                 0x10325476
#define SHA1_H4                 0xc3d2e1f0

#define SHA224_H0               0xc1059ed8
#define SHA224_H1               0x367cd507
#define SHA224_H2               0x3070dd17
#define SHA224_H3               0xf70e5939
#define SHA224_H4               0xffc00b31
#define SHA224_H5               0x68581511
#define SHA224_H6               0x64f98fa7
#define SHA224_H7               0xbefa4fa4

#define SHA256_H0               0x6a09e667
#define SHA256_H1               0xbb67ae85
#define SHA256_H2               0x3c6ef372
#define SHA256_H3               0xa54ff53a
#define SHA256_H4               0x510e527f
#define SHA256_H5               0x9b05688c
#define SHA256_H6               0x1f83d9ab
#define SHA256_H7               0x5be0cd19

#define SHA384_H0               0xcbbb9d5dc1059ed8ULL
#define SHA384_H1               0x629a292a367cd507ULL
#define SHA384_H2               0x9159015a3070dd17ULL
#define SHA384_H3               0x152fecd8f70e5939ULL
#define SHA384_H4               0x67332667ffc00b31ULL
#define SHA384_H5               0x8eb44a8768581511ULL
#define SHA384_H6               0xdb0c2e0d64f98fa7ULL
#define SHA384_H7               0x47b5481dbefa4fa4ULL

#define SHA512_H0               0x6a09e667f3bcc908ULL
#define SHA512_H1               0xbb67ae8584caa73bULL
#define SHA512_H2               0x3c6ef372fe94f82bULL
#define SHA512_H3               0xa54ff53a5f1d36f1ULL
#define SHA512_H4               0x510e527fade682d1ULL
#define SHA512_H5               0x9b05688c2b3e6c1fULL
#define SHA512_H6               0x1f83d9abfb41bd6bULL
#define SHA512_H7               0x5be0cd19137e2179ULL

#define SM3_H0                  0x7380166F
#define SM3_H1                  0x4914B2B9
#define SM3_H2                  0x172442D7
#define SM3_H3                  0xDA8A0600
#define SM3_H4                  0xA96F30BC
#define SM3_H5                  0x163138AA
#define SM3_H6                  0xE38DEE4D
#define SM3_H7                  0xB0FB0E4E

#define OT_ID_HASH              101
hash_info_s g_cipher_hash_data[HASH_CHANNAL_MAX_NUM];

static td_s32 hash_check_handle(td_handle hash_handle)
{
    if ((td_handle_get_modid(hash_handle)) != OT_ID_HASH ||
        (td_handle_get_private_data(hash_handle) != 0)) {
        ot_err_cipher("invalid cipher handle 0x%x\n", hash_handle);
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }

    if (td_handle_get_chnid(hash_handle) >= HASH_CHANNAL_MAX_NUM) {
        ot_err_cipher("chan %u is too large, max: %d\n",
            td_handle_get_chnid(hash_handle), HASH_CHANNAL_MAX_NUM);
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }

    if (g_cipher_hash_data[td_handle_get_chnid(hash_handle)].is_used == TD_FALSE) {
        ot_err_cipher("chan %u is not open\n", td_handle_get_chnid(hash_handle));
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }
    return TD_SUCCESS;
}

static td_u32 hash_msg_padding(td_u8 *msg, td_u32 byte_len, td_u32 total_len, td_u32 block_size)
{
    td_u32 tmp;
    td_u32 padding_len;

    if (block_size == 0) {
        ot_err_cipher("Block size is zero !\n");
        return TD_FAILURE;
    }

    tmp = total_len % block_size;
    if (block_size == 64) { /* 64 block size */
        padding_len = (tmp < 56) ? (56 - tmp) : (120 - tmp); /* 56, 120 */
        padding_len += 8; /* 8 padding len */
    } else {
        padding_len = (tmp < 112) ? (112 - tmp) : (240 - tmp); /* 112, 240 */
        padding_len += 16; /* 16 padding len */
    }

    /* Format(binary): {data|1000...00| fix_data_len(bits)} */
    msg[byte_len++] = 0x80;
    if (memset_s(&msg[byte_len], HASH_MAX_BLOCK_SIZE - byte_len, 0, padding_len - 1 - 8) != EOK) { /* 2, 8 */
        ot_err_cipher("call failed memset_s\n");
    }
    byte_len += padding_len - 1 - 8; /* 8 */

    /* write 8 bytes fix data length */
    msg[byte_len++] = 0x00;
    msg[byte_len++] = 0x00;
    msg[byte_len++] = 0x00;
    msg[byte_len++] = (td_u8)((total_len >> 29) & 0x07); /* 29 right shift */
    msg[byte_len++] = (td_u8)((total_len >> 21) & 0xff); /* 21 right shift */
    msg[byte_len++] = (td_u8)((total_len >> 13) & 0xff); /* 13 right shift */
    msg[byte_len++] = (td_u8)((total_len >> 5) & 0xff);  /* 5  right shift */
    msg[byte_len++] = (td_u8)((total_len << 3) & 0xff);  /* 3  right shift */

    return byte_len;
}

static td_void hash_config_info_sha1(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SHA1;
    hash_info->sha_len = SHA1_RESULT_SIZE;
    hash_info->block_size = 64; /* 64 sha1 block size */
    hash_info->sha_val[0] = cipher_cpu_to_be32(SHA1_H0); /* 0 sha1 index */
    hash_info->sha_val[1] = cipher_cpu_to_be32(SHA1_H1); /* 1 sha1 index */
    hash_info->sha_val[2] = cipher_cpu_to_be32(SHA1_H2); /* 2 sha1 index */
    hash_info->sha_val[3] = cipher_cpu_to_be32(SHA1_H3); /* 3 sha1 index */
    hash_info->sha_val[4] = cipher_cpu_to_be32(SHA1_H4); /* 4 sha1 index */
}

static td_void hash_config_info_sha224(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SHA224;
    hash_info->sha_len = SHA224_RESULT_SIZE;
    hash_info->block_size = 64; /* 64 sha224 block size */
    hash_info->sha_val[0] = cipher_cpu_to_be32(SHA224_H0); /* 0 sha224 index */
    hash_info->sha_val[1] = cipher_cpu_to_be32(SHA224_H1); /* 1 sha224 index */
    hash_info->sha_val[2] = cipher_cpu_to_be32(SHA224_H2); /* 2 sha224 index */
    hash_info->sha_val[3] = cipher_cpu_to_be32(SHA224_H3); /* 3 sha224 index */
    hash_info->sha_val[4] = cipher_cpu_to_be32(SHA224_H4); /* 4 sha224 index */
    hash_info->sha_val[5] = cipher_cpu_to_be32(SHA224_H5); /* 5 sha224 index */
    hash_info->sha_val[6] = cipher_cpu_to_be32(SHA224_H6); /* 6 sha224 index */
    hash_info->sha_val[7] = cipher_cpu_to_be32(SHA224_H7); /* 7 sha224 index */
}

static td_void hash_config_info_sha256(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SHA256;
    hash_info->sha_len = SHA256_RESULT_SIZE;
    hash_info->block_size = 64; /* 64 sha256 block size */
    hash_info->sha_val[0] = cipher_cpu_to_be32(SHA256_H0); /* 0 sha256 index */
    hash_info->sha_val[1] = cipher_cpu_to_be32(SHA256_H1); /* 1 sha256 index */
    hash_info->sha_val[2] = cipher_cpu_to_be32(SHA256_H2); /* 2 sha256 index */
    hash_info->sha_val[3] = cipher_cpu_to_be32(SHA256_H3); /* 3 sha256 index */
    hash_info->sha_val[4] = cipher_cpu_to_be32(SHA256_H4); /* 4 sha256 index */
    hash_info->sha_val[5] = cipher_cpu_to_be32(SHA256_H5); /* 5 sha256 index */
    hash_info->sha_val[6] = cipher_cpu_to_be32(SHA256_H6); /* 6 sha256 index */
    hash_info->sha_val[7] = cipher_cpu_to_be32(SHA256_H7); /* 7 sha256 index */
}

static td_void hash_config_info_sha384(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    td_u32 idx, i;
    td_u64 tmp_h;
    td_u64 arr_h[8] = {SHA384_H0, SHA384_H1, SHA384_H2, SHA384_H3, /* 8 - sha384 init array */
                       SHA384_H4, SHA384_H5, SHA384_H6, SHA384_H7};

    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SHA384;
    hash_info->sha_len = SHA384_RESULT_SIZE;
    hash_info->block_size = 128; /* 128 sha384 block size */

    for (i = 0, idx = 0; i < (sizeof(arr_h) / sizeof(arr_h[0])); i++, idx += 2) { /* 2 index offset */
        tmp_h = cipher_cpu_to_be64(arr_h[i]);
        if (memcpy_s(&hash_info->sha_val[idx], sizeof(hash_info->sha_val) - idx * sizeof(td_u32),
            &tmp_h, sizeof(td_u64)) != EOK) {
            ot_err_cipher("call failed memcpy_s\n");
            return;
        }
    }
}

static td_void hash_config_info_sha512(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    td_u32 idx, i;
    td_u64 tmp_h;
    td_u64 arr_h[8] = {SHA512_H0, SHA512_H1, SHA512_H2, SHA512_H3, /* 8 - sha512 init array */
                       SHA512_H4, SHA512_H5, SHA512_H6, SHA512_H7};

    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SHA512;
    hash_info->sha_len = SHA512_RESULT_SIZE;
    hash_info->block_size = 128; /* 128 sha512 block size */

    for (i = 0, idx = 0; i < (sizeof(arr_h) / sizeof(arr_h[0])); i++, idx += 2) { /* 2 index offset */
        tmp_h = cipher_cpu_to_be64(arr_h[i]);
        if (memcpy_s(&hash_info->sha_val[idx], sizeof(hash_info->sha_val) - idx * sizeof(td_u32),
            &tmp_h, sizeof(td_u64)) != EOK) {
            ot_err_cipher("call failed memcpy_s\n");
            return;
        }
    }
}

static td_void hash_config_info_sm3(cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    hash_data->sha_type = OT_CIPHER_HASH_TYPE_SM3;
    hash_info->sha_len = SHA256_RESULT_SIZE;
    hash_info->block_size = 64; /* 64 sm3 block size */
    hash_info->sha_val[0] = cipher_cpu_to_be32(SM3_H0); /* 0 sm3 index */
    hash_info->sha_val[1] = cipher_cpu_to_be32(SM3_H1); /* 1 sm3 index */
    hash_info->sha_val[2] = cipher_cpu_to_be32(SM3_H2); /* 2 sm3 index */
    hash_info->sha_val[3] = cipher_cpu_to_be32(SM3_H3); /* 3 sm3 index */
    hash_info->sha_val[4] = cipher_cpu_to_be32(SM3_H4); /* 4 sm3 index */
    hash_info->sha_val[5] = cipher_cpu_to_be32(SM3_H5); /* 5 sm3 index */
    hash_info->sha_val[6] = cipher_cpu_to_be32(SM3_H6); /* 6 sm3 index */
    hash_info->sha_val[7] = cipher_cpu_to_be32(SM3_H7); /* 7 sm3 index */
}

static td_s32 hash_config_info(ot_cipher_hash_type sha_type,
    cipher_hash_data_s *hash_data, hash_info_s *hash_info)
{
    switch (sha_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA1:
            hash_config_info_sha1(hash_data, hash_info);
            break;
        case OT_CIPHER_HASH_TYPE_SHA224:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA224:
            hash_config_info_sha224(hash_data, hash_info);
            break;
        case OT_CIPHER_HASH_TYPE_SHA256:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA256:
            hash_config_info_sha256(hash_data, hash_info);
            break;
        case OT_CIPHER_HASH_TYPE_SHA384:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA384:
            hash_config_info_sha384(hash_data, hash_info);
            break;
        case OT_CIPHER_HASH_TYPE_SHA512:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA512:
            hash_config_info_sha512(hash_data, hash_info);
            break;
        case OT_CIPHER_HASH_TYPE_SM3:
            hash_config_info_sm3(hash_data, hash_info);
            break;
        default:
            ot_err_cipher("Invalid sha type %d\n", sha_type);
            return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 cipher_hash_init(const ot_cipher_hash_attr *hash_attr, td_handle *hash_handle)
{
    td_s32 ret;
    cipher_hash_data_s hash_data;
    td_u32 soft_id;
    hash_info_s *hash_info = NULL;

    for (soft_id = 0; soft_id < HASH_CHANNAL_MAX_NUM; soft_id++) {
        if (!g_cipher_hash_data[soft_id].is_used)
            break;
    }

    if (soft_id >= HASH_CHANNAL_MAX_NUM) {
        ot_err_cipher("Hash module is busy!\n");
        return TD_FAILURE;
    }

    hash_info = &g_cipher_hash_data[soft_id];
    (td_void)memset_s(hash_info, sizeof(hash_info_s), 0, sizeof(hash_info_s));
    (td_void)memset_s(&hash_data, sizeof(cipher_hash_data_s), 0, sizeof(cipher_hash_data_s));
    hash_info->sha_type = hash_attr->sha_type;
    hash_data.hard_chn = SPACC_HASH_CHN;
    ret = hash_config_info(hash_info->sha_type, &hash_data, hash_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CALCHASH_INIT, &hash_data);
    if (ret == TD_SUCCESS) {
        hash_info->is_used = TD_TRUE;
        *hash_handle = td_handle_init(OT_ID_HASH, 0, soft_id);
    }

    return ret;
}

static td_s32 cipher_hash_update_block(hash_info_s *hash_info,
    const td_u8 *input_data, td_u32 input_data_len)
{
    td_s32 ret;
    cipher_hash_data_s hash_data;

    (td_void)memset_s(&hash_data, sizeof(cipher_hash_data_s), 0, sizeof(cipher_hash_data_s));
    hash_data.sha_type = hash_info->sha_type;
    hash_data.hard_chn = SPACC_HASH_CHN;
    ret = memcpy_s(&hash_data.sha_val, sizeof(hash_data.sha_val), hash_info->sha_val, sizeof(hash_info->sha_val));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    hash_data.data_len = input_data_len;
    hash_data.data_phy = get_ulong_low((td_size_t)(uintptr_t)input_data);
    hash_data.data_phy_high = get_ulong_high((td_size_t)(uintptr_t)input_data);
    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CALCHASHUPDATE, &hash_data);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Error, ioctl for hash update failed!\n");
        hash_info->is_used = TD_FALSE;
        return ret;
    }

    ret = memcpy_s(hash_info->sha_val, sizeof(hash_info->sha_val), &hash_data.sha_val, sizeof(hash_data.sha_val));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 cipher_hash_updata_tail(hash_info_s *hash_info,
    td_u8 *data_phy, const td_u8 *input_data, td_u32 input_data_len)
{
    td_s32 ret = TD_SUCCESS;

    /* process the tail of last update */
    if (hash_info->last_block_size > 0) {
        ret = memcpy_s(hash_info->last_block + hash_info->last_block_size,
            HASH_MAX_BLOCK_SIZE - hash_info->last_block_size,
            input_data, hash_info->block_size - hash_info->last_block_size);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = memcpy_s(data_phy, HASH_MAX_BLOCK_SIZE, hash_info->last_block, hash_info->block_size);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = cipher_hash_update_block(hash_info, data_phy, hash_info->block_size);
        chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_update_block);

        input_data_len -= hash_info->block_size - hash_info->last_block_size;
        input_data += hash_info->block_size - hash_info->last_block_size;
    }

    if (input_data_len >= hash_info->block_size) {
        td_u32 size;
        size = input_data_len - (input_data_len % hash_info->block_size);
        ret = cipher_hash_update_block(hash_info, input_data, size);
        chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_update_block);

        input_data_len -= size;
        input_data += size;
    }

    /* save tail data */
    (td_void)memset_s(hash_info->last_block, HASH_MAX_BLOCK_SIZE, 0, HASH_MAX_BLOCK_SIZE);

    hash_info->last_block_size = input_data_len;
    ret = memcpy_s(hash_info->last_block, HASH_MAX_BLOCK_SIZE, input_data, input_data_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return ret;
}

static td_s32 cipher_hash_update(td_handle ci_hash_id, const td_u8 *input_data, td_u32 input_data_len)
{
    td_s32 ret = TD_SUCCESS;
    hash_info_s *hash_info = TD_NULL;
    td_u8 *data_phy = TD_NULL;
    td_u32 hash_handle;

    chk_ptr_null_return(input_data);

    if (input_data_len == 0) {
        return TD_SUCCESS;
    }

    ret = hash_check_handle(ci_hash_id);
    chk_func_fail_return(ret != TD_SUCCESS, ret, hash_check_handle);

    hash_handle = td_handle_get_chnid(ci_hash_id);
    hash_info = (hash_info_s *)&g_cipher_hash_data[hash_handle];

    chk_formula_fail_return(input_data_len + hash_info->total_data_len < input_data_len);
    chk_formula_fail_return(input_data_len + hash_info->last_block_size < input_data_len);

    data_phy = (td_u8 *)memalign(ARCH_DMA_MINALIGN, HASH_MAX_BLOCK_SIZE);
    if (data_phy == TD_NULL) {
        ot_err_cipher("Error, memalign for data_phy failed!\n");
        hash_info->is_used = TD_FALSE;
        return OT_ERR_CIPHER_INVALID_POINT;
    }
    (td_void)memset_s(data_phy, HASH_MAX_BLOCK_SIZE, 0, HASH_MAX_BLOCK_SIZE);

    hash_info->total_data_len += input_data_len;
    if ((hash_info->last_block_size + input_data_len) < hash_info->block_size) {
        if (memcpy_s(hash_info->last_block + hash_info->last_block_size,
            HASH_BLOCK_SIZE - hash_info->last_block_size, input_data, input_data_len) != EOK) {
            ot_err_cipher("call failed memcpy_s\n");
            hash_info->is_used = TD_FALSE;
            cipher_free(data_phy);
            return OT_ERR_CIPHER_FAILED_SEC_FUNC;
        }

        hash_info->last_block_size += input_data_len;
        goto free_data_phy;
    }

    ret = cipher_hash_updata_tail(hash_info, data_phy, input_data, input_data_len);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("updata tail failed, ret %d\n", ret);
        hash_info->is_used = TD_FALSE;
    }

free_data_phy:
    cipher_free(data_phy);
    return ret;
}

static td_s32 cipher_hash_final(td_handle ci_hash_id, td_u8 *output_hash)
{
    td_s32 ret;
    hash_info_s *hash_info = TD_NULL;
    cipher_hash_data_s hash_data;
    td_u32 tmp, hash_handle;
    td_u8 *data_phy = TD_NULL;

    chk_ptr_null_return(output_hash);

    ret = hash_check_handle(ci_hash_id);
    chk_func_fail_return(ret != TD_SUCCESS, ret, hash_check_handle);

    hash_handle = td_handle_get_chnid(ci_hash_id);
    hash_info = (hash_info_s *)&g_cipher_hash_data[hash_handle];

    data_phy = (td_u8 *)memalign(ARCH_DMA_MINALIGN, HASH_MAX_BLOCK_SIZE);
    if (data_phy == NULL) {
        ot_err_cipher("memalign for data phy failed!\n");
        hash_info->is_used = TD_FALSE;
        return OT_ERR_CIPHER_INVALID_POINT;
    }
    (td_void)memset_s(data_phy, HASH_MAX_BLOCK_SIZE, 0, HASH_MAX_BLOCK_SIZE);
    (td_void)memset_s(&hash_data, sizeof(cipher_hash_data_s), 0, sizeof(cipher_hash_data_s));

    tmp = hash_msg_padding(hash_info->last_block,
        hash_info->last_block_size, hash_info->total_data_len, hash_info->block_size);

    ret = memcpy_s(data_phy, HASH_MAX_BLOCK_SIZE, hash_info->last_block, tmp);
    if (ret != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        cipher_free(data_phy);
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    hash_data.sha_type = hash_info->sha_type;
    hash_data.data_len = tmp;
    hash_data.data_phy = get_ulong_low((td_size_t)(uintptr_t)data_phy);
    hash_data.data_phy_high = get_ulong_high((td_size_t)(uintptr_t)data_phy);
    hash_data.hard_chn = SPACC_HASH_CHN;
    ret = memcpy_s(hash_data.sha_val, sizeof(hash_data.sha_val), hash_info->sha_val, sizeof(hash_info->sha_val));
    if (ret != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        cipher_free(data_phy);
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CALCHASHFINAL, &hash_data);
    cipher_free(data_phy); /* Must free data_phy befort return */
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_ioctl);

    ret = memcpy_s(output_hash, HASH_RESULT_MAX_LEN, hash_data.sha_val, hash_info->sha_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
    hash_info->is_used = TD_FALSE;

    return ret;
}

static td_s32 cipher_hmac_key_init(const ot_cipher_hash_attr *hash_attr, td_u8 *hmac_key)
{
    td_handle hash_handle = 0;
    td_s32 ret;

    if ((hash_attr->sha_type == OT_CIPHER_HASH_TYPE_HMAC_SHA384) ||
        (hash_attr->sha_type == OT_CIPHER_HASH_TYPE_HMAC_SHA512)) {
        if (hash_attr->hmac_key_len <= 128) { /* key length is less than 128bytes, copy directly */
            ret = memcpy_s(hmac_key, HASH_MAX_BLOCK_SIZE, hash_attr->hmac_key, hash_attr->hmac_key_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            return TD_SUCCESS;
        }
    } else {
        if (hash_attr->hmac_key_len <= 64) { /* key length is less than 64bytes, copy directly */
            ret = memcpy_s(hmac_key, HASH_MAX_BLOCK_SIZE, hash_attr->hmac_key, hash_attr->hmac_key_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            return TD_SUCCESS;
        }
    }

    /* key length more than 64bytes, calcute the hash result */
    ret = cipher_hash_init(hash_attr, &hash_handle);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_init);

    ret = cipher_hash_update(hash_handle, hash_attr->hmac_key, hash_attr->hmac_key_len);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_update);

    ret = cipher_hash_final(hash_handle, hmac_key);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_final);

    return ret;
}

static td_s32 cipher_hmac_init(const ot_cipher_hash_attr *hash_attr, td_handle *hash_handle)
{
    td_s32 ret;
    td_u32 i, soft_id;
    td_u8 *key_pad = TD_NULL;
    hash_info_s *hash_info = NULL;

    chk_ptr_null_return(hash_attr->hmac_key);
    chk_formula_fail_return(hash_attr->hmac_key_len == 0);

    key_pad = (td_u8 *)memalign(ARCH_DMA_MINALIGN, HASH_MAX_BLOCK_SIZE);
    if (key_pad == TD_NULL) {
        ot_err_cipher("memalign for key_pad failed!\n");
        return TD_FAILURE;
    }
    (td_void)memset_s(key_pad, HASH_MAX_BLOCK_SIZE, 0, HASH_MAX_BLOCK_SIZE);

    /* Init hmac key */
    ret = cipher_hmac_key_init(hash_attr, key_pad);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Hmac key initial failed!\n");
        goto free_key_pad;
    }

    /* hash i_key_pad and message start */
    ret = cipher_hash_init(hash_attr, hash_handle);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("hash i_key_pad and message start failed!\n");
        goto free_key_pad;
    }

    soft_id = td_handle_get_chnid(*hash_handle);
    hash_info = (hash_info_s *)&g_cipher_hash_data[soft_id];
    if (memcpy_s(hash_info->mac, sizeof(hash_info->mac), key_pad, hash_info->block_size) != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        ret = OT_ERR_CIPHER_FAILED_SEC_FUNC;
        goto free_key_pad;
    }

    /* generate i_key_pad */
    for (i = 0; i < hash_info->block_size; i++) {
        key_pad[i] ^= 0x36;
    }

    /* hash i_key_pad update */
    ret = cipher_hash_update(*hash_handle, key_pad, hash_info->block_size);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("cipher hash update failed!\n");
        goto free_key_pad;
    }

free_key_pad:
    cipher_free(key_pad);
    return ret;
}

static td_s32 cipher_hmac_update(td_handle ci_hash_id, const td_u8 *input_data, td_u32 input_data_len)
{
    return cipher_hash_update(ci_hash_id, input_data, input_data_len);
}

static td_s32 cipher_hmac_o_key_pad(const ot_cipher_hash_attr *hash_attr,
    hash_info_s *hash_info, td_u8 *output_data, const td_u8 *hash_sum, td_u8 *key_pad)
{
    td_u32 i;
    td_s32 ret;
    td_handle hash_handle;

    /* generate o_key_pad */
    for (i = 0; i < hash_info->block_size; i++) {
        key_pad[i] ^= 0x5c;
    }

    /* hash o_key_pad + hash_sum start */
    ret = cipher_hash_init(hash_attr, &hash_handle);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_init);

    ret = cipher_hash_update(hash_handle, key_pad, hash_info->block_size);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_update);

    ret = cipher_hash_update(hash_handle, hash_sum, hash_info->sha_len);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_update);

    ret = cipher_hash_final(hash_handle, output_data);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_hash_final);

    return ret;
}

static td_s32 cipher_hmac_final(td_handle ci_hash_id, td_u8 *output_data)
{
    td_s32 ret;
    td_handle hash_handle;
    ot_cipher_hash_attr hash_attr;
    td_u8 *hash_sum = TD_NULL;
    td_u8 *key_pad = TD_NULL;
    hash_info_s *hash_info = TD_NULL;

    ret = hash_check_handle(ci_hash_id);
    chk_func_fail_return(ret != TD_SUCCESS, ret, hash_check_handle);

    hash_handle = td_handle_get_chnid(ci_hash_id);
    hash_info = (hash_info_s *)&g_cipher_hash_data[hash_handle];

    (td_void)memset_s(&hash_attr, sizeof(ot_cipher_hash_attr), 0, sizeof(ot_cipher_hash_attr));
    hash_attr.sha_type = hash_info->sha_type;

    key_pad = (td_u8 *)memalign(ARCH_DMA_MINALIGN, HASH_MAX_BLOCK_SIZE);
    if (key_pad == TD_NULL) {
        ot_err_cipher("memalign for key_pad failed!\n");
        hash_info->is_used = TD_FALSE;
        goto free_mem;
    }
    (td_void)memset_s(key_pad, HASH_MAX_BLOCK_SIZE, 0, HASH_MAX_BLOCK_SIZE);

    hash_sum = (td_u8 *)memalign(ARCH_DMA_MINALIGN, HASH_RESULT_MAX_LEN);
    if (hash_sum == TD_NULL) {
        ot_err_cipher("memalign for key_pad failed!\n");
        hash_info->is_used = TD_FALSE;
        ret = OT_ERR_CIPHER_INVALID_POINT;
        goto free_mem;
    }
    (td_void)memset_s(hash_sum, HASH_RESULT_MAX_LEN, 0, HASH_RESULT_MAX_LEN);

    if (memcpy_s(key_pad, HASH_MAX_BLOCK_SIZE, hash_info->mac, hash_info->block_size) != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        hash_info->is_used = TD_FALSE;
        ret = OT_ERR_CIPHER_FAILED_SEC_FUNC;
        goto free_mem;
    }

    /* hash i_key_pad + message finished */
    ret = cipher_hash_final(ci_hash_id, hash_sum);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Hash Final i_key_pad + message failure, ret=%d\n", ret);
        hash_info->is_used = TD_FALSE;
        goto free_mem;
    }

    /* hash o_key_pad + message finished */
    ret = cipher_hmac_o_key_pad(&hash_attr, hash_info, output_data, hash_sum, key_pad);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Hash Final o_key_pad + message failure, ret=%d\n", ret);
        hash_info->is_used = TD_FALSE;
    }

free_mem:
    cipher_free(hash_sum);
    cipher_free(key_pad);
    return ret;
}

td_s32 ot_mpi_cipher_hash_init(const ot_cipher_hash_attr *hash_attr, td_handle *hash_handle)
{
    td_s32 ret;

    chk_dev_open_fail_return();
    chk_ptr_null_return(hash_attr);
    chk_ptr_null_return(hash_handle);

    switch (hash_attr->sha_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
        case OT_CIPHER_HASH_TYPE_SHA224:
        case OT_CIPHER_HASH_TYPE_SHA256:
        case OT_CIPHER_HASH_TYPE_SHA384:
        case OT_CIPHER_HASH_TYPE_SHA512:
        case OT_CIPHER_HASH_TYPE_SM3:
            ret = cipher_hash_init(hash_attr, hash_handle);
            break;
        case OT_CIPHER_HASH_TYPE_HMAC_SHA1:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA224:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA256:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA384:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA512:
            ret = cipher_hmac_init(hash_attr, hash_handle);
            break;
        default:
            ot_err_cipher("Invalid sha type %d!\n", hash_attr->sha_type);
            ret = OT_ERR_CIPHER_INVALID_PARAM;
    }
    if (ret != TD_SUCCESS) {
        ot_err_cipher("hash init failed, hash type: %d\n", hash_attr->sha_type);
    }

    return ret;
}

td_s32 ot_mpi_cipher_hash_update(td_handle hash_handle, const td_u8 *input_data, td_u32 input_data_len)
{
    td_s32 ret;
    td_u32 soft_id;
    hash_info_s *hash_info = TD_NULL;

    chk_dev_open_fail_return();
    chk_ptr_null_return(input_data);

    ret = hash_check_handle(hash_handle);
    chk_func_fail_return(ret != TD_SUCCESS, ret, hash_check_handle);

    soft_id = td_handle_get_chnid(hash_handle);
    hash_info = (hash_info_s *)&g_cipher_hash_data[soft_id];

    switch (hash_info->sha_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
        case OT_CIPHER_HASH_TYPE_SHA224:
        case OT_CIPHER_HASH_TYPE_SHA256:
        case OT_CIPHER_HASH_TYPE_SHA384:
        case OT_CIPHER_HASH_TYPE_SHA512:
        case OT_CIPHER_HASH_TYPE_SM3:
            ret = cipher_hash_update(hash_handle, input_data, input_data_len);
            break;
        case OT_CIPHER_HASH_TYPE_HMAC_SHA1:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA224:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA256:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA384:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA512:
            ret = cipher_hmac_update(hash_handle, input_data, input_data_len);
            break;
        default:
            ot_err_cipher("Invalid hash type %d!\n", hash_info->sha_type);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }
    if (ret != TD_SUCCESS) {
        ot_err_cipher("hash update failed, hash type: %d\n", hash_info->sha_type);
    }

    return ret;
}

td_s32 ot_mpi_cipher_hash_final(td_handle hash_handle, td_u8 *output_hash)
{
    td_s32 ret;
    td_u32 soft_id;
    hash_info_s *hash_info = TD_NULL;

    chk_dev_open_fail_return();
    chk_ptr_null_return(output_hash);

    ret = hash_check_handle(hash_handle);
    chk_func_fail_return(ret != TD_SUCCESS, ret, hash_check_handle);

    soft_id = td_handle_get_chnid(hash_handle);
    hash_info = (hash_info_s *)&g_cipher_hash_data[soft_id];

    switch (hash_info->sha_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
        case OT_CIPHER_HASH_TYPE_SHA224:
        case OT_CIPHER_HASH_TYPE_SHA256:
        case OT_CIPHER_HASH_TYPE_SHA384:
        case OT_CIPHER_HASH_TYPE_SHA512:
        case OT_CIPHER_HASH_TYPE_SM3:
            ret = cipher_hash_final(hash_handle, output_hash);
            break;
        case OT_CIPHER_HASH_TYPE_HMAC_SHA1:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA224:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA256:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA384:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA512:
            ret = cipher_hmac_final(hash_handle, output_hash);
            break;
        default:
            ot_err_cipher("Invalid hash type %d!\n", hash_info->sha_type);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }
    if (ret != TD_SUCCESS) {
        ot_err_cipher("hash update failed, hash type: %d\n", hash_info->sha_type);
    }

    return TD_SUCCESS;
}

