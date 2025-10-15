// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "cipher_osal.h"

#define CIPHER_MIN_CRYPT_LEN 8
#define CIPHER_MAX_CRYPT_LEN 0xfffff

#define ot_cipher_lock()
#define ot_cipher_unlock()
#define ot_hash_lock()
#define ot_hash_unlock()

td_s32 g_cipher_dev_fd = -1;
td_s32 g_cipher_init_counter = -1;
extern hash_info_s g_cipher_hash_data[HASH_CHANNAL_MAX_NUM];

td_s32 ot_mpi_cipher_init(td_void)
{
    ot_cipher_lock();

    if (g_cipher_init_counter > 0) {
        g_cipher_init_counter++;
        ot_cipher_unlock();
        return TD_SUCCESS;
    }

    g_cipher_dev_fd = cipher_open("/dev/" UMAP_DEVNAME_CIPHER, O_RDWR, 0);
    if (g_cipher_dev_fd < 0) {
        ot_err_cipher("Open CIPHER err.\n");
        ot_cipher_unlock();
        return OT_ERR_CIPHER_FAILED_INIT;
    }

    g_cipher_init_counter = 1;
    ot_cipher_unlock();

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_deinit(td_void)
{
    ot_cipher_lock();
    int i = 0;

    chk_dev_open_fail_return();

    if (g_cipher_init_counter > 0)
        g_cipher_init_counter--;

    if (g_cipher_init_counter != 0) {
        ot_cipher_unlock();
        return TD_SUCCESS;
    }
    for (i = 0; i < HASH_CHANNAL_MAX_NUM; i++) {
        g_cipher_hash_data[i].is_used = TD_FALSE;
    }
    cipher_close(g_cipher_dev_fd);

    g_cipher_init_counter = -1;

    ot_cipher_unlock();

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_create_handle(td_handle *handle, const ot_cipher_attr *cipher_attr)
{
    td_s32 ret;
    cipher_handle_s ci_handle;

    chk_dev_open_fail_return();
    chk_ptr_null_return(handle);
    chk_ptr_null_return(cipher_attr);

    (td_void)memset_s(&ci_handle, sizeof(ci_handle), 0, sizeof(ci_handle));
    (td_void)memcpy_s(&ci_handle.cipher_atts, sizeof(ot_cipher_attr), cipher_attr, sizeof(ot_cipher_attr));
    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CREATEHANDLE, &ci_handle);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    *handle = ci_handle.ci_handle;

    return ret;
}

td_s32 ot_mpi_cipher_destroy_handle(td_handle handle)
{
    chk_dev_open_fail_return();
    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_DESTROYHANDLE, &handle);
}

td_s32 ot_mpi_cipher_config_handle(td_handle handle, const ot_cipher_ctrl *cipher_ctrl)
{
    cipher_config_ctrl_ex_s cfg_data;

    chk_dev_open_fail_return();
    chk_ptr_null_return(cipher_ctrl);

    (td_void)memset_s(&cfg_data, sizeof(cipher_config_ctrl_ex_s), 0, sizeof(cipher_config_ctrl_ex_s));
    (td_void)memcpy_s(&cfg_data.key, sizeof(cfg_data.key), cipher_ctrl->key, sizeof(cipher_ctrl->key));
    (td_void)memcpy_s(&cfg_data.iv, sizeof(cfg_data.iv), cipher_ctrl->iv, sizeof(cipher_ctrl->iv));

    cfg_data.key_by_ca = cipher_ctrl->key_by_ca;
    cfg_data.ca_type = cipher_ctrl->ca_type;
    cfg_data.ci_alg = cipher_ctrl->alg;
    cfg_data.bit_width = cipher_ctrl->bit_width;
    cfg_data.work_mode = cipher_ctrl->work_mode;
    cfg_data.key_len = cipher_ctrl->key_len;
    cfg_data.change_flags = cipher_ctrl->chg_flags;
    cfg_data.ci_handle = handle;
    cfg_data.iv_len = sizeof(cipher_ctrl->iv);

    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CONFIGHANDLE_EX, &cfg_data);
}

