// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_hal_phy.h"
#include "hdmi_reg_tx.h"
#include "hdmi_reg_dphy.h"
#include "hdmi_reg_crg.h"
#include "hdmi_reg_ctrl.h"
#include "drv_hdmi_common.h"
#include "hdmi_product_define.h"
#include "securec.h"

#define APHY_CS_0              0x1
#define APHY_CS_1              0x2
#define APHY_CS_2              0x4
#define APHY_CS_3              0x8
#define APHY_CS_4              0x10
#define APHY_CS_012            0x7
#define APHY_CS_4567           0xf0
#define APHY_CS_8              0x100
#define APHY_CS_9              0x200
#define PHY_CPIS_VAL           0x07
#define PHY_TMDS_OUTEN         0x14
#define TMDS_CLK_100M          100000
#define TMDS_CLK_165M          165000
#define TMDS_CLK_340M          340000
#define TMDS_CLK_600M          600000
#define HW_FCON_DELAY          2
#define HW_WRITE_STB_DELAY     5
#define PHY_POW_BASE_NUM       2
#define PHY_POW_INDEX_NUM      24
#define TMDS_CLK_FREQ_MUITIPLE 5
#define HDMI_THOUSAND          1000
#define MOD_MULTI_COEFFICIENT  10000
#define LEFT_OR_MASK_OFFSET_5  5
#define FCON_RST_LEN           4

volatile td_u32 *g_gophdmi21tx_dphy_reg_all_reg = TD_NULL;

typedef enum {
    APHY_OFFSET_0,
    APHY_OFFSET_1,
    APHY_OFFSET_2,
    APHY_OFFSET_3,
    APHY_OFFSET_4,
    APHY_OFFSET_5,
    APHY_OFFSET_6,
    APHY_OFFSET_7,
    APHY_OFFSET_8,
    APHY_OFFSET_9,
    APHY_OFFSET_A,
    APHY_OFFSET_B,
    APHY_OFFSET_C,
    APHY_OFFSET_D,
    APHY_OFFSET_E,
    APHY_OFFSET_F,
    APHY_OFFSET_BUTT
} aphy_offset_addr;

typedef struct {
    td_u32 clk_min;
    td_u32 clk_max;
} phy_clk_range;

typedef struct {
    phy_clk_range  phy_tmds_clk_range;
    phy_hw_spec_cfg phy_spec_cfg;
} phy_hwspec;

typedef struct {
    phy_clk_range phy_tmds_clk_range;
    td_u8 seek_value;
} phy_clk_rang_value;

typedef struct {
    td_u32 i_ref_cnt;
    td_u32 i_ref_cnt_len;
    phy_fcon_mode fcon_mode;
} phy_fcon_para;

typedef struct {
    td_u8 ref_clk_div;
    td_u8 vp_divnsel;
    td_u8 cpzs;
    td_u8 tmds_divnsel;
    td_u8 vp_mode;
    td_u8 fcd_step;
    hdmi_phy_tmds_cfg tmds_cfg;
} phy_clk_set;

typedef struct {
    td_u8  m_val;
    td_u8  n_val;
    td_u32 pll_ref_clk;
    td_u32 mn_value;
} phy_mnx;

#ifndef HDMI_FPGA_SUPPORT
static phy_mnx g_mnx_get;
#endif
static hdmi_phy_info g_hdmi_phy_info;

static phy_clk_rang_value g_phy_hw_fcd_step_set[] = {
    {{      0,  37500 }, 4 },
    {{  37500,  75000 }, 3 },
    {{  75000, 150000 }, 2 },
    {{ 150000, 300000 }, 1 },
    {{ 300000, 600000 }, 0 }
};

static phy_clk_rang_value g_phy_hw_ref_clk_div[] = {
    {{      0,  37500 }, 0 },
    {{  37500,  75000 }, 1 },
    {{  75000, 150000 }, 2 },
    {{ 150000, 300000 }, 3 },
    {{ 300000, 600000 }, 4 }
};

static phy_clk_rang_value g_phy_hw_tmds_divnsel_tab[] = {
    {{ 300000, 600000 }, 0 },
    {{ 150000, 300000 }, 1 },
    {{  75000, 150000 }, 2 },
    {{  37500,  75000 }, 3 },
    {{      0,  37500 }, 4 }
};

#ifndef HDMI_FPGA_SUPPORT
static td_void phy_hwwrite_stb1_byte(td_u32 cs, aphy_offset_addr aphy_offset, td_u32 wdata)
{
    /* CS reset */
    hdmi_phy_csen_stb_cs_en_set(TD_FALSE);
    /* WR reset */
    hdmi_phy_wr_stb_wen_set(TD_FALSE);
    /* WDATA set */
    hdmi_phy_wdata_stb_wdata_set(wdata);
    /* ADDR set */
    hdmi_phy_addr_stb_addr_set(aphy_offset);
    /* delay time */
    udelay(1);
    /* WR set */
    hdmi_phy_wr_stb_wen_set(TD_TRUE);
    /* CS set */
    hdmi_phy_csen_stb_cs_en_set(cs);
    /* delay time */
    udelay(1);
    /* CS reset */
    hdmi_phy_csen_stb_cs_en_set(TD_FALSE);
    /* WR reset */
    hdmi_phy_wr_stb_wen_set(TD_FALSE);

    return;
}

static td_void phy_hwread_stb1_byte(td_u32 cs, aphy_offset_addr aphy_offset, td_u8 *rdata)
{
    /* CS reset */
    hdmi_phy_csen_stb_cs_en_set(TD_FALSE);
    /* WR reset */
    hdmi_phy_wr_stb_wen_set(TD_FALSE);
    /* ADDR set */
    hdmi_phy_addr_stb_addr_set(aphy_offset);
    /* delay time */
    udelay(1);
    /* CS set */
    hdmi_phy_csen_stb_cs_en_set(cs);
    /* delay time */
    udelay(1);
    /* RDATA read */
    *rdata = (td_u8)hdmi_phy_rdata_stb_rdata_get();
    /* CS reset */
    hdmi_phy_csen_stb_cs_en_set(TD_FALSE);

    return;
}

static td_void phy_hwwrite_stb(td_u32 cs, aphy_offset_addr aphy_offset, td_u8 msb, td_u8 lsb, td_u32 wdata)
{
    td_u8 rdata = 0;
    td_u8 mask_value, write_value;

    if ((msb == 0x7) && (lsb == 0x0)) {
        phy_hwwrite_stb1_byte(cs, aphy_offset, wdata);
    } else {
        if (cs == (APHY_CS_012 | APHY_CS_3)) {
            phy_hwread_stb1_byte(APHY_CS_0, aphy_offset, &rdata);
        } else if (cs == APHY_CS_4567) {
            phy_hwread_stb1_byte(APHY_CS_4, aphy_offset, &rdata);
        } else {
            phy_hwread_stb1_byte(cs, aphy_offset, &rdata);
        }

        mask_value = (0xff >> (0x7 - msb)) & (0xff << lsb);
        write_value = (~mask_value & rdata) + (mask_value & (wdata << lsb));
        phy_hwwrite_stb1_byte(cs, aphy_offset, write_value);
    }

    return;
}

