// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_HAL_PHY_H__
#define __HDMI_HAL_PHY_H__

#include "drv_hdmi_common.h"
#include "ot_type.h"

typedef enum {
    PHY_RPRE_50,
    PHY_RPRE_56,
    PHY_RPRE_71,
    PHY_RPRE_83,
    PHY_RPRE_100,
    PHY_RPRE_125,
    PHY_RPRE_250,
    PHY_RPRE_500,
    PHY_RPRE_BUTT
} phy_rpre;

typedef enum {
    PHY_RTERM_MODE_SINGLE,
    PHY_RTERM_MODE_SOURCE,
    PHY_RTERM_MODE_LOAD,
    PHY_RTERM_MODE_BUTT
} phy_rterm_mode;

typedef enum {
    PHY_FCON_MODE_AUTO,
    PHY_FCON_MODE_MANUAL
} phy_fcon_mode;

typedef struct {
    td_u32 ssc_amptd; /* 1/1M ppm(Spread rate range) */
    td_u32 ssc_freq;  /* 1Hz(Spread the frequency) */
} phy_ssc_cfg;

typedef struct {
    td_bool     ssc_enable;
    td_bool     ssc_debug_en;
    phy_ssc_cfg phy_ssc_cfg;
} hdmi_phy_ssc;

typedef struct {
    td_u32          pix_clk;    /* Pixel colck,in KHz */
    td_u32          tmds_clk;   /* TMDS colck,in KHz */
    hdmi_deep_color deep_color; /* Deep color(color depth)  */
    hdmi_phy_ssc    phy_ssc;    /* Spread Spectrum ctrl(ssc) para */
} hdmi_phy_ssc_cfg;

typedef struct {
    td_u32         main_clk;
    td_u32         main_d0;
    td_u32         main_d1;
    td_u32         main_d2;
    td_u32         pre_clk;
    td_u32         pre_d0;
    td_u32         pre_d1;
    td_u32         pre_d2;
    phy_rpre       rpre_clk;
    phy_rpre       rpre_d0;
    phy_rpre       rpre_d1;
    phy_rpre       rpre_d2;
    phy_rterm_mode term_mode_clk;
    td_u32         term_clk;
    phy_rterm_mode term_mode_d0;
    td_u32         term_d0;
    phy_rterm_mode term_mode_d1;
    td_u32         term_d1;
    phy_rterm_mode term_mode_d2;
    td_u32         term_d2;
} phy_hw_spec_cfg;

typedef struct {
    td_bool         hw_spec_debug_en;
    phy_hw_spec_cfg phy_spec_cfg;
} hdmi_phy_hw_spec;

typedef struct {
    td_u32           tmds_clk;      /* TMDS colck,in KHz */
    td_u8            frl_dat_rat;   /* FRL_DATA_RATA */
    hdmi_phy_hw_spec hdmi_phy_spec; /* phy specification para */
} hdmi_phy_hw_spec_cfg;

typedef struct {
    td_u32            pixel_clk;
    td_u32            tmds_clk;   /* TMDS colck,in kHz */
    td_bool           emi_enable;
    hdmi_deep_color   deep_color; /* Deep color(color depth) */
    hdmi_phy_mode_cfg mode_cfg;   /* TMDS/FRL/TxFFE */
    hdmi_trace_len    trace_len;
} hdmi_phy_tmds_cfg;

typedef struct {
    td_bool           init; /* 1:init 0:deinit */
    td_bool           power_enable;
    td_bool           oe_enable;
    hdmi_phy_tmds_cfg tmds_cfg;
    hdmi_phy_ssc      ssc_cfg;
    phy_hw_spec_cfg   hw_spec_cfg;
} hdmi_phy_info;

td_s32 hal_hdmi_phy_init(const hdmi_hal_init *hal_init);

td_s32 hal_hdmi_phy_deinit(td_void);

td_s32 hal_hdmi_phy_power_get(td_bool *enable);

td_void hal_hdmi_phy_power_set(td_bool enable);

td_void hal_hdmi_phy_oe_set(td_bool enable);

td_s32 hal_hdmi_phy_oe_get(td_bool *enable);

td_s32 hal_hdmi_phy_tmds_set(const hdmi_phy_tmds_cfg *tmds_cfg);

td_s32 hal_hdmi_phy_info_get(hdmi_phy_info *phy_status);

td_s32 hal_hdmi_phy_ssc_set(const hdmi_phy_ssc_cfg *hdmi_ssc_cfg);

td_s32 hal_hdmi_phy_ssc_get(td_bool *enable);

#endif /* __HDMI_HAL_PHY_H__ */

