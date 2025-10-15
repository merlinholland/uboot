// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "drv_klad.h"
#include "cipher_adapt.h"
#include "drv_rng.h"
#include "drv_klad.h"

#ifdef RSA_ENABLE

CIPHER_MUTEX g_rsa_mutex_kernel;
static td_void *g_rsa_reg_base  = TD_NULL;
static td_u32   g_rsa_done      = TD_FALSE;

#define cipher_rsa_return_invalid_param(param) \
    do { \
        if (param) { \
            ot_err_cipher("Invalid params!\n"); \
            return OT_ERR_CIPHER_INVALID_PARAM; \
        } \
    } while (0)

#define RSA_INTERRUPT_ENABLE
#define RSA_IRQ_NUMBER                          136

#define CIPHER_RSA_REG_BASE_RSA                   g_rsa_reg_base
#define SEC_RSA_BUSY_REG                          (CIPHER_RSA_REG_BASE_RSA + 0x50)
#define SEC_RSA_MOD_REG                           (CIPHER_RSA_REG_BASE_RSA + 0x54)
#define SEC_RSA_WSEC_REG                          (CIPHER_RSA_REG_BASE_RSA + 0x58)
#define SEC_RSA_WDAT_REG                          (CIPHER_RSA_REG_BASE_RSA + 0x5c)
#define SEC_RSA_RPKT_REG                          (CIPHER_RSA_REG_BASE_RSA + 0x60)
#define SEC_RSA_RRSLT_REG                         (CIPHER_RSA_REG_BASE_RSA + 0x64)
#define SEC_RSA_START_REG                         (CIPHER_RSA_REG_BASE_RSA + 0x68)
#define SEC_RSA_ADDR_REG                          (CIPHER_RSA_REG_BASE_RSA + 0x6C)
#define SEC_RSA_ERROR_REG                         (CIPHER_RSA_REG_BASE_RSA + 0x70)
#define SEC_RSA_CRC16_REG                         (CIPHER_RSA_REG_BASE_RSA + 0x74)
#define SEC_RSA_KEY_RANDOM_1                      (CIPHER_RSA_REG_BASE_RSA + 0x7c)
#define SEC_RSA_INT_EN                            (CIPHER_RSA_REG_BASE_RSA + 0x80)
#define SEC_RSA_INT_STATUS                        (CIPHER_RSA_REG_BASE_RSA + 0x84)
#define SEC_RSA_INT_RAW                           (CIPHER_RSA_REG_BASE_RSA + 0x88)
#define SEC_RSA_INT_ERR_CLR                       (CIPHER_RSA_REG_BASE_RSA + 0x8c)
#define SEC_RSA_KEY_RANDOM_2                      (CIPHER_RSA_REG_BASE_RSA + 0x94)
#define SEC_RSA_VERSION                           (CIPHER_RSA_REG_BASE_RSA + 0x90)

#define RSA_DATA_CLR               (7 << 4)
#define RSA_DATA_CLR_KEY           (1 << 4)
#define RSA_DATA_CLR_INPUT         (2 << 4)
#define RSA_DATA_CLR_OUTPUT        (4 << 4)
#define RSA_MOD_SEL                (3 << 0)
#define RSA_MOD_SEL_OPT            (0 << 0)
#define RSA_MOD_SEL_KEY_UPDATA     (1 << 0)
#define RSA_MOD_SEL_RAM_CLAER      (2 << 0)
#define RSA_MOD_SEL_CRC16          (3 << 0)
#define RSA_BUSY                   (1 << 0)
#define RSA_START                  (1 << 0)

#define RSA_RTY_CNT             500000
#define RSA_TIME_OUT            1000

#define RSA_RETRY_CNT           3

#define CRC16_POLYNOMIAL        0x1021

typedef enum {
    CIPHER_RSA_DATA_TYPE_CONTEXT,
    CIPHER_RSA_DATA_TYPE_MODULE,
    CIPHER_RSA_DATA_TYPE_KEY,
} cipher_rsa_data_type_e;

