// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "spacc_intf.h"
#include "drv_cipher_ioctl.h"
#include "cipher_adapt.h"
#include "ot_drv_compat.h"
#include "drv_rsa.h"
#include "drv_rng.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

td_s32 drv_cipher_ioctl(td_u32 cmd, td_void *argp, td_void *private_data)
{
    td_s32 ret;

    if (argp == TD_NULL) {
        ot_err_cipher("Error, argp is NULL!\n");
        return TD_FAILURE;
    }

    switch (cmd) {
        case CMD_CIPHER_CREATEHANDLE: {
            cipher_handle_s *handle = (cipher_handle_s *)argp;
            ret = ot_drv_cipher_create_handle(handle, private_data);
            break;
        }
        case CMD_CIPHER_DESTROYHANDLE: {
            td_handle *handle = (td_handle *)argp;
            ret = ot_drv_cipher_destory_handle(*handle);
            break;
        }
        case CMD_CIPHER_CONFIGHANDLE_EX: {
            cipher_config_ctrl_ex_s *config_ex = (cipher_config_ctrl_ex_s *)argp;
            ret = ot_drv_cipher_config_chn_ex(config_ex->ci_handle, config_ex);
            break;
        }
        case CMD_CIPHER_ENCRYPT: {
            cipher_data_s *data = (cipher_data_s *)argp;
            ret = ot_drv_cipher_encrypt(data);
            break;
        }
        case CMD_CIPHER_DECRYPT: {
            cipher_data_s *data = (cipher_data_s *)argp;
            ret = ot_drv_cipher_decrypt(data);
            break;
        }
        case CMD_CIPHER_ENCRYPTMULTI: {
            cipher_pkg_s *pkg = (cipher_pkg_s *)argp;
            ret = ot_drv_cipher_encrypt_multi(pkg);
            break;
        }
        case CMD_CIPHER_DECRYPTMULTI: {
            cipher_pkg_s *pkg = (cipher_pkg_s *)argp;
            ret = ot_drv_cipher_decrypt_multi(pkg);
            break;
        }
#ifdef CIPHER_KLAD_SUPPORT
        case CMD_CIPHER_KLAD_KEY: {
            cipher_klad_key_s *klad_data = (cipher_klad_key_s *)argp;
            ret = ot_drv_cipher_klad_encrypt_key(klad_data);
            break;
        }
#endif
        case CMD_CIPHER_GETTAG: {
            cipher_tag_s *tag = (cipher_tag_s *)argp;
            ret = ot_drv_cipher_get_tag(tag);
            break;
        }
        case CMD_CIPHER_GETRANDOMNUMBER: {
            cipher_rng_s *rng  = (cipher_rng_s *)argp;
            ret = ot_drv_cipher_get_random_number(rng);
            break;
        }
        case CMD_CIPHER_GETHANDLECONFIG_EX:  {
            cipher_config_ctrl_ex_s *data = (cipher_config_ctrl_ex_s *)argp;
            ret = ot_drv_cipher_get_handle_config_ex(data);
            break;
        }
        case CMD_CIPHER_CALCHASH_INIT: {
            cipher_hash_data_s *hash_data = (cipher_hash_data_s*)argp;
            ret = ot_drv_cipher_calc_hash_init(hash_data);
            break;
        }
        case CMD_CIPHER_CALCHASHUPDATE: {
            cipher_hash_data_s *hash_data = (cipher_hash_data_s*)argp;
            ret = ot_drv_cipher_calc_hash_update(hash_data);
            break;
        }
        case CMD_CIPHER_CALCHASHFINAL: {
            cipher_hash_data_s *hash_data = (cipher_hash_data_s*)argp;
            ret = ot_drv_cipher_calc_hash_final(hash_data);
            break;
        }
        case CMD_CIPHER_CALCRSA: {
            cipher_rsa_data_s *rsa_data = (cipher_rsa_data_s*)argp;
            ret = ot_drv_cipher_calc_rsa(rsa_data);
            break;
        }
#ifdef CONFIG_COMPAT
#ifdef CONFIG_RSA_HARDWARE_SUPPORT
        case CMD_CIPHER_COMPAT_CALCRSA: {
            cipher_compat_rsa_data_s *compat_rsa_data = (cipher_compat_rsa_data_s*)argp;
            cipher_rsa_data_s rsa_data;

            rsa_data.input_data = u32_to_point(compat_rsa_data->input_via);
            rsa_data.output_data = u32_to_point(compat_rsa_data->output_via);
            rsa_data.rsa_k = u32_to_point(compat_rsa_data->rsa_k_via);
            rsa_data.rsa_n = u32_to_point(compat_rsa_data->rsa_n_via);
            rsa_data.rsa_k_len = compat_rsa_data->rsa_k_len;
            rsa_data.rsa_n_len = compat_rsa_data->rsa_n_len;
            rsa_data.data_len = compat_rsa_data->data_len;

            ret = ot_drv_cipher_calc_rsa(&rsa_data);
            break;
        }
#endif
        case CMD_CIPHER_COMPAT_ENCRYPTMULTI: {
            cipher_compat_pkg_s *compat_pkg = (cipher_compat_pkg_s *)argp;
            cipher_pkg_s pkg;

            pkg.ci_handle = compat_pkg->ci_handle;
            pkg.pkg_num = compat_pkg->pkg_num;
            pkg.cipher_data = u32_to_point(compat_pkg->pkg_via);

            ret = ot_drv_cipher_encrypt_multi(&pkg);
            break;
        }
        case CMD_CIPHER_COMPAT_DECRYPTMULTI: {
            cipher_compat_pkg_s *compat_pkg = (cipher_compat_pkg_s *)argp;
            cipher_pkg_s pkg;

            pkg.ci_handle = compat_pkg->ci_handle;
            pkg.pkg_num = compat_pkg->pkg_num;
            pkg.cipher_data = u32_to_point(compat_pkg->pkg_via);

            ret = ot_drv_cipher_decrypt_multi(&pkg);
            break;
        }
#endif
        default:
            ot_err_cipher("Unsupported cmd, MOD_ID=0x%02X, NR=0x%02x, SIZE=0x%02x!\n",
                ree_cipher_ioc_type(cmd), ree_cipher_ioc_nr(cmd), ree_cipher_ioc_size(cmd));
            ret = TD_FAILURE;
            break;
    }

    return ret;
}

td_s32 cipher_module_init(td_void)
{
    td_s32 ret;

    ret = drv_cipher_init();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = drv_rng_init();
    if (ret != TD_SUCCESS) {
        (td_void)drv_cipher_deinit();
        return ret;
    }

#ifdef CIPHER_KLAD_SUPPORT
    ret = ot_drv_compat_init();
    if (ret != TD_SUCCESS) {
        (td_void)drv_cipher_deinit();
        (td_void)drv_rng_deinit();
        return ret;
    }
#endif

    ret = drv_rsa_init();
    if (ret != TD_SUCCESS) {
        (td_void)drv_cipher_deinit();
        (td_void)drv_rng_deinit();
#ifdef CIPHER_KLAD_SUPPORT
        (td_void)ot_drv_compat_deinit();
#endif
        return ret;
    }

    return TD_SUCCESS;
}

td_void cipher_module_exit(td_void)
{
    (td_void)drv_cipher_deinit();
    (td_void)drv_rng_deinit();
    (td_void)drv_rsa_deinit();

#ifdef CIPHER_KLAD_SUPPORT
    (td_void)ot_drv_compat_deinit();
#endif
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
