// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "cipher_osal.h"

typedef struct {
    ot_cipher_hash_type hash_type;
    td_u32 hlen;
    td_u32 klen;
    td_u32 em_bit;
    td_u8 key_bt;
    td_u8 *in_data;
    td_u32 in_len;
    td_u8 *out_data;
    td_u32 out_len;
} rsa_padding_s;

typedef struct {
    td_u8 *masked_db;
    td_u8 *masked_seed;
    td_u8 salt[CIPHER_MAX_RSA_KEY_LEN];
    td_u32 msb_bits;
    td_u32 slen;
    td_u32 key_len;
} rsa_pkcs1_pss_s;

typedef struct {
    td_u8 arr_em[CIPHER_MAX_RSA_KEY_LEN];
    td_u8 sign_hash[HASH_RESULT_MAX_LEN];
} rsa_sign_buf;

#define RSA_SIGN                1
#define ASN1_HASH_SHA1      "\x30\x21\x30\x09\x06\x05\x2b\x0e\x03\x02\x1a\x05\x00\x04\x14"
#define ASN1_HASH_SHA224    "\x30\x2D\x30\x0d\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x04\x05\x00\x04\x1C"
#define ASN1_HASH_SHA256    "\x30\x31\x30\x0d\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x01\x05\x00\x04\x20"
#define ASN1_HASH_SHA384    "\x30\x41\x30\x0d\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x02\x05\x00\x04\x30"
#define ASN1_HASH_SHA512    "\x30\x51\x30\x0d\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x03\x05\x00\x04\x40"

static const td_s8 g_empty_l_sha1[] =
    "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d"
    "\x32\x55\xbf\xef\x95\x60\x18\x90"
    "\xaf\xd8\x07\x09";

static const td_s8 g_empty_l_sha224[] =
    "\xd1\x4a\x02\x8c\x2a\x3a\x2b\xc9"
    "\x47\x61\x02\xbb\x28\x82\x34\xc4"
    "\x15\xa2\xb0\x1f\x82\x8e\xa6\x2a"
    "\xc5\xb3\xe4\x2f";

static const td_s8 g_empty_l_sha256[] =
    "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14"
    "\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24"
    "\x27\xae\x41\xe4\x64\x9b\x93\x4c"
    "\xa4\x95\x99\x1b\x78\x52\xb8\x55";

static const td_s8 g_empty_l_sha384[] =
    "\x38\xb0\x60\xa7\x51\xac\x96\x38"
    "\x4c\xd9\x32\x7e\xb1\xb1\xe3\x6a"
    "\x21\xfd\xb7\x11\x14\xbe\x07\x43"
    "\x4c\x0c\xc7\xbf\x63\xf6\xe1\xda"
    "\x27\x4e\xde\xbf\xe7\x6f\x65\xfb"
    "\xd5\x1a\xd2\xf1\x48\x98\xb9\x5b";

static const td_s8 g_empty_l_sha512[] =
    "\xcf\x83\xe1\x35\x7e\xef\xb8\xbd"
    "\xf1\x54\x28\x50\xd6\x6d\x80\x07"
    "\xd6\x20\xe4\x05\x0b\x57\x15\xdc"
    "\x83\xf4\xa9\x21\xd3\x6c\xe9\xce"
    "\x47\xd0\xd1\x3c\x5d\x85\xf2\xb0"
    "\xff\x83\x18\xd2\x87\x7e\xec\x2f"
    "\x63\xb9\x31\xbd\x47\x41\x7a\x81"
    "\xa5\x38\x32\x7a\xf9\x27\xda\x3e";

static td_s32 rsa_private(const ot_cipher_rsa_private_key *pri_key,
    ot_cipher_ca_type ca_type, const td_u8 *input, td_u8 *output)
{
    cipher_rsa_data_s rsa_data;

    chk_ptr_null_return(pri_key);
    chk_ptr_null_return(pri_key->d);
    chk_ptr_null_return(pri_key->n);
    chk_ptr_null_return(input);
    chk_ptr_null_return(output);

    rsa_data.rsa_n = pri_key->n;
    rsa_data.rsa_k = pri_key->d;
    rsa_data.rsa_n_len = pri_key->n_len;
    rsa_data.rsa_k_len = pri_key->d_len;
    rsa_data.input_data = (td_u8 *)input;
    rsa_data.output_data = output;
    rsa_data.data_len = pri_key->n_len;
    rsa_data.ca_type = OT_CIPHER_KEY_SRC_USER;

    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CALCRSA, &rsa_data);
}

static td_s32 rsa_public(const ot_cipher_rsa_pub_key *pub_key, const td_u8 *input, td_u8 *output)
{
    cipher_rsa_data_s rsa_data;

    chk_ptr_null_return(pub_key);
    chk_ptr_null_return(pub_key->n);
    chk_ptr_null_return(pub_key->e);
    chk_ptr_null_return(input);
    chk_ptr_null_return(output);

    rsa_data.rsa_n = pub_key->n;
    rsa_data.rsa_k = pub_key->e;
    rsa_data.rsa_n_len = pub_key->n_len;
    rsa_data.rsa_k_len = pub_key->e_len;
    rsa_data.input_data = (td_u8 *)input;
    rsa_data.output_data = output;
    rsa_data.data_len = pub_key->n_len;
    rsa_data.ca_type = OT_CIPHER_KEY_SRC_USER;
    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CALCRSA, &rsa_data);
}

static td_s32 rsa_get_attr(td_u32 scheme, td_u16 rsa_n_len, rsa_padding_s *pad)
{
    td_s32 ret = TD_SUCCESS;

    chk_formula_fail_return(rsa_n_len > 512); /* 512 rsa n length */

    pad->klen = rsa_n_len;

    switch (scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_NO_PADDING:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_1:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_2:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            pad->hlen = 0; /* 0 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_BUTT;
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1:
            pad->hlen = 20; /* 20 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_SHA1;
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224:
            pad->hlen = 28; /* 28 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_SHA224;
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256:
            pad->hlen = 32; /* 32 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_SHA256;
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384:
            pad->hlen = 48; /* 48 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_SHA384;
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512:
            pad->hlen = 64; /* 64 pad hlen */
            pad->hash_type = OT_CIPHER_HASH_TYPE_SHA512;
            break;
        default:
            ot_err_cipher("RSA scheme (0x%x) is invalid.\n", scheme);
            ret = OT_ERR_CIPHER_INVALID_PARAM;
    }

    return ret;
}

