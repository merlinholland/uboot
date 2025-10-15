// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_reg_video_path.h"
#include "hdmi_product_define.h"

volatile video_path_reg_type *g_video_path_all_reg = NULL;

td_s32 hdmi_tx_videopath_reg_type_init(td_void)
{
    g_video_path_all_reg = (volatile video_path_reg_type*)(HDMI_TX_BASE_ADDR + HDMI_TX_BASE_ADDR_VIDEO);

    return TD_SUCCESS;
}

td_void hdmi_tx_videopath_reg_type_deinit(td_void)
{
    if (g_video_path_all_reg != NULL) {
        g_video_path_all_reg = TD_NULL;
    }

    return;
}

td_s32 hdmi_data_align_ctrl_reg_vert_cbcr_sel_set(td_u32 reg_vert_cbcr_sel)
{
    td_u32 *reg_addr = NULL;
    video_dwsm_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dwsm_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_vert_cbcr_sel = reg_vert_cbcr_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tx_pack_fifo_ctrl_reg_fifo_manu_rst_set(td_u32 reg_fifo_manu_rst)
{
    td_u32 *reg_addr = NULL;
    tx_pack_fifo_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->tx_fifo_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_fifo_manu_rst = reg_fifo_manu_rst;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_multi_csc_ctrl_reg_csc_mode_get(td_void)
{
    td_u32 *reg_addr = NULL;
    multi_csc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->multi_csc.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.reg_csc_mode;
}

td_s32 hdmi_dither_config_mode_set(td_u32 dither_mode)
{
    td_u32 *reg_addr = NULL;
    dither_config tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dither.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.dither_mode = dither_mode;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_dither_config_rnd_byp_set(td_u32 dither_rnd_byp)
{
    td_u32 *reg_addr = NULL;
    dither_config tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dither.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.dither_rnd_byp = dither_rnd_byp;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_multi_csc_ctrl_reg_csc_mode_set(td_u32 reg_csc_mode)
{
    td_u32 *reg_addr = NULL;
    multi_csc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->multi_csc.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_csc_mode = reg_csc_mode;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_multi_csc_ctrl_reg_csc_saturate_en_set(td_u32 reg_csc_saturate_en)
{
    td_u32 *reg_addr = NULL;
    multi_csc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->multi_csc.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_csc_saturate_en = reg_csc_saturate_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_multi_csc_ctrl_reg_csc_en_set(td_u32 reg_csc_en)
{
    td_u32 *reg_addr = NULL;
    multi_csc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->multi_csc.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_csc_en = reg_csc_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dwsm_ctrl_reg_dwsm_vert_en_set(td_u32 reg_dwsm_vert_en)
{
    td_u32 *reg_addr = NULL;
    video_dwsm_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dwsm_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_dwsm_vert_en = reg_dwsm_vert_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dwsm_ctrl_reg_dwsm_hori_en_set(td_u32 reg_dwsm_hori_en)
{
    td_u32 *reg_addr = NULL;
    video_dwsm_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dwsm_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_dwsm_hori_en = reg_dwsm_hori_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dwsm_ctrl_reg_hori_filter_en_set(td_u32 reg_hori_filter_en)
{
    td_u32 *reg_addr = NULL;
    video_dwsm_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dwsm_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_hori_filter_en = reg_hori_filter_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_data_align_ctrl_reg_pxl_div_en_set(td_u32 reg_pxl_div_en)
{
    td_u32 *reg_addr = NULL;
    data_align_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->data_align.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_pxl_div_en = reg_pxl_div_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_data_align_ctrl_reg_demux_420_en_set(td_u32 reg_demux_420_en)
{
    td_u32 *reg_addr = NULL;
    data_align_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->data_align.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_demux_420_en = reg_demux_420_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dmux_ctrl_reg_inver_sync_set(td_u32 reg_inver_sync)
{
    td_u32 *reg_addr = NULL;
    video_dmux_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dmux_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_inver_sync = reg_inver_sync;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dmux_ctrl_reg_syncmask_en_set(td_u32 reg_syncmask_en)
{
    td_u32 *reg_addr = NULL;
    video_dmux_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dmux_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_syncmask_en = reg_syncmask_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dmux_ctrl_reg_vmux_cr_sel_set(td_u32 reg_vmux_cr_sel)
{
    td_u32 *reg_addr = NULL;
    video_dmux_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dmux_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_vmux_cr_sel = reg_vmux_cr_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dmux_ctrl_reg_vmux_cb_sel_set(td_u32 reg_vmux_cb_sel)
{
    td_u32 *reg_addr = NULL;
    video_dmux_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dmux_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_vmux_cb_sel = reg_vmux_cb_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_video_dmux_ctrl_reg_vmux_y_sel_set(td_u32 reg_vmux_y_sel)
{
    td_u32 *reg_addr = NULL;
    video_dmux_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->dmux_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_vmux_y_sel = reg_vmux_y_sel;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tx_pack_fifo_ctrl_tmds_pack_mode_set(td_u32 tmds_pack_mode)
{
    td_u32 *reg_addr = NULL;
    tx_pack_fifo_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->tx_fifo_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.tmds_pack_mode = tmds_pack_mode;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tx_pack_fifo_ctrl_tmds_pack_mode_get(td_void)
{
    td_u32 *reg_addr = NULL;
    tx_pack_fifo_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->tx_fifo_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.tmds_pack_mode;
}

td_s32 hdmi_tx_pack_fifo_ctrl_reg_fifo_auto_rst_en_set(td_u32 reg_fifo_auto_rst_en)
{
    td_u32 *reg_addr = NULL;
    tx_pack_fifo_ctrl tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->tx_fifo_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_fifo_auto_rst_en = reg_fifo_auto_rst_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tclk_lower_threshold_reg_tcnt_lower_threshold_set(td_u32 reg_tcnt_lower_threshold)
{
    td_u32 *reg_addr = NULL;
    tclk_lower_threshold tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->lower_threshold.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_tcnt_lower_threshold = reg_tcnt_lower_threshold;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_tclk_upper_threshold_reg_tcnt_upper_threshold_set(td_u32 reg_tcnt_upper_threshold)
{
    td_u32 *reg_addr = NULL;
    tclk_upper_threshold tmp;

    reg_addr = (td_u32 *)&(g_video_path_all_reg->lower_threshold.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.reg_tcnt_upper_threshold = reg_tcnt_upper_threshold;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

