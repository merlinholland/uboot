// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <version.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/platform.h>

#include "ot_type.h"
#include "ot_board.h"
#include "sys_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define io_crg_address(x) (x)
#define io_crg_pll_address(x) (x)

static td_void ot_reg_set_bit(unsigned long value, unsigned long offset, unsigned long addr)
{
    unsigned long t, mask;

    mask = 1 << offset;
    t = readl((const volatile void *)(td_uintptr_t)addr);
    t &= ~mask;
    t |= (value << offset) & mask;
    writel(t, (volatile void *)(td_uintptr_t)addr);
}

static td_void ot_reg_write32(unsigned long value, unsigned long mask, unsigned long addr)
{
    unsigned long t;

    t = readl((const volatile void *)(td_uintptr_t)addr);
    t &= ~mask;
    t |= value & mask;
    writel(t, (volatile void *)(td_uintptr_t)addr);
}

td_s32 sys_hal_vo_bus_reset_sel(td_bool reset)
{
    const td_u32 tmp = (reset == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 0; /* 0: 0bit */

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8336_ADDR));
    return TD_SUCCESS;
}

td_s32 sys_hal_vo_apb_clk_en(td_bool clk_en)
{
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 8; /* 8: 8bit */

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8336_ADDR));
    return TD_SUCCESS;
}

/* VO AXI BUS CLK */
td_s32 sys_hal_vo_bus_clk_en(td_bool clk_en)
{
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 9; /* 9: 9bit */

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8336_ADDR));
    return TD_SUCCESS;
}

td_s32 sys_hal_vo_out_hd_phase_ctrl(td_s32 vo_dev, td_bool reverse)
{
    const td_u32 tmp = (reverse == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 20; /* 20: 20bit */

    if ((vo_dev != 0) && (vo_dev != 1)) {
        return TD_FAILURE;
    }

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8338_ADDR));
    return TD_SUCCESS;
}

/* VO DHD0: HPLL, HPLL FOUT4; DHD1: FIXED, LCDMCLKs */
td_s32 sys_hal_vo_hd_clk_sel(td_s32 dev, td_u32 clk_sel)
{
    const td_u32 bit = 12; /* 12: 12bit */
    const td_u32 mask = 0xf;

    if (dev == 0) {
        ot_reg_set_bit(clk_sel, bit, io_crg_address(CRG_PERCTL8340_ADDR));
    } else if (dev == 1) {
        ot_reg_write32(clk_sel << bit, mask << bit, io_crg_address(CRG_PERCTL8341_ADDR));
    } else {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

/* VO CFG CLK */
td_s32 sys_hal_vo_cfg_clk_en(td_bool clk_en)
{
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 6; /* 6: 6bit */

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8340_ADDR));
    return TD_SUCCESS;
}