static td_s32 rsa_pkcs1_mgf1_get_md_len(ot_cipher_hash_type hash_type, td_u32 *md_len)
{
    switch (hash_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
            *md_len = 20; /* 20 md len */
            break;
        case OT_CIPHER_HASH_TYPE_SHA224:
            *md_len = 28; /* 28 md len */
            break;
        case OT_CIPHER_HASH_TYPE_SHA256:
            *md_len = 32; /* 32 md len */
            break;
        case OT_CIPHER_HASH_TYPE_SHA384:
            *md_len = 48; /* 48 md len */
            break;
        case OT_CIPHER_HASH_TYPE_SHA512:
            *md_len = 64; /* 64 md len */
            break;
        default:
            *md_len = 0;
            ot_err_cipher("hash type is invalid.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
    }
    return TD_SUCCESS;
}

static td_s32 rsa_pkcs1_mgf1(ot_cipher_hash_type hash_type,
    const td_u8 *seed, td_u32 seed_len, td_u8 *mask, td_u32 mask_len)
{
    td_s32 ret;
    td_u32 i, out_len, md_len;
    ot_cipher_hash_attr hash_attr;
    td_u8 hash[HASH_RESULT_MAX_LEN] = {0};
    td_u8 cnt[4]; /* 4 */

    ret = rsa_pkcs1_mgf1_get_md_len(hash_type, &md_len);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1_get_md_len);

    /* PKCS#1 V2.1 only use sha1 function, Others allow for future expansion */
    hash_attr.sha_type = hash_type;
    for (i = 0, out_len = 0; out_len < mask_len; i++) {
        td_handle hash_handle;
        td_u32 j;

        cnt[0] = (td_u8)((i >> 24) & 0xFF); /* 0 ptr_cnt index, 24 right shift */
        cnt[1] = (td_u8)((i >> 16) & 0xFF); /* 1 ptr_cnt index, 16 right shift */
        cnt[2] = (td_u8)((i >>  8) & 0xFF); /* 2 ptr_cnt index, 8  right shift */
        cnt[3] = (td_u8)(i & 0xFF);         /* 3 ptr_cnt index */

        ret = ot_mpi_cipher_hash_init(&hash_attr, &hash_handle);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_init);

        ret = ot_mpi_cipher_hash_update(hash_handle, seed, seed_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_update);

        ret = ot_mpi_cipher_hash_update(hash_handle, cnt, sizeof(cnt));
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_update);

        ret = ot_mpi_cipher_hash_final(hash_handle, hash);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_final);

        for (j = 0; (j < md_len) && (out_len < mask_len); j++) {
            mask[out_len++] ^= hash[j];
        }
    }

    return ret;
}

