// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "ot_mpi_otp.h"

#include <common.h>
#include "securec.h"
#include "ot_type.h"
#include "drv_ioctl_otp.h"
#include "ot_debug_otp.h"
#include "drv_otp.h"
#include "drv_lib.h"

static td_bool g_otp_init_flag = TD_FALSE;


#define LOG printf

#define crypto_chk_return(cond, err_ret, fmt, ...) do {     \
    if (cond) {                                             \
        LOG(fmt, ##__VA_ARGS__);                            \
        return err_ret;                                     \
    }                                                       \
} while (0)

#define crypto_otp_not_init_return() do {                   \
    if (g_otp_init_flag != TD_TRUE) {                       \
        LOG("Error: otp not init\n");                       \
            return OT_ERR_OTP_NOT_INIT;                     \
        }                                                   \
} while (0)

td_s32 ot_mpi_otp_init(td_void)
{
    td_s32 ret = TD_SUCCESS;
    if (g_otp_init_flag == TD_TRUE) {
        return TD_SUCCESS;
    }
    (td_void)otp_get_cpu_secure_sta();
    ret = drv_otp_init();
    crypto_chk_return(ret != TD_SUCCESS, ret, "drv_otp_init failed, ret is 0x%x\n", ret);

    g_otp_init_flag = TD_TRUE;
    return ret;
}

td_s32 ot_mpi_otp_deinit(td_void)
{
    if (g_otp_init_flag == TD_FALSE) {
        return TD_FAILURE;
    }
    drv_otp_deinit();
    g_otp_init_flag = TD_FALSE;
    return TD_SUCCESS;
}

td_s32 ot_mpi_otp_set_user_data(const td_char *field_name,
    td_u32 offset, const td_u8 *value, td_u32 value_len)
{
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(field_name == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(
        strlen(field_name) >= OT_OTP_PV_NAME_MAX_LEN, OT_ERR_OTP_INVALID_FIELD_NAME);
    ot_otp_formula_fail_return(value == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(value_len == 0, OT_ERR_OTP_INVALID_PARAM);

    return drv_otp_set_user_data(field_name, offset, value, value_len);
}

td_s32 ot_mpi_otp_get_user_data(const td_char *field_name,
    td_u32 offset, td_u8 *value, td_u32 value_len)
{
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(field_name == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(
        strlen(field_name) >= OT_OTP_PV_NAME_MAX_LEN, OT_ERR_OTP_INVALID_FIELD_NAME);
    ot_otp_formula_fail_return(value == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(value_len == 0, OT_ERR_OTP_INVALID_PARAM);

    return drv_otp_get_user_data(field_name, offset, value, value_len);
}

td_s32 ot_mpi_otp_set_user_data_lock(const td_char *field_name,
    td_u32 offset, td_u32 value_len)
{
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(field_name == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(
        strlen(field_name) >= OT_OTP_PV_NAME_MAX_LEN, OT_ERR_OTP_INVALID_FIELD_NAME);
    ot_otp_formula_fail_return(value_len == 0, OT_ERR_OTP_INVALID_PARAM);

    return drv_otp_set_user_data_lock(field_name, offset, value_len);
}

td_s32 ot_mpi_otp_get_user_data_lock(const td_char *field_name,
    td_u32 offset, td_u32 value_len, ot_otp_lock_status *lock)
{
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(field_name == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(
        strlen(field_name) >= OT_OTP_PV_NAME_MAX_LEN, OT_ERR_OTP_INVALID_FIELD_NAME);
    ot_otp_formula_fail_return(value_len == 0, OT_ERR_OTP_INVALID_PARAM);
    ot_otp_formula_fail_return(lock == TD_NULL, OT_ERR_OTP_NULL_PTR);

    return drv_otp_get_user_data_lock(field_name, offset, value_len, lock);
}

td_s32 ot_mpi_otp_burn_product_pv(const ot_otp_burn_pv_item *pv, td_u32 num)
{
    td_s32 i;
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(pv == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return((num == 0) || (num > OTP_PRODUCT_PV_MAX_NUM), OT_ERR_OTP_INVALID_PARAM);

    for (i = 0; i < num; i++) {
        ot_otp_formula_fail_return(pv[i].burn != TD_TRUE, OT_ERR_OTP_INVALID_PARAM);
        /* burn pv value_len couldn't be 0 */
        ot_otp_formula_fail_return(pv[i].value_len == 0, OT_ERR_OTP_INVALID_PARAM);
        ot_otp_formula_fail_return(
            pv[i].value_len > (OT_OTP_PV_VALUE_MAX_LEN * BYTE_BIT_WIDTH), OT_ERR_OTP_INVALID_PARAM);
        ot_otp_formula_fail_return(pv[i].lock != TD_TRUE && pv[i].lock != TD_FALSE, OT_ERR_OTP_INVALID_PARAM);
    }

    return drv_otp_burn_product_pv(pv, num);
}

td_s32 ot_mpi_otp_read_product_pv(ot_otp_burn_pv_item *pv, td_u32 num)
{
    td_s32 i;

    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(pv == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return((num == 0) || (num > OTP_PRODUCT_PV_MAX_NUM), OT_ERR_OTP_INVALID_PARAM);

    for (i = 0; i < num; i++) {
        ot_otp_formula_fail_return(pv[i].burn == TD_TRUE, OT_ERR_OTP_INVALID_PARAM);
        /* read pv value_len could be 0 */
        ot_otp_formula_fail_return(
            pv[i].value_len > (OT_OTP_PV_VALUE_MAX_LEN * BYTE_BIT_WIDTH), OT_ERR_OTP_INVALID_PARAM);
    }

    return drv_otp_read_product_pv(pv, num);
}

td_s32 ot_mpi_otp_get_key_verify_status(const td_char *key_name, td_bool *status)
{
    crypto_otp_not_init_return();

    ot_otp_formula_fail_return(key_name == TD_NULL, OT_ERR_OTP_NULL_PTR);
    ot_otp_formula_fail_return(
        strlen(key_name) >= OT_OTP_PV_NAME_MAX_LEN, OT_ERR_OTP_INVALID_FIELD_NAME);
    ot_otp_formula_fail_return(status == TD_NULL, OT_ERR_OTP_NULL_PTR);

    return drv_otp_get_key_verify_status(key_name, status);
}