typedef enum {
    CIPHER_RSA_KEY_WIDTH_1K   = 0x00,
    CIPHER_RSA_KEY_WIDTH_2K   = 0x01,
    CIPHER_RSA_KEY_WIDTH_4K   = 0x02,
    CIPHER_RSA_KEY_WIDTH_3K   = 0x03,
    CIPHER_RSA_KEY_WIDTH_BUTT = 0xff,
} cipher_rsa_key_width_e;

static td_void hal_rsa_start(td_void)
{
    hal_cipher_write_reg(SEC_RSA_START_REG, 0x05);
}

static td_s32 hal_rsa_wait_free(td_void)
{
    td_u32 value;
    td_u32 try_count = 0;

    do {
        hal_cipher_read_reg(SEC_RSA_BUSY_REG, &value);
        if ((value & RSA_BUSY) == 0)
            return TD_SUCCESS;
        try_count++;
        cipher_udelay(10); /* 10us */
    } while (try_count < RSA_RTY_CNT);

    return TD_FAILURE;
}

static td_void hal_rsa_clear_ram(td_void)
{
    td_u32 value;

    hal_cipher_read_reg(SEC_RSA_MOD_REG, &value);
    value &= 0x0c;
    value |= RSA_DATA_CLR_INPUT | RSA_DATA_CLR_OUTPUT | RSA_DATA_CLR_KEY | RSA_MOD_SEL_RAM_CLAER;
    hal_cipher_write_reg(SEC_RSA_MOD_REG, value);
}

static td_void hal_rsa_config_mode(cipher_rsa_key_width_e ken_width)
{
    td_u32 value;

    value = ((td_u32)ken_width << 2) | RSA_MOD_SEL_OPT; /* 2 left shift */
    hal_cipher_write_reg(SEC_RSA_MOD_REG, value);
}

static td_void hal_rsa_write_data(cipher_rsa_data_type_e data_type,
    const td_u8 *data, td_u32 data_len, td_u32 length, const td_u32 random[2]) /* 2 random size */
{
    td_u32 *reg = TD_NULL;
    const td_u8 *pos = TD_NULL;
    td_u32 i, value;
    td_bool id = 0;

    if (data_type == CIPHER_RSA_DATA_TYPE_CONTEXT) {
        reg = SEC_RSA_WDAT_REG;
    } else {
        reg = SEC_RSA_WSEC_REG;
    }

    pos = data;
    for (i = 0; i < length; i += 4) { /* 4 groups */
        value  = (td_u32)pos[0];
        value |= ((td_u32)pos[1]) << 8;  /* 1 index, 8  left shift */
        value |= ((td_u32)pos[2]) << 16; /* 2 index, 16 left shift */
        value |= ((td_u32)pos[3]) << 24; /* 3 index, 24 left shift */
        if (data_type != CIPHER_RSA_DATA_TYPE_CONTEXT) {
            value ^= random[id];
        }

        hal_cipher_write_reg(reg, value);
        pos += 4; /* 4 groups */
        id = (td_u32)id ^ 0x01;
    }
}

static td_void hal_rsa_read_data(td_u8 *data, td_u32 data_len, td_u32 klen)
{
    td_u32 value;
    td_u8 *pos = TD_NULL;
    td_u32 i;

    pos = data;
    for (i = 0; i < klen; i += 4) { /* 4 groups */
        hal_cipher_read_reg(SEC_RSA_RRSLT_REG, &value);
        pos[0] = (td_u8)(value & 0xFF);
        pos[1] = (td_u8)((value >> 8) & 0xFF);  /* 1 index, 8  right shift */
        pos[2] = (td_u8)((value >> 16) & 0xFF); /* 2 index, 16 right shift */
        pos[3] = (td_u8)((value >> 24) & 0xFF); /* 3 index, 24 right shift */
        pos += 4; /* 4 groups */
    }
}

static td_u32 hal_rsa_get_error_code(td_void)
{
    td_u32 value;

    hal_cipher_read_reg(SEC_RSA_ERROR_REG, &value);

    return value;
}

static td_void hal_rsa_disable_int(td_void)
{
    (td_void)hal_cipher_write_reg(SEC_RSA_INT_EN, 0x00);
}

