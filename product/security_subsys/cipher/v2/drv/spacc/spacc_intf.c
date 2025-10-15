// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "spacc_intf.h"
#include <cpu_func.h>
#include "cipher_adapt.h"
#include "spacc_body.h"
#include "spacc_union_define.h"
#include "drv_klad.h"
#include "cipher_osal.h"
#include "cipher_ext.h"

#define SPACC_MAX_CHN                   8
#define AES_BLOCK_SIZE                  16
#define SPACC_TIME_OUT                  100000
#define SPACC_PAD_BUF_SIZE              128
#define HASH_RESULT_MAX_LEN             64

#define CHN_0_CIPHER_IV                 (g_cipher_reg_base + 0x0000)
#define CHN_0_CIPHER_DOUT               (g_cipher_reg_base + 0x0080)
#define CHN_0_CIPHER_KEY                (g_cipher_reg_base + 0x0100)
#define CHN_0_SM1_SK                    (g_cipher_reg_base + 0x0200)
#define ODD_EVEN_KEY_SEL                (g_cipher_reg_base + 0x0290)
#define HDCP_MODE_CTRL                  (g_cipher_reg_base + 0x0300)
#define SEC_CHN_CFG                     (g_cipher_reg_base + 0x0304)
#define CALC_ST0                        (g_cipher_reg_base + 0x0318)
#define CALC_ERR                        (g_cipher_reg_base + 0x0320)
#define CHN_0_CCM_GCM_TAG               (g_cipher_reg_base + 0x0380)
#define CHN_0_CIPHER_CTRL               (g_cipher_reg_base + 0x0400)
#define CIPHER_INT_RAW                  (g_cipher_reg_base + 0x040c)
#define CHN_0_CIPHER_DIN                (g_cipher_reg_base + 0x0420)

#define SYMC_INT_LEVEL                  100 /* (SPACC_MAX_DEPTH / 2) */

typedef td_void (*func_cipher_callback)(td_u32);

typedef struct {
    cipher_mmz_buf_t mmz_buf;
} spacc_env_s;

typedef struct {
    td_u8 *src_vir;
    td_u8 *dest_vir;
    td_u8 *aad_vir;

    cipher_mmz_buf_t src_mmz_buf;
    cipher_mmz_buf_t dest_mmz_buf;
    cipher_mmz_buf_t aad_mmz_buf;
} spacc_mmz_s;

typedef struct {
    td_bool is_open;
    td_u32 hard_num;
    td_u32 block_size;

    td_bool symc_done;
    CIPHER_QUEUE_HEAD queue;

    ot_cipher_data *node_list;
    td_u32 node_num;
    td_u32 node_cur;
    td_u32 total_len;

    td_u8 *pad_vir_addr;
    td_size_t pad_phy_addr;

    td_u32 data_size;
    const td_void* which_file;

    cipher_config_ctrl_ex_s ctrl_ex;

    func_cipher_callback callback;
} spacc_symc_chn_s;

typedef struct {
    td_bool is_open;
    td_u32 hard_num;
    td_u32 node_num;
    td_u32 node_cur;
    td_u32 block_size;

    td_bool digest_done;
    CIPHER_QUEUE_HEAD queue;

    func_cipher_callback callback;

    td_u32 data_size;
    const td_void* which_file;
} spacc_digest_chn_s;

typedef struct {
    td_u32 src_phys_addr;
    td_u32 dst_phys_addr;
    td_u32 byte_length;
    td_bool is_use_odd_key;
} cipher_data_compat_s;

CIPHER_MUTEX                g_symc_mutex;
CIPHER_MUTEX                g_digest_mutex;
static spacc_env_s          g_spacc_env;
static spacc_symc_chn_s     g_symc_chn[SPACC_MAX_CHN];
static spacc_digest_chn_s   g_digest_chn[SPACC_MAX_CHN];
static td_void*             g_cipher_reg_base;

static td_s32 spacc_check_handle(td_handle ci_handle)
{
    if ((td_handle_get_modid(ci_handle) != OT_ID_CIPHER) || \
        (td_handle_get_private_data(ci_handle) != 0)) {
        ot_err_cipher("invalid cipher handle 0x%x\n", ci_handle);
        cipher_mutex_unlock(&g_symc_mutex);
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }
    if (td_handle_get_chnid(ci_handle) >= SPACC_MAX_CHN) {
        ot_err_cipher("chan %d is too large, max: %d\n", \
            td_handle_get_chnid(ci_handle), SPACC_MAX_CHN);
        cipher_mutex_unlock(&g_symc_mutex);
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }
    if (g_symc_chn[td_handle_get_chnid(ci_handle)].is_open == TD_FALSE) {
        ot_err_cipher("chan %d is not open\n", td_handle_get_chnid(ci_handle));
        cipher_mutex_unlock(&g_symc_mutex);
        return OT_ERR_CIPHER_INVALID_HANDLE;
    }
    return TD_SUCCESS;
}

#ifdef INT_ENABLE
CRYPTO_IRQRETURN_T drv_cipher_isr(td_s32 irq, td_void *dev_id)
{
    td_u32 chn_mask, i;

    chn_mask = spacc_symc_done_notify();
    ot_info_cipher("SPACC ISR IRQ: %d, chn_mask 0x%x\n", irq, chn_mask);

    for (i = CIPHER_PKG_N_CHN_MIN; i <= CIPHER_PKG_N_CHN_MAX; i++) {
        if ((chn_mask >> i) & 0x01) {
            if (g_symc_chn[i].callback) {
                g_symc_chn[i].callback(i);
            } else {
                g_symc_chn[i].symc_done = TD_TRUE;
                ot_info_cipher("chn %d wake up\n", i);
                cipher_queue_wait_up(&g_symc_chn[i].queue);
            }
        }
    }

    chn_mask = spacc_digest_done_notify();
    for (i = CIPHER_PKG_N_CHN_MIN; i <= CIPHER_PKG_N_CHN_MAX; i++) {
        if ((chn_mask >> i) & 0x01) {
            if (g_digest_chn[i].callback) {
                g_digest_chn[i].callback(i);
            } else {
                g_digest_chn[i].digest_done = TD_TRUE;
                cipher_queue_wait_up(&g_digest_chn[i].queue);
            }
        }
    }

    return CIPHER_IRQ_HANDLED;
}
#endif

static td_s32 drv_cipher_reset(td_void)
{
    td_u32 *pvirt = TD_NULL;
    td_u32 spacc_stat = 0;

    pvirt = cipher_ioremap_nocache(CIPHER_SPACC_CRG_ADDR_PHY, 16); /* 16 */
    if (pvirt == TD_NULL) {
        ot_err_cipher("ioremap_nocache phy addr err:%x.\n", CIPHER_SPACC_CRG_ADDR_PHY);
        return TD_FAILURE;
    }

    /* open clock, reset */
    hal_cipher_read_reg(CIPHER_SPACC_CRG_ADDR_PHY, &spacc_stat);
    spacc_stat |= SPACC_CRG_CLOCK_BIT;
    spacc_stat |= SPACC_CRG_RESET_BIT;
    hal_cipher_write_reg(CIPHER_SPACC_CRG_ADDR_PHY, spacc_stat);
    cipher_udelay(10); /* 10us */

    /* cancel reset */
    spacc_stat &= ~SPACC_CRG_RESET_BIT;
    hal_cipher_write_reg(CIPHER_SPACC_CRG_ADDR_PHY, spacc_stat);

    cipher_iounmap(pvirt);
    pvirt = TD_NULL;

    return TD_SUCCESS;
}

td_s32 drv_cipher_init(td_void)
{
    td_s32 ret;
    td_size_t size_addr = 0;
    td_u32 i;
    u_sec_chn_cfg sec_cfg;

    cipher_mutex_init(&g_symc_mutex);

    g_cipher_reg_base = cipher_ioremap_nocache(CIPHER_CIPHER_REG_BASE_ADDR_PHY, 0x2000);
    chk_func_fail_return(g_cipher_reg_base == TD_NULL, TD_FAILURE, cipher_ioremap_nocache);

    ret = drv_cipher_reset();
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_reset);

    (td_void)memset_s(&g_spacc_env, sizeof(spacc_env_s), 0, sizeof(spacc_env_s));
    (td_void)memset_s(&g_symc_chn, sizeof(g_symc_chn), 0, sizeof(g_symc_chn));
    (td_void)memset_s(&g_digest_chn, sizeof(g_digest_chn), 0, sizeof(g_digest_chn));

    g_spacc_env.mmz_buf.mmz_size = spacc_get_node_list_size() + SPACC_PAGE_SIZE;
    ret = cipher_mmz_alloc_remap("CIPHER_ChnBuf", &g_spacc_env.mmz_buf);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_mmz_alloc_remap);

    for (i = CIPHER_PKG_N_CHN_MIN; i <= CIPHER_PKG_N_CHN_MAX; i++) {
        cipher_queue_init(&g_symc_chn[i].queue);
        cipher_queue_init(&g_digest_chn[i].queue);
        g_symc_chn[i].pad_phy_addr = g_spacc_env.mmz_buf.start_phy_addr + SPACC_PAD_BUF_SIZE * i;
        g_symc_chn[i].pad_vir_addr = g_spacc_env.mmz_buf.start_vir_addr + SPACC_PAD_BUF_SIZE * i;
        g_symc_chn[i].hard_num = i;
        g_digest_chn[i].hard_num = i;
    }

    ret = spacc_init(g_cipher_reg_base, size_addr,
                     g_spacc_env.mmz_buf.start_phy_addr + SPACC_PAGE_SIZE,
                     g_spacc_env.mmz_buf.start_vir_addr + SPACC_PAGE_SIZE);
    if (ret != TD_SUCCESS)
        goto unmap_mmz;

    hal_cipher_read_reg(SEC_CHN_CFG, &sec_cfg.u32);
    sec_cfg.bits.cipher_sec_chn_cfg |= 0x01;
    sec_cfg.bits.hash_sec_chn_cfg |= 0x01;
    hal_cipher_write_reg(SEC_CHN_CFG, sec_cfg.u32);

#ifdef INT_ENABLE
    ret = cipher_request_irq(CIPHER_IRQ_NUMBER, drv_cipher_isr, "cipher");
    if (ret != TD_SUCCESS) {
        ot_err_cipher("Irq request failure, ret=%d, irq = %d", ret, CIPHER_IRQ_NUMBER);
        goto unmap_mmz;
    }
#endif
    return ret;

unmap_mmz:
    cipher_mmz_release_unmap(&g_spacc_env.mmz_buf);
    return ret;
}

td_void drv_cipher_deinit(td_void)
{
    if (spacc_deinit() != TD_SUCCESS)
        ot_err_cipher("spacc deinit failed.\n");

#ifdef INT_ENABLE
    cipher_free_irq(CIPHER_IRQ_NUMBER, "cipher");
#endif

    cipher_mmz_release_unmap(&g_spacc_env.mmz_buf);

    if (g_cipher_reg_base != TD_NULL) {
        cipher_iounmap(g_cipher_reg_base);
        g_cipher_reg_base = TD_NULL;
    }

    return;
}