static td_s32 mpi_cipher_config_aes(const ot_cipher_ctrl_ex *ctrl_ex, cipher_config_ctrl_ex_s *cfg_data)
{
    td_s32 ret;
    if ((ctrl_ex->work_mode == OT_CIPHER_WORK_MODE_CCM) ||
        (ctrl_ex->work_mode == OT_CIPHER_WORK_MODE_GCM)) {
        ot_cipher_ctrl_aes_ccm_gcm *aes_ccm_gcm_ctrl =
            (ot_cipher_ctrl_aes_ccm_gcm *)ctrl_ex->param;

        ret = memcpy_s(&cfg_data->key, sizeof(cfg_data->key),
            aes_ccm_gcm_ctrl->key, sizeof(aes_ccm_gcm_ctrl->key));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = memcpy_s(&cfg_data->iv, sizeof(cfg_data->iv),
            aes_ccm_gcm_ctrl->iv, sizeof(aes_ccm_gcm_ctrl->iv));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        cfg_data->bit_width = OT_CIPHER_BIT_WIDTH_128BIT;
        cfg_data->key_len = aes_ccm_gcm_ctrl->key_len;
        cfg_data->iv_len = aes_ccm_gcm_ctrl->iv_len;
        cfg_data->tag_len = aes_ccm_gcm_ctrl->tag_len;
        cfg_data->aphy_addr = aes_ccm_gcm_ctrl->aad_phys_addr;
        cfg_data->aphy_addr_high = get_ulong_high(aes_ccm_gcm_ctrl->aad_phy_addr);
        cfg_data->alen = aes_ccm_gcm_ctrl->aad_len;
        cfg_data->change_flags.bits_iv = 1;
    } else {
        ot_cipher_ctrl_aes *aes_ctrl = (ot_cipher_ctrl_aes *)ctrl_ex->param;

        ret = memcpy_s(&cfg_data->key, sizeof(cfg_data->key),
            aes_ctrl->even_key, sizeof(aes_ctrl->even_key));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = memcpy_s(&cfg_data->odd_key, sizeof(cfg_data->odd_key),
            aes_ctrl->odd_key, sizeof(aes_ctrl->odd_key));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = memcpy_s(&cfg_data->iv, sizeof(cfg_data->iv),
            aes_ctrl->iv, sizeof(aes_ctrl->iv));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        cfg_data->bit_width = aes_ctrl->bit_width;
        cfg_data->key_len = aes_ctrl->key_len;
        cfg_data->change_flags = aes_ctrl->chg_flags;
        cfg_data->iv_len = sizeof(aes_ctrl->iv);
    }
    return TD_SUCCESS;
}

static td_s32 mpi_cipher_config_sm1(const ot_cipher_ctrl_ex *ctrl_ex, cipher_config_ctrl_ex_s *cfg_data)
{
    td_s32 ret;
    ot_cipher_ctrl_sm1 *sm1_ctrl = (ot_cipher_ctrl_sm1 *)ctrl_ex->param;

    ret = memcpy_s(&cfg_data->key[0], sizeof(cfg_data->key),
        sm1_ctrl->ek, sizeof(sm1_ctrl->ek));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    ret = memcpy_s(&cfg_data->key[4], sizeof(cfg_data->key) - 4 * sizeof(td_u32), /* 4 - sm1 ak index */
        sm1_ctrl->ak, sizeof(sm1_ctrl->ak));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    ret = memcpy_s(&cfg_data->key[8], sizeof(cfg_data->key) - 8 * sizeof(td_u32), /* 8 - sm1 sk index */
        sm1_ctrl->sk, sizeof(sm1_ctrl->sk));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    ret = memcpy_s(&cfg_data->iv, sizeof(cfg_data->iv), sm1_ctrl->iv, sizeof(sm1_ctrl->iv));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    cfg_data->sm1_round = sm1_ctrl->sm1_round;
    cfg_data->key_len = OT_CIPHER_KEY_DEFAULT;
    cfg_data->change_flags = sm1_ctrl->chg_flags;
    cfg_data->bit_width = sm1_ctrl->bit_width;
    cfg_data->iv_len = sizeof(sm1_ctrl->iv);

    return TD_SUCCESS;
}