/* PPC */
td_s32 sys_hal_vo_core_clk_en(td_s32 dev, td_bool clk_en)
{
    const td_u32 bit = 5; /* 5: 5bit */
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;

    if ((dev == 0) || (dev == 1)) {
        ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8340_ADDR));
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 sys_hal_vo_dev_clk_en(td_s32 vo_dev, td_bool clk_en)
{
    const td_u32 bit = 4; /* 4: 4bit */
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;

    if (vo_dev == 0) {
        ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8340_ADDR));
    } else if (vo_dev == 1) {
        ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8341_ADDR));
    } else {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

/* VO HD dev div_mode */
td_s32 sys_hal_vo_dev_div_mode(td_s32 vo_dev, td_u32 div_mod)
{
    const td_u32 bit = 24; /* 24: 24bit */
    const td_u32 mask = 0x3;

    if (vo_dev == 0) {
        ot_reg_write32(div_mod << bit, mask << bit, io_crg_address(CRG_PERCTL8340_ADDR));
    } else if (vo_dev == 1) {
        ot_reg_write32(div_mod << bit, mask << bit, io_crg_address(CRG_PERCTL8341_ADDR));
    } else {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

/* VO SD DATE */
td_s32 sys_hal_vo_sd_date_clk_en(td_s32 vo_dev, td_bool clk_en)
{
    const td_u32 bit = 4; /* 4: 4bit */
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;

    if (vo_dev != 1) {
        return TD_FAILURE;
    }
    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8342_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_lcd_clk_en(td_s32 vo_dev, td_bool clk_en)
{
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;
    const td_u32 bit = 27; /* 27: 27bit */
    ot_unused(vo_dev);

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8346_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_lcd_mclk_div(td_s32 vo_dev, td_u32 mclk_div)
{
    const td_u32 mask = 0x7ffffff; /* 0x7ffffff: clock div coef */
    ot_unused(vo_dev);

    ot_reg_write32(mclk_div, mask, io_crg_address(CRG_PERCTL8346_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_bt_clk_sel(td_s32 vo_dev, td_u32 bt_clk_ch_sel)
{
    const td_u32 bit = 12; /* 12: 12bit */
    const td_u32 mask = 0x3;

    if ((vo_dev == 0) || (vo_dev == 1)) {
        ot_reg_write32(bt_clk_ch_sel << bit, mask << bit, io_crg_address(CRG_PERCTL8348_ADDR));
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 sys_hal_vo_bt_clk_en(td_s32 vo_dev, td_bool bt_clk_en)
{
    const td_u32 bit_bt = 5; /* 5: 5bit */
    const td_u32 bit_bt_bp = 4; /* 4: 4bit */
    const td_u32 tmp = (bt_clk_en == TD_TRUE) ? 1 : 0;
    ot_unused(vo_dev);

    ot_reg_set_bit(tmp, bit_bt, io_crg_address(CRG_PERCTL8348_ADDR));
    ot_reg_set_bit(tmp, bit_bt_bp, io_crg_address(CRG_PERCTL8348_ADDR));
    return TD_SUCCESS;
}

td_s32 sys_hal_vo_vdac_clk_en(td_s32 vo_dev, td_bool clk_en)
{
    const td_u32 bit = 4; /* 4: 4bit */
    const td_u32 tmp = (clk_en == TD_TRUE) ? 1 : 0;

    if (vo_dev != 1) {
        return TD_FAILURE;
    }

    ot_reg_set_bit(tmp, bit, io_crg_address(CRG_PERCTL8350_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_hdmi_clk_en(td_s32 vo_dev, td_bool hdmi_clk_en)
{
    const td_u32 bit_pixel = 4; /* 4: 4bit */
    const td_u32 bit_vdp = 5; /* 5: 5bit */
    const td_u32 tmp = (hdmi_clk_en == TD_TRUE) ? 1 : 0;
    ot_unused(vo_dev);

    ot_reg_set_bit(tmp, bit_pixel, io_crg_address(CRG_PERCTL8351_ADDR));
    ot_reg_set_bit(tmp, bit_vdp, io_crg_address(CRG_PERCTL8351_ADDR));
    return TD_SUCCESS;
}

td_s32 sys_hal_vo_hdmi_clk_sel(td_s32 vo_dev, td_u32 clk_ch_sel)
{
    const td_u32 bit = 20; /* 20: 20bit */
    const td_u32 mask = 0x1;

    if (vo_dev == 0) {
        ot_reg_write32(clk_ch_sel << bit, mask << bit, io_crg_address(CRG_PERCTL8351_ADDR));
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 sys_hal_vo_mipi_clk_en(td_s32 vo_dev, td_bool mipi_clk_en)
{
    const td_u32 bit_pixel = 4; /* 4: 4bit */
    const td_u32 bit_vdp = 5; /* 5: 5bit */
    const td_u32 tmp = (mipi_clk_en == TD_TRUE) ? 1 : 0;
    ot_unused(vo_dev);

    ot_reg_set_bit(tmp, bit_pixel, io_crg_address(CRG_PERCTL8352_ADDR));
    ot_reg_set_bit(tmp, bit_vdp, io_crg_address(CRG_PERCTL8352_ADDR));
    return TD_SUCCESS;
}

td_s32 sys_hal_vo_mipi_clk_sel(td_s32 vo_dev, td_u32 clk_ch_sel)
{
    const td_u32 bit = 20; /* 20: 20bit */
    const td_u32 mask = 0x1;

    if ((vo_dev == 0) || (vo_dev == 1)) {
        ot_reg_write32(clk_ch_sel << bit, mask << bit, io_crg_address(CRG_PERCTL8352_ADDR));
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 sys_hal_vo_set_pll_postdiv2(td_s32 pll, td_u32 bits_set)
{
    const td_u32 bit = 28; /* 28: 28bit */
    const td_u32 mask = 0x7;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_write32(bits_set << bit, mask << bit, io_crg_pll_address(CRG_PERCTL_PLL224_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_set_pll_postdiv1(td_s32 pll, td_u32 bits_set)
{
    const td_u32 bit = 24; /* 24: 24bit */
    const td_u32 mask = 0x7;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_write32(bits_set << bit, mask << bit, io_crg_pll_address(CRG_PERCTL_PLL224_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_set_pll_frac(td_s32 pll, td_u32 bits_set)
{
    const td_u32 bit = 0;
    const td_u32 bit_dsmpd = 25; /* 25: 25bit fractional or integer mode */
    const td_u32 mask = 0xffffff;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_write32(bits_set << bit, mask << bit, io_crg_pll_address(CRG_PERCTL_PLL224_ADDR));

    if (bits_set == 0) {
        ot_reg_set_bit(1, bit_dsmpd, io_crg_pll_address(CRG_PERCTL_PLL225_ADDR));   /* 1: integer */
    } else {
        ot_reg_set_bit(0, bit_dsmpd, io_crg_pll_address(CRG_PERCTL_PLL225_ADDR));   /* 0: fractional */
    }

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_set_pll_refdiv(td_s32 pll, td_u32 bits_set)
{
    const td_u32 bit = 12;    /* 12: 12bit */
    const td_u32 mask = 0x3f;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_write32(bits_set << bit, mask << bit, io_crg_pll_address(CRG_PERCTL_PLL225_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_set_pll_fbdiv(td_s32 pll, td_u32 bits_set)
{
    const td_u32 bit = 0;     /* 0: 0bit */
    const td_u32 mask = 0xfff;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_write32(bits_set << bit, mask << bit, io_crg_pll_address(CRG_PERCTL_PLL225_ADDR));

    return TD_SUCCESS;
}

td_s32 sys_hal_vo_set_pll_power_ctrl(td_s32 pll, td_bool power_down)
{
    const td_u32 bit = 20;     /* 20: 20bit */
    const td_u32 tmp = (power_down == TD_TRUE) ? 1 : 0;

    if (pll != 0) {
        return TD_FAILURE;
    }

    ot_reg_set_bit(tmp, bit, io_crg_pll_address(CRG_PERCTL_PLL225_ADDR));

    return TD_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