td_s32 ot_drv_cipher_create_handle(cipher_handle_s *ci_handle, const td_void *file)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;

    if (ci_handle == TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return TD_FAILURE;
    }

    if (ci_handle->cipher_atts.cipher_type > OT_CIPHER_TYPE_COPY_AVOID) {
        ot_err_cipher("Invalid cipher type!\n");
        return TD_FAILURE;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    if (ci_handle->cipher_atts.cipher_type == OT_CIPHER_TYPE_COPY_AVOID) {
        if (g_symc_chn[0].is_open == TD_FALSE)
            i = 0;
        else
            i = SPACC_MAX_CHN;
    } else {
        for (i = CIPHER_PKG_N_CHN_MIN; i <= CIPHER_PKG_N_CHN_MAX; i++) {
            if (g_symc_chn[i].is_open == TD_FALSE)
                break;
        }
    }

    if (i <= CIPHER_PKG_N_CHN_MAX) {
        g_symc_chn[i].is_open = TD_TRUE;
        g_symc_chn[i].which_file = file;
        g_symc_chn[i].callback = TD_NULL;
        g_symc_chn[i].node_list = TD_NULL;
        g_symc_chn[i].node_num = 0;
        ci_handle->ci_handle = td_handle_init(OT_ID_CIPHER, 0, i);
    } else {
        ot_err_cipher("No more cipher chan left.\n");
        ret = TD_FAILURE;
    }

    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

static td_s32 drv_cipher_param_check(symc_alg_en symc_alg,
                                     symc_mode_en symc_mode,
                                     symc_dat_width_en symc_width,
                                     ot_cipher_sm1_round sm1_round)
{
   /* the mode depend on alg, which limit to hardware
    * des/3des support ecb/cbc/cfb/ofb
    * aes support ecb/cbc/cfb/ofb/ctr/ccm/gcm
    * sm1 support ecb/cbc/cfb/ofb
    * sm4 support ecb/cbc/ctr
    */
    if ((symc_alg == SYMC_ALG_DES) || (symc_alg == SYMC_ALG_3DES) || (symc_alg == SYMC_ALG_SM1)) {
        if ((symc_mode != SYMC_MODE_ECB) && (symc_mode != SYMC_MODE_CBC) &&
            (symc_mode != SYMC_MODE_CFB) && (symc_mode != SYMC_MODE_OFB)) {
            ot_err_cipher("Invalid alg %d and mode: %d\n", symc_alg, symc_mode);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else if (symc_alg == SYMC_ALG_SM4) {
        if ((symc_mode != SYMC_MODE_ECB) && (symc_mode != SYMC_MODE_CBC) && (symc_mode != SYMC_MODE_CTR)) {
            ot_err_cipher("Invalid alg %d and mode %d\n", symc_alg, symc_mode);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

   /* the bit width depend on alg and mode, which limit to hardware
    * des/3des with cfb/ofb support bit1, bit8, bit 64.
    * aes with cfb/ofb only support bit128.
    * sm1 with ofb only support bit128, cfb support bit1, bit8, bit 64.
    */
    if ((symc_alg == SYMC_ALG_DES) || (symc_alg == SYMC_ALG_3DES)) {
        if ((symc_mode == SYMC_MODE_CFB) || (symc_mode == SYMC_MODE_OFB)) {
            if (symc_width != SYMC_DAT_WIDTH_64 &&
                symc_width != SYMC_DAT_WIDTH_8 && symc_width != SYMC_DAT_WIDTH_1) {
                ot_err_cipher("Invalid mode %d and bit width %d\n", symc_mode, symc_width);
                return OT_ERR_CIPHER_INVALID_PARAM;
            }
        }
    }

    if (symc_alg == SYMC_ALG_AES) {
        if (((symc_mode == SYMC_MODE_CFB) && (symc_width >= SYMC_DAT_WIDTH_COUNT)) ||
            ((symc_mode == SYMC_MODE_OFB) && (symc_width != SYMC_DAT_WIDTH_128))) {
            ot_err_cipher("Invalid alg %d mode %d and width %d\n", symc_alg, symc_mode, symc_width);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

    if (symc_alg == SYMC_ALG_SM1) {
        if (((symc_mode == SYMC_MODE_OFB) && (symc_width != SYMC_DAT_WIDTH_128)) ||
            ((symc_mode == SYMC_MODE_CFB) && (symc_width >= SYMC_DAT_WIDTH_COUNT))) {
            ot_err_cipher("Invalid alg %d mode %d and width %d\n", symc_alg, symc_mode, symc_width);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }

        if (sm1_round >= OT_CIPHER_SM1_ROUND_BUTT) {
            ot_err_cipher("Invalid alg %d and Sm1Round %d\n", symc_alg, sm1_round);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_match_alg(const cipher_config_ctrl_ex_s *config, symc_alg_en *symc_alg, td_u32 *block_size)
{
    /* set alg and block size */
    switch (config->ci_alg) {
        case OT_CIPHER_ALG_AES:
            *symc_alg = SYMC_ALG_AES;
            *block_size = 16; /* 16 block size */
            break;
        case OT_CIPHER_ALG_DMA:
            *symc_alg = SYMC_ALG_NULL_CIPHER;
            *block_size = 16; /* 16 block size */
            break;
        case OT_CIPHER_ALG_SM1:
            *symc_alg = SYMC_ALG_SM1;
            *block_size = 16; /* 16 block size */
            break;
        case OT_CIPHER_ALG_SM4:
            *symc_alg = SYMC_ALG_SM4;
            *block_size = 16; /* 16 block size */
            break;
        default:
            ot_err_cipher("Invalid alg: 0x%x\n", config->ci_alg);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_match_work_mode(const cipher_config_ctrl_ex_s *config, symc_mode_en *symc_mode)
{
    switch (config->work_mode) {
        case OT_CIPHER_WORK_MODE_ECB:
            *symc_mode = SYMC_MODE_ECB;
            break;
        case OT_CIPHER_WORK_MODE_CBC:
            *symc_mode = SYMC_MODE_CBC;
            break;
        case OT_CIPHER_WORK_MODE_CFB:
            *symc_mode = SYMC_MODE_CFB;
            break;
        case OT_CIPHER_WORK_MODE_OFB:
            *symc_mode = SYMC_MODE_OFB;
            break;
        case OT_CIPHER_WORK_MODE_CTR:
            *symc_mode = SYMC_MODE_CTR;
            break;
        case OT_CIPHER_WORK_MODE_CCM:
            *symc_mode = SYMC_MODE_CCM;
            break;
        case OT_CIPHER_WORK_MODE_GCM:
            *symc_mode = SYMC_MODE_GCM;
            break;
        default:
            ot_err_cipher("Invalid mode: 0x%x\n", config->work_mode);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_match_bit_width(const cipher_config_ctrl_ex_s *config, symc_dat_width_en *symc_width)
{
    /* set the bit width which depend on alg and mode */
    if ((config->work_mode == OT_CIPHER_WORK_MODE_CFB) ||
        (config->work_mode == OT_CIPHER_WORK_MODE_OFB)) {
        switch (config->bit_width) {
            case OT_CIPHER_BIT_WIDTH_64BIT:
                *symc_width = SYMC_DAT_WIDTH_64;
                break;
            case OT_CIPHER_BIT_WIDTH_8BIT:
                *symc_width = SYMC_DAT_WIDTH_8;
                break;
            case OT_CIPHER_BIT_WIDTH_1BIT:
                *symc_width = SYMC_DAT_WIDTH_1;
                break;
            case OT_CIPHER_BIT_WIDTH_128BIT:
                *symc_width = SYMC_DAT_WIDTH_128;
                break;
            default:
                ot_err_cipher("Invalid width: 0x%x, mode 0x%x, alg 0x%x\n",
                    config->bit_width, config->work_mode, config->ci_alg);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        *symc_width = SYMC_DAT_WIDTH_128;
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_match_key_len(const cipher_config_ctrl_ex_s *config, td_u32 *key_len)
{
    if (config->ci_alg == OT_CIPHER_ALG_AES) {
        switch (config->key_len) {
            case OT_CIPHER_KEY_AES_128BIT:
                *key_len = 16; /* 16 key len */
                break;
            case OT_CIPHER_KEY_AES_192BIT:
                *key_len = 24; /* 24 key len */
                break;
            case OT_CIPHER_KEY_AES_256BIT:
                *key_len = 32; /* 32 key len */
                break;
            default:
                ot_err_cipher("Invalid key len: 0x%x\n", config->key_len);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else if (config->ci_alg == OT_CIPHER_ALG_SM1) {
        *key_len = 48; /* 48 key len */
    } else if (config->ci_alg == OT_CIPHER_ALG_SM4) {
        *key_len = 16; /* 16 key len */
    } else if (config->ci_alg != OT_CIPHER_ALG_DMA) {
        ot_err_cipher("Invalid cipher alg: %d\n", config->ci_alg);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    return TD_SUCCESS;
}

/* change the unf params to drive params */
static td_s32 drv_cipher_param(const cipher_config_ctrl_ex_s *config,
    spacc_symc_config_s *symc_cfg, td_u32 *block_size)
{
    td_s32 ret;

    /* set alg and block size */
    ret = drv_cipher_match_alg(config, &symc_cfg->symc_alg, block_size);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_match_alg);

    /* set the mode which depend on alg */
    ret = drv_cipher_match_work_mode(config, &symc_cfg->symc_mode);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_match_work_mode);

    /* set the bit width which depend on alg and mode */
    ret = drv_cipher_match_bit_width(config, &symc_cfg->symc_width);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_match_bit_width);

   /* set the key length depend on alg
    * des/3des support 2key and 3key
    * aes support 128, 192, and 256
    * sm1 support ak/ek/sk
    * sm4 support 128
    */
    ret = drv_cipher_match_key_len(config, &symc_cfg->key_len);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_match_key_len);

    if (config->change_flags.bits_iv > OT_CIPHER_IV_CHG_ALL_PKG) {
        ot_err_cipher("Invalid IV Change Flags: 0x%x\n", config->change_flags.bits_iv);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if ((config->change_flags.bits_iv == OT_CIPHER_IV_CHG_ALL_PKG) &&
        ((config->work_mode == OT_CIPHER_WORK_MODE_CCM) ||
        (config->work_mode == OT_CIPHER_WORK_MODE_GCM))) {
        ot_err_cipher("Invalid IV Change Flags: 0x%x\n", config->change_flags.bits_iv);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (config->key_by_ca && (config->ca_type >= OT_CIPHER_KEY_SRC_BUTT)) {
        ot_err_cipher("Invalid CA Type: 0x%x\n", config->ca_type);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return drv_cipher_param_check(symc_cfg->symc_alg,
        symc_cfg->symc_mode, symc_cfg->symc_width, config->sm1_round);
}

static td_s32 drv_cipher_config_ccm(const cipher_config_ctrl_ex_s *config,
    td_u32 *iv, td_u32 ilen, td_u32 *real_ilen)
{
    td_u8 *buf = TD_NULL;

    /* The octet lengths of N are denoted  n,
     * The octet length of the binary represen tation of the
     * octet length of the payload denoted q,
     * n is an element of {7, 8, 9, 10, 11, 12, 13},
     * equation: n + q = 15
     * here the string of N  is config->iv, and n is config->iv_len.
     */
    if ((config->iv_len < 7) || (config->iv_len > 13)) { /* 7, 13 iv len range */
        ot_err_cipher("Invalid IV LEN: 0x%x\n", config->iv_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* the parameter t denotes the octet length of T(tag)
     * t is an element of  { 4, 6, 8, 10, 12, 14, 16}
     * here t is config->tag_len
     */
    if ((config->tag_len & 0x01) || (config->tag_len < 4) || (config->tag_len > 16)) { /* 4, 16 tag len value */
        ot_err_cipher("Invalid TAG LEN: 0x%x\n", config->tag_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    /* Formatting of the Counter Blocks(IV for CTR)
     *
     * According to the CCM spec, the counter is equivalent to
     * a formatting of the counter index i into a complete data block.
     * The counter blocks Ctri are formatted as shown below:
     * | Octet number:  0    1 ... 15-q    16-q ... 15
     * | Contents:     Flags     N             [i]
     * Within each block Ctri, the N is get from config->iv, n + q = 15,
     * so the q equal to 15 - config->iv_len.
     * the [i] is the block conut start with 0,
     * In the Flags field, Bits 0, 1, and 2 contain the encoding of q - 1,
     * others bits shall be set to 0.
     * so the first byte of IV shall be q -1, that is 15 - config->iv_len - 1
     */
    buf = (td_u8 *)iv;
    (td_void)memset_s(buf, ilen, 0, ilen);
    buf[0] = 14 - config->iv_len; /* equation: IV[0] = q - 1 = 15 - n - 1, 14 */
    if (memcpy_s(buf + 1, ilen - 1, config->iv, config->iv_len) != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }
    *real_ilen = config->iv_len + 1;

    return TD_SUCCESS;
}

static td_s32 drv_cipher_config_gcm(const cipher_config_ctrl_ex_s *config,
    td_u32 *iv, td_u32 ilen, td_u32 *real_ilen)
{
    /* According to the GCM spec, the IVLen >= 1, typical equal to 12,
     * but limit to hard logic devising, the IVLen can't large than 16.
     */
    if (config->iv_len > ilen) {
        ot_err_cipher("Invalid IV LEN: 0x%x\n", config->iv_len);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    if (memcpy_s(iv, ilen, config->iv, config->iv_len) != EOK) {
        ot_err_cipher("call failed memcpy_s\n");
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }
    *real_ilen = config->iv_len;
    return TD_SUCCESS;
}

static td_s32 drv_cipher_config_chn_0(const spacc_symc_chn_s *channel, const cipher_config_ctrl_ex_s *config,
    const spacc_symc_config_s *symc_cfg, const td_u32 *iv, td_u32 iv_len)
{
    u_chan0_cipher_ctrl chn0_ctrl;
    td_u32 klen = 0;
    td_u32 i;
    td_s32 ret = TD_SUCCESS;

    hal_cipher_write_reg(ODD_EVEN_KEY_SEL, 0x00);
    if (config->key_by_ca == TD_FALSE) {
        for (i = 0; i < 8; i++) /* 8 */
            hal_cipher_write_reg(CHN_0_CIPHER_KEY + i * 4, config->key[i]); /* 4 */
    } else {
        ret = drv_cipher_klad_load_key(0, channel->ctrl_ex.ca_type,
            OT_CIPHER_KLAD_TARGET_AES, (td_u8 *)config->key, symc_cfg->key_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_klad_load_key);
    }

    if (symc_cfg->symc_alg == SYMC_ALG_SM1) {
        for (i = 0; i < 4; i++) /* 4 */
            hal_cipher_write_reg(CHN_0_SM1_SK + i * 4, config->key[i + 8]); /* 4, 8 */
    }
    for (i = 0; i < 4; i++) /* 4 */
        hal_cipher_write_reg(CHN_0_CIPHER_IV + i * 4, iv[i]); /* 4 */

    if (symc_cfg->symc_alg == SYMC_ALG_AES)
        klen = symc_cfg->key_len / 8 - 2; /* 8, 2 */
    else if (symc_cfg->symc_alg == SYMC_ALG_3DES)
        klen = (symc_cfg->key_len == 16 ? 3 : 2); /* 16, 3, 2 */

    chn0_ctrl.u32 = 0x00;
    chn0_ctrl.bits.sym_ch0_sm1_round_num = config->sm1_round;
    chn0_ctrl.bits.sym_ch0_ivin_sel = 0x01;
    chn0_ctrl.bits.sym_ch0_key_sel = config->key_by_ca;
    chn0_ctrl.bits.sym_ch0_key_length = klen;
    chn0_ctrl.bits.sym_ch0_dat_width = symc_cfg->symc_width;
    chn0_ctrl.bits.sym_ch0_alg_sel = symc_cfg->symc_alg;
    chn0_ctrl.bits.sym_ch0_alg_mode = symc_cfg->symc_mode;
    if (config->work_mode == OT_CIPHER_WORK_MODE_GCM)
        chn0_ctrl.bits.sym_ch0_gcm_iv_len = config->iv_len - 1;
    hal_cipher_write_reg(CHN_0_CIPHER_CTRL, chn0_ctrl.u32);

    return ret;
}

static td_s32 drv_cipher_config_chn_n(const spacc_symc_chn_s *channel, const cipher_config_ctrl_ex_s *config,
    const spacc_symc_config_s *symc_cfg, const td_u32 *iv, td_u32 iv_len)
{
    td_s32 ret;

    ret = spacc_symc_config(channel->hard_num, symc_cfg, config->sm1_round, config->key_by_ca);
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_config);

    if (config->key_by_ca == TD_FALSE) { /* OT_CIPHER_KEY_SRC_USER, CPU config key */
        ret = spacc_symc_setkey(channel->hard_num, config->key, config->odd_key, symc_cfg->key_len);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_setkey);
#ifdef CIPHER_KLAD_SUPPORT
    } else { /* OT_CIPHER_KEY_SRC_KLAD, KLAD load key */
        ret = drv_cipher_klad_load_key(channel->hard_num, channel->ctrl_ex.ca_type,
            OT_CIPHER_KLAD_TARGET_AES, (td_u8 *)config->key, symc_cfg->key_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_klad_load_key);
#endif
    }

    ret = spacc_symc_setiv(channel->hard_num, (td_u8 *)iv, iv_len);
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_setiv);
    return ret;
}

static td_s32 drv_cipher_config_chn(td_u32 soft_chn_id, const cipher_config_ctrl_ex_s *config)
{
    td_s32 ret;
    td_u32 iv[4]; /* 4 iv arr size */
    td_u32 real_ilen = 0;
    spacc_symc_config_s symc_cfg;
    spacc_symc_chn_s *channel = &g_symc_chn[soft_chn_id];

    (td_void)memset_s(iv, sizeof(iv), 0, sizeof(iv));
    (td_void)memset_s(&symc_cfg, sizeof(symc_cfg), 0, sizeof(symc_cfg));
    (td_void)memcpy_s(&channel->ctrl_ex, sizeof(cipher_config_ctrl_ex_s), config, sizeof(cipher_config_ctrl_ex_s));

    ret = drv_cipher_param(config, &symc_cfg, &channel->block_size);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_param);

    if (config->work_mode == OT_CIPHER_WORK_MODE_CCM) {
        ret = drv_cipher_config_ccm(config, iv, sizeof(iv), &real_ilen);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_config_ccm);
    } else if (config->work_mode == OT_CIPHER_WORK_MODE_GCM) {
        ret = drv_cipher_config_gcm(config, iv, sizeof(iv), &real_ilen);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_config_gcm);
    } else if (config->work_mode <= OT_CIPHER_WORK_MODE_CTR) {
        /* IV length, 16 for aes, 8 for des/3des */
        ret = memcpy_s(iv, sizeof(iv), config->iv, sizeof(config->iv));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        real_ilen = sizeof(config->iv);
    } else {
        ot_err_cipher("Invalid mode: 0x%x\n", config->work_mode);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* chn 1~7, data transferred by DMA */
    if (soft_chn_id != 0) {
        ret = drv_cipher_config_chn_n(channel, config, &symc_cfg, iv, real_ilen);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_config_chn_n);
    } else {     /* chn 0, data transferred by CPU, spacc_body do not drvie chn0 */
        ret = drv_cipher_config_chn_0(channel, config, &symc_cfg, iv, sizeof(iv));
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_config_chn_0);
    }

    return TD_SUCCESS;
}

td_s32 ot_drv_cipher_config_chn_ex(td_handle ci_handle, const cipher_config_ctrl_ex_s *config)
{
    td_s32 ret;
    td_u32 soft_chn_id;

    if (config == TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return TD_FAILURE;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(ci_handle);
    if (ret != TD_SUCCESS)
        return ret;

    soft_chn_id = td_handle_get_chnid(ci_handle);
    ret = drv_cipher_config_chn(soft_chn_id, config);

    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

td_s32 ot_drv_cipher_destory_handle(td_handle cipher_chn)
{
    td_u32 soft_chn_id;
    td_s32 ret;

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(cipher_chn);
    if (ret != TD_SUCCESS)
        return ret;
    soft_chn_id = td_handle_get_chnid(cipher_chn);

    g_symc_chn[soft_chn_id].is_open = TD_FALSE;

    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

/* check error code
 * bit0: klad_key_use_err
 * bit1: alg_len_err
 * bit2: smmu_page_unvlid
 */
static td_s32 drv_cipher_check_error_code(td_u32 hard_num, td_u32 wait, td_u32 src_addr)
{
    td_s32 ret = TD_SUCCESS;

    if (wait & 0x01) {
        ot_err_cipher("hash error: klad_key_use_err, chn %d !!!\n", hard_num);
        ret = TD_FAILURE;
    }
    if (wait & 0x02) {
        ot_err_cipher("hash error: alg_len_err, chn %d !!!\n", hard_num);
        ret = TD_FAILURE;
    }
    if (wait & 0x04) {
        ot_err_cipher("hash error: smmu_page_unvlid, chn %d !!!\n", hard_num);
        ot_err_cipher("SRC ADDR: 0x%x\n", src_addr);
        ret = TD_FAILURE;
    }
    return ret;
}

static td_s32 drv_cipher_symc_wait_done(const spacc_symc_chn_s *channel, td_u32 time_out)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 wait;
    td_u32 src_addr, dst_addr;

#ifdef INT_ENABLE
    if (cipher_queue_wait_timeout(&channel->queue, &channel->symc_done, time_out) != TD_SUCCESS) {
        ot_err_cipher("Encrypt time out! Chn %d, CIPHER_IRQ_NUMBER: %d\n", channel->hard_num, CIPHER_IRQ_NUMBER);
        ret = TD_FAILURE;
    }
#else
    for (; time_out > 0; time_out--) {
        if (spacc_symc_done_try(channel->hard_num)) {
            break;
        }
        cipher_udelay(10); /* 10us */
    }
    if (time_out == 0) {
        ot_err_cipher("symc time out!\n");
        ret = TD_FAILURE;
    }
#endif

    wait = spacc_symc_get_err_code(channel->hard_num, &src_addr, &dst_addr);
    if (drv_cipher_check_error_code(channel->hard_num, wait, src_addr) != TD_SUCCESS)
        ret = TD_FAILURE;

    return ret;
}

static td_s32 drv_cipher_digest_wait_done(const spacc_digest_chn_s *channel)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 wait;
    td_u32 time_out = SPACC_TIME_OUT;
    td_u32 src_addr;

#ifdef INT_ENABLE
    if (channel->data_size > 100 * 1024) { /* 100, 1024 */
        ret = cipher_queue_wait_timeout(&channel->queue, &channel->digest_done, time_out);
        if (ret <= 0) {
            ot_err_cipher("hash time out! CIPHER_IRQ_NUMBER: %d\n", CIPHER_IRQ_NUMBER);
            ret = TD_FAILURE;
        }
    } else {
        time_out = 0;
        while (time_out++ < SPACC_TIME_OUT) {
            if (channel->digest_done != TD_FALSE)
                break;
            cipher_udelay(10); /* 10us */
        }
        if (time_out >= SPACC_TIME_OUT) {
            ot_err_cipher("hash time out!\n");
            ret = TD_FAILURE;
        }
    }
#else
    time_out = 0;
    while (time_out++ < SPACC_TIME_OUT) {
        if (spacc_digest_done_try(channel->hard_num))
            break;
        cipher_udelay(10); /* 10us */
    }
    if (time_out >= SPACC_TIME_OUT) {
        ot_err_cipher("hash time out!\n");
        ret = TD_FAILURE;
    }
#endif

   /* check error code
    * bit0: klad_key_use_err
    * bit1: alg_len_err
    * bit2: smmu_page_unvlid
    */
    wait = spacc_digest_get_err_code(channel->hard_num, &src_addr);
    if (drv_cipher_check_error_code(channel->hard_num, wait, src_addr) != TD_SUCCESS)
        ret = TD_FAILURE;

    return ret;
}

static td_s32 drv_cipher_ccm_head_format(const spacc_symc_chn_s *channel,
    td_u32 enc_len, td_u8 *arr_b, td_u32 blen, td_u32 *b1_len)
{
    td_s32 ret;
    td_u8 *pbuf = TD_NULL;
    td_u32 index = 0;

    /* Format B0 */
    /* The leading octet of the first block of the formatting, B0,
     * contains four flags for control information: two single bits,
     * called  Reserved  and  Adata, and two strings of three bits,
     * to encode the values t and q.  The encoding of t is [(t -2)/2],
     * and the encoding of q is [ q-1].
     * The ordering of the flags with in the octet is given:
     *  _____________________________________________________
     * |Bit number  7     |   6   | 5  4  3     |  2  1  0   |
     * |Contents  Reserved  Adata   [( t -2)/2] |  [q-1]     |
     *  -----------------------------------------------------
     * The remaining 15 octets of the first block of  the formatting are
     * devoted to the nonce and the binary representation of
     * the message length in  q octets, as given:
     *  _____________________________________________
     * |Octet number  0   | 1 ... 15-q | 16-q ... 15 |
     * |Contents    Flags |      N     |      Q      |
     *  ---------------------------------------------
    */
    pbuf = arr_b;
    ret = memset_s(pbuf, blen, 0, AES_BLOCK_SIZE);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    pbuf[index]  = (channel->ctrl_ex.alen > 0 ? 1 : 0) << 6; /* Adata, 6 left shift */
    pbuf[index] |= ((channel->ctrl_ex.tag_len - 2) / 2) << 3; /* formula: (t - 2) / 2, 3 left shift */
    pbuf[index] |= ((15 - channel->ctrl_ex.iv_len) - 1); /* formula: q - 1, n + q = 15 */
    index++;
    ret = memcpy_s(&pbuf[index], blen - index, channel->ctrl_ex.iv, channel->ctrl_ex.iv_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    index += channel->ctrl_ex.iv_len;
    if (index <= 12) { /* 12 index */
        index = 12; /* 12 index */
        pbuf[index++] = (td_u8)(enc_len >> 24); /* 24 right shift */
        pbuf[index++] = (td_u8)(enc_len >> 16); /* 16 right shift */
        pbuf[index++] = (td_u8)(enc_len >> 8);  /* 8  right shift */
        pbuf[index++] = (td_u8)(enc_len);
    } else if ((index == 13) && (enc_len <= 0xFFFFFF)) { /* 13 index */
        pbuf[index++] = (td_u8)(enc_len >> 16); /* 16 right shift */
        pbuf[index++] = (td_u8)(enc_len >> 8);  /* 8  right shift */
        pbuf[index++] = (td_u8)(enc_len);
    } else if ((index == 14) && (enc_len <= 0xFFFF)) { /* 14 index */
        pbuf[index++] = (td_u8)(enc_len >> 8); /* 8  right shift */
        pbuf[index++] = (td_u8)(enc_len);
    } else {
        ot_err_cipher("Invalid Mlen: 0x%x, q: 0x%x!\n", enc_len, 16 - index); /* 16 */
        return TD_FAILURE;
    }

   /* Formatting of the Associated Data in B1, the length of A denotes as a */
   /* The value a is encoded according to the following three cases:
    * If 0 < a < 2^16 - 2^8, then a  is encoded as a[0..15], i.e., two octets.
    * If 2^16 - 2^8 <= a < 2^32, then a  is encoded as 0xff || 0xfe || a[0..31], i.e., six octets.
    * If 2^32 <= a < 2^64, then  a is encoded as 0xff || 0xff || a[0..63], i.e., ten octets.
    * For example, if a=2^16, the encoding of a  is
    * 11111111 11111110 00000000 00000001 00000000 00000000.
    */
    pbuf = arr_b + 16; /* 16 offset */
    index = 0;
    if (channel->ctrl_ex.alen > 0) {
        if (channel->ctrl_ex.alen < (0x10000 - 0x100)) {
            pbuf[index++] = (td_u8)(channel->ctrl_ex.alen >> 8); /* 8 right shift */
            pbuf[index++] = (td_u8)(channel->ctrl_ex.alen);
        } else {
            pbuf[index++] = 0xFF;
            pbuf[index++] = 0xFE;
            pbuf[index++] = (td_u8)(channel->ctrl_ex.alen >> 24); /* 24 right shift */
            pbuf[index++] = (td_u8)(channel->ctrl_ex.alen >> 16); /* 16 right shift */
            pbuf[index++] = (td_u8)(channel->ctrl_ex.alen >> 8); /* 8 right shift */
            pbuf[index++] = (td_u8)channel->ctrl_ex.alen;
        }
    }
    *b1_len = index;

    return TD_SUCCESS;
}

static td_s32 drv_cipher_cpu_input(const td_u8 *input, td_u8 *output, td_u32 length,
    td_u32 block_size, td_u32 ctrl, td_bool set_last)
{
    td_s32 ret;
    td_u32 buf[4]; /* 4 buf arr size */
    td_u32 size, offset;
    u_chan0_cipher_ctrl chn0_ctrl;
    u_cipher_int_raw int_raw;

    chn0_ctrl.u32 = ctrl;

    chk_formula_fail_return(block_size == 0);

    for (offset = 0; offset < length; offset += block_size) {
        td_u32 i;
        td_u32 time = 0;

        /* Compute one block, if less than one block, padding with 0 */
        size = (offset + block_size) < length ? block_size : length - offset;
        (td_void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
        ret = memcpy_s(buf, sizeof(buf), &input[offset], size);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        for (i = 0; i < block_size / 4; i++) { /* 4 */
            hal_cipher_write_reg(CHN_0_CIPHER_DIN + i * 4, buf[i]); /* 4 */
        }

        /* may be needs set last flag when compute last block */
        if (((offset + block_size) >= length) && set_last) {
            chn0_ctrl.bits.sym_ch0_ccm_gcm_pc_last = 0x01;
        }
        hal_cipher_write_reg(CHN_0_CIPHER_CTRL, chn0_ctrl.u32);

        /* start working */
        hal_cipher_write_reg(CHN_0_CIPHER_CTRL, chn0_ctrl.u32 | 0x01);

        /* Waiting compute finished */
        hal_cipher_read_reg(CIPHER_INT_RAW, &int_raw.u32);
        while (!(int_raw.bits.cipher_chn_obuf_raw & 0x01) && (time++ < SPACC_TIME_OUT)) {
            hal_cipher_read_reg(CIPHER_INT_RAW, &int_raw.u32);
        }

        if (time >= SPACC_TIME_OUT) {
            ot_err_cipher("Chn 0 time out!\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
        }

        /* Clean raw interrupt */
        int_raw.u32 = 0x00;
        int_raw.bits.cipher_chn_obuf_raw = 0x01;
        hal_cipher_write_reg(CIPHER_INT_RAW, int_raw.u32);

        /* May be needs read output data */
        if (output != TD_NULL) {
            for (i = 0; i < block_size / 4; i++) { /* 4 */
                hal_cipher_read_reg(CHN_0_CIPHER_DOUT + i * 4, &buf[i]); /* 4 */
            }
            ret = memcpy_s(&output[offset], length - offset, (td_u8*)buf, size);
            chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
        }

        /* prepare compute next block */
        chn0_ctrl.u32 &= ~(0x01 << 15); /* 15 iv set */
    }
    return TD_SUCCESS;
}

static td_s32 drv_cipher_cpu_mmz_map(const spacc_symc_chn_s *channel,
    const cipher_data_s *ci_data, spacc_mmz_s *spacc_mmz)
{
    td_s32 ret;

    /* mapping phy address of SRC */
    spacc_mmz->src_mmz_buf.mmz_size = ci_data->data_length;
    spacc_mmz->src_mmz_buf.start_phy_addr =
        make_ulong(ci_data->src_phy_addr, ci_data->src_phy_addr_high);
    ret = cipher_mmz_map(&spacc_mmz->src_mmz_buf);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("DRV SRC MMZ MAP ERROR!, addr = 0x%x!\n", ci_data->src_phy_addr);
        return ret;
    }
    spacc_mmz->src_vir = spacc_mmz->src_mmz_buf.start_vir_addr;

    /* mapping phy address of DST */
    spacc_mmz->dest_mmz_buf.mmz_size = ci_data->data_length;
    spacc_mmz->dest_mmz_buf.start_phy_addr =
        make_ulong(ci_data->dest_phy_addr, ci_data->dest_phy_addr_high);
    ret = cipher_mmz_map(&spacc_mmz->dest_mmz_buf);
    if (ret !=  TD_SUCCESS) {
        ot_err_cipher("DRV DEST MMZ MAP ERROR! addr = 0x%x!\n", ci_data->dest_phy_addr);
        cipher_mmz_unmap(&spacc_mmz->src_mmz_buf);
        return ret;
    }
    spacc_mmz->dest_vir = spacc_mmz->dest_mmz_buf.start_vir_addr;

    /* mapping phy address of A for CCM/GCM */
    if ((channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) ||
        (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM)) {
        spacc_mmz->aad_mmz_buf.mmz_size = channel->ctrl_ex.alen;
        spacc_mmz->aad_mmz_buf.start_phy_addr =
            make_ulong(channel->ctrl_ex.aphy_addr, channel->ctrl_ex.aphy_addr_high);
        ret = cipher_mmz_map(&spacc_mmz->aad_mmz_buf);
        if (ret != TD_SUCCESS) {
            ot_err_cipher("DRV AD MMZ MAP ERROR!, addr = 0x%x!\n", channel->ctrl_ex.aphy_addr);
            cipher_mmz_unmap(&spacc_mmz->src_mmz_buf);
            cipher_mmz_unmap(&spacc_mmz->dest_mmz_buf);
            return ret;
        }
        spacc_mmz->aad_vir = spacc_mmz->aad_mmz_buf.start_vir_addr;
    }
    return ret;
}

static td_void drv_cipher_cpu_mmz_unmap(const spacc_symc_chn_s *channel, const spacc_mmz_s *spacc_mmz)
{
    cipher_mmz_unmap(&spacc_mmz->src_mmz_buf);
    cipher_mmz_unmap(&spacc_mmz->dest_mmz_buf);

    if ((channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) ||
        (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM))
        cipher_mmz_unmap(&spacc_mmz->aad_mmz_buf);
}

static td_s32 drv_cipher_cpu_enc_ccm(const spacc_symc_chn_s *channel,
    const cipher_data_s *ci_data, u_chan0_cipher_ctrl *chn0_ctrl, const spacc_mmz_s *spacc_mmz)
{
    td_u8 arr_b[32] = {0}; /* 32 arr size */
    td_s32 ret;
    td_u32 value;
    td_u32 offset = 0;
    td_bool set_last;

    /* Format N and A */
    ret = drv_cipher_ccm_head_format(channel, ci_data->data_length, arr_b, sizeof(arr_b), &offset);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_ccm_head_format);

    /* Set last block size */
    chn0_ctrl->bits.sym_ccm_gcm_last_block = (ci_data->data_length + 15) % 16; /* 15, 16 */

    /* Set flag N */
    chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x00; // N

    /* Compute B0, contains N */
    set_last = (channel->ctrl_ex.alen + ci_data->data_length == 0) ? TD_TRUE : TD_FALSE;
    ret = drv_cipher_cpu_input(arr_b, TD_NULL, 16, 16, chn0_ctrl->u32, set_last); /* 16 */
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_input);

    /* Don't update IV any more */
    chn0_ctrl->bits.sym_ch0_ivin_sel = 0x00;

    /* Compute A */
    value = channel->ctrl_ex.alen + offset;
    if (value > 16) { /* 16, a can't puts in one block */
        /* Set flag A */
        chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x01; // A

        /* Fill head of A to B1 split joint 16 byets */
        ret = memcpy_s(arr_b + 16 + offset, sizeof(arr_b) - 16 - offset, /* 16 */
            spacc_mmz->aad_vir, 16 - offset); /* 16 */
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        /* Compute B1, contains the coding of a and head of A */
        ret = drv_cipher_cpu_input(arr_b + 16, TD_NULL, 16, 16, chn0_ctrl->u32, TD_FALSE); /* 16 */
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_input);

        /* Compute the left data of A */
        set_last = (ci_data->data_length == 0) ? TD_TRUE : TD_FALSE;
        ret = drv_cipher_cpu_input(spacc_mmz->aad_vir + 16 - offset, TD_NULL, /* 16 */
            channel->ctrl_ex.alen - (16 - offset), 16, chn0_ctrl->u32, set_last); /* 16 */
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_input);
    } else if (value > 0) { /* A and a can puts in one block */
        /* Set flag A */
        chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x01; // A

        /* Fill A to B1 split joint 16 byets */
        ret = memcpy_s(arr_b + 16 + offset, sizeof(arr_b) - 16 - offset, /* 16 */
            spacc_mmz->aad_vir, channel->ctrl_ex.alen);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        /* Compute B1, contains the coding of a and A */
        set_last = (ci_data->data_length == 0) ? TD_TRUE : TD_FALSE;
        ret = drv_cipher_cpu_input(arr_b + 16, TD_NULL, 16, 16, chn0_ctrl->u32, set_last); /* 16 */
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_input);
    }

    /* Next, compute the P */
    chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x02; // P

    return ret;
}

static td_s32 drv_cipher_cpu_enc_gcm(const spacc_symc_chn_s *channel, const cipher_data_s *ci_data,
    u_chan0_cipher_ctrl *chn0_ctrl, const spacc_mmz_s *spacc_mmz, td_u32 block_size)
{
    td_s32 ret;
    td_bool set_last;

    /* Set last block size */
    chn0_ctrl->bits.sym_ccm_gcm_last_block = (ci_data->data_length + 15) % 16; /* 15, 16 */

    /* Compute A */
    if (channel->ctrl_ex.alen > 0) {
        /* Set flag A */
        chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x00; // A

        /* Compute A */
        set_last = (ci_data->data_length == 0) ? TD_TRUE : TD_FALSE;
        ret = drv_cipher_cpu_input(spacc_mmz->aad_vir, TD_NULL,
            channel->ctrl_ex.alen, block_size, chn0_ctrl->u32, set_last);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_input);

        chn0_ctrl->bits.sym_ch0_ivin_sel = 0x00;
    }

    /* Don't update IV any more */
    chn0_ctrl->bits.sym_ch0_ccm_gcm_input_flag = 0x01; // P
    return TD_SUCCESS;
}

static td_s32 drv_cipher_cpu_enc_phy(const cipher_data_s *ci_data, td_u32 block_size, td_bool is_decrypt)
{
    td_s32 ret;
    u_chan0_cipher_ctrl chn0_ctrl;
    td_u8 arr_b[32]; /* 32 arr size */
    spacc_mmz_s spacc_mmz;
    spacc_symc_chn_s *channel = &g_symc_chn[0];

    ret = drv_cipher_cpu_mmz_map(channel, ci_data, &spacc_mmz);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_mmz_map);

    /* Decrypt or encrypt */
    hal_cipher_read_reg(CHN_0_CIPHER_CTRL, &chn0_ctrl.u32);
    chn0_ctrl.bits.sym_ch0_decrypt = is_decrypt;
    hal_cipher_write_reg(CHN_0_CIPHER_CTRL, chn0_ctrl.u32);

    if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) {
        ret = drv_cipher_cpu_enc_ccm(channel, ci_data, &chn0_ctrl, &spacc_mmz);
    } else if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM) {
        ret = drv_cipher_cpu_enc_gcm(channel, ci_data, &chn0_ctrl, &spacc_mmz, block_size);
    }
    if (ret != TD_SUCCESS) {
        drv_cipher_cpu_mmz_unmap(channel, &spacc_mmz);
        return ret;
    }

    /* Compute P */
    if (ci_data->data_length > 0) {
        ret = drv_cipher_cpu_input(spacc_mmz.src_vir, spacc_mmz.dest_vir,
            ci_data->data_length, block_size, chn0_ctrl.u32, TD_TRUE);
        if (ret != TD_SUCCESS) {
            drv_cipher_cpu_mmz_unmap(channel, &spacc_mmz);
            return ret;
        }

        /* Don't update IV any more */
        chn0_ctrl.bits.sym_ch0_ivin_sel = 0x00;
    }

    /* Compute LEN(C) for GCM */
    if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM) {
        /* Set flag LEN(C) */
        (td_void)memset_s(arr_b, sizeof(arr_b), 0, sizeof(arr_b));
        chn0_ctrl.bits.sym_ch0_ccm_gcm_input_flag = 0x02; // LEN

        /* Format LEN(C) = LEN(A) || LEN(P), coding in bits */
        arr_b[3] = (td_u8)((channel->ctrl_ex.alen >> 29) & 0x07); /* 3 index, 29 shift bits */
        arr_b[4] = (td_u8)((channel->ctrl_ex.alen >> 21) & 0xff); /* 4 index, 21 shift bits */
        arr_b[5] = (td_u8)((channel->ctrl_ex.alen >> 13) & 0xff); /* 5 index, 13 shift bits */
        arr_b[6] = (td_u8)((channel->ctrl_ex.alen >> 5)  & 0xff); /* 6 index, 5  shift bits */
        arr_b[7] = (td_u8)((channel->ctrl_ex.alen << 3)  & 0xff); /* 7 index, 3  shift bits */

        arr_b[11] = (td_u8)((ci_data->data_length >> 29) & 0x07); /* 11 index, 29 shift bits */
        arr_b[12] = (td_u8)((ci_data->data_length >> 21) & 0xff); /* 12 index, 21 shift bits */
        arr_b[13] = (td_u8)((ci_data->data_length >> 13) & 0xff); /* 13 index, 13 shift bits */
        arr_b[14] = (td_u8)((ci_data->data_length >> 5)  & 0xff); /* 14 index, 5  shift bits */
        arr_b[15] = (td_u8)((ci_data->data_length << 3)  & 0xff); /* 15 index, 3  shift bits */

        /* Compute LEN(C) */
        ret = drv_cipher_cpu_input(arr_b, TD_NULL, 16, 16, chn0_ctrl.u32, TD_FALSE);  /* 16 */
    }

    drv_cipher_cpu_mmz_unmap(channel, &spacc_mmz);

    return ret;
}

static td_s32 drv_cipher_ccm_na(spacc_symc_chn_s *channel, td_u32 enc_len)
{
    td_s32 ret;
    td_u8 *pbuf = TD_NULL;
    td_u32 index = 0;
    td_u32 alen = 0;
    td_u32 count, flag;

    pbuf = channel->pad_vir_addr;
    ret = memset_s(pbuf, SPACC_PAD_BUF_SIZE, 0, 32);  /* 32 clean size */
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

    /* Format B0 and head of B1 */
    ret = drv_cipher_ccm_head_format(channel, enc_len, pbuf, 32, &alen);  /* 32 */
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_ccm_head_format);

    /* B0 contains the N, set flag N, for N, A, P, must set the first and last flag. */
    flag = SPACC_CTRL_SYMC_IN_CCM_N | SPACC_CTRL_SYMC_IN_LAST | SPACC_CTRL_SYMC_IN_FIRST;
    if ((channel->ctrl_ex.alen == 0) && (enc_len == 0))
        flag |= SPACC_CTRL_SYMC_CCM_LAST;

    ret = spacc_symc_addbuf(channel->hard_num, channel->pad_phy_addr, 16, SPACC_BUF_TYPE_SYMC_IN, flag); /* 16 */
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    index += 16; /* 16 */

    /* a > 0, add the phy of A into node list */
    if (alen) {
        /* 1st. add the phy of B1 into node list, which contains the coding of a */
        ret = spacc_symc_addbuf(channel->hard_num, channel->pad_phy_addr + 16, /* 16 */
            alen, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_CCM_A);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
        index += alen;

        /* 2nd. add the phy of A into node list */
        ret = spacc_symc_addbuf(channel->hard_num, channel->ctrl_ex.aphy_addr,
            channel->ctrl_ex.alen, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_CCM_A);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    }

    /* if alen + Alen do not aligned with 16, padding 0 to the tail */
    count = (channel->ctrl_ex.alen + alen) % AES_BLOCK_SIZE;
    if (count != 0) {
        /* Compute the padding length */
        count = AES_BLOCK_SIZE - count;

        /* Set zero */
        ret = memset_s(pbuf + index, SPACC_PAD_BUF_SIZE - index, 0, count);
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

        /* add the padding phy of A into node list */
        ret = spacc_symc_addbuf(channel->hard_num, channel->pad_phy_addr + index,
            count, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_CCM_A);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    }

    flag = SPACC_CTRL_SYMC_IN_LAST;
    if (enc_len == 0) { /* if do not contains the P, set CCM last flag signal to hardware */
        flag |= SPACC_CTRL_SYMC_CCM_LAST;
    }
    spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, flag);

    return TD_SUCCESS;
}

static td_s32 drv_cipher_gcm_a(spacc_symc_chn_s *channel)
{
    td_s32 ret;
    td_u32 count;
    td_u32 index = 0;
    td_u8 *pbuf = TD_NULL;

    if (channel->ctrl_ex.alen == 0)
        return TD_SUCCESS;

    pbuf = channel->pad_vir_addr;

    /* Add phy of A into node list */
    ret = spacc_symc_addbuf(channel->hard_num,
        make_ulong(channel->ctrl_ex.aphy_addr, channel->ctrl_ex.aphy_addr_high),
        channel->ctrl_ex.alen, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_GCM_A);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("spacc add A buf failed, ret = 0x%x.\n", ret);
        return TD_FAILURE;
    }

    /* if Alen do not aligned with 16, padding 0 to the tail */
    count = channel->ctrl_ex.alen % AES_BLOCK_SIZE;
    if (count != 0) {
        /* Compute the padding length */
        count = AES_BLOCK_SIZE - count;

        /* Set zero */
        ret = memset_s(pbuf + index, SPACC_PAD_BUF_SIZE, 0, 16); /* 16 clean size */
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memset_s);

        /* add the padding phy of A into node list */
        ret = spacc_symc_addbuf(channel->hard_num,
            channel->pad_phy_addr + index, count,
            SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_GCM_A);
        if (ret != TD_SUCCESS) {
            ot_err_cipher("spacc add A PAD buf failed, ret = 0x%x.\n", ret);
            return TD_FAILURE;
        }
    }

    /* Set A last flag */
    spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_LAST);

    return TD_SUCCESS;
}

static td_s32 drv_cipher_gcm_len(spacc_symc_chn_s *channel, td_u32 enc_len)
{
    td_s32 ret;
    td_u8 *pbuf = TD_NULL;
    td_u32 index = 32;

    /* Format len(C), 16 byets, coding in bits.
     * Byet0~7:  bits number of Add
     * Byet8~15: bits number of P
     */
    pbuf = channel->pad_vir_addr;

    pbuf[index + 0]  = 0x00; /* 0 arr index */
    pbuf[index + 1]  = 0x00; /* 1 arr index */
    pbuf[index + 2]  = 0x00; /* 2 arr index */
    pbuf[index + 3]  = (td_u8)((channel->ctrl_ex.alen >> 29) & 0x07); /* 3 arr index, 29 shift bits */
    pbuf[index + 4]  = (td_u8)((channel->ctrl_ex.alen >> 21) & 0xff); /* 4 arr index, 21 shift bits */
    pbuf[index + 5]  = (td_u8)((channel->ctrl_ex.alen >> 13) & 0xff); /* 5 arr index, 13 shift bits */
    pbuf[index + 6]  = (td_u8)((channel->ctrl_ex.alen >> 5)  & 0xff); /* 6 arr index, 5  shift bits */
    pbuf[index + 7]  = (td_u8)((channel->ctrl_ex.alen << 3)  & 0xff); /* 7 arr index, 3 shift bits */
    pbuf[index + 8]  = 0x00; /* 8 arr index */
    pbuf[index + 9]  = 0x00; /* 9 arr index */
    pbuf[index + 10] = 0x00; /* 10 arr index */
    pbuf[index + 11] = (td_u8)((enc_len >> 29) & 0x07); /* 11 arr index, 29 shift bits */
    pbuf[index + 12] = (td_u8)((enc_len >> 21) & 0xff); /* 12 arr index, 21 shift bits */
    pbuf[index + 13] = (td_u8)((enc_len >> 13) & 0xff); /* 13 arr index, 13 shift bits */
    pbuf[index + 14] = (td_u8)((enc_len >> 5)  & 0xff); /* 14 arr index, 5 shift bits */
    pbuf[index + 15] = (td_u8)((enc_len << 3)  & 0xff); /* 15 arr index, 3 shift bits */

    /* Add to nodes list */
    ret = spacc_symc_addbuf(channel->hard_num, channel->pad_phy_addr + index,
        16, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_GCM_LEN | SPACC_CTRL_SYMC_IN_LAST); /* 16 */
    if (ret != TD_SUCCESS) {
        ot_err_cipher("spacc add P buf failed, ret = 0x%x.\n", ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 drv_cipher_enc_ccm(spacc_symc_chn_s *channel,
    const cipher_data_s *ci_data, td_size_t src_phy_addr, td_size_t dst_phys_addr)
{
    td_s32 ret;

    /* format N and A, add to node list */
    ret = drv_cipher_ccm_na(channel, ci_data->data_length);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_ccm_na);

    /* Add the phy of P into node list */
    if (ci_data->data_length > 0) {
        /* Add in buffer */
        ret = spacc_symc_addbuf(channel->hard_num, src_phy_addr, ci_data->data_length,
            SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_CCM_P | SPACC_CTRL_SYMC_IN_LAST);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

        /* Add out buffer */
        ret = spacc_symc_addbuf(channel->hard_num, dst_phys_addr, ci_data->data_length,
            SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    } else {
        /* If P is null, must add a empty node into node list, limit to hardware devising */
        ret = spacc_symc_addbuf(channel->hard_num, 0x00, 0x00,
            SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    }

    /* Set CCM last flag */
    spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_CCM_LAST);

    return ret;
}

static td_s32 drv_cipher_enc_gcm(spacc_symc_chn_s *channel,
    const cipher_data_s *ci_data, td_size_t src_phy_addr, td_size_t dest_phy_addr)
{
    td_s32 ret;

    /* format N, add to node list */
    ret = drv_cipher_gcm_a(channel);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_gcm_a);

    /* Add the phy of P into node list */
    if (ci_data->data_length > 0) {
        /* Add in buffer */
        ret = spacc_symc_addbuf(channel->hard_num, src_phy_addr, ci_data->data_length,
            SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_GCM_P | SPACC_CTRL_SYMC_IN_LAST);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

        /* Add out buffer */
        ret = spacc_symc_addbuf(channel->hard_num, dest_phy_addr, ci_data->data_length,
            SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_NONE);
        chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);
    }

    /* At the and of GCM, must add a empty node to nodes list, limit to hardware devising */
    ret = spacc_symc_addbuf(channel->hard_num, 0x00, 0x00,
        SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

    /* Format the length fields of C and add to nodes list */
    ret = drv_cipher_gcm_len(channel, ci_data->data_length);
    chk_func_fail_return(ret != TD_SUCCESS, ret, spacc_symcdrv_cipher_gcm_len_addbuf);

    return ret;
}

/* except ccm/gcm */
static td_s32 drv_cipher_enc_others(const spacc_symc_chn_s *channel,
    const cipher_data_s *ci_data, td_size_t src_phy_addr, td_size_t dest_phy_addr)
{
    td_s32 ret;

    /* except ccm/gcm, the data length must not zero */
    if (ci_data->data_length == 0) {
        ot_err_cipher("Invalid data len 0x%x.\n", ci_data->data_length);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    /* The length of data depend on alg and mode, which limit to hardware
     * for ecb/cbc/ofb/cfb, the data length must aligned with block size.
     * for ctr/ccm/gcm, support any data length.
     */
    if (((channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_ECB) ||
         (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CBC) ||
         (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_OFB) ||
         (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CFB)) &&
         (channel->ctrl_ex.ci_alg != OT_CIPHER_ALG_DMA)) {
        if (ci_data->data_length % channel->block_size != 0) {
            ot_err_cipher("Invalid data len 0x%x.\n", ci_data->data_length);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

    /* Add in buffer */
    ret = spacc_symc_addbuf(channel->hard_num, src_phy_addr, ci_data->data_length,
        SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_LAST);
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

    /* Add out buffer */
    ret = spacc_symc_addbuf(channel->hard_num, dest_phy_addr, ci_data->data_length,
        SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
    chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

    return ret;
}

static td_s32 drv_cipher_crypto(const cipher_data_s *ci_data, td_bool is_decrypt)
{
    td_s32 ret;
    spacc_symc_chn_s *channel;
    td_u32 soft_chn_id;
    td_size_t src_phy_addr, dest_phy_addr;

    src_phy_addr = make_ulong(ci_data->src_phy_addr, ci_data->src_phy_addr_high);
    dest_phy_addr = make_ulong(ci_data->dest_phy_addr, ci_data->dest_phy_addr_high);

    soft_chn_id = td_handle_get_chnid(ci_data->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    if (soft_chn_id != 0) {
        if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) {
            ret = drv_cipher_enc_ccm(channel, ci_data, src_phy_addr, dest_phy_addr);
            chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_enc_ccm);
        } else if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM) {
            ret = drv_cipher_enc_gcm(channel, ci_data, src_phy_addr, dest_phy_addr);
            chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_enc_gcm);
        } else { /* ECB/CBC/CBF/OFB/CTR */
            ret = drv_cipher_enc_others(channel, ci_data, src_phy_addr, dest_phy_addr);
            chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_enc_others);
        }

        channel->symc_done = TD_FALSE;

        /* Start working */
        spacc_symc_start(channel->hard_num, is_decrypt, channel->ctrl_ex.change_flags.bits_iv);

        if (channel->ctrl_ex.change_flags.bits_iv == OT_CIPHER_IV_CHG_ONE_PKG)
            channel->ctrl_ex.change_flags.bits_iv = 0; /* only update IV for first pkg */

        /* Waiting hardware computing finished */
        ret = drv_cipher_symc_wait_done(channel, SPACC_TIME_OUT);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_symc_wait_done);

        flush_cache(cipher_align_down(dest_phy_addr), cipher_align_size(dest_phy_addr, ci_data->data_length));
    } else {
        /* Chn 0, CPU mode */
        ret = drv_cipher_cpu_enc_phy(ci_data, channel->block_size, is_decrypt);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_cpu_enc_phy);
    }

    /* Save IV */
    if (channel->ctrl_ex.change_flags.bits_iv == OT_CIPHER_IV_CHG_ONE_PKG)
        spacc_symc_getiv(channel->hard_num, channel->ctrl_ex.iv, sizeof(channel->ctrl_ex.iv));

    flush_cache(cipher_align_down(dest_phy_addr), cipher_align_size(dest_phy_addr, ci_data->data_length));

    return ret;
}

td_s32 ot_drv_cipher_get_tag(cipher_tag_s *tag)
{
    td_u32 soft_chn_id;
    spacc_symc_chn_s *channel = TD_NULL;
    td_u32 i;
    td_s32 ret;

    if (tag ==  TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return TD_FAILURE;
    }
    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(tag->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;

    soft_chn_id = td_handle_get_chnid(tag->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    if ((channel->ctrl_ex.work_mode != OT_CIPHER_WORK_MODE_CCM) &&
        (channel->ctrl_ex.work_mode != OT_CIPHER_WORK_MODE_GCM)) {
        ot_err_cipher("Invalid mode %d!\n", channel->ctrl_ex.work_mode);
        cipher_mutex_unlock(&g_symc_mutex);
        return TD_FAILURE;
    }
    tag->tag_len = channel->ctrl_ex.tag_len;

    /* Read tag for CCM/GCM */
    if (soft_chn_id == 0) {
        for (i = 0; i < 4; i++) /* 4 loop count */
            hal_cipher_read_reg(CHN_0_CCM_GCM_TAG + i * 4, &tag->tag[i]); /* 4 */
    } else {
        spacc_symc_gettag(channel->hard_num, (td_u8 *)tag->tag, sizeof(tag->tag));
    }

    cipher_mutex_unlock(&g_symc_mutex);

    return TD_SUCCESS;
}

td_s32 ot_drv_cipher_encrypt(const cipher_data_s *ci_data)
{
    td_s32 ret;

    if (ci_data == TD_NULL) {
        ot_err_cipher("Invalid point!\n");
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(ci_data->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;

    ret = drv_cipher_crypto(ci_data, TD_FALSE);
    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

td_s32 ot_drv_cipher_decrypt(const cipher_data_s *ci_data)
{
    td_s32 ret;

    if (ci_data ==  TD_NULL) {
        ot_err_cipher("Invalid point!\n");
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(ci_data->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;

    ret = drv_cipher_crypto(ci_data, TD_TRUE);
    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

/* Get odd/even key flag and P flag */
static td_u32 drv_cipher_get_pay_load_ctrl(td_bool odd_key, ot_cipher_work_mode mode)
{
    td_u32 ctrl;

    ctrl = odd_key ? SPACC_CTRL_SYMC_ODD_KEY : SPACC_CTRL_SYMC_EVEN_KEY;

    if (mode == OT_CIPHER_WORK_MODE_CCM)
        ctrl |= SPACC_CTRL_SYMC_IN_CCM_P;
    else if (mode == OT_CIPHER_WORK_MODE_GCM)
        ctrl |= SPACC_CTRL_SYMC_IN_GCM_P;

    return ctrl;
}

static td_s32 drv_cipher_block_align(spacc_symc_chn_s *channel, td_u32 total, td_u32 node_cur)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 ctrl;

    /* Compute the tail length */
    total %= channel->block_size;
    if (total > 0)
        total = channel->block_size - total;

    /* if the total length don't aligned with block size, split joint the follow nodes */
    while ((total > 0) && (channel->node_cur < channel->node_num)) {
        /* The next node large than tail size, just split it to 2 nodes */
        if (channel->node_list[node_cur].byte_len > total) {
            /* Add P in */
            ctrl = drv_cipher_get_pay_load_ctrl(channel->node_list[node_cur].odd_key, channel->ctrl_ex.work_mode);
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].src_phys_addr,
                total, SPACC_BUF_TYPE_SYMC_IN, ctrl);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* Add P out */
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].dst_phys_addr,
                total, SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_NONE);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* Let next node skip the tail size */
            channel->node_list[node_cur].src_phys_addr  += total;
            channel->node_list[node_cur].dst_phys_addr += total;
            channel->node_list[node_cur].byte_len -= total;
            total = 0;
        } else {
            /* The next node less than tail size, add it to nodes list */
            /* Add P in */
            ctrl = drv_cipher_get_pay_load_ctrl(channel->node_list[node_cur].odd_key, channel->ctrl_ex.work_mode);
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].src_phys_addr,
                channel->node_list[node_cur].byte_len, SPACC_BUF_TYPE_SYMC_IN, ctrl);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* Add P out */
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].dst_phys_addr,
                channel->node_list[node_cur].byte_len, SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_NONE);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* re-compute the tail size */
            total -= channel->node_list[node_cur].byte_len;

            /* Process next node */
            node_cur++;
            channel->node_cur++;
        }
    }

    return ret;
}

static td_s32 drv_cipher_add_nodes(spacc_symc_chn_s *channel, td_u32 int_level)
{
    td_s32 ret;
    td_u32 i, nodes, node_cur, ctrl, total;

    if (channel->node_cur < channel->node_num) {
        nodes = cipher_min(int_level, channel->node_num - channel->node_cur);
        node_cur = channel->node_cur;
        total = 0;
        for (i = 0; i < nodes; i++) {
            /* Get odd/even key flag and P flag */
            ctrl = drv_cipher_get_pay_load_ctrl(channel->node_list[node_cur].odd_key, channel->ctrl_ex.work_mode);

            /* Add P in */
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].src_phys_addr,
                channel->node_list[node_cur].byte_len, SPACC_BUF_TYPE_SYMC_IN, ctrl);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* Add P out */
            ret = spacc_symc_addbuf(channel->hard_num, channel->node_list[node_cur].dst_phys_addr,
                channel->node_list[node_cur].byte_len, SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_NONE);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            total += channel->node_list[node_cur].byte_len;
            channel->node_cur++;
            node_cur++;
        }

       /* For each compute, the total length of valid nodes list
        * must aligned with block size, otherwise can't recv interrupt,
        * which limit to hardware devising.
        */
        ret = drv_cipher_block_align(channel, total, node_cur);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_block_align);
    }

    if (channel->node_cur == channel->node_num) { /* Set last flag */
        if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) {
            /* Set CCM last flag */
            spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_CCM_LAST);
        } else if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM) {
            /* Set GCM last flag */
            spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_LAST);

            /* At the and of GCM, must add a empty P node to nodes list, limit to hardware devising */
            ret = spacc_symc_addbuf(channel->hard_num, 0x00, 0x00,
                SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, spacc_symc_addbuf);

            /* After compute P, compute LEN(C) for GCM */
            ret = drv_cipher_gcm_len(channel, channel->total_len);
            chk_func_fail_return(ret != TD_SUCCESS, ret, spacc_symc_addbuf);
        }

        /* Set symc last flag */
        spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_IN, SPACC_CTRL_SYMC_IN_LAST);
        spacc_symc_addctrl(channel->hard_num, SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
    }

    return TD_SUCCESS;
}

