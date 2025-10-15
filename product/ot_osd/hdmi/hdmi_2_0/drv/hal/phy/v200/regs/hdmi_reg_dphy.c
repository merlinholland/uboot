// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "hdmi_reg_dphy.h"
#include "hdmi_product_define.h"

static volatile hdmitx21_dphy_regs_type *g_hdmitx_dphy_all_reg = TD_NULL;

td_s32 hdmi_hdmitx_phy_reg_init(td_void)
{
    g_hdmitx_dphy_all_reg = (hdmitx21_dphy_regs_type *)(HDMI_TX_PHY_ADDR);
    if (g_hdmitx_dphy_all_reg == TD_NULL) {
        hdmi_err("ioremap_nocache g_hdmitx_dphy_all_reg failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 hdmi_hdmitx_phy_reg_deinit(td_void)
{
    if (g_hdmitx_dphy_all_reg != TD_NULL) {
        g_hdmitx_dphy_all_reg = TD_NULL;
    }
    return TD_SUCCESS;
}

static td_void hdmi21_txreg_write(td_u32 *reg_addr, td_u32 value)
{
    *(volatile td_u32 *)reg_addr = value;
    return;
}

static td_u32 hdmi21_txreg_read(const td_u32 *reg_addr)
{
    return *(volatile td_u32 *)(reg_addr);
}

td_s32 hdmi_phy_csen_stb_cs_en_set(td_u32 stb_cs_en)
{
    td_u32 *reg_addr = NULL;
    phy_csen tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_phy_csen.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_cs_en = stb_cs_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_phy_wr_stb_wen_set(td_u32 stb_wen)
{
    td_u32 *reg_addr = NULL;
    phy_wr tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_phy_wr.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_wen = stb_wen;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_resetn_resetn_set(td_u32 reset_n)
{
    td_u32 *reg_addr = NULL;
    resetn tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_resetn.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.reset = reset_n;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_resetn_resetn_get(td_void)
{
    td_u32 *reg_addr = NULL;
    resetn tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_resetn.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.reset;
}

td_s32 hdmi_fdsrcparam_src_enable_set(td_u32 src_enable)
{
    td_u32 *reg_addr = NULL;
    fd_src_param tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fd_src_param.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_enable = src_enable;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_phy_wdata_stb_wdata_set(td_u32 stb_wdata)
{
    td_u32 *reg_addr = NULL;
    phy_wdata tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_phy_wdata.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_wdata = stb_wdata;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcgset_p_fcg_lock_en_set(td_u32 fcg_lock_en)
{
    td_u32 *reg_addr = NULL;
    fcgset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcgset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcg_lock_en = fcg_lock_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_phy_addr_stb_addr_set(td_u32 stb_addr)
{
    td_u32 *reg_addr = NULL;
    phy_addr tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_phy_addr.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_addr = stb_addr;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_phy_rdata_stb_rdata_get(td_void)
{
    td_u32 *reg_addr = NULL;
    phy_rdata tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_phy_rdata.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.stb_rdata;
}

td_s32 hdmi_fdsrcparam_src_lock_cnt_set(td_u32 src_lock_cnt)
{
    td_u32 *reg_addr = NULL;
    fd_src_param tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fd_src_param.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_lock_cnt = src_lock_cnt;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcparam_src_lock_val_set(td_u32 src_lock_val)
{
    td_u32 *reg_addr = NULL;
    fd_src_param tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fd_src_param.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_lock_val = src_lock_val;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcfreq_src_freq_ext_set(td_u32 src_freq_ext)
{
    td_u32 *reg_addr = NULL;
    fdsrcfreq tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcfreq.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_freq_ext = src_freq_ext;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcfreq_unused_2_set(td_u32 fdsrcfreq_unused_2)
{
    td_u32 *reg_addr = NULL;
    fdsrcfreq tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcfreq.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fdsrcfreq_unused_2 = fdsrcfreq_unused_2;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_txfifoset0_unused_set(td_u32 txfifoset0_unused)
{
    td_u32 *reg_addr = NULL;
    txfifoset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->tx_fifo_set0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.txfifoset0_unused = txfifoset0_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcfreq_src_freq_opt_set(td_u32 src_freq_opt)
{
    td_u32 *reg_addr = NULL;
    fdsrcfreq tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcfreq.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_freq_opt = src_freq_opt;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcfreq_unused_1_set(td_u32 fdsrcfreq_unused_1)
{
    td_u32 *reg_addr = NULL;
    fdsrcfreq tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcfreq.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fdsrcfreq_unused_1 = fdsrcfreq_unused_1;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcfreq_src_cnt_opt_set(td_u32 src_cnt_opt)
{
    td_u32 *reg_addr = NULL;
    fdsrcfreq tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcfreq.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.src_cnt_opt = src_cnt_opt;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcres_src_cnt_out_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fdsrcres tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcres.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.src_cnt_out;
}

td_s32 hdmi_fdsrcres_src_det_stat_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fdsrcres tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdsrcres.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.src_det_stat;
}

td_s32 hdmi_ctset0_unused_set(td_u32 ctset0_unused)
{
    td_u32 *reg_addr = NULL;
    ctset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.ctset0_unused = ctset0_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ctset0_i_run_set(td_u32 run)
{
    td_u32 *reg_addr = NULL;
    ctset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_run = run;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ctset0_i_enable_set(td_u32 enable)
{
    td_u32 *reg_addr = NULL;
    ctset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_enable = enable;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccntr0_i_ref_cnt_len_set(td_u32 ref_cnt_len)
{
    td_u32 *reg_addr = NULL;
    fccntr0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccntr0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ref_cnt_len = ref_cnt_len;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccntr0_i_vco_end_wait_len_set(td_u32 vco_end_wait_len)
{
    td_u32 *reg_addr = NULL;
    fccntr0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccntr0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_vco_end_wait_len = vco_end_wait_len;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccntr0_i_vco_st_wait_len_set(td_u32 vco_st_wait_len)
{
    td_u32 *reg_addr = NULL;
    fccntr0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccntr0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_vco_st_wait_len = vco_st_wait_len;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_unused_set(td_u32 fcopt_unused)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcopt_unused = fcopt_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_i_ct_idx_sel_set(td_u32 ct_idx_sel)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ct_idx_sel = ct_idx_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_unused_2_set(td_u32 fcopt_unused_2)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcopt_unused_2 = fcopt_unused_2;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_i_ct_en_set(td_u32 ct_en)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ct_en = ct_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_unused_1_set(td_u32 fcopt_unused_1)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcopt_unused_1 = fcopt_unused_1;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_i_ct_mode_set(td_u32 ct_mode)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ct_mode = ct_mode;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_i_clkdet_sel_set(td_u32 clkdet_sel)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_clkdet_sel = clkdet_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcopt_i_ct_sel_set(td_u32 ct_sel)
{
    td_u32 *reg_addr = NULL;
    fcopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ct_sel = ct_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcstat_busy_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fcstat tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcstat.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.busy;
}

td_s32 hdmi_fcdstepset_up_sampler_ratio_sel_set(td_u32 sampler_ratio_sel)
{
    td_u32 *reg_addr = NULL;
    fcdstepset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcdstepset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.up_sampler_ratio_sel = sampler_ratio_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcdstepset_unused_set(td_u32 fcdstepset_unused)
{
    td_u32 *reg_addr = NULL;
    fcdstepset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcdstepset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcdstepset_unused = fcdstepset_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcdstepset_p_divn_h20_set(td_u32 divn_h20)
{
    td_u32 *reg_addr = NULL;
    fcdstepset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcdstepset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.divn_h20 = divn_h20;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccntr1_i_ref_cnt_set(td_u32 ref_cnt)
{
    td_u32 *reg_addr = NULL;
    fccntr1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccntr1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ref_cnt = ref_cnt;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset0_p_contin_upd_step_set(td_u32 contin_upd_step)
{
    td_u32 *reg_addr = NULL;
    fccontinset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_step = contin_upd_step;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset0_unused_set(td_u32 fccontinset0_unused)
{
    td_u32 *reg_addr = NULL;
    fccontinset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fccontinset0_unused = fccontinset0_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset0_p_contin_upd_pol_set(td_u32 contin_upd_pol)
{
    td_u32 *reg_addr = NULL;
    fccontinset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_pol = contin_upd_pol;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset0_p_contin_upd_opt_set(td_u32 contin_upd_opt)
{
    td_u32 *reg_addr = NULL;
    fccontinset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_opt = contin_upd_opt;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset0_p_contin_upd_en_set(td_u32 contin_upd_en)
{
    td_u32 *reg_addr = NULL;
    fccontinset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_en = contin_upd_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset1_p_contin_upd_time_set(td_u32 contin_upd_time)
{
    td_u32 *reg_addr = NULL;
    fccontinset1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_time = contin_upd_time;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset1_p_contin_upd_rate_set(td_u32 contin_upd_rate)
{
    td_u32 *reg_addr = NULL;
    fccontinset1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_rate = contin_upd_rate;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset2_p_contin_upd_th_up_set(td_u32 contin_upd_th_up)
{
    td_u32 *reg_addr = NULL;
    fccontinset2 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset2.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_th_up = contin_upd_th_up;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset2_unused_set(td_u32 fccontinset2_unused)
{
    td_u32 *reg_addr = NULL;
    fccontinset2 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset2.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fccontinset2_unused = fccontinset2_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fccontinset2_p_contin_upd_th_dn_set(td_u32 contin_upd_th_dn)
{
    td_u32 *reg_addr = NULL;
    fccontinset2 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fccontinset2.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.contin_upd_th_dn = contin_upd_th_dn;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset0_en_sdm_set(td_u32 sdm)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sdm = sdm;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_hdmi_mode_reg_hdmi_mode_en_set(td_u32 reg_hdmi_mode_en)
{
    td_u32 *reg_addr = NULL;
    hdmi_mode tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_hdmi_mode.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.reg_hdmi_mode_en = reg_hdmi_mode_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset0_en_sdm_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.sdm;
}

td_s32 hdmi_fdivset0_en_mod_set(td_u32 mod)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod = mod;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset0_en_mod_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.mod;
}

td_s32 hdmi_fdivset0_en_ctrl_set(td_u32 ctrl)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.ctrl = ctrl;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset0_en_ctrl_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.ctrl;
}

td_s32 hdmi_fdivset0_init_set(td_u32 init)
{
    td_u32 *reg_addr = NULL;
    fdivset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.init = init;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset3_mod_n_set(td_u32 mod_n)
{
    td_u32 *reg_addr = NULL;
    fdivset3 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset3.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod_n = mod_n;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset3_mod_t_set(td_u32 mod_t)
{
    td_u32 *reg_addr = NULL;
    fdivset3 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset3.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod_t = mod_t;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset3_mod_len_set(td_u32 mod_len)
{
    td_u32 *reg_addr = NULL;
    fdivset3 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset3.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod_len = mod_len;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivset4_mod_d_set(td_u32 mod_d)
{
    td_u32 *reg_addr = NULL;
    fdivset4 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivset4.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod_d = mod_d;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdsrcparam_unused_set(td_u32 fdsrcparam_unused)
{
    td_u32 *reg_addr = NULL;
    fd_src_param tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fd_src_param.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fdsrcparam_unused = fdsrcparam_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivstat1_i_fdiv_in_set(td_u32 fdiv_in)
{
    td_u32 *reg_addr = NULL;
    fdivstat1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivstat1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_fdiv_in = fdiv_in;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivmanual_i_mdiv_set(td_u32 mdiv)
{
    td_u32 *reg_addr = NULL;
    fdivmanual tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivmanual.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_mdiv = mdiv;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_frl_clock_set(td_u32 sw_reset_frl_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_frl_clock = sw_reset_frl_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fdivmanual_i_manual_en_set(td_u32 manual_en)
{
    td_u32 *reg_addr = NULL;
    fdivmanual tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fdivmanual.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_manual_en = manual_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_stb_delay1_set(td_u32 stb_delay1)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_delay1 = stb_delay1;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_refclksel_i_ref_clk_sel_set(td_u32 ref_clk_sel)
{
    td_u32 *reg_addr = NULL;
    refclksel tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_refclksel.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_ref_clk_sel = ref_clk_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcgset_p_fcg_dither_en_set(td_u32 fcg_dither_en)
{
    td_u32 *reg_addr = NULL;
    fcgset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcgset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcg_dither_en = fcg_dither_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcgset_p_fcg_dlf_en_set(td_u32 fcg_dlf_en)
{
    td_u32 *reg_addr = NULL;
    fcgset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcgset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcg_dlf_en = fcg_dlf_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcgset_p_fcg_en_set(td_u32 fcg_en)
{
    td_u32 *reg_addr = NULL;
    fcgset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcgset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fcg_en = fcg_en;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_txfifoset0_p_enable_h20_set(td_u32 enable_h20)
{
    td_u32 *reg_addr = NULL;
    txfifoset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->tx_fifo_set0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.enable_h20 = enable_h20;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_txfifoset0_p_pr_en_h20_set(td_u32 pr_en_h20)
{
    td_u32 *reg_addr = NULL;
    txfifoset0 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->tx_fifo_set0.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.pr_en_h20 = pr_en_h20;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_txdataoutsel_p_ch_out_sel_set(td_u32 ch_out_sel)
{
    td_u32 *reg_addr = NULL;
    txdataoutsel tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->tx_data_out_sel.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.ch_out_sel = ch_out_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_hsset_p_hsset_set(td_u32 hs_set)
{
    td_u32 *reg_addr = NULL;
    hsset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_hsset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.hsset = hs_set;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_global_reset_set(td_u32 global_reset)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.global_reset = global_reset;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_unused_set(td_u32 swreset_unused)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.swreset_unused = swreset_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_dac_clock_gat_set(td_u32 dac_clock_gat)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.dac_clock_gat = dac_clock_gat;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_stb_delay2_set(td_u32 stb_delay2)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_delay2 = stb_delay2;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_stb_cs_sel_set(td_u32 stb_cs_sel)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_cs_sel = stb_cs_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_stb_acc_sel_set(td_u32 stb_acc_sel)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_acc_sel = stb_acc_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_stb_delay0_set(td_u32 stb_delay0)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.stb_delay0 = stb_delay0;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_p_fd_clk_sel_set(td_u32 fd_clk_sel)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fd_clk_sel = fd_clk_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_p_refclk_sel_set(td_u32 refclk_sel)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.refclk_sel = refclk_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_p_ctman_set(td_u32 ctman)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.ctman = ctman;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_stbopt_req_length_set(td_u32 req_length)
{
    td_u32 *reg_addr = NULL;
    stbopt tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_stbopt.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.req_length = req_length;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_p_fdivclk_sel_set(td_u32 fdivclk_sel)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.fdivclk_sel = fdivclk_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_mod_div_val_set(td_u32 mod_div_val)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.mod_div_val = mod_div_val;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_clkset_p_modclk_sel_set(td_u32 modclk_sel)
{
    td_u32 *reg_addr = NULL;
    clkset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_clkset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.modclk_sel = modclk_sel;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ctset1_i_mpll_fcon_set(td_u32 mpll_fcon)
{
    td_u32 *reg_addr = NULL;
    ctset1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_mpll_fcon = mpll_fcon;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ctset1_i_mpll_ctlck_set(td_u32 mpll_ctlck)
{
    td_u32 *reg_addr = NULL;
    ctset1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.i_mpll_ctlck = mpll_ctlck;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ctset1_ctset1_unused_set(td_u32 ctset1_unused)
{
    td_u32 *reg_addr = NULL;
    ctset1 tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_ctset1.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.ctset1_unused = ctset1_unused;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_fcstat_error_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fcstat tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcstat.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.error;
}

td_s32 hdmi_fcstat_done_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fcstat tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcstat.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.done;
}

td_s32 hdmi_fcstat_clk_ok_get(td_void)
{
    td_u32 *reg_addr = NULL;
    fcstat tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_fcstat.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    return tmp.bits.clk_ok;
}

td_s32 hdmi_swreset_sw_reset_mod_clock_set(td_u32 sw_reset_mod_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_mod_clock = sw_reset_mod_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_tmds_clock_set(td_u32 sw_reset_tmds_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_tmds_clock = sw_reset_tmds_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_mpll_clock_set(td_u32 sw_reset_mpll_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_mpll_clock = sw_reset_mpll_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_nco_clock_set(td_u32 sw_reset_nco_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_nco_clock = sw_reset_nco_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_fd_clock_set(td_u32 sw_reset_fd_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_fd_clock = sw_reset_fd_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_mod_and_mpll_clock_set(td_u32 mpll_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_mod_and_mpll_clock = mpll_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_mod_and_nco_clock_set(td_u32 nco_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_mod_and_nco_clock = nco_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_mod_and_fd_clock_set(td_u32 fd_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_mod_and_fd_clock = fd_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_hsfifo_clock_set(td_u32 hsfifo_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_hsfifo_clock = hsfifo_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_txfifo_clock_set(td_u32 txfifo_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_txfifo_clock = txfifo_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_data_clock_set(td_u32 data_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_data_clock = data_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_hs_clock_set(td_u32 hs_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_hs_clock = hs_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_pllref_clock_set(td_u32 pllref_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_pllref_clock = pllref_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_sw_reset_dac_clock_set(td_u32 dac_clock)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.sw_reset_dac_clock = dac_clock;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_swreset_up_sample_fifo_clock_swrst_set(td_u32 clock_swrst)
{
    td_u32 *reg_addr = NULL;
    swreset tmp;

    reg_addr = (td_u32 *)&(g_hdmitx_dphy_all_reg->reg_swreset.u32);
    tmp.u32 = hdmi21_txreg_read(reg_addr);
    tmp.bits.up_sample_fifo_clock_swrst = clock_swrst;
    hdmi21_txreg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