static td_s32 phy_hwref_clk_divset(td_u32 pixel_clk)
{
    td_u32 ref_clk_div = 0;

    if (pixel_clk < g_phy_hw_ref_clk_div[0].phy_tmds_clk_range.clk_max) {
        ref_clk_div = 0x0;
    } else if (pixel_clk >= g_phy_hw_ref_clk_div[1].phy_tmds_clk_range.clk_min &&
               pixel_clk < g_phy_hw_ref_clk_div[1].phy_tmds_clk_range.clk_max) {
        ref_clk_div = 0x1;
    } else if (pixel_clk >= g_phy_hw_ref_clk_div[2].phy_tmds_clk_range.clk_min &&
               pixel_clk < g_phy_hw_ref_clk_div[1].phy_tmds_clk_range.clk_max) {
        ref_clk_div = 0x2;
    } else if (pixel_clk >= g_phy_hw_ref_clk_div[3].phy_tmds_clk_range.clk_min &&
               pixel_clk < g_phy_hw_ref_clk_div[1].phy_tmds_clk_range.clk_max) {
        ref_clk_div = 0x3;
    } else if (pixel_clk >= g_phy_hw_ref_clk_div[4].phy_tmds_clk_range.clk_min &&
               pixel_clk < g_phy_hw_ref_clk_div[1].phy_tmds_clk_range.clk_max) {
        ref_clk_div = 0x4;
    }

    phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_2, ref_clk_div);

    return TD_SUCCESS;
}

static td_s32 phy_hwinitset(const hdmi_phy_tmds_cfg *hdmi_spec_cfg)
{
    hdmi_info("enModeCfg = %u\n", hdmi_spec_cfg->mode_cfg);
    if (hdmi_spec_cfg->mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
        /*  DDC QEC Settings */
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_0, 0x20);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_1, 0x20);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_2, 0x20);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_3, 0x20);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_4, 0x10);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_5, 0x10);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_6, 0x10);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_7, 0x10);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_8, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_4567, APHY_OFFSET_9, 0x00);
        /* PLL Settings */
#if defined(HDMI_PRODUCT_SS919V100)
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_0, 0x7f); /* ff->7f */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_1, 0x31);
        phy_hwref_clk_divset(hdmi_spec_cfg->pixel_clk);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_3, 0x78); /* 1f */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_4, 0x02);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_5, 0x13);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_6, 0x4c);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_7, 0x46);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_8, 0x84); /* 94 */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_9, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_A, 0x06); /* 00 */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_B, 0xE0); /* 00 */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_C, 0x10); /* 00 */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_D, 0xf0); /* e7 */
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_E, 0x73); /* 00 */
#elif defined(HDMI_PRODUCT_SS918V100)
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_0, 0xff);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_1, 0x30);
        phy_hwref_clk_divset(hdmi_spec_cfg->pixel_clk);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_3, 0x01);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_4, 0x02);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_5, 0x50);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_6, 0x1e);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_7, 0x14);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_8, 0x84);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_9, 0x90);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_9, 0x80);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_A, 0x03);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_B, 0xE0);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_C, 0x10);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_D, 0xff);
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_E, 0x01);
#endif
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_0, 0x07);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_1, 0xa0);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_2, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_3, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_4, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_5, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_6, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_7, 0xa0);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_8, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_9, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_A, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_B, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_C, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_D, 0x00);
        phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_E, 0x00);
    }

    return TD_SUCCESS;
}

static td_s32 phy_hwinput_clock_check(td_u32 *pixel_clk)
{
    td_u32 reg_ret_value, input_clk;

    /* select pixel clock */
    hdmi_clkset_p_fd_clk_sel_set(TD_FALSE);
    hdmi_clkset_p_refclk_sel_set(TD_FALSE);
    hdmi_clkset_p_ctman_set(TD_FALSE);
    hdmi_clkset_p_modclk_sel_set(TD_TRUE);
    hdmi_clkset_p_fdivclk_sel_set(TD_FALSE);
    hdmi_clkset_mod_div_val_set(TD_FALSE);
    /* set lock_val and lock_cnt */
    hdmi_fdsrcparam_src_lock_val_set(0x04);
    hdmi_fdsrcparam_src_lock_cnt_set(0x02);
    hdmi_fdsrcparam_src_enable_set(TD_FALSE);
    hdmi_fdsrcparam_unused_set(TD_FALSE);
    /* set frequency options */
    hdmi_fdsrcfreq_src_cnt_opt_set(TD_TRUE);
    hdmi_fdsrcfreq_unused_1_set(TD_FALSE);
    hdmi_fdsrcfreq_src_freq_opt_set(TD_FALSE);
    hdmi_fdsrcfreq_unused_2_set(TD_FALSE);
    hdmi_fdsrcfreq_src_freq_ext_set(TD_FALSE);
    /* FD enable */
    hdmi_fdsrcparam_src_lock_val_set(0x4);
    hdmi_fdsrcparam_src_lock_cnt_set(0x2);
    hdmi_fdsrcparam_src_enable_set(TD_TRUE);
    hdmi_fdsrcparam_unused_set(TD_FALSE);
    /* delay time */
    mdelay(1);
    /* read status and result */
    reg_ret_value = (td_u32)hdmi_fdsrcres_src_det_stat_get();
    input_clk = (td_u32)hdmi_fdsrcres_src_cnt_out_get();
    hdmi_info("input clock = : %u  kHz\n", input_clk);
    if ((reg_ret_value & 0xF) == 0xF) {
        hdmi_info(">> input clock quality : stable,\n");
        *pixel_clk = input_clk;
    } else {
        hdmi_info(">> Warning! input clock is unstable! \n");
        *pixel_clk = input_clk;
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void phy_hwreset_release(td_void)
{
    /* phy bus hardware reset and release */
    reg_hdmi_crg_hdmitx_phy_bus_srst_req_set(TD_TRUE);
    udelay(1);
    reg_hdmi_crg_hdmitx_phy_bus_srst_req_set(TD_FALSE);
    /* STB_RESET reset (0x11160010) */
    hdmi_resetn_resetn_set(TD_FALSE);
    /* SPPDHTG2 Software Reset 0x11160310 */
    hdmi_swreset_sw_reset_mod_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_tmds_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_mpll_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_nco_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_fd_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_mod_and_mpll_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_mod_and_nco_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_mod_and_fd_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_hsfifo_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_txfifo_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_data_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_hs_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_pllref_clock_set(TD_FALSE);
    hdmi_swreset_sw_reset_dac_clock_set(TD_FALSE);
    hdmi_swreset_dac_clock_gat_set(TD_FALSE);
    hdmi_swreset_sw_reset_frl_clock_set(TD_FALSE);
    hdmi_swreset_up_sample_fifo_clock_swrst_set(TD_FALSE);
    hdmi_swreset_unused_set(TD_FALSE);
    hdmi_swreset_global_reset_set(TD_TRUE);
    /* STB_RESET reset release (0x11160010) */
    hdmi_resetn_resetn_set(TD_TRUE);
    /* SPPDHTG2 Software Reset Release 0x11160310 */
    hdmi_swreset_global_reset_set(TD_FALSE);
    /* DAC CLOCK enable 0x11160310 */
    hdmi_swreset_dac_clock_gat_set(TD_TRUE);
    /* select mod clk */
    hdmi_clkset_p_fd_clk_sel_set(TD_FALSE);
    hdmi_clkset_p_refclk_sel_set(TD_FALSE);
    hdmi_clkset_p_ctman_set(TD_FALSE);
    hdmi_clkset_p_modclk_sel_set(TD_TRUE);
    hdmi_clkset_p_fdivclk_sel_set(TD_FALSE);
    hdmi_clkset_mod_div_val_set(TD_FALSE);
    /* select 24MHz clock as reference clock */
    hdmi_refclksel_i_ref_clk_sel_set(TD_TRUE);
    /* strobe auto m mode set */
    hdmi_stbopt_req_length_set(TD_FALSE);
    hdmi_stbopt_stb_delay2_set(0x2);
    hdmi_stbopt_stb_delay1_set(0x2);
    hdmi_stbopt_stb_delay0_set(0x2);
    hdmi_stbopt_stb_acc_sel_set(TD_FALSE);
    hdmi_stbopt_stb_cs_sel_set(TD_FALSE);

    return;
}

static td_s32 phy_hw_div_nsel_get(td_u32 tmds_clk, td_u8 *tmds_divn)
{
    td_u32  i = 0;
    phy_clk_rang_value *phy_tmds_divnsel = TD_NULL;

    for (phy_tmds_divnsel = &g_phy_hw_tmds_divnsel_tab[0];
        (phy_tmds_divnsel != TD_NULL && (i < hdmi_array_size(g_phy_hw_tmds_divnsel_tab)));
        phy_tmds_divnsel++, i++) {
        if ((tmds_clk >= phy_tmds_divnsel->phy_tmds_clk_range.clk_min) &&
            (tmds_clk < phy_tmds_divnsel->phy_tmds_clk_range.clk_max)) {
            *tmds_divn = phy_tmds_divnsel->seek_value;
            return TD_SUCCESS;
        }
    }
    hdmi_err("can't find param, tmds_clk:%u\n", tmds_clk);

    return TD_FAILURE;
}

static td_u32 phy_hwpow(td_u32 base_num, td_u32 index_num)
{
    td_u32 i;
    td_u32 ret_val = 1;

    for (i = 0; i < index_num; i++) {
        ret_val = ret_val * base_num;
    }

    return ret_val;
}

static td_s32 phy_hwpll_vco_fre_get(td_u32 tmds_clk, td_u32 *pll_vco_fre)
{
    td_u8 tmds_divnsel = 0;

    hdmi_if_null_return(pll_vco_fre, TD_FAILURE);

    if (phy_hw_div_nsel_get(tmds_clk, &tmds_divnsel) != TD_SUCCESS) {
        return TD_FAILURE;
    }

    hdmi_info("tmds_divnsel = %x \n", tmds_divnsel);
    *pll_vco_fre = tmds_clk * TMDS_CLK_FREQ_MUITIPLE * phy_hwpow(PHY_POW_BASE_NUM, tmds_divnsel);
    hdmi_info("tmds_clk = %u, pll_vco_fre=%u\n", tmds_clk, *pll_vco_fre);

    return TD_SUCCESS;
}

/* u32TmdsClk > 25000 && u32TmdsClk < 100000 */
static td_void phy_space_cfg_100m(hdmi_trace_len trace_len)
{
#if defined(HDMI_PRODUCT_SS919V100)
    /* #Data Drv Settings */
    phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
    phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
    phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
    phy_hwwrite_stb1_byte(0x007, 0x03, 0xDF);
    phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x05, 0x1D);
    phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x08, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x09, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0xB, 0x50);
    /* CLK Drv Settings */
    phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
    phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
    phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
    phy_hwwrite_stb1_byte(0x008, 0x03, 0xDF);
    phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x05, 0x22);
    phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
    phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
    phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0xB, 0x50);
