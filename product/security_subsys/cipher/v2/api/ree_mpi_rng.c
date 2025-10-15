// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "cipher_osal.h"

td_s32 mpi_cipher_get_random_number(td_u32 *random_number, td_u32 time_out_us)
{
    td_s32 ret;
    cipher_rng_s ci_rng;

    chk_ptr_null_return(random_number);

    ci_rng.ci_rng = 0;
    ci_rng.time_out_us = time_out_us;

    ret = cipher_ioctl(g_cipher_dev_fd, CMD_CIPHER_GETRANDOMNUMBER, &ci_rng);
    if (ret == TD_SUCCESS)
        *random_number = ci_rng.ci_rng;

    return ret;
}

td_s32 ot_mpi_cipher_get_random_num(td_u32 *random_num)
{
    chk_dev_open_fail_return();

    return mpi_cipher_get_random_number(random_num, 0);
}