static td_s32 mpi_cipher_config_sm4(const ot_cipher_ctrl_ex *ctrl_ex, cipher_config_ctrl_ex_s *cfg_data)
{
    td_s32 ret;
    ot_cipher_ctrl_sm4 *sm4_ctrl = (ot_cipher_ctrl_sm4 *)ctrl_ex->param;

    ret = memcpy_s(&cfg_data->key, sizeof(cfg_data->key), sm4_ctrl->key, sizeof(sm4_ctrl->key));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    ret = memcpy_s(&cfg_data->iv, sizeof(cfg_data->iv), sm4_ctrl->iv, sizeof(sm4_ctrl->iv));
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    cfg_data->key_len = OT_CIPHER_KEY_DEFAULT;
    cfg_data->change_flags = sm4_ctrl->chg_flags;
    cfg_data->bit_width = OT_CIPHER_BIT_WIDTH_128BIT;
    cfg_data->iv_len = sizeof(sm4_ctrl->iv);

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_config_handle_ex(td_handle handle, const ot_cipher_ctrl_ex *ctrl_ex)
{
    td_s32 ret;
    cipher_config_ctrl_ex_s cfg_data;

    chk_dev_open_fail_return();
    chk_ptr_null_return(ctrl_ex);

    if ((ctrl_ex->alg != OT_CIPHER_ALG_DMA) && (ctrl_ex->param == NULL)) {
        ot_err_cipher("para ctrl_ex param is invalid.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    (td_void)memset_s(&cfg_data, sizeof(cipher_config_ctrl_ex_s), 0, sizeof(cipher_config_ctrl_ex_s));
    cfg_data.ci_handle = handle;
    cfg_data.ci_alg = ctrl_ex->alg;
    cfg_data.work_mode = ctrl_ex->work_mode;
    cfg_data.key_by_ca = ctrl_ex->key_by_ca;

    /* ****************************************************************************
     * for AES, the pointer should point to ot_cipher_ctrl_aes;
     * for AES_CCM or AES_GCM, the pointer should point to ot_cipher_ctrl_aes_ccm_gcm;
     * for DES, the pointer should point to OT_CIPHER_CTRL_DES_S;
     * for 3DES, the pointer should point to OT_CIPHER_CTRL_3DES_S;
     * for SM1, the pointer should point to ot_cipher_ctrl_sm1;
     * for SM4, the pointer should point to ot_cipher_ctrl_sm4;
     */
    switch (ctrl_ex->alg) {
        case OT_CIPHER_ALG_AES:
            ret = mpi_cipher_config_aes(ctrl_ex, &cfg_data);
            chk_func_fail_return(ret != TD_SUCCESS, ret, mpi_cipher_config_aes);
            break;
        case OT_CIPHER_ALG_SM1:
            ret = mpi_cipher_config_sm1(ctrl_ex, &cfg_data);
            chk_func_fail_return(ret != TD_SUCCESS, ret, mpi_cipher_config_sm1);
            break;
        case OT_CIPHER_ALG_SM4:
            ret = mpi_cipher_config_sm4(ctrl_ex, &cfg_data);
            chk_func_fail_return(ret != TD_SUCCESS, ret, mpi_cipher_config_sm4);
            break;
        case OT_CIPHER_ALG_DMA:
            break;
        default:
            ot_err_cipher("cipher alg is invalid.\n");
            return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_CONFIGHANDLE_EX, &cfg_data);
}

td_s32 ot_mpi_cipher_encrypt(td_handle handle, td_size_t src_phy_addr, td_size_t dest_phy_addr, td_u32 byte_len)
{
    cipher_data_s ci_data;

    chk_dev_open_fail_return();
    ci_data.src_phy_addr = get_ulong_low(src_phy_addr);
    ci_data.src_phy_addr_high = get_ulong_high(src_phy_addr);
    ci_data.dest_phy_addr = get_ulong_low(dest_phy_addr);
    ci_data.dest_phy_addr_high = get_ulong_high(dest_phy_addr);

    ci_data.data_length = byte_len;
    ci_data.ci_handle = handle;

    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_ENCRYPT, &ci_data);
}

td_s32 ot_mpi_cipher_decrypt(td_handle handle, td_size_t src_phy_addr, td_size_t dest_phy_addr, td_u32 byte_len)
{
    cipher_data_s ci_data;

    chk_dev_open_fail_return();

    ci_data.src_phy_addr = get_ulong_low(src_phy_addr);
    ci_data.src_phy_addr_high = get_ulong_high(src_phy_addr);
    ci_data.dest_phy_addr = get_ulong_low(dest_phy_addr);
    ci_data.dest_phy_addr_high = get_ulong_high(dest_phy_addr);
    ci_data.data_length = byte_len;
    ci_data.ci_handle = handle;

    return cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_DECRYPT, &ci_data);
}

td_s32 ot_mpi_cipher_encrypt_multi_pack(td_handle handle, const ot_cipher_data *data_pkg, td_u32 data_pkg_num)
{
    cipher_pkg_s ci_pkg;
    ot_cipher_data *ci_data = NULL;
    td_u32 len;
    td_s32 ret;

    chk_dev_open_fail_return();
    chk_ptr_null_return(data_pkg);
    chk_formula_fail_return((data_pkg_num == 0) || (data_pkg_num > MAX_MULTI_PKG_NUM));

    len = sizeof(ot_cipher_data) * data_pkg_num;
    ci_data = cipher_malloc(len);
    if (ci_data == TD_NULL) {
        ot_err_cipher("Error, malloc pkg buffer failed\n");
        return TD_FAILURE;
    }
    (td_void)memcpy_s(ci_data, len, data_pkg, len);

    ci_pkg.ci_handle = handle;
    ci_pkg.cipher_data = ci_data;
    ci_pkg.pkg_num = data_pkg_num;
    ci_pkg.user_bit_width = sizeof(td_size_t);
    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_ENCRYPTMULTI, &ci_pkg);

    cipher_free(ci_data);
    return ret;
}