#elif defined(HDMI_PRODUCT_SS918V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0A); /* 09->0A */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        phy_hwwrite_stb1_byte(0x100, 0xC, 0x12);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0A); /* 09->0A */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x47); /* 00->47 */
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0A); /* 09->0A */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        phy_hwwrite_stb1_byte(0x100, 0xC, 0x12);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0A); /* 09->0A */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x47);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    } else {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0A);
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0A);
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    }
#endif
    return;
}

static td_void phy_space_cfg_165m(hdmi_trace_len trace_len)
{
#if defined(HDMI_PRODUCT_SS919V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* #Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x26);
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        /* #CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x24);
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0xE7);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x2C);
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x10);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x4A);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x24);
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
    }
#elif defined(HDMI_PRODUCT_SS918V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x00, 0xE7); /* 87->E7 */
        phy_hwwrite_stb1_byte(0x007, 0x01, 0xBE); /* 3E->BE */
        phy_hwwrite_stb1_byte(0x007, 0x02, 0xBE); /* 3E->BE */
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0C); /* 0A->0C */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x25); /* 00->25 */
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x81); /* 80->81 */
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x81); /* 80->81 */
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0xE7);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0C);
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x25);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x81);
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x81);
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    } else {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0xE7); /* 87->E7 */
        phy_hwwrite_stb1_byte(0x007, 0x01, 0xBE); /* 3E->BE */
        phy_hwwrite_stb1_byte(0x007, 0x02, 0xBE); /* 3E->BE */
        phy_hwwrite_stb1_byte(0x007, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0C); /* 0A->0C */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x25); /* 00->25 */
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x81); /* 80->81 */
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x81); /* 80->81 */
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    }
#endif
    return;
}

static td_void phy_space_cfg_165m_340m(hdmi_trace_len trace_len)
{
#if defined(HDMI_PRODUCT_SS919V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x3D); /* MAIN_ICAL<4:0> : Data Swing Control */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00); /* POST1_ICAL<4:0> : Pre Emphasis Strength Control */
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        /* TXTERMEN <5:0> : Termination Impedance Control. 1~31: 3.2k~200ohm, 32~63:200~68ohm */
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x50);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x007, 0x0B, 0x50);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x24);
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0xE7);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0xBE);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x3F); /* MAIN_ICAL<4:0> : Data Swing Control */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00); /* POST1_ICAL<4:0> : Pre Emphasis Strength Control */
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x05);
        /* TXTERMEN <5:0> : Termination Impedance Control. 1~31: 3.2k~200ohm, 32~63:200~68ohm */
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x55);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x007, 0x0B, 0x5F);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0xDF);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x24);
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
    }
#elif defined(HDMI_PRODUCT_SS918V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xE7); /* C7->E7 */
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0C); /* 10->0C */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x45); /* 50->45 */
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        phy_hwwrite_stb1_byte(0x100, 0xC,  0x09); /* 0B->09 */
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07); /* C7->07 */
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48); /* 55->48 */
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xF7);
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x0F);
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x51);
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        phy_hwwrite_stb1_byte(0x100, 0xC,  0x0E);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    } else {
        phy_hwwrite_stb1_byte(0x007, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x03, 0xE7); /* C7->E7 */
        phy_hwwrite_stb1_byte(0x007, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x05, 0x10); /* 0F->10 */
        phy_hwwrite_stb1_byte(0x007, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x08, 0x50); /* 55->50 */
        phy_hwwrite_stb1_byte(0x007, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x007, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB,  0x20);
        phy_hwwrite_stb1_byte(0x100, 0xC,  0x0B);
        phy_hwwrite_stb1_byte(0x008, 0x00, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x01, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x02, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x03, 0x07); /* C7->07 */
        phy_hwwrite_stb1_byte(0x008, 0x04, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x05, 0x0C); /* 0B->0C */
        phy_hwwrite_stb1_byte(0x008, 0x06, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x07, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x08, 0x48); /* 55->48 */
        phy_hwwrite_stb1_byte(0x008, 0x09, 0x80);
        phy_hwwrite_stb1_byte(0x008, 0xA,  0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB,  0x20);
    }