#ifdef RSA_RAND_MASK

static td_u16 g_crc_table[256]; /* 256 table size */

static td_void drv_rsa_crc16_init(td_void)
{
    td_u16 remainder;
    td_u16 n, m;
    td_u16 *table = g_crc_table;

    for (n = 0; n < 256; n++) { /* 256 */
        remainder = (td_u16)n << 8; /* 8 left shift */
        for (m = 8; m > 0; m--) { /* 8 */
            if (remainder & 0x8000)
                remainder = (remainder << 1) ^ CRC16_POLYNOMIAL;
            else
                remainder = (remainder << 1);
        }
        *(table + n) = remainder;
    }
}

static td_u16 drv_rsa_crc16_block(td_u16 crc, td_u8 block[8], td_u8 random[8]) /* 8 */
{
    td_u8 i, j;
    td_u8 val;

    for (i = 0; i < 2; i++) { /* 2 */
        for (j = 0; j < 4; j++) { /* 4 */
            val = block[i * 4 + 3 - j] ^ random[i * 4 + 3 - j]; /* 4, 3 */
            crc = (crc << 8) ^ g_crc_table[((crc >> 8) ^ val) & 0xFF]; /* 8 right shift */
        }
    }

    return crc;
}

static td_u16 drv_rsa_key_crc(const td_u8 *rsa_n, const td_u8 *rsa_k, td_u32 klen, td_u32 random[2]) /* 2 */
{
    td_u32 i;
    td_u16 crc = 0;

    for (i = 0; i < klen; i += 8) /* 8 */
        crc = drv_rsa_crc16_block(crc, rsa_n + i, (td_u8*)random);

    for (i = 0; i < klen; i += 8) /* 8 */
        crc = drv_rsa_crc16_block(crc, rsa_k + i, (td_u8*)random);

    return crc;
}
#endif

CIPHER_QUEUE_HEAD g_rsa_wait_queue;
#ifdef INT_ENABLE
static CRYPTO_IRQRETURN_T drv_rsa_isr(td_s32 irq, td_void *dev_id)
{
    td_u32 int_stat;

    int_stat = hal_rsa_get_int();

    ot_info_cipher("RSA INT: 0x%x\n", int_stat);

    if (int_stat & 0x01) {
        g_rsa_done = TD_TRUE;
        ot_info_cipher("RSA Done\n");
        cipher_queue_wait_up(&g_rsa_wait_queue);
    }

    hal_rsa_clr_int();

    return CIPHER_IRQ_HANDLED;
}
#endif

td_s32 drv_rsa_init(td_void)
{
    td_u32 rsa_stat = 0;
    td_u32 rng_stat = 0;
#ifdef INT_ENABLE
    td_s32 ret;
#endif

    cipher_mutex_init(&g_rsa_mutex_kernel);
    cipher_queue_init(&g_rsa_wait_queue);

    /* rng reset and clock */
    hal_cipher_read_reg(CIPHER_RNG_CRG_ADDR_PHY, &rng_stat);
    rng_stat |= RNG_CRG_CLOCK_BIT;
    rng_stat &= ~RNG_CRG_RESET_BIT;
    hal_cipher_write_reg(CIPHER_RNG_CRG_ADDR_PHY, rng_stat);
    cipher_udelay(10); /* 10us */

    /* rsa reset and clock */
    hal_cipher_read_reg(CIPHER_RSA_CRG_ADDR_PHY, &rsa_stat);
    rsa_stat |= RSA_CRG_CLOCK_BIT;
    rsa_stat |= RSA_CRG_RESET_BIT;
    hal_cipher_write_reg(CIPHER_RSA_CRG_ADDR_PHY, rsa_stat);
    cipher_udelay(10); /* 10us */

    /* rsa cancel reset */
    rsa_stat &= ~RSA_CRG_RESET_BIT;
    hal_cipher_write_reg(CIPHER_RSA_CRG_ADDR_PHY, rsa_stat);

    g_rsa_reg_base = cipher_ioremap_nocache(CIPHER_RSA_REG_BASE_ADDR_PHY, 0x1000);
    if (g_rsa_reg_base == TD_NULL) {
        ot_err_cipher("ioremap_nocache rsa Reg failed\n");
        return TD_FAILURE;
    }

#ifdef INT_ENABLE
    /* request irq */
    ret = cipher_request_irq(RSA_IRQ_NUMBER, drv_rsa_isr, "rsa");
    if (ret != TD_SUCCESS) {
        hal_rsa_disable_int();
        ot_err_cipher("Irq request failure, ret=%#x.\n", ret);
        return TD_FAILURE;
    }
    hal_rsa_enable_int();
#endif

#ifdef RSA_RAND_MASK
    drv_rsa_crc16_init();
#endif

    return TD_SUCCESS;
}

