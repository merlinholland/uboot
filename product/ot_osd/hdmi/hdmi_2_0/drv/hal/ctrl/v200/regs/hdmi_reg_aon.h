// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_REG_AON_H__
#define __HDMI_REG_AON_H__

#include "ot_type.h"

typedef union {
    struct {
        td_u32 ddc_aon_access : 1;  /* [0] */
        td_u32 dcc_man_en     : 1;  /* [1] */
        td_u32 rsv_11         : 2;  /* [3:2] */
        td_u32 ddc_speed_cnt  : 9;  /* [12:4] */
        td_u32 rsv_12         : 19; /* [31:13] */
    } bits;
    td_u32 u32;
} ddc_mst_ctrl;

typedef union {
    struct {
        td_u32 ddc_i2c_no_ack     : 1;  /* [0] */
        td_u32 ddc_i2c_bus_low    : 1;  /* [1] */
        td_u32 ddc_i2c_in_prog    : 1;  /* [2] */
        td_u32 ddc_fifo_wr_in_use : 1;  /* [3] */
        td_u32 ddc_fifo_rd_in_use : 1;  /* [4] */
        td_u32 ddc_fifo_empty     : 1;  /* [5] */
        td_u32 ddc_fifo_half_full : 1;  /* [6] */
        td_u32 ddc_fifo_full      : 1;  /* [7] */
        td_u32 rsv_18             : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} ddc_mst_state;

typedef union {
    struct {
        td_u32 ddc_scl_st  : 1;  /* [0] */
        td_u32 ddc_sda_st  : 1;  /* [1] */
        td_u32 ddc_scl_oen : 1;  /* [2] */
        td_u32 ddc_sda_oen : 1;  /* [3] */
        td_u32 rsv_20      : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} ddc_man_ctrl;

typedef struct {
    volatile ddc_mst_ctrl  mst_ctrl;      /* 4040 */
    td_u32                 reserved_1[4]; /* 4044-4050 */
    volatile ddc_mst_state mst_state;     /* 4054 */
    td_u32                 reserved_2;    /* 4058 */
    volatile ddc_man_ctrl  man_ctrl;      /* 405C */
    td_u32                 reserved_3;    /* 4060 */
} tx_aon_reg_type;

td_s32 hdmi_tx_aon_reg_type_init(td_void);
td_void hdmi_tx_aon_reg_type_deinit(td_void);
td_s32 hdmi_dcc_man_en_set(td_u32 dcc_man_en);
td_s32 hdmi_ddc_scl_oen_set(td_u32 ddc_scl_oen);
td_s32 hdmi_ddc_sda_oen_set(td_u32 ddc_sda_oen);
td_u32 hdmi_ddc_i2c_bus_low_get(td_void);
td_u32 hdmi_ddc_i2c_no_ack_get(td_void);
td_s32 hdmi_ddc_speed_cnt_set(td_u32 ddc_speed_cnt);
td_u32 hdmi_ddc_scl_st_get(td_void);
td_u32 hdmi_ddc_sda_st_get(td_void);

#endif /* __HDMI_REG_AON_H__ */

