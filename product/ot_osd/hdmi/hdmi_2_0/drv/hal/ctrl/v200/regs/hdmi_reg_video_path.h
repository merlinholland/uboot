// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef __HDMI_REG_VIDEO_PATH_H__
#define __HDMI_REG_VIDEO_PATH_H__

#include "ot_type.h"

typedef union {
    struct {
        td_u32 dither_rnd_byp      : 1;  /* [0] */
        td_u32 dither_mode         : 2;  /* [2:1] */
        td_u32 dither_rnd_en       : 1;  /* [3] */
        td_u32 dither_spatial_en   : 1;  /* [4] */
        td_u32 dither_spatial_dual : 1;  /* [5] */
        td_u32 rsv_31              : 26; /* [31:6] */
    } bits;
    td_u32 u32;
} dither_config;

typedef union {
    struct {
        td_u32 reg_csc_en          : 1;  /* [0] */
        td_u32 reg_csc_coef_ext    : 1;  /* [1] */
        td_u32 reg_csc_dither_en   : 1;  /* [2] */
        td_u32 reg_csc_saturate_en : 1;  /* [3] */
        td_u32 reg_csc_mode        : 8;  /* [11:4] */
        td_u32 rsv_41              : 20; /* [31:12] */
    } bits;
    td_u32 u32;
} multi_csc_ctrl;

