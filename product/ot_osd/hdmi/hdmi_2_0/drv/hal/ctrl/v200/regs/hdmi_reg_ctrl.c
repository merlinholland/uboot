// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_reg_ctrl.h"
#include "hdmi_product_define.h"

volatile tx_ctrl_reg_type *g_tx_ctrl_all_reg = NULL;

td_s32 hdmi_tx_ctrl_reg_type_init(td_void)
{
    g_tx_ctrl_all_reg = (volatile tx_ctrl_reg_type *)(HDMI_TX_BASE_ADDR);

    return TD_SUCCESS;
}

td_void hdmi_tx_ctrl_reg_type_deinit(td_void)
{
    if (g_tx_ctrl_all_reg != NULL) {
        g_tx_ctrl_all_reg = NULL;
    }
    return;
}

td_s32 hdmi_tx_channel_reg_vid_bypass_sel_set(td_u32 reg_vid_bypass_sel)
{
    td_u32 *reg_addr = NULL;
    tx_channel_sel tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->channel_sel.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_vid_bypass_sel = reg_vid_bypass_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tx_channel_reg_vid_bypass_sel_get(td_void)
{
    td_u32 *reg_addr = NULL;
    tx_channel_sel tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->channel_sel.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.reg_vid_bypass_sel;
}

td_s32 hdmi_tx_hdmi_srst_req_set(td_u32 tx_hdmi_srst_req)
{
    td_u32 *reg_addr = NULL;
    tx_pwd_rst_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->tx_rst_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.tx_hdmi_srst_req = tx_hdmi_srst_req;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tx_pwd_srst_req_set(td_u32 tx_pwd_srst_req)
{
    td_u32 *reg_addr = NULL;
    tx_pwd_rst_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->tx_rst_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.tx_pwd_srst_req = tx_pwd_srst_req;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vidpath_dout_clk_sel_set(td_u32 vidpath_dout_clk_sel)
{
    td_u32 *reg_addr = NULL;
    tx_pwd_rst_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->tx_rst_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vidpath_dout_clk_sel = vidpath_dout_clk_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_yuv_rgb_cfg_reg_set(td_u32 reg_yuv_rgb_cfg)
{
    td_u32 *reg_addr = NULL;
    yuv_rgb_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->yuv_rgb.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_yuv_rgb_cfg = reg_yuv_rgb_cfg;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_fifo_wdata_set(td_u32 pwd_fifo_data_in)
{
    td_u32 *reg_addr = NULL;
    pwd_fifo_wdata tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_wdata.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_fifo_data_in = pwd_fifo_data_in;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_data_out_cnt_set(td_u32 pwd_data_out_cnt)
{
    td_u32 *reg_addr = NULL;
    pwd_data_cnt tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_cnt.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_data_out_cnt = pwd_data_out_cnt;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_slave_seg_set(td_u32 pwd_slave_seg)
{
    td_u32 *reg_addr = NULL;
    pwd_slave_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_slave.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_slave_seg = pwd_slave_seg;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_slave_offset_set(td_u32 pwd_slave_offset)
{
    td_u32 *reg_addr = NULL;
    pwd_slave_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_slave.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_slave_offset = pwd_slave_offset;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_slave_addr_set(td_u32 pwd_slave_addr)
{
    td_u32 *reg_addr = NULL;
    pwd_slave_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_slave.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_slave_addr = pwd_slave_addr;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_mst_cmd_set(td_u32 pwd_cmd)
{
    td_u32 *reg_addr = NULL;
    pwd_mst_cmd tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_cmd.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.pwd_mst_cmd = pwd_cmd;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ddc_mst_arb_req_set(td_u32 cpu_ddc_req)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_arb_req tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->ddc_arb_req.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cpu_ddc_req = cpu_ddc_req;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_pwd_fifo_rdata_get(td_void)
{
    td_u32 *reg_addr = NULL;
    pwd_fifo_rdata tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_rdata.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.pwd_fifo_data_out;
}

td_u32 hdmi_pwd_fifo_data_cnt_get(td_void)
{
    td_u32 *reg_addr = NULL;
    pwd_data_cnt tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_cnt.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.pwd_fifo_data_cnt;
}

td_u32 hdmi_pwd_fifo_empty_get(td_void)
{
    td_u32 *reg_addr = NULL;
    pwd_mst_state tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_state.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.pwd_fifo_empty;
}

td_u32 hdmi_pwd_i2c_in_prog_get(td_void)
{
    td_u32 *reg_addr = NULL;
    pwd_mst_state tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->pwd_state.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.pwd_i2c_in_prog;
}

td_s32 hdmi_ddc_mst_arb_ack_cpu_ddc_req_ack_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_arb_ack tmp;

    reg_addr = (td_u32 *)&(g_tx_ctrl_all_reg->ddc_arb_ack.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.cpu_ddc_req_ack;
}

td_u32 hdmi_tx_ctrl_read(td_u32 tx_crl_addr)
{
    td_char *addr = NULL;
    td_u32  reg_val = 0;

    if (g_tx_ctrl_all_reg != NULL) {
        addr = (td_char *)g_tx_ctrl_all_reg + tx_crl_addr;
        reg_val = hdmi_tx_reg_read((td_u32 *)addr);
    }

    return reg_val;
}

td_s32 hdmi_tx_ctrl_write(td_u32 tx_crl_addr, td_u32 val)
{
    td_char *addr = NULL;

    if (g_tx_ctrl_all_reg != NULL) {
        addr = (td_char *)g_tx_ctrl_all_reg + tx_crl_addr;
        hdmi_tx_reg_write((td_u32 *)addr, val);
    }

    return TD_SUCCESS;
}

