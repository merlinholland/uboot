// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef DRV_CIPHER_IOCTL_H
#define DRV_CIPHER_IOCTL_H

#include "ot_type.h"
#include "ot_mpi_cipher.h"

td_s32 cipher_module_init(td_void);
td_void cipher_module_exit(td_void);
td_s32 drv_cipher_ioctl(td_u32 cmd, td_void *argp, td_void *private_data);

typedef struct {
    td_u32 chn_id;
    td_char *open_status;
    td_char *alg;
    td_char *mode;
    td_u32 key_len;
    td_char *key_from;
    td_bool is_decrypt;
    td_u32 data_in_size;
    td_u32 data_in_addr;
    td_u32 data_out_size;
    td_u32 data_out_addr;
    td_bool in_int_all_en;
    td_bool in_int_en;
    td_bool in_int_raw;
    td_bool out_int_en;
    td_bool out_int_raw;
    td_u32 out_int_count;       /* CHANn_INT_OCNTCFG */
    char iv_string[33];         /* 33 iv string size */
} cipher_chn_status_s;

#define REE_CIPHER_IOC_NA   0U
#define REE_CIPHER_IOC_W    1U
#define REE_CIPHER_IOC_R    2U
#define REE_CIPHER_IOC_RW   3U

#define ree_cipher_ioc(dir, type, nr, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | ((nr) << 0))

#define ree_cipher_ior(type,  nr, size) ree_cipher_ioc(REE_CIPHER_IOC_R,  (type), (nr), sizeof(size))
#define ree_cipher_iow(type,  nr, size) ree_cipher_ioc(REE_CIPHER_IOC_W,  (type), (nr), sizeof(size))
#define ree_cipher_iowr(type, nr, size) ree_cipher_ioc(REE_CIPHER_IOC_RW, (type), (nr), sizeof(size))

#define ree_cipher_ioc_dir(nr)  (((nr) >> 30) & 0x03)
#define ree_cipher_ioc_type(nr) (((nr) >> 8)  & 0xFF)
#define ree_cipher_ioc_nr(nr)   (((nr) >> 0)  & 0xFF)
#define ree_cipher_ioc_size(nr) (((nr) >> 16) & 0x3FFF)

#define OT_ID_CIPHER 100

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#define CMD_CIPHER_CREATEHANDLE          ree_cipher_iowr(OT_ID_CIPHER, 0x1,  cipher_handle_s)
#define CMD_CIPHER_DESTROYHANDLE         ree_cipher_iow(OT_ID_CIPHER,  0x2,  td_u32)
#define CMD_CIPHER_CONFIGHANDLE          ree_cipher_iow(OT_ID_CIPHER,  0x3,  cipher_config_ctrl_s)
#define CMD_CIPHER_ENCRYPT               ree_cipher_iow(OT_ID_CIPHER,  0x4,  cipher_data_s)
#define CMD_CIPHER_DECRYPT               ree_cipher_iow(OT_ID_CIPHER,  0x5,  cipher_data_s)
#define CMD_CIPHER_DECRYPTMULTI          ree_cipher_iow(OT_ID_CIPHER,  0x6,  cipher_pkg_s)
#define CMD_CIPHER_ENCRYPTMULTI          ree_cipher_iow(OT_ID_CIPHER,  0x7,  cipher_pkg_s)
#define CMD_CIPHER_GETRANDOMNUMBER       ree_cipher_iowr(OT_ID_CIPHER, 0x8,  cipher_rng_s)
#define CMD_CIPHER_GETHANDLECONFIG       ree_cipher_iowr(OT_ID_CIPHER, 0x9,  cipher_config_ctrl_s)
#define CMD_CIPHER_CALCHASH_INIT         ree_cipher_iowr(OT_ID_CIPHER, 0xa,  cipher_hash_data_s)
#define CMD_CIPHER_CALCHASHUPDATE        ree_cipher_iowr(OT_ID_CIPHER, 0xb,  cipher_hash_data_s)
#define CMD_CIPHER_CALCHASHFINAL         ree_cipher_iowr(OT_ID_CIPHER, 0xc,  cipher_hash_data_s)
#define CMD_CIPHER_CALCRSA               ree_cipher_iowr(OT_ID_CIPHER, 0x10, cipher_rsa_data_s)
#define CMD_CIPHER_GETTAG                ree_cipher_iowr(OT_ID_CIPHER, 0x11, cipher_tag_s)
#define CMD_CIPHER_CONFIGHANDLE_EX       ree_cipher_iowr(OT_ID_CIPHER, 0x23, cipher_config_ctrl_ex_s)
#define CMD_CIPHER_GETHANDLECONFIG_EX    ree_cipher_iowr(OT_ID_CIPHER, 0x24, cipher_config_ctrl_ex_s)
#define CMD_CIPHER_KLAD_KEY              ree_cipher_iowr(OT_ID_CIPHER, 0x12, cipher_klad_key_s)

#ifdef CONFIG_COMPAT
#define CMD_CIPHER_COMPAT_DECRYPTMULTI   ree_cipher_iow(OT_ID_CIPHER,  0x6,  cipher_compat_pkg_s)
#define CMD_CIPHER_COMPAT_ENCRYPTMULTI   ree_cipher_iow(OT_ID_CIPHER,  0x7,  cipher_compat_pkg_s)
#define CMD_CIPHER_COMPAT_CALCRSA        ree_cipher_iowr(OT_ID_CIPHER, 0x10, cipher_compat_rsa_data_s)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DRV_CIPHER_IOCTL_H */
