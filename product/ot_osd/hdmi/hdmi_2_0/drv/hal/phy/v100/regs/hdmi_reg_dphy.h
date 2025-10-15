// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef __HDMI_REG_DPHY_H__
#define __HDMI_REG_DPHY_H__

#include "ot_type.h"

typedef union {
    struct {
        td_u32 reg_set               : 24; /* [23:0] */
        td_u32 reg_set_load          : 1;  /* [24] */
        td_u32 reg_sscfifo_tune_en   : 1;  /* [25] */
        td_u32 reg_sscfifo_depth_clr : 1;  /* [26] */
        td_u32 reg_deep_color        : 2;  /* [28:27] */
        td_u32 reg_ssc_mode          : 1;  /* [29] */
        td_u32 reg_pllfbmash111_en   : 1;  /* [30] */
        td_u32 reg_sscin_bypass_en   : 1;  /* [31] */
    } bits;
    td_u32 u32;
} hdmitx_inssc_set;

typedef union {
    struct {
        td_u32 reg_dphy_srst_req : 1;  /* [0] */
        td_u32 rsv_0             : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} hdmitx_dphy_rst;

typedef union {
    struct {
        td_u32 reg_txafifo_status_rst : 1;  /* [0] */
        td_u32 reg_txafifo_rst        : 1;  /* [1] */
        td_u32 reg_aphy_data_ch0_sel  : 1;  /* [2] */
        td_u32 reg_aphy_data_ch1_sel  : 1;  /* [3] */
        td_u32 reg_aphy_data_ch2_sel  : 1;  /* [4] */
        td_u32 txafifo_aful_status    : 1;  /* [5] */
        td_u32 txafifo_full_status    : 1;  /* [6] */
        td_u32 txafifo_aempt_status   : 1;  /* [7] */
        td_u32 txafifo_empt_status    : 1;  /* [8] */
        td_u32 reg_aphy_data_clk_h    : 10; /* [18:9] */
        td_u32 rsv_12                 : 13; /* [31:19] */
    } bits;
    td_u32 u32;
} hdmitx_afifo_data_sel;

typedef union {
    struct {
        td_u32 reg_aphy_data_clk_l : 30; /* [29:0] */
        td_u32 rsv_13              : 2;  /* [31:30] */
    } bits;
    td_u32 u32;
} hdmitx_afifo_clk;

typedef union {
    struct {
        td_u32 reg_gc_pd         : 4; /* [3:0] */
        td_u32 reg_gc_pd_bist    : 1; /* [4] */
        td_u32 reg_gc_pd_de      : 4; /* [8:5] */
        td_u32 reg_gc_pd_ldo     : 2; /* [10:9] */
        td_u32 reg_gc_pd_rterm   : 8; /* [18:11] */
        td_u32 reg_gc_pd_rxsense : 1; /* [19] */
        td_u32 reg_gc_txpll_pd   : 1; /* [20] */
        td_u32 reg_divsel        : 2; /* [22:21] */
        td_u32 rsv_17            : 9; /* [31:23] */
    } bits;
    td_u32 u32;
} aphy_top_pd;

typedef union {
    struct {
        td_u32 reg_isel_main_clk    : 6; /* [5:0] */
        td_u32 reg_isel_main_d0     : 6; /* [11:6] */
        td_u32 reg_isel_main_d1     : 6; /* [17:12] */
        td_u32 reg_isel_main_d2     : 6; /* [23:18] */
        td_u32 reg_isel_main_de_clk : 6; /* [29:24] */
        td_u32 rsv_18               : 2; /* [31:30] */
    } bits;
    td_u32 u32;
} aphy_driver_imain;

typedef union {
    struct {
        td_u32 reg_isel_main_de_d0 : 6; /* [5:0] */
        td_u32 reg_isel_main_de_d1 : 6; /* [11:6] */
        td_u32 reg_isel_main_de_d2 : 6; /* [17:12] */
        td_u32 reg_isel_pre_clk    : 6; /* [23:18] */
        td_u32 reg_isel_pre_d0     : 6; /* [29:24] */
        td_u32 rsv_19              : 2; /* [31:30] */
    } bits;
    td_u32 u32;
} aphy_driver_ipre;

typedef union {
    struct {
        td_u32 reg_isel_pre_d1     : 6; /* [5:0] */
        td_u32 reg_isel_pre_d2     : 6; /* [11:6] */
        td_u32 reg_isel_pre_de_clk : 6; /* [17:12] */
        td_u32 reg_isel_pre_de_d0  : 6; /* [23:18] */
        td_u32 reg_isel_pre_de_d1  : 6; /* [29:24] */
        td_u32 rsv_20              : 2; /* [31:30] */
    } bits;
    td_u32 u32;
} aphy_driver_iprede;

typedef union {
    struct {
        td_u32 reg_isel_pre_de_d2  : 6; /* [5:0] */
        td_u32 reg_rsel_pre_clk    : 3; /* [8:6] */
        td_u32 reg_rsel_pre_d0     : 3; /* [11:9] */
        td_u32 reg_rsel_pre_d1     : 3; /* [14:12] */
        td_u32 reg_rsel_pre_d2     : 3; /* [17:15] */
        td_u32 reg_rsel_pre_de_clk : 3; /* [20:18] */
        td_u32 reg_rsel_pre_de_d0  : 3; /* [23:21] */
        td_u32 reg_rsel_pre_de_d1  : 3; /* [26:24] */
        td_u32 reg_rsel_pre_de_d2  : 3; /* [29:27] */
        td_u32 rsv_21              : 2; /* [31:30] */
    } bits;
    td_u32 u32;
} aphy_driver_rpre;

typedef union {
    struct {
        td_u32 reg_rt_clk : 8; /* [7:0] */
        td_u32 reg_rt_d0  : 8; /* [15:8] */
        td_u32 reg_rt_d1  : 8; /* [23:16] */
        td_u32 reg_rt_d2  : 8; /* [31:24] */
    } bits;
    td_u32 u32;
} aphy_rterm_ctrl;

typedef union {
    struct {
        td_u32 reg_test    : 16; /* [15:0] */
        td_u32 reg_test_dc : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} aphy_test_ctrl;

typedef union {
    struct {
        td_u32 reg_txpll_clkin_sel         : 2;  /* [1:0] */
        td_u32 reg_txpll_divsel_in         : 3;  /* [4:2] */
        td_u32 reg_txpll_divsel_loop       : 7;  /* [11:5] */
        td_u32 reg_txpll_divsel_post       : 2;  /* [13:12] */
        td_u32 reg_txpll_icp_ictrl         : 4;  /* [17:14] */
        td_u32 reg_gc_txpll_en_ref_fb_div2 : 1;  /* [18] */
        td_u32 reg_gc_txpll_en_sscdiv      : 1;  /* [19] */
        td_u32 rsv_23                      : 12; /* [31:20] */
    } bits;
    td_u32 u32;
} aphy_txpll_ctrl;

typedef union {
    struct {
        td_u32 reg_txpll_test : 20; /* [19:0] */
        td_u32 rsv_24         : 12; /* [31:20] */
    } bits;
    td_u32 u32;
} aphy_txpll_test;

typedef union {
    struct {
        td_u32 reg_set_fb               : 24; /* [23:0] */
        td_u32 reg_set_load_fb          : 1;  /* [24] */
        td_u32 reg_sscfifo_tune_en_fb   : 1;  /* [25] */
        td_u32 reg_sscfifo_depth_clr_fb : 1;  /* [26] */
        td_u32 reg_ssc_mode_fb          : 1;  /* [27] */
        td_u32 rsv_27                   : 4;  /* [31:28] */
    } bits;
    td_u32 u32;
} hdmitx_fbssc_set;

typedef union {
    struct {
        td_u32 reg_step_fb  : 10; /* [9:0] */
        td_u32 reg_span_fb  : 14; /* [23:10] */
        td_u32 reg_testd_fb : 8;  /* [31:24] */
    } bits;
    td_u32 u32;
} hdmitx_fbssc_step;

typedef struct {
    volatile hdmitx_inssc_set      inssc_set;       /* 0 */
    volatile td_u32                reserved_0[3];   /* 4~c */
    volatile hdmitx_dphy_rst       dphy_rst;        /* 0x10 */
    volatile td_u32                reserved_1[15];  /* 0x14~0x4c */
    volatile hdmitx_afifo_data_sel afifo_data_sel;  /* 50 */
    volatile hdmitx_afifo_clk      afifo_clk;       /* 54 */
    volatile td_u32                reserved_2[3];   /* 0x58~0x60 */
    volatile aphy_top_pd           top_pd;          /* 64 */
    volatile aphy_driver_imain     driver_i_main;   /* 68 */
    volatile aphy_driver_ipre      driver_i_pre;    /* 6C */
    volatile aphy_driver_iprede    driver_i_pre_de; /* 70 */
    volatile aphy_driver_rpre      driver_r_pre;    /* 74 */
    volatile aphy_rterm_ctrl       rterm_ctrl;      /* 78 */
    volatile td_u32                reserved_3;      /* 0x7C */
    volatile aphy_test_ctrl        test_ctrl_aphy;  /* 80 */
    volatile td_u32                reserved_4[2];   /* 84~88 */
    volatile aphy_txpll_ctrl       txpll_ctrl;      /* 8C */
    volatile aphy_txpll_test       txpll_test;      /* 90 */
    volatile td_u32                reserved_5[7];   /* 0x94~0xac */
    volatile hdmitx_fbssc_set      fbssc_set;       /* B0 */
    volatile td_u32                reserved_6[1];   /* 0xB4 */
    volatile hdmitx_fbssc_step     fbssc_step;      /* B8 */
} hdmi2tx_dphy_regs;

td_s32 hdmi_hdmitx_phy_reg_init(td_void);
td_void hdmi_hdmitx_phy_reg_deinit(td_void);
td_void hdmi_hdmitx_inssc_set_reg_pllfbmash111_en_set(td_u32 reg_pllfbmash111_en);
td_void hdmi_hdmitx_inssc_set_reg_sscin_bypass_en_set(td_u32 reg_sscin_bypass_en);
td_void hdmi_hdmitx_dphy_rst_reg_rstset(td_u32 reg_rst);
td_void hdmi_hdmitx_afifo_data_sel_reg_aphy_data_clk_h_set(td_u32 reg_aphy_data_clk_h);
td_void hdmi_hdmitx_afifo_clk_reg_aphy_data_clk_l_set(td_u32 reg_aphy_data_clk_l);
td_void hdmi_aphy_top_pd_reg_gc_pd_set(td_u32 reg_gc_pd);
td_s32 hdmi_aphy_top_pd_reg_gc_pd_get(td_void);
td_void hdmi_aphy_top_pd_reg_gc_pd_bist_set(td_u32 reg_gc_pd_bist);
td_s32 hdmi_aphy_top_pd_reg_gc_pd_bist_get(td_void);
td_void hdmi_aphy_top_pd_reg_gc_pd_de_set(td_u32 reg_gc_pd_de);
td_s32 hdmi_aphy_top_pd_reg_gc_pd_de_get(td_void);
td_void hdmi_aphy_top_pd_reg_gc_pd_ldo_set(td_u32 reg_gc_pd_ldo);
td_s32 hdmi_aphy_top_pd_reg_gc_pd_ldo_get(td_void);
td_u32 hdmi_aphy_top_pd_reg_gc_pd_rterm_get(td_void);
td_void hdmi_aphy_top_pd_reg_gc_pd_rterm_set(td_u32 reg_gc_pd_rterm);
td_void hdmi_aphy_top_pd_reg_gc_pd_rxsense_set(td_u32 reg_gc_pd_rxsense);
td_s32 hdmi_aphy_top_pd_reg_gc_pd_rxsense_get(td_void);
td_void hdmi_aphy_top_pd_reg_gc_txpll_pd_set(td_u32 reg_gc_txpll_pd);
td_s32 hdmi_aphy_top_pd_reg_gc_txpll_pd_get(td_void);
td_void hdmi_aphy_top_pd_reg_divsel_set(td_u32 reg_divsel);
td_void hdmi_aphy_driver_imain_reg_isel_main_clk_set(td_u32 reg_isel_main_clk);
td_void hdmi_aphy_driver_imain_reg_isel_main_d0_set(td_u32 reg_isel_main_d0);
td_void hdmi_aphy_driver_imain_reg_isel_main_d1_set(td_u32 reg_isel_main_d1);
td_void hdmi_aphy_driver_imain_reg_isel_main_d2_set(td_u32 reg_isel_main_d2);
td_void hdmi_aphy_driver_imain_reg_isel_main_de_clk_set(td_u32 reg_isel_main_de_clk);
td_s32 hdmi_aphy_driver_imain_reg_isel_main_de_clk_get(td_void);
td_void hdmi_aphy_driver_ipre_reg_isel_main_de_d0_set(td_u32 reg_isel_main_de_d0);
td_s32 hdmi_aphy_driver_ipre_reg_isel_main_de_d0_get(td_void);
td_void hdmi_aphy_driver_ipre_reg_isel_main_de_d1_set(td_u32 reg_isel_main_de_d1);
td_s32 hdmi_aphy_driver_ipre_reg_isel_main_de_d1_get(td_void);
td_void hdmi_aphy_driver_ipre_reg_isel_main_de_d2_set(td_u32 reg_isel_main_de_d2);
td_s32 hdmi_aphy_driver_ipre_reg_isel_main_de_d2_get(td_void);
td_void hdmi_aphy_driver_ipre_reg_isel_pre_clk_set(td_u32 reg_isel_pre_clk);
td_void hdmi_aphy_driver_ipre_reg_isel_pre_d0_set(td_u32 reg_isel_pre_d0);
td_void hdmi_aphy_driver_iprede_reg_isel_pre_d1_set(td_u32 reg_isel_pre_d1);
td_void hdmi_aphy_driver_iprede_reg_isel_pre_d2_set(td_u32 reg_isel_pre_d2);
td_void hdmi_aphy_driver_iprede_reg_isel_pre_de_clk_set(td_u32 reg_isel_pre_de_clk);
td_void hdmi_aphy_driver_iprede_reg_isel_pre_de_d0_set(td_u32 reg_isel_pre_de_d0);
td_void hdmi_aphy_driver_iprede_reg_isel_pre_de_d1_set(td_u32 reg_isel_pre_de_d1);
td_void hdmi_aphy_driver_rpre_reg_isel_pre_de_d2_set(td_u32 reg_isel_pre_de_d2);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_clk_set(td_u32 reg_rsel_pre_clk);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_d0_set(td_u32 reg_rsel_pre_d0);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_d1_set(td_u32 reg_rsel_pre_d1);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_d2_set(td_u32 reg_rsel_pre_d2);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_de_clk_set(td_u32 reg_rsel_pre_de_clk);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_de_d0_set(td_u32 reg_rsel_pre_de_d0);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_de_d1_set(td_u32 reg_rsel_pre_de_d1);
td_void hdmi_aphy_driver_rpre_reg_rsel_pre_de_d2_set(td_u32 reg_rsel_pre_de_d2);
td_void hdmi_aphy_rterm_ctrl_reg_rt_clk_set(td_u32 reg_rt_clk);
td_void hdmi_aphy_rterm_ctrl_reg_rt_d0_set(td_u32 reg_rt_d0);
td_void hdmi_aphy_rterm_ctrl_reg_rt_d1_set(td_u32 reg_rt_d1);
td_void hdmi_aphy_rterm_ctrl_reg_rt_d2_set(td_u32 reg_rt_d2);
td_void hdmi_aphy_txpll_ctrl_reg_txpll_divsel_loop_set(td_u32 divsel_loop);
td_void hdmi_aphy_txpll_ctrl_reg_txpll_icp_ictrl_set(td_u32 reg_txpll_icp_ictrl);
td_void hdmi_aphy_txpll_ctrl_reg_gc_txpll_en_sscdiv_set(td_u32 en_sscdiv);
td_void hdmi_aphy_txpll_test_reg_txpll_test_set(td_u32 reg_txpll_test);
td_void hdmi_hdmitx_fbssc_set_reg_set_fb_set(td_u32 reg_set_fb);
td_void hdmi_hdmitx_fbssc_set_reg_set_load_fb_set(td_u32 reg_set_load_fb);
td_void hdmi_hdmitx_fbssc_set_reg_ssc_mode_fb_set(td_u32 reg_ssc_mode_fb);
td_void hdmi_hdmitx_fbssc_step_reg_step_fb_set(td_u32 reg_step_fb);
td_s32 hdmi_hdmitx_fbssc_step_reg_step_fb_get(td_void);
td_void hdmi_hdmitx_fbssc_step_reg_span_fb_set(td_u32 reg_span_fb);
td_s32 hdmi_hdmitx_fbssc_step_reg_span_fb_get(td_void);
td_s32 hdmi_reg_test_set(td_u32 reg_test);
#endif /* __HDMI_REG_DPHY_H__ */