static td_void drv_cipher_callback(td_u32 chn_id)
{
    td_s32 ret;
    spacc_symc_chn_s *channel = TD_NULL;

    if (chn_id >= SPACC_MAX_CHN) {
        ot_err_cipher("invalid chn_id %u\n", chn_id);
        return;
    }
    channel = &g_symc_chn[chn_id];

    /* Compute the follow nodes */
    if (channel->node_cur < channel->node_num) {
        ret = drv_cipher_add_nodes(channel, SYMC_INT_LEVEL);
        if (ret == TD_SUCCESS)
            spacc_symc_restart(channel->hard_num, channel->ctrl_ex.change_flags.bits_iv);
    } else {
        /* All the nodes compute finished, wake up user */
        channel->symc_done = TD_TRUE;
        ot_info_cipher("chn %d wake up\n", channel->hard_num);
        cipher_queue_wait_up(&channel->queue);
    }
}

static td_s32 drv_cipher_enc_prepare(spacc_symc_chn_s *channel)
{
    td_s32 ret = TD_SUCCESS;

    /* Before compute P, compute N and A for CCM firstly */
    if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CCM) {
        /* Format N and A, add to node list */
        ret = drv_cipher_ccm_na(channel, channel->total_len);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_ccm_na);

        if (channel->total_len == 0) {
            /* If P is null, must add a empty node into node list, limit to hardware devising */
            ret = spacc_symc_addbuf(channel->hard_num, 0x00, 0x00,
                SPACC_BUF_TYPE_SYMC_OUT, SPACC_CTRL_SYMC_OUT_LAST);
            chk_func_fail_return(ret != SPACC_OK, TD_FAILURE, drv_cipher_ccm_na);
        }
    } else if (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_GCM) {
        /* Before compute P, compute A for GCM firstly */
        ret = drv_cipher_gcm_a(channel);
        chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_gcm_a);
    } else if ((channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_ECB) ||
        (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CBC) ||
        (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_CFB) ||
        (channel->ctrl_ex.work_mode == OT_CIPHER_WORK_MODE_OFB)) {
       /* The length of data depend on alg and mode, which limit to hardware
        * for ecb/cbc/ofb/cfb, the total data length must aligned with block size.
        * for ctr/ccm/gcm, support any data length.
        */
        if (channel->total_len % channel->block_size != 0) {
            ot_err_cipher("PKG len must align with 16.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    }

    return ret;
}

static td_s32 drv_cipher_encrypt_multi(const cipher_pkg_s *pkg, td_bool is_decrypt)
{
    td_s32 ret;
    td_u32 i, soft_chn_id, remainder;
    spacc_symc_chn_s *channel = TD_NULL;
    cipher_data_compat_s *cipher_data = TD_NULL;

    chk_formula_fail_return((pkg->pkg_num == 0) || (pkg->pkg_num > MAX_MULTI_PKG_NUM));

    soft_chn_id = td_handle_get_chnid(pkg->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    ot_info_cipher("pkg_num %d\n", pkg->pkg_num);
    channel->node_list = cipher_malloc(pkg->pkg_num * sizeof(ot_cipher_data));
    chk_func_fail_return(channel->node_list == TD_NULL, OT_ERR_CIPHER_INVALID_POINT, cipher_malloc);

    cipher_data = (cipher_data_compat_s *)channel->node_list;

    /* copy node list from user space to kernel */
    (td_void)memcpy_s(channel->node_list, pkg->pkg_num * sizeof(ot_cipher_data),
        pkg->cipher_data, pkg->pkg_num * sizeof(ot_cipher_data));

    /* Compute and check the nodes length */
    channel->total_len = 0;

    for (i = pkg->pkg_num; i > 0; i--) {
        if (pkg->user_bit_width != MY_CPU_BIT_WIDTH) {
            channel->node_list[i - 1].byte_len = cipher_data[i - 1].byte_length;
            channel->node_list[i - 1].dst_phys_addr = cipher_data[i - 1].dst_phys_addr;
            channel->node_list[i - 1].src_phys_addr = cipher_data[i - 1].src_phys_addr;
        }

        /* Can't used the odd key */
        if (channel->node_list[i - 1].odd_key) {
            ot_err_cipher("Odd key unsupported.\n");
            return TD_FAILURE;
        }

        /* each node length can't be zero */
        if (channel->node_list[i - 1].byte_len == 0) {
            ot_err_cipher("PKG len must large than 0.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
        }

        channel->total_len += channel->node_list[i - 1].byte_len;
    }

    ret = drv_cipher_enc_prepare(channel);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_enc_prepare);

    channel->node_num = pkg->pkg_num;
    channel->node_cur = 0;

    /* For one time compute, the max nodes is 127, but 126 for first time,
     * here we compute 100 nodes firstly,
     * Because under each compute, the total length of valid nodes list
     * must aligned with block size, otherwise can't recv interrupt.
     * if the total length don't aligned with block size, we must
     * split joint the follow nodes with current nodes to multiple block size.
     * so it follows that, the nodes num for this time compute may be larger than 100,
     * The worst is that we need add 15 nodes(each node only carry 1 bye data)
     * within this time, that is the SYMC_INT_LEVEL must less than 127 - 16 - 1= 110.
     */
    remainder = cipher_min(pkg->pkg_num, SYMC_INT_LEVEL);
    ret = drv_cipher_add_nodes(channel, remainder);
    chk_func_fail_return(ret != TD_SUCCESS, ret, drv_cipher_add_nodes);

    channel->callback = drv_cipher_callback;
    channel->symc_done = TD_FALSE;
    spacc_symc_start(channel->hard_num, is_decrypt, channel->ctrl_ex.change_flags.bits_iv);
    return ret;
}

td_s32 ot_drv_cipher_encrypt_multi(const cipher_pkg_s *pkg)
{
    td_s32 ret;
    td_u32 soft_chn_id;
    spacc_symc_chn_s *channel = TD_NULL;

    if (pkg == TD_NULL) {
        ot_err_cipher("Invalid pkg.\n");
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(pkg->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;
    soft_chn_id = td_handle_get_chnid(pkg->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    ret = drv_cipher_encrypt_multi(pkg, TD_FALSE);
    if (ret != TD_SUCCESS) {
        if (channel->node_list != TD_NULL) {
            cipher_free(channel->node_list);
            channel->node_list = TD_NULL;
        }
        cipher_mutex_unlock(&g_symc_mutex);
        return ret;
    }

    ret = drv_cipher_symc_wait_done(channel, SPACC_TIME_OUT);
    if (ret != TD_SUCCESS)
        ot_err_cipher("spacc symc active failed, ret = 0x%x.\n", ret);

    if (channel->node_list != TD_NULL) {
        cipher_free(channel->node_list);
        channel->node_list = TD_NULL;
    }

    if (channel->ctrl_ex.change_flags.bits_iv == OT_CIPHER_IV_CHG_ONE_PKG)
        spacc_symc_getiv(channel->hard_num, channel->ctrl_ex.iv, sizeof(channel->ctrl_ex.iv));
    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

td_s32 ot_drv_cipher_decrypt_multi(const cipher_pkg_s *pkg)
{
    td_s32 ret;
    td_u32 soft_chn_id;
    spacc_symc_chn_s *channel = TD_NULL;

    if (pkg == TD_NULL) {
        ot_err_cipher("Invalid pkg.\n");
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(pkg->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;
    soft_chn_id = td_handle_get_chnid(pkg->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    ret = drv_cipher_encrypt_multi(pkg, TD_TRUE);
    if (ret != TD_SUCCESS) {
        if (channel->node_list != TD_NULL) {
            cipher_free(channel->node_list);
            channel->node_list = TD_NULL;
        }
        cipher_mutex_unlock(&g_symc_mutex);
        return ret;
    }

    ret = drv_cipher_symc_wait_done(channel, SPACC_TIME_OUT);
    if (ret != TD_SUCCESS)
        ot_err_cipher("spacc symc active failed, ret = 0x%x.\n", ret);

    if (channel->node_list != TD_NULL) {
        cipher_free(channel->node_list);
        channel->node_list = TD_NULL;
    }

    if (channel->ctrl_ex.change_flags.bits_iv == OT_CIPHER_IV_CHG_ONE_PKG)
        spacc_symc_getiv(channel->hard_num, channel->ctrl_ex.iv, sizeof(channel->ctrl_ex.iv));
    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

static td_s32 drv_digest_config(const cipher_hash_data_s *cipher_hash_data, spacc_ctrl_en *spacc_ctrl)
{
    td_s32 ret;
    digest_alg_en digest_alg;
    digest_mode_en digest_mode;

    *spacc_ctrl = SPACC_CTRL_NONE;

    switch (cipher_hash_data->sha_type) {
        case OT_CIPHER_HASH_TYPE_SHA1:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA1:
            digest_alg = DIGEST_ALG_SHA1;
            digest_mode = DIGEST_MODE_HASH;
            break;
        case OT_CIPHER_HASH_TYPE_SHA224:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA224:
            digest_alg = DIGEST_ALG_SHA224;
            digest_mode = DIGEST_MODE_HASH;
            break;
        case OT_CIPHER_HASH_TYPE_SHA256:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA256:
            digest_alg = DIGEST_ALG_SHA256;
            digest_mode = DIGEST_MODE_HASH;
            break;
        case OT_CIPHER_HASH_TYPE_SHA384:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA384:
            digest_alg = DIGEST_ALG_SHA384;
            digest_mode = DIGEST_MODE_HASH;
            break;
        case OT_CIPHER_HASH_TYPE_SHA512:
        case OT_CIPHER_HASH_TYPE_HMAC_SHA512:
            digest_alg = DIGEST_ALG_SHA512;
            digest_mode = DIGEST_MODE_HASH;
            break;
        case OT_CIPHER_HASH_TYPE_SM3:
            digest_alg = DIGEST_ALG_SM3;
            digest_mode = DIGEST_MODE_HASH;
            break;
        default:
            ot_err_cipher("Invalid hash type: 0x%x\n", cipher_hash_data->sha_type);
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    ret = spacc_digest_config(cipher_hash_data->hard_chn, digest_alg, digest_mode, TD_FALSE);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("spacc set digest mode failed, chn %d, alg %d, mode %d.\n",
            cipher_hash_data->hard_chn, digest_alg, digest_mode);
        return TD_FAILURE;
    }

    *spacc_ctrl = ((td_u32)*spacc_ctrl) | SPACC_CTRL_HASH_IN_FIRST;
    *spacc_ctrl = ((td_u32)*spacc_ctrl) | SPACC_CTRL_HASH_IN_LAST;

    return TD_SUCCESS;
}

td_s32 ot_drv_cipher_calc_hash_init(const cipher_hash_data_s *cipher_hash_data)
{
    return TD_SUCCESS;
}

td_s32 ot_drv_cipher_calc_hash_update(cipher_hash_data_s *cipher_hash_data)
{
    td_s32 ret;
    spacc_ctrl_en spacc_ctrl;
    spacc_digest_chn_s *channel = TD_NULL;

    chk_formula_fail_return(cipher_hash_data->hard_chn >= SPACC_MAX_CHN);
    channel = &g_digest_chn[cipher_hash_data->hard_chn];

    /* configure hash register */
    ret = drv_digest_config(cipher_hash_data, &spacc_ctrl);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("cipher config failed, ret = 0x%x.\n", ret);
        return TD_FAILURE;
    }

    /* Add the phy of data to nodes list */
    ret = spacc_digest_addbuf(cipher_hash_data->hard_chn, make_ulong(cipher_hash_data->data_phy,
        cipher_hash_data->data_phy_high), cipher_hash_data->data_len, spacc_ctrl);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("spacc add in buf failed, ret = 0x%x.\n", ret);
        return TD_FAILURE;
    }

    channel->data_size = cipher_hash_data->data_len;
    channel->digest_done = TD_FALSE;

    /* Start working */
    ret = spacc_digest_start(cipher_hash_data->hard_chn, spacc_ctrl, cipher_hash_data->sha_val);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("spacc add in buf failed, ret = 0x%x.\n", ret);
        return TD_FAILURE;
    }

    /* Waiting hardware computing finished */
    ret = drv_cipher_digest_wait_done(channel);
    if (ret == TD_SUCCESS) /* Read hash result */
        spacc_digest_get(cipher_hash_data->hard_chn, cipher_hash_data->sha_val);

    return ret;
}

td_s32 ot_drv_cipher_calc_hash_final(cipher_hash_data_s *cipher_hash_data)
{
    return ot_drv_cipher_calc_hash_update(cipher_hash_data);
}

td_s32 ot_drv_cipher_get_handle_config_ex(cipher_config_ctrl_ex_s *config_ctrl)
{
    spacc_symc_chn_s *channel = TD_NULL;
    td_u32 soft_chn_id;
    td_s32 ret;

    if (config_ctrl == TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return OT_ERR_CIPHER_INVALID_POINT;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("cipher_mutex_lock failed!\n");
        return TD_FAILURE;
    }

    ret = spacc_check_handle(config_ctrl->ci_handle);
    if (ret != TD_SUCCESS)
        return ret;
    soft_chn_id = td_handle_get_chnid(config_ctrl->ci_handle);
    channel = &g_symc_chn[soft_chn_id];

    (td_void)memcpy_s(config_ctrl, sizeof(cipher_config_ctrl_ex_s), &channel->ctrl_ex, sizeof(channel->ctrl_ex));
    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}

#ifdef CIPHER_KLAD_SUPPORT
td_s32 ot_drv_cipher_klad_encrypt_key(cipher_klad_key_s *klad_key)
{
    td_s32 ret;

    if (klad_key == TD_NULL) {
        ot_err_cipher("Invalid params!\n");
        return TD_FAILURE;
    }

    if (cipher_mutex_lock(&g_symc_mutex)) {
        ot_err_cipher("down_interruptible failed!\n");
        return TD_FAILURE;
    }

    ret = drv_cipher_klad_encrypt_key(klad_key->root_key,
        klad_key->klad_target, klad_key->clean_key, klad_key->encrypt_key);
    if (ret != TD_SUCCESS) {
        ot_err_cipher("KladEncryptKey failed!\n");
        cipher_mutex_unlock(&g_symc_mutex);
        return ret;
    }

    cipher_mutex_unlock(&g_symc_mutex);

    return ret;
}
#endif