typedef union {
    struct {
        td_u32 reg_dwsm_hori_en   : 1;  /* [0] */
        td_u32 reg_hori_filter_en : 1;  /* [1] */
        td_u32 reg_dwsm_vert_en   : 1;  /* [2] */
        td_u32 reg_vert_cbcr_sel  : 1;  /* [3] */
        td_u32 rsv_57             : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} video_dwsm_ctrl;

typedef union {
    struct {
        td_u32 reg_demux_420_en     : 1;  /* [0] */
        td_u32 reg_ddr_en           : 1;  /* [1] */
        td_u32 reg_yc_mux_en        : 1;  /* [2] */
        td_u32 reg_blank_replace_en : 1;  /* [3] */
        td_u32 reg_pixel_rate       : 2;  /* [5:4] */
        td_u32 reg_ddr_polarity     : 1;  /* [6] */
        td_u32 reg_yc_mux_polarity  : 1;  /* [7] */
        td_u32 reg_cbcr_order       : 1;  /* [8] */
        td_u32 reg_demux_cb_or_cr   : 1;  /* [9] */
        td_u32 reg_pxl_div_en       : 1;  /* [10] */
        td_u32 rsv_58               : 21; /* [31:11] */
    } bits;
    td_u32 u32;
} data_align_ctrl;

typedef union {
    struct {
        td_u32 reg_vmux_y_sel  : 3; /* [2:0] */
        td_u32 reg_vmux_cb_sel : 3; /* [5:3] */
        td_u32 reg_vmux_cr_sel : 3; /* [8:6] */
        td_u32 reg_bitmask_y   : 2; /* [10:9] */
        td_u32 reg_bitmask_cb  : 2; /* [12:11] */
        td_u32 reg_bitmask_cr  : 2; /* [14:13] */
        td_u32 reg_bitrev_en   : 3; /* [17:15] */
        td_u32 reg_datamask_en : 3; /* [20:18] */
        td_u32 reg_syncmask_en : 4; /* [24:21] */
        td_u32 reg_inver_sync  : 4; /* [28:25] */
        td_u32 rsv_62          : 3; /* [31:29] */
    } bits;
    td_u32 u32;
} video_dmux_ctrl;

typedef union {
    struct {
        td_u32 tmds_pack_mode       : 2;  /* [1:0] */
        td_u32 reg_fifo_auto_rst_en : 1;  /* [2] */
        td_u32 reg_fifo_manu_rst    : 1;  /* [3] */
        td_u32 reg_clock_det_en     : 1;  /* [4] */
        td_u32 reg_ext_tmds_para    : 1;  /* [5] */
        td_u32 reg_tmds_delay_sel   : 1;  /* [6] */
        td_u32 rsv_0                : 1;  /* [7] */
        td_u32 reg_fifo_delay_cnt   : 8;  /* [15..8] */
        td_u32 rsv_63               : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} tx_pack_fifo_ctrl;

typedef union {
    struct {
        td_u32 pclk2tclk_stable  : 1;  /* [0] */
        td_u32 tmds_fifo_rerror  : 1;  /* [1] */
        td_u32 tmds_fifo_raempty : 1;  /* [2] */
        td_u32 tmds_fifo_rempty  : 1;  /* [3] */
        td_u32 tmds_fifo_werror  : 1;  /* [4] */
        td_u32 tmds_fifo_wafull  : 1;  /* [5] */
        td_u32 tmds_fifo_wfull   : 1;  /* [6] */
        td_u32 rsv_64            : 25; /* [31:7] */
    } bits;
    td_u32 u32;
} tx_pack_fifo_st;

typedef union {
    struct {
        td_u32 reg_tcnt_lower_threshold : 18; /* [17:0] */
        td_u32 rsv_66                   : 14; /* [31:18] */
    } bits;
    td_u32 u32;
} tclk_lower_threshold;

typedef union {
    struct {
        td_u32 reg_tcnt_upper_threshold : 18; /* [17:0] */
        td_u32 rsv_66                   : 14; /* [31:18] */
    } bits;
    td_u32 u32;
} tclk_upper_threshold;

typedef struct {
    volatile dither_config        dither;          /* 8EC */
    td_u32                        reserved_1[17];  /* 8F0-930 */
    volatile multi_csc_ctrl       multi_csc;       /* 934 */
    td_u32                        reserved_2[18];  /* 938-97c */
    volatile video_dwsm_ctrl      dwsm_ctrl;       /* 980 */
    td_u32                        reserved_3[2];   /* 984-988 */
    volatile data_align_ctrl      data_align;      /* 98C */
    td_u32                        reserved_4[6];   /* 990-9A4 */
    volatile video_dmux_ctrl      dmux_ctrl;       /* 9A8 */
    volatile video_dmux_ctrl      reserved_5[21];  /* 9AC-9FC */
    volatile tx_pack_fifo_ctrl    tx_fifo_ctrl;    /* A00 */
    volatile tx_pack_fifo_st      tx_fifo_st;      /* A04 */
    td_u32                        reserved_6;      /* A08 */
    volatile tclk_lower_threshold lower_threshold; /* A0c */
    volatile tclk_upper_threshold upper_threshold; /* A10 */
} video_path_reg_type;

td_s32 hdmi_tx_videopath_reg_type_init(td_void);
td_void hdmi_tx_videopath_reg_type_deinit(td_void);
td_s32 hdmi_dither_config_rnd_byp_set(td_u32 dither_rnd_byp);
td_s32 hdmi_dither_config_mode_set(td_u32 dither_mode);
td_s32 hdmi_multi_csc_ctrl_reg_csc_en_set(td_u32 reg_csc_en);
td_s32 hdmi_multi_csc_ctrl_reg_csc_saturate_en_set(td_u32 reg_csc_saturate_en);
td_s32 hdmi_multi_csc_ctrl_reg_csc_mode_set(td_u32 reg_csc_mode);
td_s32 hdmi_multi_csc_ctrl_reg_csc_mode_get(td_void);
td_s32 hdmi_video_dwsm_ctrl_reg_dwsm_hori_en_set(td_u32 reg_dwsm_hori_en);
td_s32 hdmi_video_dwsm_ctrl_reg_hori_filter_en_set(td_u32 reg_hori_filter_en);
td_s32 hdmi_video_dwsm_ctrl_reg_dwsm_vert_en_set(td_u32 reg_dwsm_vert_en);
td_s32 hdmi_data_align_ctrl_reg_demux_420_en_set(td_u32 reg_demux_420_en);
td_s32 hdmi_data_align_ctrl_reg_vert_cbcr_sel_set(td_u32 reg_vert_cbcr_sel);
td_s32 hdmi_data_align_ctrl_reg_pxl_div_en_set(td_u32 reg_pxl_div_en);
td_s32 hdmi_video_dmux_ctrl_reg_vmux_y_sel_set(td_u32 reg_vmux_y_sel);
td_s32 hdmi_video_dmux_ctrl_reg_vmux_cb_sel_set(td_u32 reg_vmux_cb_sel);
td_s32 hdmi_video_dmux_ctrl_reg_vmux_cr_sel_set(td_u32 reg_vmux_cr_sel);
td_s32 hdmi_video_dmux_ctrl_reg_syncmask_en_set(td_u32 reg_syncmask_en);
td_s32 hdmi_video_dmux_ctrl_reg_inver_sync_set(td_u32 reg_inver_sync);
td_s32 hdmi_tx_pack_fifo_ctrl_reg_fifo_manu_rst_set(td_u32 reg_fifo_manu_rst);
td_s32 hdmi_tx_pack_fifo_ctrl_tmds_pack_mode_set(td_u32 tmds_pack_mode);
td_s32 hdmi_tx_pack_fifo_ctrl_tmds_pack_mode_get(td_void);
td_s32 hdmi_tx_pack_fifo_ctrl_reg_fifo_auto_rst_en_set(td_u32 reg_fifo_auto_rst_en);

#endif /* __HDMI_REG_VIDEO_PATH_H__ */