#endif

    return;
}

static td_void phy_space_cfg_340m_600m(hdmi_trace_len trace_len)
{
#if defined(HDMI_PRODUCT_SS919V100)
    phy_hwwrite_stb1_byte(0x007, 0x0, 0x87);
    phy_hwwrite_stb1_byte(0x007, 0x1, 0x7E);
    phy_hwwrite_stb1_byte(0x007, 0x2, 0x7E);
    phy_hwwrite_stb1_byte(0x007, 0x3, 0xDF);
    phy_hwwrite_stb1_byte(0x007, 0x4, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x5, 0x3E);
    phy_hwwrite_stb1_byte(0x007, 0x6, 0x0B);
    phy_hwwrite_stb1_byte(0x007, 0x7, 0x00);
    phy_hwwrite_stb1_byte(0x007, 0x8, 0x70);
    phy_hwwrite_stb1_byte(0x007, 0x9, 0x81);
    phy_hwwrite_stb1_byte(0x007, 0xB, 0x5F);
    phy_hwwrite_stb1_byte(0x100, 0xC, 0x12);
    /* CLK Drv Settings */
    phy_hwwrite_stb1_byte(0x008, 0x0, 0x87);
    phy_hwwrite_stb1_byte(0x008, 0x1, 0x3E);
    phy_hwwrite_stb1_byte(0x008, 0x2, 0x3E);
    phy_hwwrite_stb1_byte(0x008, 0x3, 0xDF);
    phy_hwwrite_stb1_byte(0x008, 0x4, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x5, 0x22);
    phy_hwwrite_stb1_byte(0x008, 0x6, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x7, 0x00);
    phy_hwwrite_stb1_byte(0x008, 0x8, 0x44);
    phy_hwwrite_stb1_byte(0x008, 0x9, 0x81);
    phy_hwwrite_stb1_byte(0x008, 0xB, 0x50);
#elif defined(HDMI_PRODUCT_SS918V100)
    if (trace_len == HDMI_TRACE_LEN_0) {
        /* Data Drv Settings */
        phy_hwwrite_stb1_byte(0x007, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x1, 0x3E); /* 7E->3E */
        phy_hwwrite_stb1_byte(0x007, 0x2, 0x3E); /* 7E->3E */
        phy_hwwrite_stb1_byte(0x007, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x5, 0x12); /* 16->12 */
        phy_hwwrite_stb1_byte(0x007, 0x6, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x8, 0x54); /* 70->54 */
        phy_hwwrite_stb1_byte(0x007, 0x9, 0x81);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        /* CLK Drv Settings */
        phy_hwwrite_stb1_byte(0x008, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x1, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x2, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x5, 0x0B); /* 14->0B */
        phy_hwwrite_stb1_byte(0x008, 0x6, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x8, 0x48); /* 70->48 */
        phy_hwwrite_stb1_byte(0x008, 0x9, 0x80); /* 81->80 */
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    } else if (trace_len == HDMI_TRACE_LEN_1) {
        phy_hwwrite_stb1_byte(0x007, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x1, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x2, 0x3E);
        phy_hwwrite_stb1_byte(0x007, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x5, 0x12); /* 16->12 */
        phy_hwwrite_stb1_byte(0x007, 0x6, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x8, 0x54); /* 70->54 */
        phy_hwwrite_stb1_byte(0x007, 0x9, 0x81);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        phy_hwwrite_stb1_byte(0x008, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x1, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x2, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x5, 0x0B);
        phy_hwwrite_stb1_byte(0x008, 0x6, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x8, 0x48);
        phy_hwwrite_stb1_byte(0x008, 0x9, 0x80);
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    } else {
        phy_hwwrite_stb1_byte(0x007, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x007, 0x1, 0x7E); /* 3E->7E */
        phy_hwwrite_stb1_byte(0x007, 0x2, 0x7E); /* 3E->7E */
        phy_hwwrite_stb1_byte(0x007, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x007, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x5, 0x14); /* 16->14 */
        phy_hwwrite_stb1_byte(0x007, 0x6, 0x08); /* 00->08 */
        phy_hwwrite_stb1_byte(0x007, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0x8, 0x54); /* 70->54 */
        phy_hwwrite_stb1_byte(0x007, 0x9, 0x81);
        phy_hwwrite_stb1_byte(0x007, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x007, 0xB, 0x20);
        phy_hwwrite_stb1_byte(0x008, 0x0, 0x87);
        phy_hwwrite_stb1_byte(0x008, 0x1, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x2, 0x3E);
        phy_hwwrite_stb1_byte(0x008, 0x3, 0x07);
        phy_hwwrite_stb1_byte(0x008, 0x4, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x5, 0x10); /* 0B->10 */
        phy_hwwrite_stb1_byte(0x008, 0x6, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x7, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0x8, 0x5a); /* 48->5a */
        phy_hwwrite_stb1_byte(0x008, 0x9, 0x80); /* 81->80 */
        phy_hwwrite_stb1_byte(0x008, 0xA, 0x00);
        phy_hwwrite_stb1_byte(0x008, 0xB, 0x20);
    }
#endif

    return;
}

static td_s32 phy_hwfcon_cfg_para_get(phy_fcon_para *fcon_mode, td_u32 tmds_clk, const hdmi_phy_tmds_cfg *tmds_cfg)
{
    td_u32 pll_vco_fre = 0;

    if (tmds_cfg->mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
        if (phy_hwpll_vco_fre_get(tmds_clk, &pll_vco_fre) != TD_SUCCESS) {
            return TD_FAILURE;
        }
        /*
         *              PLL_VCO_FRE(MHz)
         *  I_REF_CNT = ----------------- *  16(constant)*IRefCnt_len(FCCNTR0[31:24])
         *                  24 * 20
         */
        fcon_mode->i_ref_cnt = (pll_vco_fre * 2 * (fcon_mode->i_ref_cnt_len) / (1000 * 20 * 3));
    }
    hdmi_info("mode_cfg: %u, i_ref_cnt: %x\n", tmds_cfg->mode_cfg, fcon_mode->i_ref_cnt);

    return TD_SUCCESS;
}

static td_void phy_hwspec_tmds_set(const hdmi_phy_tmds_cfg *spec_cfg)
{
    hdmi_info("tmds_clk=%u, trace_len:%u \n", spec_cfg->tmds_clk, spec_cfg->trace_len);

    if (spec_cfg->tmds_clk < TMDS_CLK_100M) {
        phy_space_cfg_100m(spec_cfg->trace_len);
    } else if (spec_cfg->tmds_clk >= TMDS_CLK_100M && spec_cfg->tmds_clk < TMDS_CLK_165M) {
        phy_space_cfg_165m(spec_cfg->trace_len);
    } else if (spec_cfg->tmds_clk >= TMDS_CLK_165M && spec_cfg->tmds_clk < TMDS_CLK_340M) {
        phy_space_cfg_165m_340m(spec_cfg->trace_len);
    } else if (spec_cfg->tmds_clk >= TMDS_CLK_340M && spec_cfg->tmds_clk < TMDS_CLK_600M) {
        phy_space_cfg_340m_600m(spec_cfg->trace_len);
    }

    return;
}