td_void drv_rsa_deinit(td_void)
{
    hal_rsa_disable_int();

#ifdef INT_ENABLE
    cipher_free_irq(RSA_IRQ_NUMBER, "rsa");
#endif

    if (g_rsa_reg_base != TD_NULL) {
        cipher_iounmap(g_rsa_reg_base);
        g_rsa_reg_base = TD_NULL;
    }
}

static td_s32 drv_rsa_wait_done(td_void)
{
#ifdef INT_ENABLE
    if (cipher_queue_wait_timeout(&g_rsa_wait_queue, &g_rsa_done, RSA_TIME_OUT) == 0) {
        ot_err_cipher("RSA time out! \n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
#else
    return hal_rsa_wait_free();
#endif
}

static td_s32 drv_cipher_check_rsa_data(const td_u8 *rsa_n, const td_u8 *rsa_e, const td_u8 *rsa_mc, td_u32 length)
{
    td_u32 i;

    /* formula: rsa_mc > 0 */
    for (i = 0; i < length; i++) {
        if (rsa_mc[i] > 0)
            break;
    }
    if (i >= length) {
        ot_err_cipher("RSA M/C is zero, error!\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* formula: rsa_mc < rsa_n */
    for (i = 0; i < length; i++) {
        if (rsa_mc[i] < rsa_n[i])
            break;
    }
    if (i >= length) {
        ot_err_cipher("RSA M/C is larger than rsa_n, error!\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* formula: rsa_e >= 1 */
    for (i = 0; i < length; i++) {
        if (rsa_e[i] > 0)
            break;
    }
    if (i >= length) {
        ot_err_cipher("RSA D/rsa_e is zero, error!\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_clear_rsa_ram(td_void)
{
    if (hal_rsa_wait_free() != TD_SUCCESS) {
        ot_err_cipher("RSA is busy and timeout,error!\n");
        return TD_FAILURE;
    }

    g_rsa_done = TD_FALSE;

    hal_rsa_clear_ram();
    hal_rsa_start();

    if (drv_rsa_wait_done() != TD_SUCCESS) {
        ot_err_cipher("RSA is busy and timeout,error!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_u8 g_rsa_n[CIPHER_MAX_RSA_KEY_LEN];
static td_u8 g_rsa_k[CIPHER_MAX_RSA_KEY_LEN];
static td_u8 g_rsa_m[CIPHER_MAX_RSA_KEY_LEN];

static td_void drv_rsa_rand_mask(const cipher_rsa_data_s *rsa_data,
                                 td_u32 key_len,
                                 td_u32 *random)
{
#ifdef RSA_RAND_MASK
    td_u16 crc;

    random[0] = drv_cipher_rand();
    random[1] = drv_cipher_rand();
    crc = drv_rsa_key_crc(rsa_data->rsa_n, rsa_data->rsa_k, key_len, random);
    ot_info_cipher("CRC16: 0x%x\n", crc);
    hal_rsa_set_random(random);
    hal_rsa_set_crc(crc);
#endif
}

static td_s32 drv_rsa_cipher_klad(const cipher_rsa_data_s *rsa_data,
                                  td_u32 key_len,
                                  const td_u32 *random)
{
    td_s32 ret = TD_SUCCESS;
#ifdef CIPHER_KLAD_SUPPORT
    if (rsa_data->ca_type != OT_CIPHER_KEY_SRC_USER) {
        drv_cipher_klad_load_key(0, rsa_data->ca_type,
            OT_CIPHER_KLAD_TARGET_RSA, rsa_data->rsa_k, rsa_data->rsa_k_len);
        if (ret != TD_SUCCESS) {
            ot_err_cipher("drv_cipher_klad_load_key, error!\n");
            return ret;
        }
    } else {
        hal_rsa_write_data(CIPHER_RSA_DATA_TYPE_KEY, rsa_data->rsa_k,
            rsa_data->rsa_n_len, key_len, random);
    }
#else
    hal_rsa_write_data(CIPHER_RSA_DATA_TYPE_KEY, rsa_data->rsa_k,
        rsa_data->rsa_n_len, key_len, random);
#endif
    return ret;
}

static td_s32 drv_rsa_key_info(const cipher_rsa_data_s *rsa_data,
    td_u32 *key_len, cipher_rsa_key_width_e *key_width)
{
    td_s32 ret;
    td_u8 *p = TD_NULL;

    /* Only support the key width of 1024, 2048 and 4096 */
    if (rsa_data->rsa_n_len <= 128) { /* key n size 128 */
        *key_len = 128; /* key n size 128 */
        *key_width = CIPHER_RSA_KEY_WIDTH_1K;
    } else if (rsa_data->rsa_n_len <= 256) { /* key n size 256 */
        *key_len = 256; /* key n size 256 */
        *key_width = CIPHER_RSA_KEY_WIDTH_2K;
    } else if (rsa_data->rsa_n_len <= 384) { /* key n size 384 */
        *key_len = 384; /* key n size 384 */
        *key_width = CIPHER_RSA_KEY_WIDTH_3K;
    } else if (rsa_data->rsa_n_len <= 512) { /* key n size 512 */
        *key_len = 512; /* key n size 512 */
        *key_width = CIPHER_RSA_KEY_WIDTH_4K;
    } else {
        ot_err_cipher("rsa_n_len(0x%x) is invalid\n", rsa_data->rsa_n_len);
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    /* if dataLen < key_len, padding 0 before data */
    p = g_rsa_n + (*key_len - rsa_data->rsa_n_len);
    ret = memcpy_s(p, sizeof(g_rsa_n) - (*key_len - rsa_data->rsa_n_len), rsa_data->rsa_n, rsa_data->rsa_n_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p = g_rsa_k + (*key_len - rsa_data->rsa_k_len);
    ret = memcpy_s(p, sizeof(g_rsa_k) - (*key_len - rsa_data->rsa_k_len), rsa_data->rsa_k, rsa_data->rsa_k_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    p = g_rsa_m + (*key_len - rsa_data->data_len);
    ret = memcpy_s(p, sizeof(g_rsa_m) - (*key_len - rsa_data->data_len), rsa_data->input_data, rsa_data->data_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return TD_SUCCESS;
}

static td_s32 drv_cipher_calc_rsa_ex(const cipher_rsa_data_s *rsa_data,
    td_u32 key_len, cipher_rsa_key_width_e key_width)
{
    td_u8 err_cnt;
    td_s32 ret;
    td_u32 err_code;
    td_u64 random = 0;

    ret = drv_cipher_check_rsa_data(rsa_data->rsa_n, rsa_data->rsa_k, rsa_data->input_data, key_len);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("RSA data invalid!\n");
        return ret;
    }

    g_rsa_done = TD_FALSE;

    for (err_cnt = 0; err_cnt < RSA_RETRY_CNT; err_cnt++) {
        ret = hal_rsa_wait_free();
        if (ret != TD_SUCCESS) {
            ot_err_cipher("RSA is busy!\n");
            return ret;
        }

        /* Config Mode */
        hal_rsa_config_mode(key_width);

        drv_rsa_rand_mask(rsa_data, key_len, (td_u32 *)&random);

        /* Write rsa_n, rsa_e, rsa_m */
        hal_rsa_write_data(CIPHER_RSA_DATA_TYPE_MODULE,
            rsa_data->rsa_n, rsa_data->rsa_n_len, key_len, (td_u32 *)&random);

        ret = drv_rsa_cipher_klad(rsa_data, key_len, (td_u32 *)&random);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        hal_rsa_write_data(CIPHER_RSA_DATA_TYPE_CONTEXT,
            rsa_data->input_data, rsa_data->rsa_n_len, key_len, (td_u32 *)&random);

        /* Sart */
        hal_rsa_start();

        ret = drv_rsa_wait_done();
        if (ret != TD_SUCCESS) {
            ot_err_cipher("RSA is busy and timeout,error!\n");
            return ret;
        }

        /* Get result */
        hal_rsa_read_data(rsa_data->output_data, rsa_data->rsa_n_len, key_len);

        ret = drv_cipher_clear_rsa_ram();
        if (ret != TD_SUCCESS) {
            return ret;
        }

        err_code = hal_rsa_get_error_code();
        if (err_code == 0) {
            return TD_SUCCESS;
        } else {
            continue;
        }
    }

    ot_err_cipher("RSA is err: chipset error code: 0x%x!\n", err_code);
    return TD_FAILURE;
}
#endif

static td_s32 drv_cipher_calc_rsa(cipher_rsa_data_s *rsa_data)
{
    td_s32 ret;
    td_u32 key_len = 0;
    cipher_rsa_data_s cipher_rsa_data;
    cipher_rsa_key_width_e key_width = CIPHER_RSA_KEY_WIDTH_BUTT;

    cipher_rsa_return_invalid_param(rsa_data == TD_NULL);
    cipher_rsa_return_invalid_param(rsa_data->input_data == TD_NULL);
    cipher_rsa_return_invalid_param(rsa_data->output_data == TD_NULL);
    cipher_rsa_return_invalid_param(rsa_data->rsa_n == TD_NULL);
    cipher_rsa_return_invalid_param(rsa_data->rsa_k == TD_NULL);
    cipher_rsa_return_invalid_param(rsa_data->data_len != rsa_data->rsa_n_len);
    cipher_rsa_return_invalid_param(rsa_data->rsa_k_len > rsa_data->rsa_n_len);

    (td_void)memset_s(g_rsa_n, sizeof(g_rsa_n), 0, sizeof(g_rsa_n));
    (td_void)memset_s(g_rsa_k, sizeof(g_rsa_k), 0, sizeof(g_rsa_k));
    (td_void)memset_s(g_rsa_m, sizeof(g_rsa_m), 0, sizeof(g_rsa_m));

    ret = drv_rsa_key_info(rsa_data, &key_len, &key_width);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    (td_void)memset_s(&cipher_rsa_data, sizeof(cipher_rsa_data), 0, sizeof(cipher_rsa_data_s));
    cipher_rsa_data.rsa_n = g_rsa_n;
    cipher_rsa_data.rsa_k = g_rsa_k;
    cipher_rsa_data.rsa_n_len = key_len;
    cipher_rsa_data.rsa_k_len = key_len;
    cipher_rsa_data.input_data = g_rsa_m;
    cipher_rsa_data.data_len = key_len;
    cipher_rsa_data.output_data = g_rsa_m;
    cipher_rsa_data.ca_type = rsa_data->ca_type;

    ret = drv_cipher_calc_rsa_ex(&cipher_rsa_data, key_len, key_width);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = memcpy_s(rsa_data->output_data, sizeof(g_rsa_m),
        g_rsa_m + (key_len - rsa_data->rsa_n_len), rsa_data->rsa_n_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    return ret;
}

td_s32 ot_drv_cipher_calc_rsa(cipher_rsa_data_s *rsa_data)
{
    td_s32 ret;

    if (rsa_data == TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (cipher_mutex_lock(&g_rsa_mutex_kernel)) {
        ot_err_cipher("down_interruptible failed!\n");
        return TD_FAILURE;
    }

    ret = drv_cipher_calc_rsa(rsa_data);

    cipher_mutex_unlock(&g_rsa_mutex_kernel);

    return ret;
}

