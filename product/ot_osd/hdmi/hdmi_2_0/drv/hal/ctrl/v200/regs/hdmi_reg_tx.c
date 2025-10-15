// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "hdmi_reg_tx.h"
#include "hdmi_product_define.h"

volatile tx_hdmi_regs *g_tx_hdmi_all_reg = NULL;

td_s32 hdmi_tx_reg_type_init(td_void)
{
    g_tx_hdmi_all_reg = (volatile tx_hdmi_regs *)(HDMI_TX_BASE_ADDR + HDMI_TX_BASE_ADDR_HDMITX);

    return TD_SUCCESS;
}

td_void hdmi_tx_reg_type_deinit(td_void)
{
    if (g_tx_hdmi_all_reg != NULL) {
        g_tx_hdmi_all_reg = TD_NULL;
    }

    return;
}

td_s32 hdmi_avi_pkt_header_hb_set(td_u32 hb0, td_u32 hb1, td_u32 hb2)
{
    td_u32 *reg_addr = NULL;
    avi_pkt_header tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_packet_header.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_pkt_hb2 = hb2;
    tmp.bits.avi_pkt_hb1 = hb1;
    tmp.bits.avi_pkt_hb0 = hb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);
    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt0_l_pb_set(td_u32 avi_pkt0_pb0, td_u32 avi_pkt0_pb1, td_u32 avi_pkt0_pb2,
    td_u32 avi_pkt0_pb3)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt0_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck0_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt0_pb3 = avi_pkt0_pb3;
    tmp.bits.avi_sub_pkt0_pb2 = avi_pkt0_pb2;
    tmp.bits.avi_sub_pkt0_pb1 = avi_pkt0_pb1;
    tmp.bits.avi_sub_pkt0_pb0 = avi_pkt0_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt0_h_pb_set(td_u32 avi_pkt0_pb4, td_u32 avi_pkt0_pb5, td_u32 avi_pkt0_pb6)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt0_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck0_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt0_pb6 = avi_pkt0_pb6;
    tmp.bits.avi_sub_pkt0_pb5 = avi_pkt0_pb5;
    tmp.bits.avi_sub_pkt0_pb4 = avi_pkt0_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt1_l_pb_set(td_u32 avi_pkt1_pb0, td_u32 avi_pkt1_pb1, td_u32 avi_pkt1_pb2,
    td_u32 avi_pkt1_pb3)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt1_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck1_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt1_pb3 = avi_pkt1_pb3;
    tmp.bits.avi_sub_pkt1_pb2 = avi_pkt1_pb2;
    tmp.bits.avi_sub_pkt1_pb1 = avi_pkt1_pb1;
    tmp.bits.avi_sub_pkt1_pb0 = avi_pkt1_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt1_h_pb_set(td_u32 avi_pkt1_pb4, td_u32 avi_pkt1_pb5, td_u32 avi_pkt1_pb6)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt1_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck1_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt1_pb6 = avi_pkt1_pb6;
    tmp.bits.avi_sub_pkt1_pb5 = avi_pkt1_pb5;
    tmp.bits.avi_sub_pkt1_pb4 = avi_pkt1_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt2_l_pb_set(td_u32 avi_pkt2_pb0, td_u32 avi_pkt2_pb1, td_u32 avi_pkt2_pb2,
    td_u32 avi_pkt2_pb3)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt2_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck2_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt2_pb3 = avi_pkt2_pb3;
    tmp.bits.avi_sub_pkt2_pb2 = avi_pkt2_pb2;
    tmp.bits.avi_sub_pkt2_pb1 = avi_pkt2_pb1;
    tmp.bits.avi_sub_pkt2_pb0 = avi_pkt2_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt2_h_pb_set(td_u32 avi_pkt2_pb4, td_u32 avi_pkt2_pb5, td_u32 avi_pkt2_pb6)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt2_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck2_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt2_pb6 = avi_pkt2_pb6;
    tmp.bits.avi_sub_pkt2_pb5 = avi_pkt2_pb5;
    tmp.bits.avi_sub_pkt2_pb4 = avi_pkt2_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt3_l_pb_set(td_u32 avi_pkt3_pb0, td_u32 avi_pkt3_pb1, td_u32 avi_pkt3_pb2,
    td_u32 avi_pkt3_pb3)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt3_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck3_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt3_pb3 = avi_pkt3_pb3;
    tmp.bits.avi_sub_pkt3_pb2 = avi_pkt3_pb2;
    tmp.bits.avi_sub_pkt3_pb1 = avi_pkt3_pb1;
    tmp.bits.avi_sub_pkt3_pb0 = avi_pkt3_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avi_sub_pkt3_h_pb_set(td_u32 avi_pkt3_pb4, td_u32 avi_pkt3_pb5, td_u32 avi_pkt3_pb6)
{
    td_u32 *reg_addr = NULL;
    avi_sub_pkt3_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_pck3_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.avi_sub_pkt3_pb6 = avi_pkt3_pb6;
    tmp.bits.avi_sub_pkt3_pb5 = avi_pkt3_pb5;
    tmp.bits.avi_sub_pkt3_pb4 = avi_pkt3_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_pkt_header_hb_set(td_u32 hb0, td_u32 hb1, td_u32 hb2)
{
    td_u32 *reg_addr = NULL;
    vsif_pkt_header tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_packet_header.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_pkt_hb2 = hb2;
    tmp.bits.vsif_pkt_hb1 = hb1;
    tmp.bits.vsif_pkt_hb0 = hb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt0_l_pb_set(td_u32 vsif_pkt0_pb0, td_u32 vsif_pkt0_pb1, td_u32 vsif_pkt0_pb2,
    td_u32 vsif_pkt0_pb3)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt0_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck0_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt0_pb3 = vsif_pkt0_pb3;
    tmp.bits.vsif_sub_pkt0_pb2 = vsif_pkt0_pb2;
    tmp.bits.vsif_sub_pkt0_pb1 = vsif_pkt0_pb1;
    tmp.bits.vsif_sub_pkt0_pb0 = vsif_pkt0_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt0_h_pb_set(td_u32 vsif_pkt0_pb4, td_u32 vsif_pkt0_pb5, td_u32 vsif_pkt0_pb6)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt0_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck0_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt0_pb6 = vsif_pkt0_pb6;
    tmp.bits.vsif_sub_pkt0_pb5 = vsif_pkt0_pb5;
    tmp.bits.vsif_sub_pkt0_pb4 = vsif_pkt0_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt1_l_pb_set(td_u32 vsif_pkt1_pb0, td_u32 vsif_pkt1_pb1, td_u32 vsif_pkt1_pb2,
    td_u32 vsif_pkt1_pb3)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt1_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck1_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt1_pb3 = vsif_pkt1_pb3;
    tmp.bits.vsif_sub_pkt1_pb2 = vsif_pkt1_pb2;
    tmp.bits.vsif_sub_pkt1_pb1 = vsif_pkt1_pb1;
    tmp.bits.vsif_sub_pkt1_pb0 = vsif_pkt1_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt1_h_pb_set(td_u32 vsif_pkt1_pb4, td_u32 vsif_pkt1_pb5, td_u32 vsif_pkt1_pb6)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt1_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck1_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt1_pb6 = vsif_pkt1_pb6;
    tmp.bits.vsif_sub_pkt1_pb5 = vsif_pkt1_pb5;
    tmp.bits.vsif_sub_pkt1_pb4 = vsif_pkt1_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt2_l_pb_set(td_u32 vsif_pkt2_pb0, td_u32 vsif_pkt2_pb1, td_u32 vsif_pkt2_pb2,
    td_u32 vsif_pkt2_pb3)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt2_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck2_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt2_pb3 = vsif_pkt2_pb3;
    tmp.bits.vsif_sub_pkt2_pb2 = vsif_pkt2_pb2;
    tmp.bits.vsif_sub_pkt2_pb1 = vsif_pkt2_pb1;
    tmp.bits.vsif_sub_pkt2_pb0 = vsif_pkt2_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt2_h_pb_set(td_u32 vsif_pkt2_pb4, td_u32 vsif_pkt2_pb5, td_u32 vsif_pkt2_pb6)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt2_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck2_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt2_pb6 = vsif_pkt2_pb6;
    tmp.bits.vsif_sub_pkt2_pb5 = vsif_pkt2_pb5;
    tmp.bits.vsif_sub_pkt2_pb4 = vsif_pkt2_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt3_l_pb_set(td_u32 vsif_pkt3_pb0, td_u32 vsif_pkt3_pb1, td_u32 vsif_pkt3_pb2,
    td_u32 vsif_pkt3_pb3)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt3_l tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck3_l.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt3_pb3 = vsif_pkt3_pb3;
    tmp.bits.vsif_sub_pkt3_pb2 = vsif_pkt3_pb2;
    tmp.bits.vsif_sub_pkt3_pb1 = vsif_pkt3_pb1;
    tmp.bits.vsif_sub_pkt3_pb0 = vsif_pkt3_pb0;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_vsif_sub_pkt3_h_pb_set(td_u32 vsif_pkt3_pb4, td_u32 vsif_pkt3_pb5, td_u32 vsif_pkt3_pb6)
{
    td_u32 *reg_addr = NULL;
    vsif_sub_pkt3_h tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_pck3_h.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.vsif_sub_pkt3_pb6 = vsif_pkt3_pb6;
    tmp.bits.vsif_sub_pkt3_pb5 = vsif_pkt3_pb5;
    tmp.bits.vsif_sub_pkt3_pb4 = vsif_pkt3_pb4;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_avi_rpt_en_set(td_u32 cea_avi_rpt_en)
{
    td_u32 *reg_addr = NULL;
    cea_avi_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_avi_rpt_en = cea_avi_rpt_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_avi_en_set(td_u32 cea_avi_en)
{
    td_u32 *reg_addr = NULL;
    cea_avi_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_avi_en = cea_avi_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_gen_en_set(td_u32 cea_gen_en)
{
    td_u32 *reg_addr = NULL;
    cea_gen_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->gen_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_gen_en = cea_gen_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_cp_rpt_cnt_set(td_u32 cea_cp_rpt_cnt)
{
    td_u32 *reg_addr = NULL;
    cea_cp_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_cp_rpt_cnt = cea_cp_rpt_cnt;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_cp_rpt_en_set(td_u32 cea_cp_rpt_en)
{
    td_u32 *reg_addr = NULL;
    cea_cp_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_cp_rpt_en = cea_cp_rpt_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_cp_en_set(td_u32 cea_cp_en)
{
    td_u32 *reg_addr = NULL;
    cea_cp_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_cp_en = cea_cp_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_vsif_rpt_en_set(td_u32 cea_vsif_rpt_en)
{
    td_u32 *reg_addr = NULL;
    cea_vsif_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_vsif_rpt_en = cea_vsif_rpt_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cea_vsif_en_set(td_u32 cea_vsif_en)
{
    td_u32 *reg_addr = NULL;
    cea_vsif_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->vsif_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cea_vsif_en = cea_vsif_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avmixer_config_dc_pkt_en_set(td_u32 dc_pkt_en)
{
    td_u32 *reg_addr = NULL;
    avmixer_config tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avmixer_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.dc_pkt_en = dc_pkt_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_avmixer_config_hdmi_mode_set(td_u32 hdmi_mode)
{
    td_u32 *reg_addr = NULL;
    avmixer_config tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avmixer_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.hdmi_mode = hdmi_mode;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cp_clr_avmute_set(td_u32 cp_clr_avmute)
{
    td_u32 *reg_addr = NULL;
    cp_pkt_avmute tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_avmute.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cp_clr_avmute = cp_clr_avmute;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_cp_set_avmute_set(td_u32 cp_set_avmute)
{
    td_u32 *reg_addr = NULL;
    cp_pkt_avmute tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_avmute.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.cp_set_avmute = cp_set_avmute;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_enc_bypass_set(td_u32 enc_bypass)
{
    td_u32 *reg_addr = NULL;
    hdmi_enc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->enc_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.enc_bypass = enc_bypass;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_enc_scr_on_set(td_u32 enc_scr_on)
{
    td_u32 *reg_addr = NULL;
    hdmi_enc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->enc_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.enc_scr_on = enc_scr_on;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_enc_hdmi2_on_set(td_u32 enc_hdmi2_on)
{
    td_u32 *reg_addr = NULL;
    hdmi_enc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->enc_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.enc_hdmi2_on = enc_hdmi2_on;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_u32 hdmi_cea_avi_en_get(td_void)
{
    td_u32 *reg_addr = NULL;
    cea_avi_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avi_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.cea_avi_en;
}

td_u32 hdmi_cea_cp_rpt_en_get(td_void)
{
    td_u32 *reg_addr = NULL;
    cea_cp_cfg tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.cea_cp_rpt_en;
}

td_s32 hdmi_avmixer_config_null_pkt_en_set(td_u32 null_pkt_en)
{
    td_u32 *reg_addr = NULL;
    avmixer_config tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avmixer_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.null_pkt_en = null_pkt_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_u32 hdmi_avmixer_config_hdmi_mode_get(td_void)
{
    td_u32 *reg_addr = NULL;
    avmixer_config tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->avmixer_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.hdmi_mode;
}

td_u32 hdmi_cp_set_avmute_get(td_void)
{
    td_u32 *reg_addr = NULL;
    cp_pkt_avmute tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->cp_avmute.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.cp_set_avmute;
}

td_u32 hdmi_enc_scr_on_get(td_void)
{
    td_u32 *reg_addr = NULL;
    hdmi_enc_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->enc_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.enc_scr_on;
}

td_s32 hdmi_ctl_type_config_set(td_u32 ctl_config)
{
    td_u32 *reg_addr = NULL;
    ctl_type_config tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->ctl_type_cfg.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.ctl_type_config = ctl_config;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_u32 hdmi_htotal_hw_get(td_void)
{
    td_u32 *reg_addr = NULL;
    htotal_hw tmp;

    reg_addr = (td_u32 *)&(g_tx_hdmi_all_reg->htotal.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.htotal_hw;
}