static td_void phy_hwclock_tmds_set(const phy_clk_set *clk_set)
{
    phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_2, clk_set->ref_clk_div & PHY_CPIS_VAL);
    phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_1, 0xA0);
    /* 3'b, BIT[8:7] */
    phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_7, (clk_set->tmds_divnsel & 0x07) | (PHY_TMDS_OUTEN << 3));
    phy_hwwrite_stb1_byte(APHY_CS_9, APHY_OFFSET_E, 0x00);
    hdmi_fcdstepset_p_divn_h20_set(clk_set->fcd_step & (0x07));
    hdmi_fcdstepset_unused_set(TD_FALSE);
    hdmi_fcdstepset_up_sampler_ratio_sel_set(TD_FALSE);
    hdmi_fdivmanual_i_manual_en_set(0xe);
    hdmi_info("g_mnx_get: m:%u, n:%u\n", g_mnx_get.m_val, g_mnx_get.n_val);
    hdmi_fdivmanual_i_mdiv_set(g_mnx_get.m_val);
    hdmi_fdivstat1_i_fdiv_in_set(((td_u32)g_mnx_get.n_val) << 24); /* 24'b, BIT[32:25] */
    hdmi_hdmi_mode_reg_hdmi_mode_en_set((clk_set->tmds_cfg.tmds_clk > TMDS_CLK_340M) ? TD_TRUE : TD_FALSE);
    hdmi_info("tmds_clk: %u\n", clk_set->tmds_cfg.tmds_clk);
    /* FDIV init */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_FALSE);
    hdmi_fdivset0_en_mod_set(TD_FALSE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);
    hdmi_fdivset0_init_set(0x1);
    /*  delay 1us */
    udelay(1);
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_FALSE);
    hdmi_fdivset0_en_mod_set(TD_FALSE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);

    return;
}

static td_void phy_hwcon_auto_set(const phy_fcon_para *mode)
{
    hdmi_ctset0_i_enable_set(TD_TRUE);
    hdmi_ctset0_i_run_set(TD_FALSE);
    hdmi_ctset0_unused_set(TD_FALSE);
    hdmi_fccntr0_i_vco_st_wait_len_set(0x50);
    hdmi_fccntr0_i_vco_end_wait_len_set(0x50);
    hdmi_fccntr0_i_ref_cnt_len_set(mode->i_ref_cnt_len);
    hdmi_fcopt_i_ct_sel_set(TD_TRUE);
    hdmi_fcopt_i_clkdet_sel_set(TD_TRUE);
    hdmi_fcopt_i_ct_mode_set(HW_FCON_DELAY);
    hdmi_fcopt_unused_1_set(TD_FALSE);
    hdmi_fcopt_i_ct_en_set(TD_TRUE);
    hdmi_fcopt_unused_2_set(TD_FALSE);
    hdmi_fcopt_i_ct_idx_sel_set(TD_FALSE);
    hdmi_fcopt_unused_set(TD_FALSE);
    hdmi_fccntr1_i_ref_cnt_set(mode->i_ref_cnt & (0xffff));
    hdmi_info("mode.i_ref_cnt = %x\n", mode->i_ref_cnt);
    hdmi_fccontinset1_p_contin_upd_rate_set(0x0000960);
    hdmi_fccontinset1_p_contin_upd_time_set(TD_FALSE);
    /* FCCONRINSET2, fcon max value : 0x3FF, min value : 0x5 2017.3.21updataed */
    hdmi_fccontinset2_p_contin_upd_th_dn_set(0x005);
    hdmi_fccontinset2_unused_set(TD_FALSE);
    hdmi_fccontinset2_p_contin_upd_th_up_set(0x3ff);
    /* FCCONRINSET0, [0] enable, [2] LKVDET polarity */
    hdmi_fccontinset0_p_contin_upd_en_set(TD_TRUE);
    hdmi_fccontinset0_p_contin_upd_opt_set(TD_FALSE);
    hdmi_fccontinset0_p_contin_upd_pol_set(TD_TRUE);
    hdmi_fccontinset0_unused_set(TD_FALSE);
    hdmi_fccontinset0_p_contin_upd_step_set(0x2);
    /* CTSET0, [0] i_enable, [1] run */
    hdmi_ctset0_i_enable_set(TD_TRUE);
    hdmi_ctset0_i_run_set(TD_TRUE);
    hdmi_ctset0_unused_set(TD_FALSE);

    return;
}

static td_s32 phy_hwfcon(const phy_fcon_para *fcon_mode)
{
    td_s32 fcon_result[FCON_RST_LEN] = {0};
    td_u32 count = 0;

    if (fcon_mode->fcon_mode == PHY_FCON_MODE_AUTO) {
        phy_hwcon_auto_set(fcon_mode);
        mdelay(HW_FCON_DELAY);
        /* 3, is each fcon_result default set needs delay 2ms, total 2*4 ms */
        while (count <= 3) {
            fcon_result[0] = hdmi_fcstat_clk_ok_get();
            fcon_result[1] = hdmi_fcstat_busy_get();
            fcon_result[2] = hdmi_fcstat_done_get();
            fcon_result[3] = hdmi_fcstat_error_get();
            if (fcon_result[1] == 0) {
                break;
            }
            mdelay(HW_FCON_DELAY);
            count++;
        }
        if (fcon_result[3] == 0 && fcon_result[2] == 1 &&
            fcon_result[1] == 0 && fcon_result[0] == 1) {
            hdmi_info("FCON is done\n");
        } else if (fcon_result[3] == 1) {
            hdmi_info("FCON is failed\n");
            return TD_FAILURE;
        }
    } else {
        hdmi_fcopt_i_ct_sel_set(TD_FALSE);
        hdmi_fcopt_i_clkdet_sel_set(TD_FALSE);
        hdmi_fcopt_i_ct_mode_set(TD_FALSE);
        hdmi_fcopt_unused_1_set(TD_FALSE);
        hdmi_fcopt_i_ct_en_set(TD_FALSE);
        hdmi_fcopt_unused_2_set(TD_FALSE);
        hdmi_fcopt_i_ct_idx_sel_set(TD_FALSE);
        hdmi_fcopt_unused_set(TD_FALSE);
        hdmi_fccontinset0_p_contin_upd_en_set(TD_FALSE);
        hdmi_fccontinset0_p_contin_upd_opt_set(TD_FALSE);
        hdmi_fccontinset0_p_contin_upd_pol_set(TD_FALSE);
        hdmi_fccontinset0_unused_set(TD_FALSE);
        hdmi_fccontinset0_p_contin_upd_step_set(TD_FALSE);
        hdmi_ctset1_i_mpll_fcon_set(0x022);
        hdmi_ctset1_i_mpll_ctlck_set(TD_TRUE);
        hdmi_ctset1_ctset1_unused_set(TD_FALSE);
    }

    return TD_SUCCESS;
}