static td_s32 mbedtls_get_random_number(td_void *param, td_u8 *rand, td_u32 size)
{
    td_u32 i;
    td_s32 ret;
    td_u32 randnum, left_size;

    (td_void)memset_s(rand, size, 0, size);
    for (i = 0; i < size; i += 4) { /* 4 word bytes */
        ret = mpi_cipher_get_random_number(&randnum, -1);
        chk_func_fail_return(ret != TD_SUCCESS, ret, mpi_cipher_get_random_number);

        left_size = (size - i) > 4 ? 4 : (size - i); /* 4 word bytes */
        switch (left_size) {
            case 4: /* left 4 */
                rand[i + 3] = (td_u8)(randnum >> 24) & 0xFF; /* 3 rand index, 24 right shift */
                /* fall through */
            case 3: /* left 3 */
                rand[i + 2] = (td_u8)(randnum >> 16) & 0xFF; /* 2 rand index, 16 right shift */
                /* fall through */
            case 2: /* left 2 */
                rand[i + 1] = (td_u8)(randnum >> 8) & 0xFF; /* 1 rand index, 8  right shift */
                /* fall through */
            case 1: /* left 1 */
                rand[i + 0] = (td_u8)(randnum >> 0) & 0xFF; /* 0 rand index */
                break;
            default:
                ot_err_cipher("left size %u is error\n", left_size);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

    /* non-zero random octet string */
    for (i = 0; i < size; i++) {
        if (rand[i] != 0x00) {
            continue;
        }

        ret = mpi_cipher_get_random_number(&randnum, -1);
        chk_func_fail_return(ret != TD_SUCCESS, ret, mpi_cipher_get_random_number);
        rand[i] = (td_u8)(randnum) & 0xFF;
        i = 0;
    }

    return TD_SUCCESS;
}

static td_u32 rsa_get_bit_num(const td_u8 *big_num, td_u32 num_len)
{
    static const td_s8 bits[16] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4}; /* 16 bits size */
    td_u32 i;

    chk_ptr_null_return(big_num);

    for (i = 0; i < num_len; i++) {
        td_u32 num;
        num = bits[(big_num[i] & 0xF0) >> 4]; /* 4 right shift */
        if (num > 0) {
            return (num_len - i - 1) * 8 + num + 4; /* 8, 4 */
        }

        num = bits[big_num[i] & 0xF];
        if (num > 0) {
            return (num_len - i - 1) * 8 + num; /* 8 */
        }
    }
    return 0;
}

/* PKCS #1: EME-OAEP encoding */
/* ************************************************************
                    +----------+---------+--+-------+
               DB = |  lHash   |    PS   |01|   M   |
                    +----------+---------+--+-------+
                                   |
         +----------+              V
         |   ptr_seed   |--> MGF ---> xor
         +----------+              |
               |                   |
      +--+     V                   |
      |00|    xor <----- MGF <-----|
      +--+     |                   |
        |      |                   |
        V      V                   V
      +--+----------+----------------------------+
EM =  |00|maskedSeed|          maskedDB          |
      +--+----------+----------------------------+
       1     hlen              k - hlen- 1

so: PS_LEN = k - hlen - 1 -  (hlen  + mlen + 1) = k - 2hlen - mlen - 2 > 0
so: mlen < k - 2hlen - 2
************************************************************ */
static td_s32 rsa_padding_add_pkcs1_oaep(rsa_padding_s *pad)
{
    td_s32 ret;
    td_u32 db_len;
    td_u8 *ptr_db = TD_NULL;
    td_u8 *ptr_seed = TD_NULL;
    const td_s8 *l_hash = g_empty_l_sha1;

    /* In the v2.1 of PKCS #1, L is the empty string;    */
    /* other uses outside the scope of rsa specifications */
    if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA224) {
        l_hash = g_empty_l_sha224;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA256) {
        l_hash = g_empty_l_sha256;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA384) {
        l_hash = g_empty_l_sha384;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA512) {
        l_hash = g_empty_l_sha512;
    }

    chk_formula_fail_return(pad->in_len > pad->klen - 2 * pad->hlen - 2); /* 2 */

    pad->out_data[0] = 0;
    ptr_seed = pad->out_data + 1;
    ptr_db = pad->out_data + pad->hlen + 1;
    db_len = pad->klen - pad->hlen - 1;

    /* set lHash */
    ret = memcpy_s(ptr_db, db_len, l_hash, pad->hlen);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    /* set PS with 0x00 */
    ret = memset_s(&ptr_db[pad->hlen], db_len - pad->hlen, 0, db_len - pad->in_len - pad->hlen - 1);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    /* set 0x01 after PS */
    ptr_db[db_len - pad->in_len - 1] = 0x01;

    /* set M */
    ret = memcpy_s(&ptr_db[db_len - pad->in_len], pad->in_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    /* set seed */
    ret = mbedtls_get_random_number(TD_NULL, ptr_seed, pad->hlen);
    chk_func_fail_return(ret != TD_SUCCESS, ret, mbedtls_get_random_number);

    ret = rsa_pkcs1_mgf1(pad->hash_type, ptr_seed, pad->hlen, ptr_db, pad->klen - pad->hlen - 1);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    /* compute maskedSeed */
    ret = rsa_pkcs1_mgf1(pad->hash_type, ptr_db, pad->klen - pad->hlen - 1, ptr_seed, pad->hlen);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    pad->out_len = pad->klen;

    return ret;
}

/* PKCS #1: RSAES-PKCS1-V1_5-ENCRYPT */
/*************************************************
formula: EM = 0x00 || 0x02 || PS || 0x00 || M

formula: PS_LEN > 8, mlen < klen - 11
*************************************************/
static td_s32 rsa_padding_add_pkcs1_v15(rsa_padding_s *pad)
{
    td_u32 index = 0;
    td_s32 ret;

    if (pad->in_len > pad->klen - 11) { /* 11 */
        ot_err_cipher("input_len is invalid.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    pad->out_data[index++] = 0x00;
    pad->out_data[index++] = 0x02;
    ret = mbedtls_get_random_number(TD_NULL, &pad->out_data[index], pad->klen - pad->in_len - 3); /* 3 */
    chk_func_fail_return(ret != TD_SUCCESS, ret, mbedtls_get_random_number);

    index += pad->klen - pad->in_len - 3; /* 3 */
    pad->out_data[index++] = 0x00;
    ret = memcpy_s(&pad->out_data[index], pad->klen - index, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    pad->out_len = pad->klen;

    return TD_SUCCESS;
}

/* PKCS #1: block type 0,1,2 message padding */
/* ************************************************
formula: EB = 00 || BT || PS || 00 || D

formula: PS_LEN >= 8, mlen < klen - 11
************************************************ */
static td_s32 rsa_padding_add_pkcs1_type(rsa_padding_s *pad)
{
    td_u32 pad_len;
    td_u8 *key_eb = TD_NULL;
    td_s32 ret;

    if (pad->in_len > pad->klen - 11) { /* 11 */
        ot_err_cipher("input_len is invalid.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    key_eb = pad->out_data;

    *(key_eb++) = 0;
    *(key_eb++) = pad->key_bt; /* Private Key BT (Block Type) */

    /* pad out with 0xff data */
    pad_len = pad->klen - 3 - pad->in_len; /* 3 */
    if (pad->key_bt == 0x00) {
        ret = memset_s(key_eb, pad->klen - (td_u32)(key_eb - pad->out_data), 0x00, pad_len);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);
    } else if (pad->key_bt == 0x01) {
        ret = memset_s(key_eb, pad->klen - (td_u32)(key_eb - pad->out_data), 0xFF, pad_len);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);
    } else if (pad->key_bt == 0x02) {
        ret = mbedtls_get_random_number(TD_NULL, key_eb, pad_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, mbedtls_get_random_number);
    } else {
        ot_err_cipher("BT(0x%x) is invalid.\n", pad->key_bt);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    key_eb += pad_len;
    *(key_eb++) = 0x00;
    ret = memcpy_s(key_eb, pad->klen - (td_u32)(key_eb - pad->out_data), pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
    pad->out_len = pad->klen;

    return TD_SUCCESS;
}

/* PKCS #1: RSAES-PKCS1-V1_5-Signature */
/* ********************************************************
formula: EM = 0x00 || 0x01 || PS || 0x00 || T

T ::= SEQUENCE {
          digestAlgorithm AlgorithmIdentifier,
          digest OCTET STRING
      }
The first field identifies the hash function and the second
contains the hash value
********************************************************* */
static td_s32 rsa_padding_add_emsa_pkcs1_v15_sha1(const rsa_padding_s *pad, td_u8 *p)
{
    td_s32 ret;
    td_u32 pad_len, buf_len;

    pad_len = pad->klen - 3 - 35; /* 3, 35 */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memset_s(p, buf_len, 0xFF, pad_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    p += pad_len;
    *p++ = 0;
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, ASN1_HASH_SHA1, 15); /* 15 copy size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p += 15; /* 15 p shift */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 rsa_padding_add_emsa_pkcs1_v15_sha224(const rsa_padding_s *pad, td_u8 *p)
{
    td_s32 ret;
    td_u32 pad_len, buf_len;

    pad_len = pad->klen - 3 - 19 - pad->in_len; /* 3, 19 */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memset_s(p, buf_len, 0xFF, pad_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    p += pad_len;
    *p++ = 0;
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, ASN1_HASH_SHA224, 19); /* 19 copy size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p += 19; /* 19 p shift */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 rsa_padding_add_emsa_pkcs1_v15_sha256(const rsa_padding_s *pad, td_u8 *p)
{
    td_s32 ret;
    td_u32 pad_len, buf_len;

    pad_len = pad->klen - 3 - 19 - pad->in_len; /* 3, 19 */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memset_s(p, buf_len, 0xFF, pad_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    p += pad_len;
    *p++ = 0;
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, ASN1_HASH_SHA256, 19); /* 19 copy size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p += 19; /* 19 p shift */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 rsa_padding_add_emsa_pkcs1_v15_sha384(const rsa_padding_s *pad, td_u8 *p)
{
    td_s32 ret;
    td_u32 pad_len, buf_len;

    pad_len = pad->klen - 3 - 19 - pad->in_len; /* 3, 19 */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memset_s(p, buf_len, 0xFF, pad_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    p += pad_len;
    *p++ = 0;
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, ASN1_HASH_SHA384, 19); /* 19 copy size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p += 19; /* 19 p shift */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 rsa_padding_add_emsa_pkcs1_v15_sha512(const rsa_padding_s *pad, td_u8 *p)
{
    td_s32 ret;
    td_u32 pad_len, buf_len;

    pad_len = pad->klen - 3 - 19 - pad->in_len; /* 3, 19 */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memset_s(p, buf_len, 0xFF, pad_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    p += pad_len;
    *p++ = 0;
    ret = memcpy_s(p, pad->klen, ASN1_HASH_SHA512, 19); /* 19 copy size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p += 19; /* 19 p shift */
    buf_len = pad->klen - (td_u32)(p - pad->out_data);
    ret = memcpy_s(p, buf_len, pad->in_data, pad->in_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 rsa_padding_add_emsa_pkcs1_v15(rsa_padding_s *pad)
{
    td_u8 *p = pad->out_data;

    pad->out_len = pad->klen;
    *p++ = 0;
    *p++ = RSA_SIGN;

    switch (pad->hash_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
            return rsa_padding_add_emsa_pkcs1_v15_sha1(pad, p);
        case OT_CIPHER_HASH_TYPE_SHA224:
            return rsa_padding_add_emsa_pkcs1_v15_sha224(pad, p);
        case OT_CIPHER_HASH_TYPE_SHA256:
            return rsa_padding_add_emsa_pkcs1_v15_sha256(pad, p);
        case OT_CIPHER_HASH_TYPE_SHA384:
            return rsa_padding_add_emsa_pkcs1_v15_sha384(pad, p);
        case OT_CIPHER_HASH_TYPE_SHA512:
            return rsa_padding_add_emsa_pkcs1_v15_sha512(pad, p);
        default:
            ot_err_cipher("RSA unsuporrt hash type: 0x%x.\n", pad->hash_type);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

/* *****************************************************************
                                  +-----------+
                                  |     M     |
                                  +-----------+
                                        |
                                        V
                                       Hash
                                        |
                                        V
                         +--------+----------+----------+
                   M' =  |Padding1|  mHash   |   salt   |
                         +--------+----------+----------+
                                        |
            +--------+----------+       V
     DB =   |Padding2|maskedseed|      Hash
            +--------+----------+       |
                    |                    |
                    V                    |      +--+
                    xor <----- MGF <----|      |bc|
                    |                    |      +--+
                    |                    |      |
                    V                    V      V
            +-------------------+----- -------+--+
    EM =    |    maskedDB       |  maskedseed |bc|
            +-------------------+-------------+--+
 ***************************************************************** */
static td_s32 rsa_padding_add_pkcs1_pss_hash(rsa_padding_s *pad, rsa_pkcs1_pss_s *pss)
{
    td_s32 ret;
    td_u32 mlen, index;
    td_u8 *ptr_m = TD_NULL;
    td_handle hash_handle = 0;
    ot_cipher_hash_attr hash_attr;

    mlen = pss->slen + pad->hlen + 8; /* 8 */
    ptr_m = (td_u8 *)memalign(ARCH_DMA_MINALIGN, mlen);
    chk_func_fail_return(ptr_m == TD_NULL, OT_ERR_CIPHER_INVALID_POINT, memalign);

    /* M' = (0x)00 00 00 00 00 00 00 00 || mHash || salt */
    if ((memset_s(ptr_m, mlen, 0x00, 8) != EOK) ||  /* 8 pad size */
        (memcpy_s(&ptr_m[8], mlen - 8, pad->in_data, pad->in_len) != EOK) || /* 8 pad size */
        (memcpy_s(&ptr_m[8 + pad->in_len], mlen - 8 - pad->in_len, pss->salt, pss->slen) != EOK)) { /* 8 pad size */
        ot_err_cipher("call sec func failed\n");
        cipher_free(ptr_m);
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    (td_void)memset_s(&hash_attr, sizeof(ot_cipher_hash_attr), 0, sizeof(ot_cipher_hash_attr));
    hash_attr.sha_type = pad->hash_type;
    if ((ot_mpi_cipher_hash_init(&hash_attr, &hash_handle) != TD_SUCCESS) ||
        (ot_mpi_cipher_hash_update(hash_handle, ptr_m, mlen) != TD_SUCCESS) ||
        (ot_mpi_cipher_hash_final(hash_handle, pss->masked_seed) != TD_SUCCESS)) {
        ot_err_cipher("call hash func failed\n");
        cipher_free(ptr_m);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    cipher_free(ptr_m); /* Must free ptr_m befort return */

    /* formula: maskedDB = DB xor dbMask, DB = PS || 0x01 || salt */
    index = 0;
    ret = memset_s(&pss->masked_db[index], pss->key_len - index,
        0x00, pss->key_len - pss->slen - pad->hlen - 2); /* 2 */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    index += pss->key_len - pss->slen - pad->hlen - 2; /* 2 */
    pss->masked_db[index++] = 0x01;
    ret = memcpy_s(&pss->masked_db[index], pss->key_len - index, pss->salt, pss->slen);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    ret = rsa_pkcs1_mgf1(pad->hash_type,
        pss->masked_seed, pad->hlen, pss->masked_db, pss->key_len - pad->hlen - 1);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    pad->out_data[pss->key_len - 1] = 0xBC;

    if (pss->msb_bits) {
        pad->out_data[0] &= 0xFF >> (8 - pss->msb_bits); /* 8 */
    }

    return ret;
}

static td_s32 rsa_padding_add_pkcs1_pss(rsa_padding_s *pad)
{
    rsa_pkcs1_pss_s pss;
    td_s32 ret;

    (td_void)memset_s(&pss, sizeof(rsa_pkcs1_pss_s), 0, sizeof(rsa_pkcs1_pss_s));
    pss.slen = pad->hlen;
    pss.key_len = (pad->em_bit + 7) / 8; /* 7, 8 */
    pss.msb_bits = (pad->em_bit - 1) & 0x07;

    pad->out_len = pss.key_len;

    if (pss.key_len < (pad->hlen + pss.slen + 2)) { /* 2 */
        ot_err_cipher("message too long\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (pss.msb_bits == 0) {
        *pad->out_data++ = 0;
        pss.key_len--;
    }

    pss.masked_db = pad->out_data;
    pss.masked_seed = pad->out_data + pss.key_len - pad->hlen - 1;

    /* Generate a random octet string salt of length sLen */
    ret = mbedtls_get_random_number(TD_NULL, pss.salt, pss.slen);
    chk_func_fail_return(ret != TD_SUCCESS, ret, mbedtls_get_random_number);

    return rsa_padding_add_pkcs1_pss_hash(pad, &pss);
}

static td_s32 rsa_padding_check_pkcs1_oaep(rsa_padding_s *pad)
{
    td_s32 ret;
    td_u32 i;
    const td_s8 *l_hash = g_empty_l_sha1;
    td_u8 *ptr_seed = TD_NULL;
    td_u8 *ptr_db = TD_NULL;
    td_u8 *masked_db = TD_NULL;

    if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA224) {
        l_hash = g_empty_l_sha224;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA256) {
        l_hash = g_empty_l_sha256;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA384) {
        l_hash = g_empty_l_sha384;
    } else if (pad->hash_type == OT_CIPHER_HASH_TYPE_SHA512) {
        l_hash = g_empty_l_sha512;
    }

    if (pad->klen < 2 * pad->hlen + 2) { /* 2 */
        ot_err_cipher("input_len is invalid.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (pad->in_data[0] != 0x00) {
        ot_err_cipher("EM[0] != 0.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    pad->out_len = 0;
    masked_db = pad->in_data + pad->hlen + 1;
    ptr_seed = pad->in_data + 1;
    ptr_db = pad->in_data + pad->hlen + 1;
    ret = rsa_pkcs1_mgf1(pad->hash_type, masked_db, pad->klen - pad->hlen - 1, ptr_seed, pad->hlen);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    ret = rsa_pkcs1_mgf1(pad->hash_type, ptr_seed, pad->hlen, ptr_db, pad->klen - pad->hlen - 1);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    ret = memcmp(ptr_db, l_hash, pad->hlen);
    chk_func_fail_return(ret != 0, OT_ERR_CIPHER_FAILED_DECRYPT, memcmp);

    for (i = pad->hlen; i < pad->klen - pad->hlen - 1; i++) {
        if ((ptr_db[i] == 0x01)) {
            ret = memcpy_s(pad->out_data, pad->klen, ptr_db + i + 1, pad->klen - pad->hlen - i - 2); /* 2 */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

            pad->out_len = pad->klen - pad->hlen - i - 2; /* 2 */
            break;
        }
    }
    if (i >= pad->klen - pad->hlen - 1) {
        ot_err_cipher("PS error.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    return TD_SUCCESS;
}

static td_s32 rsa_padding_check_pkcs1_v15(rsa_padding_s *pad)
{
    td_u32 index = 0;

    if (pad->klen < 11) { /* 11 pad->klen max size */
        ot_err_cipher("input_len is invalid.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (pad->in_data[index] != 0x00) {
        ot_err_cipher("EM[0] != 0x00.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    index++;
    if (pad->in_data[index] != 0x02) {
        ot_err_cipher("EM[1] != 0x02.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    index++;
    for (; index < pad->klen; index++) {
        /* The length of PS is large than 8 octets */
        if ((index >= 10) && (pad->in_data[index] == 0x00)) { /* 10 */
            if (memcpy_s(pad->out_data, pad->klen, &pad->in_data[index + 1], pad->klen - 1 - index) != EOK) {
                ot_err_cipher("call failed memcpy_s\n");
                return OT_ERR_CIPHER_FAILED_SEC_FUNC;
            }
            pad->out_len = pad->klen - 1 - index;
            break;
        }
    }

    if (index >= pad->klen) {
        ot_err_cipher("PS error.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    return TD_SUCCESS;
}

static td_s32 rsa_padding_check_pkcs1_type(rsa_padding_s *pad)
{
    td_u8 *key_eb = pad->in_data;

    if (*key_eb != 0x00) {
        ot_err_cipher("EB[0] != 0x00.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    key_eb++;
    if (*key_eb != pad->key_bt) {
        ot_err_cipher("EB[1] != BT(0x%x).\n", pad->key_bt);
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    key_eb++;
    if (pad->key_bt == 0x00) {
        for (; key_eb < pad->in_data + pad->in_len - 1; key_eb++) {
            if ((*key_eb == 0x00) && (*(key_eb + 1) != 0))
                break;
        }
    } else if (pad->key_bt == 0x01) {
        for (; key_eb < pad->in_data + pad->in_len - 1; key_eb++) {
            if (*key_eb == 0xFF) {
                continue;
            } else if (*key_eb == 0x00) {
                break;
            } else {
                key_eb = pad->in_data + pad->in_len - 1;
                break;
            }
        }
    } else if (pad->key_bt == 0x02) {
        for (; key_eb < pad->in_data + pad->in_len - 1; key_eb++) {
            if (*key_eb == 0x00)
                break;
        }
    } else {
        ot_err_cipher("BT(0x%x) is invalid.\n", pad->key_bt);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (key_eb >= (pad->in_data + pad->in_len - 1)) {
        ot_err_cipher("PS Error.\n");
        return OT_ERR_CIPHER_FAILED_DECRYPT;
    }

    key_eb++;
    pad->out_len = pad->in_data + pad->klen - key_eb;
    if (memcpy_s(pad->out_data, pad->klen, key_eb, pad->out_len) != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    return TD_SUCCESS;
}

static td_s32 rsa_padding_check_emsa_pkcs1_v15_type(rsa_padding_s *pad, const td_u8 *p)
{
    td_s32 ret;
    td_u32 len;

    len = pad->klen - (td_u32)(p - pad->in_data);

    switch (pad->hash_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
            chk_u32_data_fail_return(len, 35); /* 35 len size */
            ret = memcmp(p, ASN1_HASH_SHA1, 15); /* 15 compare size */
            chk_func_fail_return(ret != 0, TD_FAILURE, memcmp);

            ret = memcpy_s(pad->out_data, pad->klen, p + 15, pad->hlen); /* 15 copy shift */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_HASH_TYPE_SHA224:
            chk_u32_data_fail_return(len, (19 + pad->hlen)); /* len size: 19 + pad->hlen */
            ret = memcmp(p, ASN1_HASH_SHA224, 19); /* 19 compare size */
            chk_func_fail_return(ret != 0, TD_FAILURE, memcmp);

            ret = memcpy_s(pad->out_data, pad->klen, p + 19, pad->hlen); /* 19 copy shift */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_HASH_TYPE_SHA256:
            chk_u32_data_fail_return(len, (19 + pad->hlen)); /* len size: 19 + pad->hlen */
            ret = memcmp(p, ASN1_HASH_SHA256, 19); /* 19 compare size */
            chk_func_fail_return(ret != 0, TD_FAILURE, memcmp);

            ret = memcpy_s(pad->out_data, pad->klen, p + 19, pad->hlen); /* 19 copy shift */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_HASH_TYPE_SHA384:
            chk_u32_data_fail_return(len, (19 + pad->hlen)); /* len size: 19 + pad->hlen */
            ret = memcmp(p, ASN1_HASH_SHA384, 19); /* 19 compare size */
            chk_func_fail_return(ret != 0, TD_FAILURE, memcmp);

            ret = memcpy_s(pad->out_data, pad->klen, p + 19, pad->hlen); /* 19 copy shift */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_HASH_TYPE_SHA512:
            chk_u32_data_fail_return(len, (19 + pad->hlen)); /* len size: 19 + pad->hlen */
            ret = memcmp(p, ASN1_HASH_SHA512, 19); /* 19 compare size */
            chk_func_fail_return(ret != 0, TD_FAILURE, memcmp);

            ret = memcpy_s(pad->out_data, pad->klen, p + 19, pad->hlen); /* 19 copy shift */
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        default:
            ot_err_cipher("RSA unsuporrt hash type: 0x%x.\n", pad->hash_type);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }
    return ret;
}

static td_s32 rsa_padding_check_emsa_pkcs1_v15(rsa_padding_s *pad)
{
    td_u8 *p = pad->in_data;

    pad->out_len = pad->hlen;

    /* formula: EM = 01 || PS || 00 || T */
    if (*p++ != 0) {
        ot_err_cipher("RSA EM[0] must be 0\n");
        return TD_FAILURE;
    }

    if (*p++ != RSA_SIGN) {
        ot_err_cipher("RSA EM PS error\n");
        return TD_FAILURE;
    }

    while (*p != 0) {
        if (p >= pad->in_data + pad->klen - 1 || *p != 0xFF) {
            ot_err_cipher("RSA PS error\n");
            return TD_FAILURE;
        }
        p++;
    }
    p++; // skip 0x00

    return rsa_padding_check_emsa_pkcs1_v15_type(pad, p);
}

static td_s32 rsa_padding_check_pkcs1_pss_hash(const rsa_padding_s *pad,
    const td_u8 *mhash, const rsa_pkcs1_pss_s *pss)
{
    td_u32 mlen;
    td_u8 *ptr_m = TD_NULL;
    td_handle hash_handle = 0;
    ot_cipher_hash_attr hash_attr;
    td_u8 arr_h[HASH_RESULT_MAX_LEN] = {0};

    mlen = pss->slen + pad->hlen + 8; /* 8 */
    ptr_m = (td_u8 *)cipher_malloc(mlen);
    chk_func_fail_return(ptr_m == TD_NULL, OT_ERR_CIPHER_INVALID_POINT, cipher_malloc);
    (td_void)memset_s(ptr_m, mlen, 0, mlen);

    /* M' = (0x)00 00 00 00 00 00 00 00 || mHash || salt */
    if ((memset_s(ptr_m, mlen, 0x00, 8) != EOK) ||  /* 8 pad size */
        (memcpy_s(&ptr_m[8], mlen - 8, mhash, pad->hlen) != EOK) || /* 8 pad size */
        (memcpy_s(&ptr_m[8 + pad->hlen], mlen - 8 - pad->hlen, pss->salt, pss->slen) != EOK)) { /* 8 pad size */
        ot_err_cipher("call sec func failed\n");
        cipher_free(ptr_m);
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    (td_void)memset_s(&hash_attr, sizeof(ot_cipher_hash_attr), 0, sizeof(ot_cipher_hash_attr));
    hash_attr.sha_type = pad->hash_type;
    if ((ot_mpi_cipher_hash_init(&hash_attr, &hash_handle) != TD_SUCCESS) ||
        (ot_mpi_cipher_hash_update(hash_handle, ptr_m, mlen) != TD_SUCCESS) ||
        (ot_mpi_cipher_hash_final(hash_handle, arr_h) != TD_SUCCESS)) {
        ot_err_cipher("call hash func failed\n");
        cipher_free(ptr_m);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    cipher_free(ptr_m); /* Must free ptr_m befort return */

    if (memcmp(arr_h, pss->masked_seed, pad->hlen) != 0) {
        ot_err_cipher("compare failed\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 rsa_padding_check_pkcs1_pss(rsa_padding_s *pad, const td_u8 *mhash, td_u32 hash_len)
{
    td_s32 ret;
    td_u32 index, tmp_len;
    rsa_pkcs1_pss_s pss;

    (td_void)memset_s(&pss, sizeof(rsa_pkcs1_pss_s), 0, sizeof(rsa_pkcs1_pss_s));
    pss.slen = pad->hlen;
    pss.key_len = (pad->em_bit + 7) / 8; /* 7, 8 */
    pss.msb_bits = (pad->em_bit - 1) & 0x07;

    if (pss.key_len < (pad->hlen + pss.slen + 2)) { /* 2 */
        ot_err_cipher("message too long\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (pad->in_data[0] & (0xFF << pss.msb_bits)) {
        ot_err_cipher("inconsistent, EM[0] invalid\n");
        return TD_FAILURE;
    }

    if (pss.msb_bits == 0) {
        pad->in_data++;
        pss.key_len--;
    }

    pss.masked_db = pad->in_data;
    pss.masked_seed = pad->in_data + pss.key_len - pad->hlen - 1;

    if (pad->in_data[pss.key_len - 1] != 0xBC) {
        ot_err_cipher("inconsistent, EM[key_len - 1] != 0xBC\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* formula: maskedDB = DB xor dbMask, DB = PS || 0x01 || salt */
    ret = rsa_pkcs1_mgf1(pad->hash_type, pss.masked_seed,
        pad->hlen, pss.masked_db, pss.key_len - pad->hlen - 1);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pkcs1_mgf1);

    if (pss.msb_bits)
        pss.masked_db[0] &= 0xFF >> (8 - pss.msb_bits); /* 8 */

    tmp_len = pss.key_len - pss.slen - pad->hlen - 2; /* 2 */
    if (tmp_len >= CIPHER_MAX_RSA_KEY_LEN - 1) { /* -1 is for index++, avoid masked_db overflow */
        ot_err_cipher("operate masked_db maybe overflow %u\n", tmp_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    for (index = 0; index < tmp_len; index++) {
        if (pss.masked_db[index] != 0x00) {
            break;
        }
    }
    pss.slen = pss.key_len - pad->hlen - index - 2; /* 2 */

    chk_u32_data_fail_return(pss.masked_db[index], 0x01);
    index++;
    ret = memcpy_s(pss.salt, sizeof(pss.salt), &pss.masked_db[index], pss.slen);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return rsa_padding_check_pkcs1_pss_hash(pad, mhash, &pss);
}

static td_s32 rsa_pub_enc_pad_init(rsa_padding_s *pad, const ot_cipher_rsa_pub_encrypt *rsa_enc,
    const ot_cipher_rsa_crypt *rsa_crypt, td_u8 *arr_em, td_u32 em_len)
{
    td_s32 ret;

    if (em_len < rsa_crypt->in_len) {
        ot_err_cipher("buf len %u < in len %u\n", em_len, rsa_crypt->in_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    pad->in_data = (td_u8 *)rsa_crypt->in;
    pad->in_len = rsa_crypt->in_len;
    pad->out_data = arr_em;
    ret = rsa_get_attr(rsa_enc->scheme, rsa_enc->pub_key.n_len, pad);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    chk_formula_fail_return(pad->in_len > pad->klen);

    return ret;
}

static td_s32 rsa_pri_dec_pad_init(rsa_padding_s *pad, const ot_cipher_rsa_private_encrypt *rsa_decrypt,
    ot_cipher_rsa_crypt *rsa_crypt, td_u8 *arr_em, td_u32 em_len)
{
    td_s32 ret;

    if (em_len < rsa_crypt->in_len) {
        ot_err_cipher("buf len %u < in len %u\n", em_len, rsa_crypt->in_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    pad->in_data = arr_em;
    pad->in_len = rsa_crypt->in_len;
    pad->out_data = rsa_crypt->out;
    ret = rsa_get_attr(rsa_decrypt->scheme, rsa_decrypt->private_key.n_len, pad);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    chk_u32_data_fail_return(pad->in_len, pad->klen);

    return ret;
}

static td_s32 rsa_pri_enc_pad_init(rsa_padding_s *pad, const ot_cipher_rsa_private_encrypt *rsa_encrypt,
    const ot_cipher_rsa_crypt *rsa_crypt, td_u8 *arr_em, td_u32 em_len)
{
    td_s32 ret;

    if (em_len < rsa_crypt->in_len) {
        ot_err_cipher("buf len %u < in len %u\n", em_len, rsa_crypt->in_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    pad->in_data = (td_u8 *)rsa_crypt->in;
    pad->in_len = rsa_crypt->in_len;
    pad->out_data = arr_em;
    ret = rsa_get_attr(rsa_encrypt->scheme, rsa_encrypt->private_key.n_len, pad);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    chk_formula_fail_return(pad->in_len > pad->klen);

    return ret;
}

static td_s32 rsa_pub_dec_pad_init(rsa_padding_s *pad, const ot_cipher_rsa_pub_encrypt *rsa_decrypt,
    ot_cipher_rsa_crypt *rsa_crypt, td_u8 *arr_em, td_u32 em_len)
{
    td_s32 ret;

    if (em_len < rsa_crypt->in_len) {
        ot_err_cipher("buf len %u < in len %u\n", em_len, rsa_crypt->in_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    pad->in_data = arr_em;
    pad->in_len = rsa_crypt->in_len;
    pad->out_data = rsa_crypt->out;
    ret = rsa_get_attr(rsa_decrypt->scheme, rsa_decrypt->pub_key.n_len, pad);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    chk_u32_data_fail_return(pad->in_len, pad->klen);

    return ret;
}

static td_s32 rsa_sign_pad_init(rsa_padding_s *pad,
    const ot_cipher_rsa_sign *rsa_sign, const ot_cipher_sign_data *sign_data, rsa_sign_buf *sign_buf)
{
    td_s32 ret;
    td_handle hash_handle;

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    ret = rsa_get_attr(rsa_sign->scheme, rsa_sign->private_key.n_len, pad);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    pad->out_data = sign_buf->arr_em;
    pad->in_len = pad->hlen;

    /* hash is NULl, need to calc by self */
    if (sign_data->hash_data != TD_NULL) {
        pad->in_data = (td_u8 *)sign_data->hash_data;
    } else {
        ot_cipher_hash_attr hash_attr = {0};
        hash_attr.sha_type = pad->hash_type;
        ret = ot_mpi_cipher_hash_init(&hash_attr, &hash_handle);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_init);

        ret = ot_mpi_cipher_hash_update(hash_handle, (td_u8 *)sign_data->in, sign_data->in_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_update);

        ret = ot_mpi_cipher_hash_final(hash_handle, sign_buf->sign_hash);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_final);

        pad->in_data = sign_buf->sign_hash;
    }

    return ret;
}

static td_s32 rsa_verify_pad_init(rsa_padding_s *pad, const ot_cipher_rsa_verify *rsa_verify,
    const ot_cipher_verify_data *verify_data, rsa_sign_buf *sign_buf)
{
    td_s32 ret;

    (td_void)memset_s(pad, sizeof(rsa_padding_s), 0, sizeof(rsa_padding_s));
    ret = rsa_get_attr(rsa_verify->scheme, rsa_verify->pub_key.n_len, pad);
    pad->in_data = sign_buf->arr_em;
    pad->in_len = verify_data->sign_len;
    pad->out_data = sign_buf->sign_hash;
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_get_attr);
    chk_u32_data_fail_return(verify_data->sign_len, pad->klen);

    ret = rsa_public(&rsa_verify->pub_key, verify_data->sign, pad->in_data);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_public);

    return ret;
}

static td_s32 rsa_verify_get_hash(const ot_cipher_verify_data *verify_data,
    td_u8 *arr_hash, td_u32 hash_len, ot_cipher_hash_type hash_type)
{
    td_s32 ret;
    ot_cipher_hash_attr hash_attr;
    td_handle hash_handle;

    if (verify_data->hash_data != TD_NULL) {
        ret = memcpy_s(arr_hash, hash_len, verify_data->hash_data, verify_data->hash_data_len);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
    } else {
        (td_void)memset_s(&hash_attr, sizeof(ot_cipher_hash_attr), 0, sizeof(ot_cipher_hash_attr));
        hash_attr.sha_type = hash_type;
        ret = ot_mpi_cipher_hash_init(&hash_attr, &hash_handle);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_init);

        ret = ot_mpi_cipher_hash_update(hash_handle, (td_u8 *)verify_data->in, verify_data->in_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_init);

        ret = ot_mpi_cipher_hash_final(hash_handle, arr_hash);
        chk_func_fail_return(ret != TD_SUCCESS, ret, ot_mpi_cipher_hash_init);
    }
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_rsa_pub_encrypt(
    const ot_cipher_rsa_pub_encrypt *rsa_enc, ot_cipher_rsa_crypt *rsa_crypt)
{
    td_s32 ret;
    rsa_padding_s pad;
    td_u8 arr_em[CIPHER_MAX_RSA_KEY_LEN] = {0};

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_enc);
    chk_ptr_null_return(rsa_crypt);
    chk_ptr_null_return(rsa_crypt->in);
    chk_ptr_null_return(rsa_crypt->out);
    chk_ptr_null_return(rsa_crypt->out_len);
    chk_formula_fail_return(rsa_crypt->in_len == 0);

    ret = rsa_pub_enc_pad_init(&pad, rsa_enc, rsa_crypt, arr_em, sizeof(arr_em));
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pub_enc_pad_init);

    switch (rsa_enc->scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_NO_PADDING:
            /* NO PADDING scheme inlen must be the same as klen */
            chk_formula_fail_return(pad.in_len != pad.klen);

            pad.out_len = pad.klen;
            ret = memcpy_s(pad.out_data, pad.out_len, pad.in_data, pad.in_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_1:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_enc->scheme);
            ot_err_cipher("For a public key encryption operation, the block type shall be 02.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_2:
            pad.key_bt = (td_u8)(rsa_enc->scheme - OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0);
            ret = rsa_padding_add_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_type);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA1:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA224:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
            ret = rsa_padding_add_pkcs1_oaep(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_oaep);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            ret = rsa_padding_add_pkcs1_v15(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_v15);
            break;
        default:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_enc->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    *rsa_crypt->out_len = pad.out_len;
    return rsa_public(&rsa_enc->pub_key, pad.out_data, rsa_crypt->out);
}

td_s32 ot_mpi_cipher_rsa_private_decrypt(
    const ot_cipher_rsa_private_encrypt *rsa_decrypt, ot_cipher_rsa_crypt *rsa_crypt)
{
    td_s32 ret;
    td_u8 arr_em[CIPHER_MAX_RSA_KEY_LEN] = {0};
    rsa_padding_s pad;

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_decrypt);
    chk_ptr_null_return(rsa_crypt);
    chk_ptr_null_return(rsa_crypt->in);
    chk_ptr_null_return(rsa_crypt->out);
    chk_ptr_null_return(rsa_crypt->out_len);
    chk_formula_fail_return(rsa_crypt->in_len == 0);

    ret = rsa_pri_dec_pad_init(&pad, rsa_decrypt, rsa_crypt, arr_em, sizeof(arr_em));
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pri_dec_pad_init);

    ret = rsa_private(&rsa_decrypt->private_key, rsa_decrypt->ca_type, rsa_crypt->in, pad.in_data);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_private);

    switch (rsa_decrypt->scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_NO_PADDING:
            pad.out_len = pad.in_len;
            ret = memcpy_s(pad.out_data, pad.out_len, pad.in_data, pad.in_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_1:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_decrypt->scheme);
            ot_err_cipher("For a private key decryption operation, the block type shall be 02.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_2:
            pad.key_bt = (td_u8)(rsa_decrypt->scheme - OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0);
            ret = rsa_padding_check_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_type);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA1:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA224:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
            ret = rsa_padding_check_pkcs1_oaep(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_oaep);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            ret = rsa_padding_check_pkcs1_v15(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_oaep);
            break;
        default:
            ot_err_cipher("RSA scheme error, scheme = 0x%x.\n", rsa_decrypt->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    *rsa_crypt->out_len = pad.out_len;
    return ret;
}

td_s32 ot_mpi_cipher_rsa_private_encrypt(
    const ot_cipher_rsa_private_encrypt *rsa_encrypt, ot_cipher_rsa_crypt *rsa_crypt)
{
    td_s32 ret;
    rsa_padding_s pad;
    td_u8 arr_em[CIPHER_MAX_RSA_KEY_LEN] = {0};

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_encrypt);
    chk_ptr_null_return(rsa_crypt);
    chk_ptr_null_return(rsa_crypt->in);
    chk_ptr_null_return(rsa_crypt->out);
    chk_ptr_null_return(rsa_crypt->out_len);
    chk_formula_fail_return(rsa_crypt->in_len == 0);

    ret = rsa_pri_enc_pad_init(&pad, rsa_encrypt, rsa_crypt, arr_em, sizeof(arr_em));
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pri_enc_pad_init);

    switch (rsa_encrypt->scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_NO_PADDING:
            /* NO PADDING scheme inlen must be the same as klen */
            chk_formula_fail_return(pad.in_len != pad.klen);

            pad.out_len = pad.klen;
            ret = memcpy_s(pad.out_data, pad.out_len, pad.in_data, pad.in_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_1:
            pad.key_bt = (td_u8)(rsa_encrypt->scheme - OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0);
            ret = rsa_padding_add_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_type);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_2:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_encrypt->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA1:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA224:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
            ret = rsa_padding_add_pkcs1_oaep(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_oaep);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            pad.key_bt = 0x01;
            ret = rsa_padding_add_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_type);
            break;
        default:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_encrypt->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    chk_u32_data_fail_return(pad.out_len, pad.klen);
    *rsa_crypt->out_len = pad.out_len;

    return rsa_private(&rsa_encrypt->private_key, rsa_encrypt->ca_type, pad.out_data, rsa_crypt->out);
}

td_s32 ot_mpi_cipher_rsa_pub_decrypt(
    const ot_cipher_rsa_pub_encrypt *rsa_decrypt, ot_cipher_rsa_crypt *rsa_crypt)
{
    td_s32 ret;
    td_u8 arr_em[CIPHER_MAX_RSA_KEY_LEN] = {0};
    rsa_padding_s pad;

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_decrypt);
    chk_ptr_null_return(rsa_crypt);
    chk_ptr_null_return(rsa_crypt->in);
    chk_ptr_null_return(rsa_crypt->out);
    chk_ptr_null_return(rsa_crypt->out_len);
    chk_formula_fail_return(rsa_crypt->in_len == 0);

    ret = rsa_pub_dec_pad_init(&pad, rsa_decrypt, rsa_crypt, arr_em, sizeof(arr_em));
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_pub_dec_pad_init);

    ret = rsa_public(&rsa_decrypt->pub_key, rsa_crypt->in, pad.in_data);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_public);

    switch (rsa_decrypt->scheme) {
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_NO_PADDING:
            pad.out_len = pad.in_len;
            ret = memcpy_s(pad.out_data, pad.out_len, pad.in_data, pad.in_len);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_1:
            pad.key_bt = (td_u8)(rsa_decrypt->scheme - OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_0);
            ret = rsa_padding_check_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_type);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_BLOCK_TYPE_2:
            ot_err_cipher("RSA padding mode error, mode = 0x%x.\n", rsa_decrypt->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA1:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA224:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384:
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512:
            ret = rsa_padding_check_pkcs1_oaep(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_oaep);
            break;
        case OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5:
            pad.key_bt = 0x01;
            ret = rsa_padding_check_pkcs1_type(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_type);
            break;
        default:
            ot_err_cipher("RSA scheme error, scheme = 0x%x.\n", rsa_decrypt->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    *rsa_crypt->out_len = pad.out_len;
    return ret;
}

td_s32 ot_mpi_cipher_rsa_sign(
    const ot_cipher_rsa_sign *rsa_sign, ot_cipher_sign_data *sign_data)
{
    td_s32 ret;
    rsa_padding_s pad;
    rsa_sign_buf sign_buf;

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_sign);
    chk_ptr_null_return(sign_data);
    chk_ptr_null_return(sign_data->sign);
    chk_ptr_null_return(sign_data->sign_len);
    chk_ptr_null_return(rsa_sign->private_key.n);

    chk_formula_fail_return(((sign_data->hash_data == TD_NULL) || (sign_data->hash_data_len == 0)) &&
        ((sign_data->in == TD_NULL) || (sign_data->in_len == 0)));

    (td_void)memset_s(&sign_buf, sizeof(rsa_sign_buf), 0, sizeof(rsa_sign_buf));
    ret = rsa_sign_pad_init(&pad, rsa_sign, sign_data, &sign_buf);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_sign_pad_init);

    switch (rsa_sign->scheme) {
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512:
            ret = rsa_padding_add_emsa_pkcs1_v15(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_emsa_pkcs1_v15);
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512:
            pad.em_bit = rsa_get_bit_num(rsa_sign->private_key.n, pad.klen);
            ret = rsa_padding_add_pkcs1_pss(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_add_pkcs1_pss);
            break;
        default:
            ot_err_cipher("invalid scheme; 0x%x\n", rsa_sign->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    chk_u32_data_fail_return(pad.out_len, pad.klen);
    *sign_data->sign_len = pad.out_len;

    return rsa_private(&rsa_sign->private_key, rsa_sign->ca_type, pad.out_data, sign_data->sign);
}

td_s32 ot_mpi_cipher_rsa_verify(
    const ot_cipher_rsa_verify *rsa_verify, const ot_cipher_verify_data *verify_data)
{
    td_s32 ret;
    rsa_padding_s pad;
    rsa_sign_buf sign_buf;
    td_u8 arr_hash[HASH_RESULT_MAX_LEN] = {0};

    chk_dev_open_fail_return();
    chk_ptr_null_return(rsa_verify);
    chk_ptr_null_return(verify_data);
    chk_ptr_null_return(verify_data->sign);

    chk_formula_fail_return(((verify_data->hash_data == TD_NULL) || (verify_data->hash_data_len == 0)) &&
        ((verify_data->in == TD_NULL) || (verify_data->in_len == 0)));

    (td_void)memset_s(&sign_buf, sizeof(rsa_sign_buf), 0, sizeof(rsa_sign_buf));
    ret = rsa_verify_pad_init(&pad, rsa_verify, verify_data, &sign_buf);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_verify_pad_init);

    ret = rsa_verify_get_hash(verify_data, arr_hash, sizeof(arr_hash), pad.hash_type);
    chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_verify_get_hash);

    switch (rsa_verify->scheme) {
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512:
            ret = rsa_padding_check_emsa_pkcs1_v15(&pad);
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_emsa_pkcs1_v15);
            ret = memcmp(arr_hash, pad.out_data, pad.hlen);

            chk_func_fail_return(ret != 0, OT_ERR_CIPHER_FAILED_DECRYPT, memcmp);
            break;
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384:
        case OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512:
            pad.em_bit = rsa_get_bit_num(rsa_verify->pub_key.n, pad.klen);
            ret = rsa_padding_check_pkcs1_pss(&pad, arr_hash, sizeof(arr_hash));
            chk_func_fail_return(ret != TD_SUCCESS, ret, rsa_padding_check_pkcs1_pss);
            break;
        default:
            ot_err_cipher("invalid scheme; 0x%x\n", rsa_verify->scheme);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return ret;
}