td_s32 ot_mpi_cipher_decrypt_multi_pack(td_handle handle, const ot_cipher_data *data_pkg, td_u32 data_pkg_num)
{
    cipher_pkg_s ci_pkg;
    ot_cipher_data *ci_data = NULL;
    td_u32 len;
    td_s32 ret;

    chk_dev_open_fail_return();
    chk_ptr_null_return(data_pkg);
    chk_formula_fail_return((data_pkg_num == 0) || (data_pkg_num > MAX_MULTI_PKG_NUM));

    len = sizeof(ot_cipher_data) * data_pkg_num;
    ci_data = cipher_malloc(len);
    if (ci_data == TD_NULL) {
        ot_err_cipher("Error, malloc pkg buffer failed\n");
        return TD_FAILURE;
    }
    (td_void)memcpy_s(ci_data, len, data_pkg, len);

    ci_pkg.ci_handle = handle;
    ci_pkg.cipher_data = ci_data;
    ci_pkg.pkg_num = data_pkg_num;
    ci_pkg.user_bit_width = sizeof(td_size_t);

    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_DECRYPTMULTI, &ci_pkg);

    cipher_free(ci_data);
    ci_data = TD_NULL;

    return ret;
}

td_s32 ot_mpi_cipher_get_handle_cfg(td_handle handle, ot_cipher_ctrl *cipher_ctrl)
{
    td_s32 ret;
    cipher_config_ctrl_ex_s cfg_data;

    chk_dev_open_fail_return();
    chk_ptr_null_return(cipher_ctrl);

    (td_void)memset_s(&cfg_data, sizeof(cipher_config_ctrl_ex_s), 0, sizeof(cipher_config_ctrl_ex_s));
    cfg_data.ci_handle = handle;
    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_GETHANDLECONFIG_EX, &cfg_data);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    (td_void)memcpy_s(cipher_ctrl->key, sizeof(cipher_ctrl->key), &cfg_data.key, sizeof(cipher_ctrl->key));
    (td_void)memcpy_s(cipher_ctrl->iv, sizeof(cipher_ctrl->iv), &cfg_data.iv, sizeof(cfg_data.iv));

    cipher_ctrl->key_by_ca = cfg_data.key_by_ca;
    cipher_ctrl->ca_type = cfg_data.ca_type;
    cipher_ctrl->alg = cfg_data.ci_alg;
    cipher_ctrl->bit_width = cfg_data.bit_width;
    cipher_ctrl->work_mode = cfg_data.work_mode;
    cipher_ctrl->key_len = cfg_data.key_len;
    cipher_ctrl->chg_flags = cfg_data.change_flags;

    return TD_SUCCESS;
}