static td_s32 phy_hwpost_setup(const hdmi_phy_tmds_cfg *tmds_cfg)
{
    /* HS FIFO enable, HS LINK external */
    hdmi_hsset_p_hsset_set(3);
    /* transmitter FIFO and up sampler enable */
    hdmi_txfifoset0_p_pr_en_h20_set(1);
    hdmi_txfifoset0_p_enable_h20_set(1);
    hdmi_txfifoset0_unused_set(0);
    /* loop divider close */
    phy_hwwrite_stb(APHY_CS_8, APHY_OFFSET_D, LEFT_OR_MASK_OFFSET_5, LEFT_OR_MASK_OFFSET_5, 0x0);
    /* loop divider open */
    phy_hwwrite_stb(APHY_CS_8, APHY_OFFSET_D, LEFT_OR_MASK_OFFSET_5, LEFT_OR_MASK_OFFSET_5, 0x1);
    mdelay(HW_WRITE_STB_DELAY);
    /* HS input data selection */
    hdmi_txdataoutsel_p_ch_out_sel_set(0x0);
    /* PHY data serializer reset/set */
    phy_hwwrite_stb(APHY_CS_0, APHY_OFFSET_0, 1, 1, 0x0);
    phy_hwwrite_stb(APHY_CS_1, APHY_OFFSET_0, 1, 1, 0x0);
    phy_hwwrite_stb(APHY_CS_2, APHY_OFFSET_0, 1, 1, 0x0);
    phy_hwwrite_stb(APHY_CS_3, APHY_OFFSET_0, 1, 1, 0x0);
    mdelay(1);
    phy_hwwrite_stb(APHY_CS_0, APHY_OFFSET_0, 1, 1, 0x1);
    phy_hwwrite_stb(APHY_CS_1, APHY_OFFSET_0, 1, 1, 0x1);
    phy_hwwrite_stb(APHY_CS_2, APHY_OFFSET_0, 1, 1, 0x1);
    phy_hwwrite_stb(APHY_CS_3, APHY_OFFSET_0, 1, 1, 0x1);

    return TD_SUCCESS;
}

static td_s32 phy_hwphy_hwclk_rang_value_get(td_u32 tmds_clk, td_u8 size,
    const phy_clk_rang_value *phy_rang_sel, td_u8 *seek_value)
{
    td_u32 i = 0;
    const phy_clk_rang_value *phy_rang = TD_NULL;

    for (phy_rang = &phy_rang_sel[0]; (i < size); phy_rang++, i++) {
        if ((tmds_clk >= phy_rang->phy_tmds_clk_range.clk_min) && (tmds_clk < phy_rang->phy_tmds_clk_range.clk_max)) {
            *seek_value = phy_rang->seek_value;
            return TD_SUCCESS;
        }
    }
    hdmi_err("can't find param,tmds_clk:%u, i=%u\n", tmds_clk, i);

    return TD_FAILURE;
}

