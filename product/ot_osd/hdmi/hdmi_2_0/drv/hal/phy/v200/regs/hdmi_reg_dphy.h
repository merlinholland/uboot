// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef __HDMI_REG_DPHY_H__
#define __HDMI_REG_DPHY_H__

#include "ot_type.h"

#define PR_OFFSET 0x20000

typedef union {
    struct {
        td_u32 stb_cs_en : 16; /* [15:0] */
        td_u32 rsv_2     : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} phy_csen;

typedef union {
    struct {
        td_u32 stb_wen : 1;  /* [0] */
        td_u32 rsv_3   : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} phy_wr;

typedef union {
    struct {
        td_u32 reset  : 1;  /* [0] */
        td_u32 rsv_4  : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} resetn;

typedef union {
    struct {
        td_u32 stb_addr : 4;  /* [3:0] */
        td_u32 rsv_5    : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} phy_addr;

typedef union {
    struct {
        td_u32 stb_wdata : 8;  /* [7:0] */
        td_u32 rsv_6     : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} phy_wdata;

typedef union {
    struct {
        td_u32 stb_rdata : 8;  /* [7:0] */
        td_u32 rsv_7     : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} phy_rdata;

typedef union {
    struct {
        td_u32 src_lock_val      : 8;  /* [7:0] */
        td_u32 src_lock_cnt      : 8;  /* [15:8] */
        td_u32 src_enable        : 1;  /* [16] */
        td_u32 fdsrcparam_unused : 3;  /* [19:17] */
        td_u32 rsv_13            : 12; /* [31:20] */
    } bits;
    td_u32 u32;
} fd_src_param;

typedef union {
    struct {
        td_u32 src_cnt_opt        : 3;  /* [2:0] */
        td_u32 fdsrcfreq_unused_1 : 1;  /* [3] */
        td_u32 src_freq_opt       : 2;  /* [5:4] */
        td_u32 fdsrcfreq_unused_2 : 2;  /* [7:6] */
        td_u32 src_freq_ext       : 16; /* [23:8] */
        td_u32 rsv_14             : 8;  /* [31:24] */
    } bits;
    td_u32 u32;
} fdsrcfreq;

typedef union {
    struct {
        td_u32 src_det_stat : 4;  /* [3:0] */
        td_u32 src_cnt_out  : 20; /* [23:4] */
        td_u32 rsv_15       : 8;  /* [31:24] */
    } bits;
    td_u32 u32;
} fdsrcres;

typedef union {
    struct {
        td_u32 i_enable      : 1;  /* [0] */
        td_u32 i_run         : 1;  /* [1] */
        td_u32 ctset0_unused : 2;  /* [3:2] */
        td_u32 rsv_16        : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} ctset0;

typedef union {
    struct {
        td_u32 i_mpll_fcon   : 10; /* [9:0] */
        td_u32 i_mpll_divn   : 3;  /* [12:10] */
        td_u32 i_mpll_ctlck  : 1;  /* [13] */
        td_u32 ctset1_unused : 18; /* [31:14] */
    } bits;
    td_u32 u32;
} ctset1;

typedef union {
    struct {
        td_u32 i_deci_cnt_len     : 8; /* [7:0] */
        td_u32 i_vco_st_wait_len  : 8; /* [15:8] */
        td_u32 i_vco_end_wait_len : 8; /* [23:16] */
        td_u32 i_ref_cnt_len      : 8; /* [31:24] */
    } bits;
    td_u32 u32;
} fccntr0;

typedef union {
    struct {
        td_u32 i_ct_sel       : 1;  /* [0] */
        td_u32 i_clkdet_sel   : 1;  /* [1] */
        td_u32 i_ct_mode      : 2;  /* [3:2] */
        td_u32 fcopt_unused_1 : 4;  /* [7:4] */
        td_u32 i_ct_en        : 1;  /* [8] */
        td_u32 fcopt_unused_2 : 3;  /* [11:9] */
        td_u32 i_ct_idx_sel   : 1;  /* [12] */
        td_u32 i_deci_try_sel : 1;  /* [13] */
        td_u32 fcopt_unused   : 2;  /* [15:14] */
        td_u32 rsv_17         : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} fcopt;

typedef union {
    struct {
        td_u32 clk_ok          : 1;  /* [0] */
        td_u32 busy            : 1;  /* [1] */
        td_u32 done            : 1;  /* [2] */
        td_u32 error           : 1;  /* [3] */
        td_u32 divn            : 3;  /* [6:4] */
        td_u32 fcstat_unused_1 : 1;  /* [7] */
        td_u32 ref_clk_stat    : 1;  /* [8] */
        td_u32 pllvco_clk_stat : 1;  /* [9] */
        td_u32 fcstat_unused_2 : 2;  /* [11:10] */
        td_u32 confin_stat     : 6;  /* [17:12] */
        td_u32 fcstat_unused_3 : 2;  /* [19:18] */
        td_u32 fcon_init       : 10; /* [29:20] */
        td_u32 rsv_18          : 2;  /* [31:30] */
    } bits;
    td_u32 u32;
} fcstat;

typedef union {
    struct {
        td_u32 divn_h20             : 3;  /* [2:0] */
        td_u32 fcdstepset_unused    : 1;  /* [3] */
        td_u32 up_sampler_ratio_sel : 1;  /* [4] */
        td_u32 rsv_22               : 27; /* [31:5] */
    } bits;
    td_u32 u32;
} fcdstepset;

typedef union {
    struct {
        td_u32 i_ref_cnt : 16; /* [15:0] */
        td_u32 rsv_24    : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} fccntr1;

typedef union {
    struct {
        td_u32 contin_upd_en       : 1;  /* [0] */
        td_u32 contin_upd_opt      : 1;  /* [1] */
        td_u32 contin_upd_pol      : 1;  /* [2] */
        td_u32 fccontinset0_unused : 1;  /* [3] */
        td_u32 contin_upd_step     : 4;  /* [7:4] */
        td_u32 rsv_25              : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} fccontinset0;

typedef union {
    struct {
        td_u32 contin_upd_rate : 28; /* [27:0] */
        td_u32 contin_upd_time : 4;  /* [31:28] */
    } bits;
    td_u32 u32;
} fccontinset1;

typedef union {
    struct {
        td_u32 contin_upd_th_dn    : 10; /* [9:0] */
        td_u32 fccontinset2_unused : 2;  /* [11:10] */
        td_u32 contin_upd_th_up    : 10; /* [21:12] */
        td_u32 rsv_26              : 10; /* [31:22] */
    } bits;
    td_u32 u32;
} fccontinset2;

typedef union {
    struct {
        td_u32 init   : 1;  /* [0] */
        td_u32 ctrl   : 1;  /* [1] */
        td_u32 mod    : 1;  /* [2] */
        td_u32 sdm    : 1;  /* [3] */
        td_u32 rsv_27 : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} fdivset0;

typedef union {
    struct {
        td_u32 mod_len : 8;  /* [7:0] */
        td_u32 mod_t   : 8;  /* [15:8] */
        td_u32 mod_n   : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} fdivset3;

typedef union {
    struct {
        td_u32 mod_d  : 16; /* [15:0] */
        td_u32 rsv_29 : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} fdivset4;

typedef union {
    struct {
        td_u32 i_fdiv_in : 32; /* [31:0] */
    } bits;
    td_u32 u32;
} fdivstat1;

typedef union {
    struct {
        td_u32 i_manual_en       : 4; /* [3:0] */
        td_u32 i_divn            : 3; /* [6:4] */
        td_u32 fdivmanual_unused : 1; /* [7] */
        td_u32 i_mdiv            : 4; /* [11:8] */
        td_u32 i_ref_cnt_div     : 2; /* [13:12] */
        td_u32 i_dc_sel          : 2; /* [15:14] */
        td_u32 i_vic             : 8; /* [23:16] */
        td_u32 rsv_34            : 8; /* [31:24] */
    } bits;
    td_u32 u32;
} fdivmanual;

typedef union {
    struct {
        td_u32 t2_refclksel       : 1;  /* [0] */
        td_u32 t2_refclksel2      : 1;  /* [1] */
        td_u32 i_ref_clk_sel      : 1;  /* [2] */
        td_u32 refclksel_unused_1 : 1;  /* [3] */
        td_u32 t2_pixelclksel     : 1;  /* [4] */
        td_u32 refclksel_unused_2 : 1;  /* [5] */
        td_u32 pr_enc_val         : 2;  /* [7:6] */
        td_u32 rsv_35             : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} refclksel;

typedef union {
    struct {
        td_u32 fcg_en        : 1;  /* [0] */
        td_u32 fcg_dlf_en    : 1;  /* [1] */
        td_u32 fcg_dither_en : 1;  /* [2] */
        td_u32 fcg_lock_en   : 1;  /* [3] */
        td_u32 rsv_39        : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} fcgset;

typedef union {
    struct {
        td_u32 pr_en_h20         : 1;  /* [0] */
        td_u32 enable_h20        : 1;  /* [1] */
        td_u32 txfifoset0_unused : 6;  /* [7:2] */
        td_u32 rsv_46            : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} txfifoset0;

typedef union {
    struct {
        td_u32 ch_out_sel : 2;  /* [1:0] */
        td_u32 rsv_52     : 30; /* [31:2] */
    } bits;
    td_u32 u32;
} txdataoutsel;

typedef union {
    struct {
        td_u32 reg_hdmi_mode_en : 1;  /* [0] */
        td_u32 rsv_53           : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} hdmi_mode;

typedef union {
    struct {
        td_u32 hsset   : 2;  /* [1:0] */
        td_u32 rsv_58  : 30; /* [31:2] */
    } bits;
    td_u32 u32;
} hsset;

typedef union {
    struct {
        td_u32 fd_clk_sel  : 2;  /* [1:0] */
        td_u32 refclk_sel  : 2;  /* [3:2] */
        td_u32 ctman       : 2;  /* [5:4] */
        td_u32 modclk_sel  : 1;  /* [6] */
        td_u32 fdivclk_sel : 1;  /* [7] */
        td_u32 mod_div_val : 4;  /* [11:8] */
        td_u32 rsv_63      : 20; /* [31:12] */
    } bits;
    td_u32 u32;
} clkset;

typedef union {
    struct {
        td_u32 sw_reset_mod_clock          : 1;  /* [0] */
        td_u32 sw_reset_tmds_clock         : 1;  /* [1] */
        td_u32 sw_reset_mpll_clock         : 1;  /* [2] */
        td_u32 sw_reset_nco_clock          : 1;  /* [3] */
        td_u32 sw_reset_fd_clock           : 1;  /* [4] */
        td_u32 sw_reset_mod_and_mpll_clock : 1;  /* [5] */
        td_u32 sw_reset_mod_and_nco_clock  : 1;  /* [6] */
        td_u32 sw_reset_mod_and_fd_clock   : 1;  /* [7] */
        td_u32 sw_reset_hsfifo_clock       : 1;  /* [8] */
        td_u32 sw_reset_txfifo_clock       : 1;  /* [9] */
        td_u32 sw_reset_data_clock         : 1;  /* [10] */
        td_u32 sw_reset_hs_clock           : 1;  /* [11] */
        td_u32 sw_reset_pllref_clock       : 1;  /* [12] */
        td_u32 sw_reset_dac_clock          : 1;  /* [13] */
        td_u32 dac_clock_gat               : 1;  /* [14] */
        td_u32 up_sample_fifo_clock_swrst  : 1;  /* [15] */
        td_u32 sw_reset_frl_clock          : 1;  /* [16] */
        td_u32 swreset_unused              : 14; /* [30:17] */
        td_u32 global_reset                : 1;  /* [31] */
    } bits;
    td_u32 u32;
} swreset;

typedef union {
    struct {
        td_u32 req_length  : 2;  /* [1:0] */
        td_u32 stb_delay2  : 4;  /* [5:2] */
        td_u32 stb_delay1  : 4;  /* [9:6] */
        td_u32 stb_delay0  : 4;  /* [13:10] */
        td_u32 stb_acc_sel : 1;  /* [14] */
        td_u32 stb_cs_sel  : 1;  /* [15] */
        td_u32 rsv_65      : 16; /* [31:16] */
    } bits;
    td_u32 u32;
} stbopt;

typedef struct {
    td_u32                reserved_0[2];    /* 0-4 */
    volatile phy_csen     reg_phy_csen;     /* 8 */
    volatile phy_wr       reg_phy_wr;       /* C */
    volatile resetn       reg_resetn;       /* 10 */
    volatile phy_addr     reg_phy_addr;     /* 14 */
    volatile phy_wdata    reg_phy_wdata;    /* 18 */
    volatile phy_rdata    reg_phy_rdata;    /* 1C */
    td_u32                reserved_1[4];    /* 20-2C */
    volatile fd_src_param reg_fd_src_param; /* 30 */
    volatile fdsrcfreq    reg_fdsrcfreq;    /* 34 */
    volatile fdsrcres     reg_fdsrcres;     /* 38 */
    volatile ctset0       reg_ctset0;       /* 3C */
    volatile ctset1       reg_ctset1;       /* 40 */
    td_u32                reserved_2;       /* 44 */
    volatile fccntr0      reg_fccntr0;      /* 48 */
    volatile fcopt        reg_fcopt;        /* 4C */
    volatile fcstat       reg_fcstat;       /* 50 */
    td_u32                reserved_3[3];    /* 54-5C */
    volatile fcdstepset   reg_fcdstepset;   /* 60 */
    td_u32                reserved_4[3];    /* 64-6C */
    volatile fccntr1      reg_fccntr1;      /* 70 */
    volatile fccontinset0 reg_fccontinset0; /* 74 */
    volatile fccontinset1 reg_fccontinset1; /* 78 */
    volatile fccontinset2 reg_fccontinset2; /* 7C */
    td_u32                reserved_6[4];    /* 80-8C */
    volatile fdivset0     reg_fdivset0;     /* 90 */
    td_u32                reserved_7[2];    /* 94-98 */
    volatile fdivset3     reg_fdivset3;     /* 9C */
    volatile fdivset4     reg_fdivset4;     /* A0 */
    td_u32                reserved_8[2];    /* A4-A8 */
    volatile fdivstat1    reg_fdivstat1;    /* AC */
    td_u32                reserved_9[3];    /* B0-B8 */
    volatile fdivmanual   reg_fdivmanual;   /* BC */
    volatile refclksel    reg_refclksel;    /* C0 */
    td_u32                reserved_10[23];  /* c4-11c */
    volatile fcgset       reg_fcgset;       /* 120 */
    td_u32                reserved_11[59];  /* 124-20C */
    volatile txfifoset0   tx_fifo_set0;     /* 210 */
    td_u32                reserved_12[6];   /* 214-228 */
    volatile txdataoutsel tx_data_out_sel;  /* 22C */
    volatile hdmi_mode    reg_hdmi_mode;    /* 230 */
    td_u32                reserved_13[11];  /* 234-25c */
    volatile hsset        reg_hsset;        /* 260 */
    td_u32                reserved_14[42];  /* 264-308 */
    volatile clkset       reg_clkset;       /* 30C */
    volatile swreset      reg_swreset;      /* 310 */
    td_u32                reserved_15[251]; /* 314-6FC */
    volatile stbopt       reg_stbopt;       /* 700 */
} hdmitx21_dphy_regs_type;

td_s32 hdmi_hdmitx_phy_reg_init(td_void);
td_s32 hdmi_hdmitx_phy_reg_deinit(td_void);
td_s32 hdmi_phy_csen_stb_cs_en_set(td_u32 stb_cs_en);
td_s32 hdmi_phy_wr_stb_wen_set(td_u32 stb_wen);
td_s32 hdmi_resetn_resetn_set(td_u32 resetn);
td_s32 hdmi_resetn_resetn_get(td_void);
td_s32 hdmi_phy_addr_stb_addr_set(td_u32 stb_addr);
td_s32 hdmi_phy_wdata_stb_wdata_set(td_u32 stb_wdata);
td_s32 hdmi_phy_rdata_stb_rdata_get(td_void);
td_s32 hdmi_fdsrcparam_src_lock_val_set(td_u32 src_lock_val);
td_s32 hdmi_fdsrcparam_src_lock_cnt_set(td_u32 src_lock_cnt);
td_s32 hdmi_fdsrcparam_src_enable_set(td_u32 src_enable);
td_s32 hdmi_fdsrcfreq_src_cnt_opt_set(td_u32 src_cnt_opt);
td_s32 hdmi_fdsrcfreq_unused_1_set(td_u32 fdsrcfreq_unused_1);
td_s32 hdmi_fdsrcfreq_src_freq_opt_set(td_u32 src_freq_opt);
td_s32 hdmi_fdsrcfreq_unused_2_set(td_u32 fdsrcfreq_unused_2);
td_s32 hdmi_fdsrcfreq_src_freq_ext_set(td_u32 src_freq_ext);
td_s32 hdmi_fdsrcres_src_det_stat_get(td_void);
td_s32 hdmi_fdsrcres_src_cnt_out_get(td_void);
td_s32 hdmi_ctset0_i_enable_set(td_u32 enable);
td_s32 hdmi_ctset0_i_run_set(td_u32 run);
td_s32 hdmi_ctset0_unused_set(td_u32 ctset0_unused);
td_s32 hdmi_fccntr0_i_vco_st_wait_len_set(td_u32 vco_st_wait_len);
td_s32 hdmi_fccntr0_i_vco_end_wait_len_set(td_u32 vco_end_wait_len);
td_s32 hdmi_fccntr0_i_ref_cnt_len_set(td_u32 ref_cnt_len);
td_s32 hdmi_fcopt_i_ct_sel_set(td_u32 ct_sel);
td_s32 hdmi_fcopt_i_clkdet_sel_set(td_u32 clkdet_sel);
td_s32 hdmi_fcopt_i_ct_mode_set(td_u32 ct_mode);
td_s32 hdmi_fcopt_unused_1_set(td_u32 fcopt_unused_1);
td_s32 hdmi_fcopt_i_ct_en_set(td_u32 ct_en);
td_s32 hdmi_fcopt_unused_2_set(td_u32 fcopt_unused_2);
td_s32 hdmi_fcopt_i_ct_idx_sel_set(td_u32 ct_idx_sel);
td_s32 hdmi_fcopt_unused_set(td_u32 fcopt_unused);
td_s32 hdmi_fcstat_busy_get(td_void);
td_s32 hdmi_fcdstepset_p_divn_h20_set(td_u32 divn_h20);
td_s32 hdmi_fcdstepset_up_sampler_ratio_sel_set(td_u32 sampler_ratio_sel);
td_s32 hdmi_fccntr1_i_ref_cnt_set(td_u32 ref_cnt);
td_s32 hdmi_fccontinset0_p_contin_upd_en_set(td_u32 contin_upd_en);
td_s32 hdmi_fccontinset0_p_contin_upd_opt_set(td_u32 contin_upd_opt);
td_s32 hdmi_fccontinset0_p_contin_upd_pol_set(td_u32 contin_upd_pol);
td_s32 hdmi_fccontinset0_p_contin_upd_step_set(td_u32 contin_upd_step);
td_s32 hdmi_fccontinset1_p_contin_upd_rate_set(td_u32 contin_upd_rate);
td_s32 hdmi_fccontinset1_p_contin_upd_time_set(td_u32 contin_upd_time);
td_s32 hdmi_fccontinset2_p_contin_upd_th_dn_set(td_u32 contin_upd_th_dn);
td_s32 hdmi_fccontinset2_unused_set(td_u32 fccontinset2_unused);
td_s32 hdmi_fccontinset2_p_contin_upd_th_up_set(td_u32 contin_upd_th_up);
td_s32 hdmi_fdivset0_init_set(td_u32 init);
td_s32 hdmi_fdivset0_en_ctrl_set(td_u32 ctrl);
td_s32 hdmi_fdivset0_en_ctrl_get(td_void);
td_s32 hdmi_fdivset0_en_mod_set(td_u32 mod);
td_s32 hdmi_fdivset0_en_mod_get(td_void);
td_s32 hdmi_fdivset0_en_sdm_set(td_u32 sdm);
td_s32 hdmi_fdivset0_en_sdm_get(td_void);
td_s32 hdmi_fdivset3_mod_len_set(td_u32 mod_len);
td_s32 hdmi_fdivset3_mod_t_set(td_u32 mod_t);
td_s32 hdmi_fdivset3_mod_n_set(td_u32 mod_n);
td_s32 hdmi_fdivset4_mod_d_set(td_u32 mod_d);
td_s32 hdmi_fdivstat1_i_fdiv_in_set(td_u32 fdiv_in);
td_s32 hdmi_fdivmanual_i_manual_en_set(td_u32 manual_en);
td_s32 hdmi_fdivmanual_i_mdiv_set(td_u32 mdiv);
td_s32 hdmi_refclksel_i_ref_clk_sel_set(td_u32 ref_clk_sel);
td_s32 hdmi_txfifoset0_p_pr_en_h20_set(td_u32 pr_en_h20);
td_s32 hdmi_txfifoset0_p_enable_h20_set(td_u32 enable_h20);
td_s32 hdmi_txdataoutsel_p_ch_out_sel_set(td_u32 ch_out_sel);
td_s32 hdmi_hsset_p_hsset_set(td_u32 hsset);
td_s32 hdmi_clkset_p_fd_clk_sel_set(td_u32 fd_clk_sel);
td_s32 hdmi_clkset_p_refclk_sel_set(td_u32 refclk_sel);
td_s32 hdmi_clkset_p_ctman_set(td_u32 ctman);
td_s32 hdmi_clkset_p_modclk_sel_set(td_u32 modclk_sel);
td_s32 hdmi_clkset_p_fdivclk_sel_set(td_u32 fdivclk_sel);
td_s32 hdmi_clkset_mod_div_val_set(td_u32 mod_div_val);
td_s32 hdmi_swreset_dac_clock_gat_set(td_u32 dac_clock_gat);
td_s32 hdmi_swreset_unused_set(td_u32 swreset_unused);
td_s32 hdmi_swreset_global_reset_set(td_u32 global_reset);
td_s32 hdmi_swreset_sw_reset_mod_clock_set(td_u32 sw_reset_mod_clock);
td_s32 hdmi_swreset_sw_reset_tmds_clock_set(td_u32 sw_reset_tmds_clock);
td_s32 hdmi_swreset_sw_reset_mpll_clock_set(td_u32 sw_reset_mpll_clock);
td_s32 hdmi_swreset_sw_reset_nco_clock_set(td_u32 sw_reset_nco_clock);
td_s32 hdmi_swreset_sw_reset_fd_clock_set(td_u32 sw_reset_fd_clock);
td_s32 hdmi_swreset_sw_reset_mod_and_mpll_clock_set(td_u32 mpll_clock);
td_s32 hdmi_swreset_sw_reset_mod_and_nco_clock_set(td_u32 nco_clock);
td_s32 hdmi_swreset_sw_reset_mod_and_fd_clock_set(td_u32 fd_clock);
td_s32 hdmi_swreset_sw_reset_hsfifo_clock_set(td_u32 hsfifo_clock);
td_s32 hdmi_swreset_sw_reset_txfifo_clock_set(td_u32 txfifo_clock);
td_s32 hdmi_swreset_sw_reset_data_clock_set(td_u32 sw_reset_data_clock);
td_s32 hdmi_swreset_sw_reset_hs_clock_set(td_u32 sw_reset_hs_clock);
td_s32 hdmi_swreset_sw_reset_pllref_clock_set(td_u32 pllref_clock);
td_s32 hdmi_swreset_sw_reset_dac_clock_set(td_u32 sw_reset_dac_clock);
td_s32 hdmi_swreset_up_sample_fifo_clock_swrst_set(td_u32 clock_swrst);
td_s32 hdmi_fccontinset0_unused_set(td_u32 fccontinset0_unused);
td_s32 hdmi_ctset1_i_mpll_fcon_set(td_u32 mpll_fcon);
td_s32 hdmi_ctset1_i_mpll_ctlck_set(td_u32 mpll_ctlck);
td_s32 hdmi_ctset1_ctset1_unused_set(td_u32 ctset1_unused);
td_s32 hdmi_fcdstepset_unused_set(td_u32 fcdstepset_unused);
td_s32 hdmi_stbopt_req_length_set(td_u32 req_length);
td_s32 hdmi_stbopt_stb_cs_sel_set(td_u32 stb_cs_sel);
td_s32 hdmi_fdsrcparam_unused_set(td_u32 fdsrcparam_unused);
td_s32 hdmi_fcgset_p_fcg_en_set(td_u32 fcg_en);
td_s32 hdmi_fcgset_p_fcg_dlf_en_set(td_u32 fcg_dlf_en);
td_s32 hdmi_fcgset_p_fcg_dither_en_set(td_u32 fcg_dither_en);
td_s32 hdmi_fcgset_p_fcg_lock_en_set(td_u32 fcg_lock_en);
td_s32 hdmi_txfifoset0_unused_set(td_u32 txfifoset0_unused);
td_s32 hdmi_hdmi_mode_reg_hdmi_mode_en_set(td_u32 reg_hdmi_mode_en);
td_s32 hdmi_swreset_sw_reset_frl_clock_set(td_u32 sw_reset_frl_clock);
td_s32 hdmi_stbopt_stb_delay2_set(td_u32 stb_delay2);
td_s32 hdmi_stbopt_stb_delay1_set(td_u32 stb_delay1);
td_s32 hdmi_stbopt_stb_delay0_set(td_u32 stb_delay0);
td_s32 hdmi_stbopt_stb_acc_sel_set(td_u32 stb_acc_sel);
td_s32 hdmi_fcstat_clk_ok_get(td_void);
td_s32 hdmi_fcstat_done_get(td_void);
td_s32 hdmi_fcstat_error_get(td_void);

#endif /* __TX_DPHY_REG_C_UNION_DEFINE_H__ */