#ifdef CIPHER_KLAD_SUPPORT
td_s32 ot_mpi_cipher_klad_encrypt_key(ot_cipher_ca_type root_key,
                                      ot_cipher_klad_target klad_target,
                                      const td_u8 *clean_key,
                                      td_u8 *enc_key,
                                      td_u32 key_len)
{
    td_s32 ret;
    cipher_klad_key_s ci_klad;
    td_u32 i;

    chk_dev_open_fail_return();
    (td_void)memset_s(&ci_klad, sizeof(cipher_klad_key_s), 0, sizeof(cipher_klad_key_s));

    if ((clean_key == TD_NULL) || (enc_key == TD_NULL)) {
        ot_err_cipher("invalid para.\n");
        return TD_FAILURE;
    }

    if ((key_len == 0) || (key_len % 16 != 0)) { /* key_len must multiples of 16 */
        ot_err_cipher("invalid key len 0x%x.\n", key_len);
        return TD_FAILURE;
    }

    if ((root_key >= OT_CIPHER_KEY_SRC_BUTT) || (klad_target >= OT_CIPHER_KLAD_TARGET_BUTT)) {
        ot_err_cipher("invalid para.\n");
        return TD_FAILURE;
    }

    ci_klad.root_key = root_key;
    ci_klad.klad_target = klad_target;

    for (i = 0; i < key_len / 16; i++) { /* 16 groups */
        ret = memcpy_s(ci_klad.clean_key, sizeof(ci_klad.clean_key), clean_key + i * 16, 16); /* 16 groups */
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

        ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_KLAD_KEY, &ci_klad);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ret = memcpy_s(enc_key + i * 16, key_len - i * 16,  /* 16 groups */
            ci_klad.encrypt_key, sizeof(ci_klad.encrypt_key));
        chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);
    }

    return ret;
}
#endif

td_s32 ot_mpi_cipher_get_tag(td_handle handle, td_u8 *tag, td_u32 *tag_len)
{
    cipher_tag_s ci_tag;
    td_s32 ret;

    chk_dev_open_fail_return();
    chk_ptr_null_return(tag);
    chk_ptr_null_return(tag_len);

    (td_void)memset_s(&ci_tag, sizeof(cipher_tag_s), 0, sizeof(cipher_tag_s));
    ci_tag.ci_handle = handle;
    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_GETTAG, &ci_tag);
    chk_func_fail_return(ret != TD_SUCCESS, ret, cipher_ioctl);

    chk_formula_fail_return(ci_tag.tag_len > *tag_len);

    ret = memcpy_s(tag, *tag_len, ci_tag.tag, ci_tag.tag_len);
    chk_func_fail_return(ret != EOK, OT_ERR_CIPHER_FAILED_SEC_FUNC, memcpy_s);

    *tag_len = ci_tag.tag_len;

    return ret;
}