static td_s32 phy_hwfractional_mnx_get(td_u32 tmds_clk, td_u32 pixel_clk, hdmi_deep_color deep_color)
{
    td_u8  k, m_value, size;
    td_u32 mn_value, pll_ref_clk;
    td_u8  seek_value   = 0;
    td_u8  tmds_divnsel = 0;

    /* HDMI 2.0 Configure pll feedback coefficient M ,N ,X */
    size = hdmi_array_size(g_phy_hw_ref_clk_div);
    /* Determine the reference clock division factor */
    if (phy_hwphy_hwclk_rang_value_get(pixel_clk, size, &g_phy_hw_ref_clk_div[0], &seek_value) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    /* obtain TMDS_DIVNSEL */
    if (phy_hw_div_nsel_get(tmds_clk, &tmds_divnsel) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    hdmi_info("pixel_clk: %u, seek_value: %u-%u\n", pixel_clk, seek_value, phy_hwpow(PHY_POW_BASE_NUM, seek_value));
    pll_ref_clk = pixel_clk / phy_hwpow(PHY_POW_BASE_NUM, seek_value);
    g_mnx_get.pll_ref_clk = pll_ref_clk;
    hdmi_info("pll_ref_clk(%u), tmds_divnsel(%u), deep_color(%u) \n",
              g_mnx_get.pll_ref_clk, tmds_divnsel, deep_color);
    switch (deep_color) {
        case HDMI_DEEP_COLOR_30BIT:
            /* 8bit pix is std bandwidth, 5 and 4 means 10bit is std 5/4 multiple */
            mn_value = TMDS_CLK_FREQ_MUITIPLE * phy_hwpow(PHY_POW_BASE_NUM, tmds_divnsel) *
            phy_hwpow(PHY_POW_BASE_NUM, seek_value) * 5 / 4;
            break;
        case HDMI_DEEP_COLOR_36BIT:
            /* 10bit pix is std bandwidth, 3 and 2 means 12bit is std 3/2 multiple */
            mn_value = TMDS_CLK_FREQ_MUITIPLE * phy_hwpow(PHY_POW_BASE_NUM, tmds_divnsel) *
            phy_hwpow(PHY_POW_BASE_NUM, seek_value) * 3 / 2;
            break;
        default:
            mn_value = TMDS_CLK_FREQ_MUITIPLE * phy_hwpow(PHY_POW_BASE_NUM, tmds_divnsel) *
                phy_hwpow(PHY_POW_BASE_NUM, seek_value);
            break;
    }
    hdmi_info("mn_value = %u \n", mn_value);
    g_mnx_get.mn_value = mn_value;
    /* calculate N value */
    g_mnx_get.n_val = (td_u8)(mn_value % 10); /* 10, about pll coefficient N calculate protocol */
    k = 1;
    if (g_mnx_get.n_val == 0) {
        k = 2;                /* 2, means 1+1, about pll coefficient N real value calculate */
        g_mnx_get.n_val = 10; /* calculate result n_val is 0, get real n_val 10 */
    }
    /* calculate M value */
    m_value = (td_u8)(mn_value / 10); /* 10, about pll coefficient M calculate protocol */
    g_mnx_get.m_val = m_value - k;
    hdmi_info("g_mnx_get m:%x, n:%u\n", g_mnx_get.m_val, g_mnx_get.n_val);

    return TD_SUCCESS;
}

static td_s32 phy_hwclock_set(const phy_clk_set *clk_set)
{
    switch (clk_set->tmds_cfg.mode_cfg) {
        case HDMI_PHY_MODE_CFG_TMDS:
            phy_hwclock_tmds_set(clk_set);
            break;
        default:
            break;
    }

    return TD_SUCCESS;
}

static td_s32 phy_hwspec_set(const hdmi_phy_tmds_cfg *spec_cfg)
{
    switch (spec_cfg->mode_cfg) {
        case HDMI_PHY_MODE_CFG_TMDS:
            phy_hwspec_tmds_set(spec_cfg);
            break;
        default:
            break;
    }

    return TD_SUCCESS;
}

static td_s32 hal_hdmi_clk_set_para_get(phy_clk_set *clk_set)
{
    td_u8 size;

    /* get p_divn_h20 */
    if (clk_set->tmds_cfg.mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
        size = hdmi_array_size(g_phy_hw_fcd_step_set);
        if (phy_hwphy_hwclk_rang_value_get(clk_set->tmds_cfg.tmds_clk, size,
            &g_phy_hw_fcd_step_set[0], &clk_set->fcd_step) != TD_SUCCESS) {
            return TD_FAILURE;
        }
    }
    hdmi_info("Get fcd_step = %x\n", clk_set->fcd_step);
    /* REFCLKDIV (TMDS) */
    if (clk_set->tmds_cfg.mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
        size = hdmi_array_size(g_phy_hw_ref_clk_div);
        if (phy_hwphy_hwclk_rang_value_get(clk_set->tmds_cfg.pixel_clk, size,
            &g_phy_hw_ref_clk_div[0], &clk_set->ref_clk_div) != TD_SUCCESS) {
            return TD_FAILURE;
        }
    } else if (clk_set->tmds_cfg.mode_cfg == HDMI_PHY_MODE_CFG_FRL) { /* FRL */
        clk_set->ref_clk_div = 0;
    }
    hdmi_info("Get ref_clk_div = %x\n", clk_set->ref_clk_div);
    /* TMDS_DIVNSEL */
    if (clk_set->tmds_cfg.mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
        /* TMDS */
        size = hdmi_array_size(g_phy_hw_tmds_divnsel_tab);
        if (phy_hwphy_hwclk_rang_value_get(clk_set->tmds_cfg.tmds_clk, size,
            &g_phy_hw_tmds_divnsel_tab[0], &clk_set->tmds_divnsel) != TD_SUCCESS) {
            return TD_FAILURE;
        }
        /* shut down FCG */
        phy_hwwrite_stb1_byte(0x100, 0xB, 0x00);
        hdmi_fcgset_p_fcg_en_set(0x0);
        hdmi_fcgset_p_fcg_dlf_en_set(0x0);
        hdmi_fcgset_p_fcg_dither_en_set(0x0);
        hdmi_fcgset_p_fcg_lock_en_set(0x0);
    }
    hdmi_info("Get tmds_divnsel = %x\n", clk_set->tmds_divnsel);

    return TD_SUCCESS;
}
#endif

td_s32 hal_hdmi_phy_init(const hdmi_hal_init *hal_init)
{
    if (g_hdmi_phy_info.init == TD_FALSE) {
        hdmi_hdmitx_phy_reg_init();
        g_hdmi_phy_info.init = TD_TRUE;
    }

    return TD_SUCCESS;
}

td_s32 hal_hdmi_phy_deinit(td_void)
{
#ifndef HDMI_FPGA_SUPPORT
    /* the base addr of the register  map */
    hdmi_hdmitx_phy_reg_deinit();
#endif
    g_hdmi_phy_info.init = TD_FALSE;

    return TD_SUCCESS;
}

td_s32 hal_hdmi_phy_power_get(td_bool *enable)
{
    hdmi_if_fpga_return(TD_SUCCESS);
    hdmi_if_null_return(enable, TD_FAILURE);

    *enable = TD_TRUE;
    /* obtain power's status:0 reset; 1:set */
    if (hdmi_resetn_resetn_get() == 0x00) {
        *enable = TD_FALSE;
    }

    return TD_SUCCESS;
}

/* aphy power */
td_void hal_hdmi_phy_power_set(td_bool enable)
{
#ifndef HDMI_FPGA_SUPPORT
    /* PHY MODE power on  or off */
    hdmi_resetn_resetn_set(enable);
#endif
    g_hdmi_phy_info.power_enable = enable;

    return;
}

td_void hal_hdmi_phy_oe_set(td_bool enable)
{
#ifndef HDMI_FPGA_SUPPORT
    td_u8 phy_oe_stat;

    phy_oe_stat = (enable == TD_TRUE) ? 0x01 : 0x00;
    /* CH0-3 DRIVER OE */
    phy_hwwrite_stb(0x01, 0x03, 0x00, 0x00, phy_oe_stat);
    phy_hwwrite_stb(0x02, 0x03, 0x00, 0x00, phy_oe_stat);
    phy_hwwrite_stb(0x04, 0x03, 0x00, 0x00, phy_oe_stat);
    phy_hwwrite_stb(0x08, 0x03, 0x00, 0x00, phy_oe_stat);
#endif
    g_hdmi_phy_info.oe_enable = enable;

    return;
}

td_s32 hal_hdmi_phy_oe_get(td_bool *enable)
{
    /* get oe's status */
    td_u8 oe_d0_status = 0;
    td_u8 oe_d1_status = 0;
    td_u8 oe_d2_status = 0;
    td_u8 oe_clk_status = 0;

    hdmi_if_fpga_return(TD_SUCCESS);
    hdmi_if_null_return(enable, TD_FAILURE);

    if (hdmi_resetn_resetn_get() == 0) {
        hdmi_info("phy is reset now, OE disable.\n");
        *enable = TD_FALSE;
        return TD_SUCCESS;
    }
    phy_hwread_stb1_byte(0x1, 0x03, &oe_d0_status);
    phy_hwread_stb1_byte(0x2, 0x03, &oe_d1_status);
    phy_hwread_stb1_byte(0x4, 0x03, &oe_d2_status);
    phy_hwread_stb1_byte(0x8, 0x03, &oe_clk_status);
    *enable = TD_FALSE;
    if ((oe_d0_status & 0x01) && (oe_d1_status & 0x01) && (oe_d2_status & 0x01) && (oe_clk_status & 0x01)) {
        *enable = TD_TRUE;
    }

    return TD_SUCCESS;
}

/* for video path */
td_s32 hal_hdmi_phy_tmds_set(const hdmi_phy_tmds_cfg *tmds_cfg)
{
    errno_t          ret;
    td_u32           pixel_clk = 0;
    phy_fcon_para    fcon_mode = {0};
    hdmi_phy_ssc_cfg hdmi_ssc_cfg = {0};
    phy_clk_set      clk_set = {0};

    hdmi_if_fpga_return(TD_SUCCESS);
    hdmi_if_null_return(tmds_cfg, TD_FAILURE);
    if ((tmds_cfg->mode_cfg == HDMI_PHY_MODE_CFG_TMDS) || (tmds_cfg->mode_cfg == HDMI_PHY_MODE_CFG_FRL)) {
        hdmi_info("TMDS mode: %u, pixelclk: %u, tmdsclk: %u, deepcolor: %u, tracelen: %u\n",
            tmds_cfg->mode_cfg, tmds_cfg->pixel_clk, tmds_cfg->tmds_clk, tmds_cfg->deep_color, tmds_cfg->trace_len);

        fcon_mode.i_ref_cnt_len = 128; /* 128, is i_ref_cnt length */
        /* Reset:clear all the aphy register */
        phy_hwreset_release();
        /* input clock check:to configurate dphy Source clock detection module */
        if (phy_hwinput_clock_check(&pixel_clk) != TD_SUCCESS) {
            hdmi_info("input clock unstable\n");
        }
        /* initial: finish aphy, dphy configuration, configurate the register no change with standard */
        phy_hwinitset(tmds_cfg);
        clk_set.tmds_cfg.mode_cfg   = tmds_cfg->mode_cfg;
        clk_set.tmds_cfg.deep_color = tmds_cfg->deep_color;
        clk_set.tmds_cfg.pixel_clk  = tmds_cfg->pixel_clk;
        clk_set.tmds_cfg.tmds_clk   = tmds_cfg->tmds_clk;
        /* calculate pLL parameter */
        if (hal_hdmi_clk_set_para_get(&clk_set) != TD_SUCCESS) {
            hdmi_info("Init para get fail\n");
            return TD_FAILURE;
        }

        if (tmds_cfg->mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
            /* pll Feedback clock divider */
            if (phy_hwfractional_mnx_get(clk_set.tmds_cfg.tmds_clk,
                clk_set.tmds_cfg.pixel_clk, clk_set.tmds_cfg.deep_color) != TD_SUCCESS) {
                hdmi_info("MNX get fail\n");
                return TD_FAILURE;
            }
        }
        /* initial:PHY clock configuration */
        phy_hwclock_set(&clk_set);
        fcon_mode.fcon_mode = PHY_FCON_MODE_AUTO;
        /* calculate FCON */
        phy_hwfcon_cfg_para_get(&fcon_mode, clk_set.tmds_cfg.tmds_clk, tmds_cfg);
        /* PLL configuration (FCON configuration) */
        if (phy_hwfcon(&fcon_mode) != TD_SUCCESS) {
            hdmi_info("Fcon is failed and You have to restart FCON\n");
        }
        hdmi_ssc_cfg.phy_ssc.ssc_enable = 0;
        /* calculate Spread spectrum */
        if (tmds_cfg->mode_cfg == HDMI_PHY_MODE_CFG_TMDS) {
            hal_hdmi_phy_ssc_set(&hdmi_ssc_cfg);
        }
        /* initial:index */
        phy_hwspec_set(tmds_cfg);
        /* data path enabl e */
        phy_hwpost_setup(tmds_cfg);
        /* Then disable output and enable by user. */
        hal_hdmi_phy_oe_set(TD_FALSE);
        /* status info set */
        ret = memcpy_s(&g_hdmi_phy_info.tmds_cfg, sizeof(hdmi_phy_tmds_cfg), tmds_cfg, sizeof(*tmds_cfg));
        hdmi_unequal_eok(ret);
    }

    return TD_SUCCESS;
}

static td_void hal_hdmi_phy_ssc_init(td_u16 mod_d, td_u16 mod_n)
{
    hdmi_if_fpga_return_void();
    /* MOD_N MOD_T */
    hdmi_fdivset3_mod_len_set(TD_FALSE);
    hdmi_fdivset3_mod_t_set(TD_TRUE);
    hdmi_fdivset3_mod_n_set(mod_n);
    /* MOD_D */
    hdmi_fdivset4_mod_d_set(mod_d);
    /* FDIV init */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_FALSE);
    hdmi_fdivset0_en_mod_set(TD_FALSE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);
    /* FDIV init */
    hdmi_fdivset0_init_set(TD_TRUE);
    udelay(1); /* 150 nsec */
    /* FDIV control */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_FALSE);
    hdmi_fdivset0_en_mod_set(TD_FALSE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);
    udelay(1); /* 150 nsec */
    /* FDIV control */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_TRUE);
    hdmi_fdivset0_en_mod_set(TD_FALSE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);
    /* FDIV control */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_TRUE);
    hdmi_fdivset0_en_mod_set(TD_TRUE);
    hdmi_fdivset0_en_sdm_set(TD_FALSE);
    /* FDIV control */
    hdmi_fdivset0_init_set(TD_FALSE);
    hdmi_fdivset0_en_ctrl_set(TD_TRUE);
    hdmi_fdivset0_en_mod_set(TD_TRUE);
    hdmi_fdivset0_en_sdm_set(TD_TRUE);

    return;
}

