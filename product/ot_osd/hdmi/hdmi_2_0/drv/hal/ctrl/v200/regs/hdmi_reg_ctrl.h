// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_REG_CTRL_H__
#define __HDMI_REG_CTRL_H__

#include "ot_type.h"

typedef union {
    struct {
        td_u32 reg_vid_bypass_sel : 1;  /* [0] */
        td_u32 reg_rx_frl_sel     : 1;  /* [1] */
        td_u32 flbk_sel           : 1;  /* [2] */
        td_u32 rsv_0              : 29; /* [31:3] */
    } bits;
    td_u32 u32;
} tx_channel_sel;

typedef union {
    struct {
        td_u32 reg_yuv_rgb_cfg : 1;  /* [0] */
        td_u32 rsv_0           : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} yuv_rgb_cfg;

typedef union {
    struct {
        td_u32 tx_pwd_srst_req      : 1; /* [0] */
        td_u32 tx_sys_srst_req      : 1; /* [1] */
        td_u32 tx_vid_srst_req      : 1; /* [2] */
        td_u32 tx_hdmi_srst_req     : 1; /* [3] */
        td_u32 tx_hdcp1x_srst_req   : 1; /* [4] */
        td_u32 tx_phy_srst_req      : 1; /* [5] */
        td_u32 tx_aud_srst_req      : 1; /* [6] */
        td_u32 tx_acr_srst_req      : 1; /* [7] */
        td_u32 tx_afifo_srst_req    : 1; /* [8] */
        td_u32 tx_hdcp2x_srst_req   : 1; /* [9] */
        td_u32 tx_mcu_srst_req      : 1; /* [10] */
        td_u32 frl_soft_arst_req    : 1; /* [11] */
        td_u32 frl_fast_arst_req    : 1; /* [12] */
        td_u32 rsv_0                : 3; /* [15:13] */
        td_u32 vidpath_dout_clk_sel : 1; /* [16] */
        td_u32 vidpath_div4_clk_sel : 1; /* [17] */
        td_u32 hdmi_clk_sel         : 1; /* [18] */
        td_u32 hdmi_clk_div_sel     : 1; /* [19] */
        td_u32 audpath_acr_clk_sel  : 1; /* [20] */
        td_u32 clk_tmds_source_sel  : 1; /* [21] */
        td_u32 clk_frl_source_sel   : 1; /* [22] */
        td_u32 clk_test_out_sel     : 1; /* [23] */
        td_u32 rsv_1                : 8; /* [31:24] */
    } bits;
    td_u32 u32;
} tx_pwd_rst_ctrl;

typedef union {
    struct {
        td_u32 pwd_fifo_data_out : 8;  /* [7:0] */
        td_u32 rsv_5             : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} pwd_fifo_rdata;

typedef union {
    struct {
        td_u32 pwd_fifo_data_in : 8;  /* [7:0] */
        td_u32 rsv_6            : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} pwd_fifo_wdata;

typedef union {
    struct {
        td_u32 pwd_fifo_data_cnt : 5;  /* [4:0] */
        td_u32 rsv_7             : 3;  /* [7:5] */
        td_u32 pwd_data_out_cnt  : 10; /* [17:8] */
        td_u32 rsv_8             : 14; /* [31:18] */
    } bits;
    td_u32 u32;
} pwd_data_cnt;

typedef union {
    struct {
        td_u32 pwd_slave_addr   : 8; /* [7:0] */
        td_u32 pwd_slave_offset : 8; /* [15:8] */
        td_u32 pwd_slave_seg    : 8; /* [23:16] */
        td_u32 rsv_9            : 8; /* [31:24] */
    } bits;
    td_u32 u32;
} pwd_slave_cfg;

typedef union {
    struct {
        td_u32 pwd_i2c_no_ack     : 1;  /* [0] */
        td_u32 pwd_i2c_bus_low    : 1;  /* [1] */
        td_u32 pwd_i2c_in_prog    : 1;  /* [2] */
        td_u32 pwd_fifo_wr_in_use : 1;  /* [3] */
        td_u32 pwd_fifo_rd_in_use : 1;  /* [4] */
        td_u32 pwd_fifo_empty     : 1;  /* [5] */
        td_u32 pwd_fifo_half_full : 1;  /* [6] */
        td_u32 pwd_fifo_full      : 1;  /* [7] */
        td_u32 rsv_10             : 24; /* [31:8] */
    } bits;
    td_u32 u32;
} pwd_mst_state;

typedef union {
    struct {
        td_u32 pwd_mst_cmd : 4;  /* [3:0] */
        td_u32 rsv_11      : 28; /* [31:4] */
    } bits;
    td_u32 u32;
} pwd_mst_cmd;

typedef union {
    struct {
        td_u32 cpu_ddc_req : 1;  /* [0] */
        td_u32 rsv_14      : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} ddc_mst_arb_req;

typedef union {
    struct {
        td_u32 cpu_ddc_req_ack : 1;  /* [0] */
        td_u32 rsv_15          : 31; /* [31:1] */
    } bits;
    td_u32 u32;
} ddc_mst_arb_ack;

typedef struct {
    volatile tx_channel_sel  channel_sel;    /* 00 */
    volatile yuv_rgb_cfg     yuv_rgb;        /* 04 */
    td_u32                   reserved_0[2];  /* 8-C */
    volatile tx_pwd_rst_ctrl tx_rst_ctrl;    /* 10 */
    td_u32                   reserved_1[9];  /* 14-34 */
    volatile pwd_fifo_rdata  pwd_rdata;      /* 38 */
    volatile pwd_fifo_wdata  pwd_wdata;      /* 3C */
    volatile pwd_data_cnt    pwd_cnt;        /* 40 */
    volatile pwd_slave_cfg   pwd_slave;      /* 44 */
    volatile pwd_mst_state   pwd_state;      /* 48 */
    volatile pwd_mst_cmd     pwd_cmd;        /* 4C */
    td_u32                   reserved_2[6];  /* 50-64 */
    volatile ddc_mst_arb_req ddc_arb_req;    /* 68 */
    volatile ddc_mst_arb_ack ddc_arb_ack;    /* 6C */
} tx_ctrl_reg_type;

td_s32 hdmi_tx_ctrl_reg_type_init(td_void);
td_void hdmi_tx_ctrl_reg_type_deinit(td_void);
td_s32 hdmi_tx_channel_reg_vid_bypass_sel_set(td_u32 reg_vid_bypass_sel);
td_s32 hdmi_tx_channel_reg_vid_bypass_sel_get(td_void);
td_s32 hdmi_yuv_rgb_cfg_reg_set(td_u32 reg_yuv_rgb_cfg);
td_s32 hdmi_tx_afifo_srst_req_set(td_u32 tx_afifo_srst_req);
td_s32 hdmi_tx_pwd_srst_req_set(td_u32 tx_pwd_srst_req);
td_s32 hdmi_tx_acr_srst_req_set(td_u32 tx_acr_srst_req);
td_s32 hdmi_tx_hdmi_srst_req_set(td_u32 tx_hdmi_srst_req);
td_s32 hdmi_tx_aud_srst_req_set(td_u32 tx_aud_srst_req);
td_s32 hdmi_pwd_fifo_wdata_set(td_u32 pwd_fifo_data_in);
td_s32 hdmi_pwd_data_out_cnt_set(td_u32 pwd_data_out_cnt);
td_s32 hdmi_pwd_slave_addr_set(td_u32 pwd_slave_addr);
td_s32 hdmi_pwd_slave_offset_set(td_u32 pwd_slave_offset);
td_s32 hdmi_pwd_slave_seg_set(td_u32 pwd_slave_seg);
td_s32 hdmi_pwd_mst_cmd_set(td_u32 pwd_cmd);
td_s32 hdmi_ddc_mst_arb_req_set(td_u32 cpu_ddc_req);
td_s32 hdmi_tx_mcu_srst_req_set(td_u32 tx_mcu_srst_req);
td_s32 hdmi_vidpath_dout_clk_sel_set(td_u32 vidpath_dout_clk_sel);
td_s32 hdmi_pwd_fifo_rdata_get(td_void);
td_u32 hdmi_pwd_fifo_data_cnt_get(td_void);
td_u32 hdmi_pwd_i2c_in_prog_get(td_void);
td_u32 hdmi_pwd_fifo_empty_get(td_void);
td_s32 hdmi_ddc_mst_arb_ack_cpu_ddc_req_ack_get(td_void);
td_u32 hdmi_tx_ctrl_read(td_u32 tx_crl_addr);
td_s32 hdmi_tx_ctrl_write(td_u32 tx_crl_addr, td_u32 val);
#endif /* __HDMI_REG_CTRL_H__ */