td_s32 hal_hdmi_phy_ssc_set(const hdmi_phy_ssc_cfg *hdmi_ssc_cfg)
{
    td_u16 mod_d, mod_n;
    td_u32 mod_dn, ssc_freq, ssc_amptd;
    td_u64 mod_d0;
    td_u32 rem = 0;

    hdmi_if_fpga_return(TD_SUCCESS);
    hdmi_if_null_return(hdmi_ssc_cfg, TD_FAILURE);
    ssc_amptd = hdmi_ssc_cfg->phy_ssc.phy_ssc_cfg.ssc_amptd;
    ssc_freq = hdmi_ssc_cfg->phy_ssc.phy_ssc_cfg.ssc_freq / HDMI_THOUSAND;
    if (ssc_freq == 0) {
        return TD_FAILURE;
    }
    if (phy_hwfractional_mnx_get(hdmi_ssc_cfg->tmds_clk, hdmi_ssc_cfg->pix_clk, hdmi_ssc_cfg->deep_color) !=
        TD_SUCCESS) {
        hdmi_info("MNX get fail\n");
    }
    /* g_mnx_get.pll_ref_clk * 10000 / (ssc_freq * 4) */
    mod_dn = (td_u32)div_u64(((td_u64)g_mnx_get.pll_ref_clk * 2500), ssc_freq); /* 2500, means 10000/4 */
    /* 10000 eq 1000*10. 1000 get to unit k_hz; 10 is get point 1 significant digits. 5000 means rounding off mod_n */
    if ((mod_dn % MOD_MULTI_COEFFICIENT) >= 5000) {
        mod_n = (td_u16)(mod_dn / MOD_MULTI_COEFFICIENT + 1);
    } else {
        mod_n = (td_u16)(mod_dn / MOD_MULTI_COEFFICIENT);
    }

    if (mod_n == 0) {
        return TD_FAILURE;
    }
    mod_d0 = (td_u64)div_u64(((td_u64)phy_hwpow(PHY_POW_BASE_NUM, PHY_POW_INDEX_NUM) * g_mnx_get.mn_value * ssc_amptd),
                             mod_n);
    div_u64_rem(mod_d0, MOD_MULTI_COEFFICIENT, &rem);
    if (rem >= 50000) { /* 50000 means rounding off mod_n */
        mod_d = (td_u16)(div_u64(mod_d0, MOD_MULTI_COEFFICIENT) + 1);
    } else {
        mod_d = (td_u16)div_u64(mod_d0, MOD_MULTI_COEFFICIENT);
    }
    hdmi_info("mod_n = %x, mod_d = %x \n", mod_n, mod_d);
    if (hdmi_ssc_cfg->phy_ssc.ssc_enable) {
#if defined(HDMI_PRODUCT_SS919V100)
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_D, 0xFD);
#else
        phy_hwwrite_stb1_byte(APHY_CS_8, APHY_OFFSET_D, 0xFF);
#endif
        hal_hdmi_phy_ssc_init(mod_d, mod_n);
    } else {
        /* FDIV init */
        hdmi_fdivset0_init_set(TD_TRUE);
        udelay(1);
        hdmi_fdivset0_init_set(TD_FALSE);
        hdmi_fdivset0_en_ctrl_set(TD_FALSE);
        hdmi_fdivset0_en_mod_set(TD_FALSE);
    }

    return TD_SUCCESS;
}

td_s32 hal_hdmi_phy_ssc_get(td_bool *enable)
{
    hdmi_if_fpga_return(TD_SUCCESS);
    hdmi_if_null_return(enable, TD_FAILURE);

    *enable = TD_FALSE;
    if (hdmi_fdivset0_en_mod_get() && hdmi_fdivset0_en_ctrl_get() && hdmi_fdivset0_en_sdm_get()) {
        *enable = TD_TRUE;
    }

    return TD_SUCCESS;
}

td_s32 hal_hdmi_phy_info_get(hdmi_phy_info *phy_status)
{
    errno_t ret;
    hdmi_if_null_return(phy_status, TD_FAILURE);
    /* status information obtain */
    ret = memcpy_s(phy_status, sizeof(*phy_status), &g_hdmi_phy_info, sizeof(hdmi_phy_info));
    hdmi_unequal_eok(ret);

    return TD_SUCCESS;
}

