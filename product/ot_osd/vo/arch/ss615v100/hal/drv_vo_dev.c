// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "drv_vo_dev.h"
#include "drv_vo.h"
#include "hal_vo_video.h"
#include "hal_vo_dev_comm.h"
#include "hal_vo_dev.h"
#include "ot_math.h"
#include <common.h>
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#if vo_desc("UBOOT_VO")

#define VO_VGA_GAIN_DEF                 0xB
#define VO_CVBS_DAC_GC_DEF_PAL          0x36
#define VO_CVBS_DAC_GC_DEF_NTSC         0x35

static td_u32 g_bg_color[OT_VO_MAX_PHYS_DEV_NUM] = {0x00ffff, 0xFF00, 0xFF00}; /* default value is CYAN */

td_u32 vo_dev_get_bg_color(ot_vo_dev dev)
{
    return g_bg_color[dev];
}

td_void vo_dev_set_bg_color(ot_vo_dev dev, td_u32 bg_color)
{
    g_bg_color[dev] = bg_color;
}

#if vo_desc("dev user sync timing")
/* dhd0: 1080P60, dhd1: 1080P60, dsd0: pal */
static const ot_vo_sync_info g_vo_user_sync_timing[OT_VO_MAX_PHYS_DEV_NUM] = {
    /*
     * |--INTFACE---||-----TOP-----||----HORIZON--------||----BOTTOM-----||-PULSE-||-INVERSE-|
     * syncm, iop, itf,   vact, vbb,  vfb,  hact,  hbb,  hfb, hmid,bvact,bvbb, bvfb, hpw, vpw,idv, ihs, ivs
     */
    { 0, 1, 1, 1080, 41, 4, 1920, 192, 88,  1,    1,   1,  1,  44, 5, 0, 0, 0 }, /* 1080P@60_hz */
    { 0, 1, 1, 1080, 41, 4, 1920, 192, 88,  1,    1,   1,  1,  44, 5, 0, 0, 0 }, /* 1080P@60_hz */
    { 0, 0, 0,  288, 22, 2,  720, 132, 12,  1,  288,  23,  2, 126, 3, 0, 0, 0 }, /* 576I(PAL)  */
};

const ot_vo_sync_info *vo_drv_get_dev_user_sync_timing(ot_vo_dev dev)
{
    if ((dev < 0) || (dev >= OT_VO_MAX_PHYS_DEV_NUM)) {
        return TD_NULL;
    }
    return &(g_vo_user_sync_timing[dev]);
}
#endif

#if vo_desc("dev user sync info")
/* dhd0: 1080P60, dhd1: 1080P60, dsd0: pal */
static const ot_vo_user_sync_info g_vo_user_sync_info[OT_VO_MAX_PHYS_DEV_NUM] = {
    {
        .user_sync_attr = {
            .clk_src = OT_VO_CLK_SRC_PLL,
            .vo_pll = { /* if hdmi, set it by pixel clk and div mode */
                .fb_div = 99, /* 99 fb div */
                .frac = 0,
                .ref_div = 2, /* 2 ref div */
                .post_div1 = 4, /* 4 post div1 */
                .post_div2 = 2, /* 2 post div2 */
            },
        },
        .pre_div = 1, /* if hdmi, set it by pixel clk */
        .dev_div = 1, /* if rgb, set it by serial mode */
        .clk_reverse_en = TD_TRUE,
    }, {
        .user_sync_attr = {
            .clk_src = OT_VO_CLK_SRC_PLL,
            .vo_pll = { /* if hdmi, set it by pixel clk and div mode */
                .fb_div = 99, /* 99 fb div */
                .frac = 0,
                .ref_div = 2, /* 2 ref div */
                .post_div1 = 4, /* 4 post div1 */
                .post_div2 = 2, /* 2 post div2 */
            },
        },
        .pre_div = 1, /* if hdmi, set it by pixel clk */
        .dev_div = 1, /* if rgb, set it by serial mode */
        .clk_reverse_en = TD_TRUE,
    }, {
        .user_sync_attr = {
            .clk_src = OT_VO_CLK_SRC_PLL,
            .vo_pll = { /* if hdmi, set it by pixel clk and div mode */
                .fb_div = 99, /* 99 fb div */
                .frac = 0,
                .ref_div = 2, /* 2 ref div */
                .post_div1 = 4, /* 4 post div1 */
                .post_div2 = 2, /* 2 post div2 */
            },
        },
        .pre_div = 1, /* if hdmi, set it by pixel clk */
        .dev_div = 4, /* if cvbs, set it by 4 div mode */
        .clk_reverse_en = TD_FALSE,
    }
};

const ot_vo_user_sync_info *vo_drv_get_dev_user_sync_info(ot_vo_dev dev)
{
    if ((dev < 0) || (dev >= OT_VO_MAX_PHYS_DEV_NUM)) {
        return TD_NULL;
    }
    return &g_vo_user_sync_info[dev];
}
#endif

#if vo_desc("dev")

td_void vo_drv_set_dev_user_intf_sync_attr(ot_vo_dev dev, const ot_vo_user_sync_info *sync_info)
{
    td_u32 hd_clk_sel;
    ot_vo_pll vo_pll;
    ot_mpp_chn mpp_chn;

    if ((dev == VO_DEV_DHD0) || (dev == VO_DEV_DHD1)) {
        if ((sync_info->user_sync_attr.clk_src == OT_VO_CLK_SRC_PLL) ||
            (sync_info->user_sync_attr.clk_src == OT_VO_CLK_SRC_PLL_FOUT4)) {
            /* clk source */
            if (sync_info->user_sync_attr.clk_src == OT_VO_CLK_SRC_PLL) {
                hd_clk_sel = 0;
            } else {
                hd_clk_sel = 1;
            }
            vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
            (td_void)memcpy_s(&vo_pll, sizeof(ot_vo_pll), &sync_info->user_sync_attr.vo_pll, sizeof(ot_vo_pll));
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HD_CLK_SEL, &hd_clk_sel);
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_FBDIV_SET, &vo_pll.fb_div);
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_FRAC_SET, &vo_pll.frac);
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_REFDIV_SET, &vo_pll.ref_div);
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_POSTDIV1_SET, &vo_pll.post_div1);
            call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_POSTDIV2_SET, &vo_pll.post_div2);
        }
    }
}

td_void vo_drv_set_dev_div(ot_vo_dev dev, td_u32 dev_div)
{
    ot_mpp_chn mpp_chn = {0};
    td_u32 div_mode = dev_div - 1;
    ot_vo_intf_type intf_type;

    if (dev == VO_DEV_DSD0) {
        if (dev_div == VO_INTF_CVBS_DIV_MODE) { /* if 4div, 0 to regs */
            div_mode = 0;
        }
    }

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DIV_MOD, &div_mode);

    /* RGB div mode */
    intf_type = vo_drv_get_dev_intf_type(dev);
    if (((OT_VO_INTF_RGB_6BIT & intf_type)
        || (OT_VO_INTF_RGB_8BIT & intf_type))) {
        if (dev_div == VO_INTF_RGB_DIV_MODE_3) {
            hal_disp_set_lcd_serial_perd(0);
        } else {
            hal_disp_set_lcd_serial_perd(1);
        }
    }
}

static td_void vo_drv_dev_set_hdmi_ssc_vdp_div(ot_vo_dev dev, td_u32 hdmi_ssc_vdp_div)
{
    ot_mpp_chn mpp_chn;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HDMI_SSC_VDP_DIV, &hdmi_ssc_vdp_div);
}

td_void vo_drv_set_hdmi_div(ot_vo_dev dev, td_u32 pre_div)
{
    vo_drv_dev *dev_ctx = vo_drv_get_dev_ctx(dev);
    if (vo_drv_is_hdmi_intf(dev_ctx->vou_attr.intf_type)) {
        vo_drv_dev_set_hdmi_ssc_vdp_div(dev, pre_div - 1);
    }
}

static ot_vo_intf_type vou_drv_get_dev_supported_intf(td_void)
{
    ot_vo_intf_type dev_support_intf = OT_VO_INTF_HDMI |
                                       OT_VO_INTF_BT1120 |
                                       OT_VO_INTF_BT656 |
                                       OT_VO_INTF_VGA;
    return dev_support_intf;
}

static ot_vo_intf_type vou_drv_get_dev_homologous_intf(td_void)
{
    ot_vo_intf_type homologous_intf = OT_VO_INTF_HDMI |
                                      OT_VO_INTF_BT1120 |
                                      OT_VO_INTF_VGA;
    return homologous_intf;
}

static td_s32 vou_drv_check_dhd_homologous_intf(ot_vo_dev dev, ot_vo_intf_type intf_type)
{
    ot_vo_intf_type homologous_intf = vou_drv_get_dev_homologous_intf();
    if ((intf_type & ~(homologous_intf)) &&
        (intf_type & ~(OT_VO_INTF_BT656))) {
        vo_err_trace("device HD%d, does not support homologous intf(%d)!\n", dev, intf_type);
        return OT_ERR_VO_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}

static td_s32 vou_drv_check_dhd_intf(ot_vo_dev dev, ot_vo_intf_type intf_type)
{
    ot_vo_intf_type dev_support_intf = vou_drv_get_dev_supported_intf();
    if ((intf_type & ~(dev_support_intf)) || (intf_type == 0)) {
        vo_err_trace("device HD%d does not support intf(%d)!\n", dev, intf_type);
        return OT_ERR_VO_NOT_SUPPORT;
    }

    return vou_drv_check_dhd_homologous_intf(dev, intf_type);
}

static ot_vo_intf_type vou_drv_get_dev_exclution_intf(td_void)
{
    ot_vo_intf_type dev_support_intf = OT_VO_INTF_BT1120 |
                                       OT_VO_INTF_BT656;
    return dev_support_intf;
}

td_s32 vo_drv_check_other_dev_exclution_intf(ot_vo_dev other_dev, ot_vo_intf_type intf_type)
{
    ot_vo_intf_type digital_intf = vou_drv_get_dev_exclution_intf();
    vo_drv_dev *drv_dev_ctx = vo_drv_get_dev_ctx(other_dev);

    if ((drv_dev_ctx->vou_attr.intf_type & digital_intf) && (intf_type & digital_intf)) {
        vo_err_trace("intf(%d, %d) can only use one at the same time!\n", drv_dev_ctx->vou_attr.intf_type, intf_type);
        return OT_ERR_VO_NOT_SUPPORT;
    }
    return TD_SUCCESS;
}

static td_s32 vou_drv_check_dsd_intf(ot_vo_dev dev, ot_vo_intf_type intf_type)
{
    if ((intf_type & ~(OT_VO_INTF_CVBS)) || (intf_type == 0)) {
        vo_err_trace("vo device%d does not support intf(%d)!\n", dev, intf_type);
        return OT_ERR_VO_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}

td_s32 vou_drv_check_dev_intf(ot_vo_dev dev, ot_vo_intf_type intf_type)
{
    td_s32 ret = OT_ERR_VO_INVALID_DEV_ID;

    if ((dev == VO_DEV_DHD0) || (dev == VO_DEV_DHD1)) {
        ret = vou_drv_check_dhd_intf(dev, intf_type);
    } else if (dev == VO_DEV_DSD0) {
        ret = vou_drv_check_dsd_intf(dev, intf_type);
    }

    if (ret != TD_SUCCESS) {
        return ret;
    }

    return vo_drv_check_intf_share(dev, intf_type);
}

static ot_vo_intf_sync vo_drv_get_vga_max_intf_sync(ot_vo_dev dev)
{
    ot_vo_intf_sync max_intf_sync;

    if (dev == VO_DEV_DHD0) {
        max_intf_sync = OT_VO_OUT_2560x1600_60;
    } else {
        max_intf_sync = OT_VO_OUT_1080P60;
    }

    return max_intf_sync;
}

td_s32 vou_drv_check_hdmi_sync(ot_vo_dev dev, ot_vo_intf_type intf_type, ot_vo_intf_sync intf_sync)
{
    if (intf_type & OT_VO_INTF_HDMI) {
        if ((dev == VO_DEV_DHD0)) {
            if ((intf_sync < OT_VO_OUT_640x480_60) || (intf_sync > OT_VO_OUT_3840x2160_30)) {
                vo_err_trace("for HDMI interface, vo%d's intfsync %d illegal!\n", dev, intf_sync);
                return OT_ERR_VO_NOT_SUPPORT;
            }
        } else if ((dev == VO_DEV_DHD1)) {
            if (((intf_sync < OT_VO_OUT_640x480_60) || (intf_sync > OT_VO_OUT_1080P60))) {
                vo_err_trace("for HDMI interface, vo%d's intfsync %d illegal!\n", dev, intf_sync);
                return OT_ERR_VO_NOT_SUPPORT;
            }
        }
    }
    return TD_SUCCESS;
}

td_s32 vou_drv_check_bt_sync(ot_vo_dev dev, ot_vo_intf_type intf_type, ot_vo_intf_sync intf_sync)
{
    if (intf_type & OT_VO_INTF_BT1120) {
        if (((intf_sync < OT_VO_OUT_640x480_60) ||
            (intf_sync > OT_VO_OUT_1080P60))) {
            vo_err_trace("for BT.1120 interface, vo%d's intfsync %d illegal!\n", dev, intf_sync);
            return OT_ERR_VO_NOT_SUPPORT;
        }
    }

    if (intf_type & OT_VO_INTF_BT656) {
        if ((intf_sync != OT_VO_OUT_PAL) &&
            (intf_sync != OT_VO_OUT_NTSC)) {
            vo_err_trace("for BT.656 interface, vo%d's intfsync %d illegal!\n", dev, intf_sync);
            return OT_ERR_VO_NOT_SUPPORT;
        }
    }

    return TD_SUCCESS;
}

td_s32 vou_drv_check_vga_sync(ot_vo_dev dev, ot_vo_intf_type intf_type, ot_vo_intf_sync intf_sync)
{
    if (intf_type & OT_VO_INTF_VGA) {
        if ((intf_sync < OT_VO_OUT_640x480_60) || (intf_sync > vo_drv_get_vga_max_intf_sync(dev)) ||
            /* not vesa sync, vga not suppport the following intf_sync */
            (intf_sync == OT_VO_OUT_1920x2160_30) || (intf_sync == OT_VO_OUT_2560x1440_30) ||
            (intf_sync == OT_VO_OUT_2560x1440_60)) {
            vo_err_trace("for VGA interface, vo%d's intfsync %d illegal!\n", dev, intf_sync);
            return OT_ERR_VO_NOT_SUPPORT;
        }
    }

    return TD_SUCCESS;
}

td_s32 vou_drv_check_rgb_sync(ot_vo_dev dev, ot_vo_intf_type intf_type, ot_vo_intf_sync intf_sync)
{
    if (vo_drv_is_rgb_parallel_intf(intf_type)) {
        if (intf_sync != OT_VO_OUT_USER) {
            vo_err_trace("for RGB interface(%d), vo%d's intfsync %d illegal!\n", intf_type, dev, intf_sync);
            return OT_ERR_VO_NOT_SUPPORT;
        }
    }

    return TD_SUCCESS;
}

td_u32 vo_drv_get_vga_gain_def(td_void)
{
    return VO_VGA_GAIN_DEF;
}

#endif

#if vo_desc("dev")

static td_void vo_drv_dev_cfg_init(ot_vo_dev dev)
{
    vo_drv_dev *drv_dev_ctx = vo_drv_get_dev_ctx(dev);
    vo_dev_capability *dev_cap = &drv_dev_ctx->dev_cap;

    if (dev == VO_DEV_DHD0) {
        dev_cap->dev_type = VO_UHD_HW_DEV;
    } else if (dev == VO_DEV_DHD1) {
        dev_cap->dev_type = VO_HD_HW_DEV;
    } else if (dev == VO_DEV_DSD0) {
        dev_cap->dev_type = VO_SD_HW_DEV;
    }
}

static td_void vo_drv_dev_layer_info_init(ot_vo_dev dev)
{
    vo_drv_dev *drv_dev_ctx = vo_drv_get_dev_ctx(dev);

    if (dev == VO_DEV_DHD0) {
        /* 2 video layer and 3 graphic layer in DHD0 */
        drv_dev_ctx->layer_num = 2;         /* 2: 2 video layer V0/V2 */
    } else if (dev == VO_DEV_DHD1) {
        /* 2 video layer and 3 graphic layer in DHD1 */
        drv_dev_ctx->layer_num = 1;         /* 1: default 1 video layer V1, can bind to V2 */
    } else if (dev == VO_DEV_DSD0) {
        /* 1 video layer and 2 graphic layer in DSD0 */
        drv_dev_ctx->layer_num = 1;             /* 1: default 1 video layer V3 */
    }
}

td_void vo_drv_dev_info_init(td_void)
{
    ot_vo_dev dev;
    td_u32 ctx_len;
    vo_drv_dev *drv_dev_ctx = vo_drv_get_dev_ctx(0);

    ctx_len = sizeof(vo_drv_dev) * OT_VO_MAX_DEV_NUM;
    (td_void)memset_s(drv_dev_ctx, ctx_len, 0, ctx_len);

    for (dev = 0; dev < OT_VO_MAX_DEV_NUM; dev++) {
        vo_drv_dev_cfg_init(dev);
        vo_drv_dev_layer_info_init(dev);
    }
}

#endif

#if vo_desc("dev")

ot_vo_layer vo_drv_get_layer(ot_vo_dev dev)
{
    hal_disp_layer hal_layer = HAL_DISP_LAYER_VHD0;

    if (dev == VO_DEV_DHD0) {
        hal_layer = HAL_DISP_LAYER_VHD0;
    } else if (dev == VO_DEV_DHD1) {
        hal_layer = HAL_DISP_LAYER_VHD1;
    } else if (dev == VO_DEV_DSD0) {
        hal_layer = HAL_DISP_LAYER_VSD0;
    }

    return hal_layer;
}

td_void vo_drv_set_dev_clk_sel(ot_vo_dev dev, td_u32 clk_sel)
{
    ot_mpp_chn mpp_chn;
    td_u32 hd_clk_sel = clk_sel;
    td_u32 sd_clk_sel = clk_sel;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    if ((dev == VO_DEV_DHD0) || (dev == VO_DEV_DHD1)) {
        vo_drv_dev_get_clk_pll_src(dev, &hd_clk_sel);
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HD_CLK_SEL, &hd_clk_sel);
    } else if (dev == VO_DEV_DSD0) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_SD_CLK_SEL, &sd_clk_sel);
    }
}

td_void vo_drv_set_dev_clk_en(ot_vo_dev dev, td_bool clk_en)
{
    ot_mpp_chn mpp_chn;
    td_bool vo_clk_en = clk_en;

    /* sd dev clk need open when init */
    if ((dev == VO_DEV_DSD0) && (clk_en == TD_FALSE)) {
        return;
    }
    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    if ((dev == VO_DEV_DHD0) || (dev == VO_DEV_DHD1) || (dev == VO_DEV_DSD0)) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_CLK_EN, &vo_clk_en);
    }
}

static td_void vo_drv_set_cbm_bkg(ot_vo_dev dev)
{
    hal_disp_bkcolor bkg;
    td_u32 bg_color;
    td_u32 yuv_bk_grd;
    hal_cbmmix mixer = dev;

    /* bg_color only yuv output */
    bg_color = vo_drv_get_dev_bg_color(dev);
    yuv_bk_grd = rgb_to_yuv_full(bg_color);
    bkg.bkg_y = YUV_Y(yuv_bk_grd);
    bkg.bkg_cb = YUV_U(yuv_bk_grd);
    bkg.bkg_cr = YUV_V(yuv_bk_grd);

    hal_cbm_set_cbm_bkg(mixer, &bkg);
}

static td_void vo_drv_set_intf_hdmi_cfg(ot_vo_dev dev)
{
    ot_mpp_chn mpp_chn;
    td_bool hdmi_clk_en = TD_TRUE;
    td_bool hdmi_clk_sel = dev;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HDMI_CLK_EN, &hdmi_clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HDMI_CLK_SEL, &hdmi_clk_sel);

    vo_hal_intf_set_mux_sel(dev, OT_VO_INTF_HDMI);
}

static td_void vo_drv_set_intf_bt1120_cfg(ot_vo_dev dev)
{
    td_bool bt_clk_en = TD_TRUE;
    td_u32 bt_sel = dev;
    td_u32 bt_out_dly_tune = 8; /* 8:dly_tune */
    ot_mpp_chn mpp_chn;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &bt_clk_en);  /*  [8][9] bt clk en ,bt bp en. */
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_SEL, &bt_sel);  /* [25] bt selection ch 0 or ch 1. */
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_OUT_DLY_TUNE, &bt_out_dly_tune);

    vo_hal_intf_set_mux_sel(dev, OT_VO_INTF_BT1120);

    hal_intf_bt_set_dfir_en(0x1);
    hal_intf_bt_set_data_width(0x1);
}

static td_void vo_drv_set_intf_bt656_cfg(ot_vo_dev dev)
{
    ot_mpp_chn mpp_chn;
    td_bool bt_clk_en = TD_TRUE;
    td_u32 bt_sel = dev;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &bt_clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_SEL, &bt_sel);
    vo_hal_intf_set_mux_sel(dev, OT_VO_INTF_BT656);
    hal_intf_bt_set_dfir_en(0x1);
}

static td_void vo_drv_set_intf_vga_cfg(ot_vo_dev dev)
{
    ot_mpp_chn mpp_chn;
    td_bool dac_clk_en = TD_TRUE;
    td_bool dac_clk_sel = dev;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DAC_EN, &dac_clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HD_DAC_SEL, &dac_clk_sel);

    vo_hal_intf_set_mux_sel(dev, OT_VO_INTF_VGA);

    vo_hal_intf_set_dac_sel(OT_VO_INTF_VGA);
    vo_hal_intf_set_vga_and_cvbs_dac_enable(TD_TRUE);
    vo_hal_intf_set_dac_chn_enable(OT_VO_INTF_VGA, TD_TRUE);
    vo_hal_intf_set_dac_gc(OT_VO_INTF_VGA, VO_VGA_GAIN_DEF);
}

static td_u32 vo_drv_get_intf_cvbs_dac_gc(ot_vo_intf_sync intf_sync, td_u32 vact)
{
    if (intf_sync == OT_VO_OUT_PAL) {
        return VO_CVBS_DAC_GC_DEF_PAL;
    } else if (intf_sync == OT_VO_OUT_NTSC) {
        return VO_CVBS_DAC_GC_DEF_NTSC;
    }

    if (vact == 288) { /* 288 = 576 / 2 */
        return VO_CVBS_DAC_GC_DEF_PAL;
    } else if (vact == 240) { /* 240 = 480 / 2 */
        return VO_CVBS_DAC_GC_DEF_NTSC;
    } else {
        vo_err_trace("user sync wrong vact: %u\n", vact);
        return VO_CVBS_DAC_GC_DEF_PAL;
    }
}

static td_void vo_drv_set_intf_cvbs_cfg(ot_vo_dev dev)
{
    ot_mpp_chn mpp_chn;
    hal_disp_syncinfo sync_info = {0};
    td_bool dac_clk_en = TD_TRUE;
    td_u32 date;
    td_u32 dac_gc;

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DAC_EN, &dac_clk_en);

    vo_drv_get_sync_info(dev, &sync_info);
    date = vo_hal_intf_get_date(vo_drv_get_dev_intf_sync(dev), sync_info.vact);
    vo_hal_intf_set_date_cfg(vo_drv_get_dev_intf_sync(dev), date, VO_CVBS_DATE_GAIN);

    vo_hal_intf_set_mux_sel(dev, OT_VO_INTF_CVBS);
    vo_hal_intf_set_dac_sel(OT_VO_INTF_CVBS);
    vo_hal_intf_set_vga_and_cvbs_dac_enable(TD_TRUE);
    vo_hal_intf_set_cvbs_dac_cfg();
    vo_hal_intf_set_dac_chn_enable(OT_VO_INTF_CVBS, TD_TRUE);
    dac_gc = vo_drv_get_intf_cvbs_dac_gc(vo_drv_get_dev_intf_sync(dev), sync_info.vact);
    vo_hal_intf_set_dac_gc(OT_VO_INTF_CVBS, dac_gc);
}

static td_void vo_drv_get_intf_rgb_ctrl(ot_vo_intf_type intf_type, reg_intf_lcd_ctrl *lcd_ctrl)
{
    if (OT_VO_INTF_RGB_6BIT & intf_type) {
        lcd_ctrl->bits.lcd_format = 4; /* 4: serial */
        lcd_ctrl->bits.lcd_serial_perd = 0; /* 0: 3 period */
        lcd_ctrl->bits.dfir_en = 0;
        lcd_ctrl->bits.hdmi_mode = 0;
    } else if (OT_VO_INTF_RGB_8BIT & intf_type) {
        lcd_ctrl->bits.lcd_format = 3; /* 3: serial 8bit */
        lcd_ctrl->bits.lcd_serial_perd = 1; /* 0: 4 period */
        lcd_ctrl->bits.dfir_en = 0;
        lcd_ctrl->bits.hdmi_mode = 0;
    } else if (OT_VO_INTF_RGB_16BIT & intf_type) {
        lcd_ctrl->bits.lcd_format = 1; /* 1: parallel 16bit,RGB656 */
        lcd_ctrl->bits.lcd_serial_perd = 0;
        lcd_ctrl->bits.dfir_en = 0;
        lcd_ctrl->bits.hdmi_mode = 0;
    } else if (OT_VO_INTF_RGB_18BIT & intf_type) {
        lcd_ctrl->bits.lcd_format = 2; /* 2: parallel 18bit,RGB666 */
        lcd_ctrl->bits.lcd_serial_perd = 0;
        lcd_ctrl->bits.dfir_en = 0;
        lcd_ctrl->bits.hdmi_mode = 0;
    } else if (OT_VO_INTF_RGB_24BIT & intf_type) {
        lcd_ctrl->bits.lcd_format = 0; /* 0: parallel 24bit,RGB888 */
        lcd_ctrl->bits.lcd_serial_perd = 0;
        lcd_ctrl->bits.dfir_en = 0;
        lcd_ctrl->bits.hdmi_mode = 0;
    }
}

static td_u32 vo_drv_get_rgb_dither_out_width(ot_vo_intf_type intf_type)
{
    if ((intf_type == OT_VO_INTF_RGB_16BIT) ||
        (intf_type == OT_VO_INTF_RGB_18BIT)) {
        return 6; /* 6bit */
    } else {
        return 8; /* 8bit */
    }
}

static td_void vo_drv_set_intf_rgb_cfg(ot_vo_dev dev)
{
    td_u32 bt_sel = dev;
    td_bool bt_clk_en = TD_TRUE;
    ot_vo_intf_type intf_type;
    reg_intf_lcd_ctrl lcd_ctrl = {0};
    ot_vo_intf_sync intf_sync;
    ot_mpp_chn mpp_chn;

    intf_type = vo_drv_get_dev_intf_type(dev);
    intf_sync = vo_drv_get_dev_intf_sync(dev);

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    /* rgb also need config */
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &bt_clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_SEL, &bt_sel);

    vo_hal_intf_set_mux_sel(dev, intf_type);

    /* rgb should config rgb_ctrl */
    vo_drv_get_intf_rgb_ctrl(intf_type, &lcd_ctrl);

    vo_hal_set_intf_ctrl(intf_type, &(lcd_ctrl.u32));

    /* dither config */
    vo_drv_set_intf_dither(intf_type, vo_drv_get_rgb_dither_out_width(intf_type));

    if (intf_sync != OT_VO_OUT_USER) {
        hal_disp_set_lcd_serial_perd(lcd_ctrl.bits.lcd_serial_perd);
    }
}

static td_void vo_drv_set_intf_cfg(ot_vo_dev dev)
{
    ot_vo_intf_type intf_type;

    intf_type = vo_drv_get_dev_intf_type(dev);
    if (OT_VO_INTF_HDMI & intf_type) {
        vo_drv_set_intf_hdmi_cfg(dev);
    }

    if (OT_VO_INTF_BT1120 & intf_type) {
        vo_drv_set_intf_bt1120_cfg(dev);
    }

    if (OT_VO_INTF_BT656 & intf_type) {
        vo_drv_set_intf_bt656_cfg(dev);
    }

    if (OT_VO_INTF_VGA & intf_type) {
        vo_drv_set_intf_vga_cfg(dev);
    }

    if (OT_VO_INTF_CVBS & intf_type) {
        vo_drv_set_intf_cvbs_cfg(dev);
    }

    if (vo_drv_is_rgb_intf(intf_type)) {
        vo_drv_set_intf_rgb_cfg(dev);
    }
}

static td_void vo_drv_get_sync_inv(ot_vo_dev dev, hal_disp_syncinv *inv)
{
    hal_disp_syncinfo sync_info;
    ot_vo_intf_type intf_type;
    ot_vo_intf_sync intf_sync;

    intf_type = vo_drv_get_dev_intf_type(dev);
    intf_sync = vo_drv_get_dev_intf_sync(dev);

    vo_drv_get_sync_info(dev, &sync_info);

    inv->hs_inv = sync_info.ihs ? 1 : 0;
    inv->vs_inv = sync_info.ivs ? 1 : 0;
    inv->dv_inv = sync_info.idv ? 1 : 0;

    if (OT_VO_INTF_HDMI & intf_type) {
        /* vsync/hsync should be 1 for hdmi test */
        if ((intf_sync == OT_VO_OUT_576P50) || (intf_sync == OT_VO_OUT_480P60)) {
            inv->hs_inv = 1 - inv->hs_inv;
            inv->vs_inv = 1 - inv->vs_inv;
        }
    }

    if (OT_VO_INTF_BT656 & intf_type) {
        inv->hs_inv = 1;
    }

    if (OT_VO_INTF_CVBS & intf_type) {
        inv->hs_inv = 1;
    }
}

static td_void vo_drv_set_sync_inv(ot_vo_dev dev, const hal_disp_syncinv *inv)
{
    ot_vo_intf_type intf_type = vo_drv_get_dev_intf_type(dev);
    if (vo_drv_is_hdmi_intf(intf_type)) {
        vo_hal_intf_set_hdmi_sync_inv(inv);
    }

    if (vo_drv_is_bt_intf(intf_type)) {
        vo_hal_intf_set_bt_sync_inv(inv);
    }

    if (vo_drv_is_vga_intf(intf_type)) {
        vo_hal_intf_set_vga_sync_inv(inv);
    }

    if (vo_drv_is_cvbs_intf(intf_type)) {
        vo_hal_intf_set_cvbs_sync_inv(inv);
    }

    if (vo_drv_is_rgb_intf(intf_type)) {
        vo_hal_intf_set_rgb_sync_inv(inv);
    }
}

td_void vo_drv_set_clk_reverse(ot_vo_dev dev, td_bool reverse)
{
    ot_mpp_chn mpp_chn = {0};
    ot_vo_intf_type intf_type = vo_drv_get_dev_intf_type(dev);
    if ((vo_drv_is_rgb_intf(intf_type) != TD_TRUE) && (vo_drv_is_bt_intf(intf_type) != TD_TRUE)) {
        return;
    }

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HD_CLKOUT_PHASIC_REVERSE_EN, &reverse);
}

static td_bool vo_drv_get_dev_clk_reverse(ot_vo_dev dev)
{
    td_bool clk_reverse_en = TD_TRUE;
    ot_vo_intf_type intf_type = vo_drv_get_dev_intf_type(dev);
    if (intf_type & OT_VO_INTF_RGB_24BIT) {
        clk_reverse_en = TD_FALSE;
    }

    return clk_reverse_en;
}

static td_void vo_drv_set_dev_clk_reverse(ot_vo_dev dev)
{
    td_bool clk_reverse_en = vo_drv_get_dev_clk_reverse(dev);
    ot_vo_intf_sync intf_sync = vo_drv_get_dev_intf_sync(dev);
    if (intf_sync == OT_VO_OUT_USER) {
        return;
    }

    vo_drv_set_clk_reverse(dev, clk_reverse_en);
}

static td_void vo_drv_get_div_mod_by_rgb_intf(ot_vo_intf_type intf_type, td_u32 *div_mode)
{
    if (OT_VO_INTF_RGB_8BIT & intf_type) {
        *div_mode = 3; /* 3: 4div */
    } else if (OT_VO_INTF_RGB_6BIT & intf_type) {
        *div_mode = 2; /* 2: 3div */
    } else if ((OT_VO_INTF_RGB_16BIT & intf_type) ||
               (OT_VO_INTF_RGB_18BIT & intf_type) ||
               (OT_VO_INTF_RGB_24BIT & intf_type)) {
        *div_mode = 0; /* 0: 1div */
    }
}

static td_void vo_drv_get_div_mod(ot_vo_dev dev, td_u32 *div_mode)
{
    ot_vo_intf_type intf_type;

    intf_type = vo_drv_get_dev_intf_type(dev);
    if (OT_VO_INTF_HDMI & intf_type) {
        *div_mode = 0; /* 0: 1div */
    }

    if (OT_VO_INTF_BT1120 & intf_type) {
        *div_mode = 0; /* 0: 1div */
    }

    if (OT_VO_INTF_BT656 & intf_type) {
        *div_mode = 1; /* 1: 2div */
    }

    if (OT_VO_INTF_VGA & intf_type) {
        *div_mode = 0; /* 0: 1div */
    }

    if (OT_VO_INTF_CVBS & intf_type) {
        *div_mode = 0; /* 0: 4div, 1: 1div */
    }

    if (vo_drv_is_rgb_intf(intf_type)) {
        vo_drv_get_div_mod_by_rgb_intf(intf_type, div_mode);
    }
}

static td_void vo_drv_set_div_mod(ot_vo_dev dev, td_u32 div_mode)
{
    ot_mpp_chn mpp_chn;
    ot_vo_intf_sync intf_sync;

    intf_sync = vo_drv_get_dev_intf_sync(dev);
    if (intf_sync == OT_VO_OUT_USER) {
        return;
    }

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DIV_MOD, &div_mode);
}

static td_void vo_drv_set_intf_clip(ot_vo_dev dev)
{
    ot_vo_intf_type intf_type;

    /* clip to limit y:16~235 c:16~240to 10 bit config */
    intf_type = vo_drv_get_dev_intf_type(dev);
    if (OT_VO_INTF_BT1120 & intf_type) {
        hal_disp_clip clip_data = {0x40, 0x40, 0x40, 0x3ac, 0x3c0, 0x3c0};
        vo_hal_intf_set_clip_cfg(OT_VO_INTF_BT1120, TD_TRUE, &clip_data);
    }

    if (OT_VO_INTF_CVBS & intf_type) {
        hal_disp_clip clip_data = {0x40, 0x40, 0x40, 0x3ac, 0x3c0, 0x3c0};
        vo_hal_intf_set_clip_cfg(OT_VO_INTF_CVBS, TD_TRUE, &clip_data);
    }

    if (OT_VO_INTF_BT656 & intf_type) {
        hal_disp_clip clip_data = {0x40, 0x40, 0x40, 0x3ac, 0x3c0, 0x3c0};
        vo_hal_intf_set_clip_cfg(OT_VO_INTF_BT656, TD_TRUE, &clip_data);
    }
}

td_void vo_drv_open(ot_vo_dev dev)
{
    hal_disp_syncinfo sync_info;
    hal_disp_syncinv inv = {0};
    td_u32 div_mode = 0;

    vo_drv_set_cbm_bkg(dev);

    vo_drv_set_intf_cfg(dev);

    vo_drv_get_sync_inv(dev, &inv);
    vo_drv_set_sync_inv(dev, &inv);
    vo_drv_get_sync_info(dev, &sync_info);
    vo_drv_set_sync_info(dev, &sync_info);

    vo_drv_get_div_mod(dev, &div_mode);
    vo_drv_set_div_mod(dev, div_mode);

    vo_drv_set_dev_clk_reverse(dev);

    hal_disp_set_dev_multi_chn_en(dev, HAL_MULTICHN_EN_1P1C);

    vo_drv_set_intf_clip(dev);

    /* should open crg first then vdp enable */
    hal_disp_set_intf_enable(dev, TD_TRUE);

    hal_disp_set_reg_up(dev);
}

td_void vo_drv_close(ot_vo_dev dev)
{
    ot_mpp_chn mpp_chn;
    td_bool clk_en = TD_FALSE;
    td_bool hdmi_ssc_div = 0x0;
    ot_vo_intf_type intf_type;

    hal_disp_set_intf_enable(dev, TD_FALSE);
    hal_disp_set_reg_up(dev);

    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);

    vo_drv_get_dev_reg_up(dev);

    intf_type = vo_drv_get_dev_intf_type(dev);
    if (OT_VO_INTF_HDMI & intf_type) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HDMI_SSC_VDP_DIV, &hdmi_ssc_div);
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_HDMI_CLK_EN, &clk_en);
    }

    if (OT_VO_INTF_BT1120 & intf_type) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &clk_en);
    }

    if (OT_VO_INTF_BT656 & intf_type) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &clk_en);
    }

    if (vo_drv_is_rgb_intf(intf_type)) {
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BT1120_CLK_EN, &clk_en);
    }

    /* vo_dac_ctrl envbg is for both vga and cvbs, so not close it */
    if (OT_VO_INTF_CVBS & intf_type) {
        vo_hal_intf_set_dac_chn_enable(OT_VO_INTF_CVBS, TD_FALSE);
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DAC_EN, &clk_en);
    }

    /* vo_dac_ctrl envbg is for both vga and cvbs, so not close it */
    if (OT_VO_INTF_VGA & intf_type) {
        vo_hal_intf_set_dac_chn_enable(OT_VO_INTF_VGA, TD_FALSE);
        call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_DAC_EN, &clk_en);
    }
}

static td_s32 vo_drv_check_dev_id_support(ot_vo_dev dev)
{
    return TD_SUCCESS;
}

static td_s32 vo_drv_check_layer_id_support(ot_vo_layer layer)
{
    if (layer == OT_VO_LAYER_V2) {
        return OT_ERR_VO_INVALID_LAYER_ID;
    }

    return TD_SUCCESS;
}

td_s32 vo_drv_check_dev_id(ot_vo_dev dev)
{
    td_s32 ret = vo_drv_check_dev_id_pub(dev);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return vo_drv_check_dev_id_support(dev);
}

td_s32 vo_drv_check_layer_id(ot_vo_layer layer)
{
    td_s32 ret = vo_drv_check_layer_id_pub(layer);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return vo_drv_check_layer_id_support(layer);
}

#endif

#if vo_desc("dev")

td_void vo_drv_dev_get_clk_pll_src(ot_vo_dev dev, td_u32 *pll_src)
{
    vo_drv_dev *dev_ctx = vo_drv_get_dev_ctx(dev);
    if (vo_drv_is_rgb_intf(dev_ctx->vou_attr.intf_type)) {
        *pll_src = VO_CLK_PLL_SRC_FOUT4;
    }
}

static vo_pll_param g_vo_pll_param_no_div[OT_VO_OUT_BUTT] = {
    /* index,                fb_div, frac, ref_div, post_div1,post_div2 */
    {OT_VO_OUT_PAL,          {81,   0,       2,  6, 6}, 0}, /* 27MHz, 2 * 13.5MHz */
    {OT_VO_OUT_NTSC,         {81,   0,       2,  6, 6}, 0}, /* 27MHz, 2 * 13.5MHz */
    {OT_VO_OUT_960H_PAL,     {36,   0,       1,  6, 2}, 0}, /* 36MHz */
    {OT_VO_OUT_960H_NTSC,    {36,   0,       1,  6, 2}, 0}, /* 36MHz */
    {OT_VO_OUT_640x480_60,   {36,  0xc00000, 1,  7, 5}, 0}, /* 25.175MHz */
    {OT_VO_OUT_480P60,       {81,   0,       2,  6, 6}, 0}, /* 27MHz */
    {OT_VO_OUT_576P50,       {81,   0,       2,  6, 6}, 0}, /* 27MHz */
    {OT_VO_OUT_800x600_60,   {80,   0,       2,  6, 4}, 0}, /* 40MHz */
    {OT_VO_OUT_1024x768_60,  {65,   0,       1,  6, 4}, 0}, /* 65MHz */
    {OT_VO_OUT_720P50,       {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_720P60,       {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1280x800_60,  {167,  0,       4,  6, 2}, 0}, /* 83.5MHz */
    {OT_VO_OUT_1280x1024_60, {72,   0,       2,  4, 2}, 0}, /* 108MHz */
    {OT_VO_OUT_1366x768_60,  {171,  0,       4,  6, 2}, 0}, /* 85.5MHz */
    {OT_VO_OUT_1400x1050_60, {121, 0xc00000, 1,  6, 4}, 0}, /* 121.75MHz */
    {OT_VO_OUT_1440x900_60,  {213,  0,       4,  6, 2}, 0}, /* 106.5MHz */
    {OT_VO_OUT_1680x1050_60, {97,  0x800000, 1,  4, 4}, 0}, /* 146.25MHz */
    {OT_VO_OUT_1080P24,      {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080P25,      {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080P30,      {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080I50,      {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080I60,      {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080P50,      {99,   0,       2,  4, 2}, 0}, /* 148.5MHz */
    {OT_VO_OUT_1080P60,      {99,   0,       2,  4, 2}, 0}, /* 148.5MHz */
    {OT_VO_OUT_1600x1200_60, {81,   0,       2,  3, 2}, 0}, /* 162MHz */
    {OT_VO_OUT_1920x1200_60, {154,  0,       4,  3, 2}, 0}, /* 154MHz */
    {OT_VO_OUT_1920x2160_30, {99,   0,       2,  4, 2}, 0}, /* 148.5MHz */
    {OT_VO_OUT_2560x1440_30, {50, 0x600000,  1,  5, 2}, 0}, /* 120.9MHz */
    {OT_VO_OUT_2560x1440_60, {161,  0,       4,  4, 1}, 0}, /* 241.5MHz */
    {OT_VO_OUT_2560x1600_60, {179,  0,       2,  4, 2}, 0}, /* 268.5MHz */
    {OT_VO_OUT_3840x2160_24, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_3840x2160_25, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_3840x2160_30, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_3840x2160_50, {99,   0,       2,  2, 1}, 0}, /* 594MHz */
    {OT_VO_OUT_3840x2160_60, {99,   0,       2,  2, 1}, 0}, /* 594MHz */
    {OT_VO_OUT_4096x2160_24, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_4096x2160_25, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_4096x2160_30, {99,   0,       2,  4, 1}, 0}, /* 297MHz */
    {OT_VO_OUT_4096x2160_50, {99,   0,       2,  2, 1}, 0}, /* 594MHz */
    {OT_VO_OUT_4096x2160_60, {99,   0,       2,  2, 1}, 0}, /* 594MHz */
    {OT_VO_OUT_7680x4320_30, {99,   0,       2,  2, 1}, 0}, /* 1188MHz */
    {OT_VO_OUT_240x320_50,   {36, 1207960,   1,  4, 2}, 0}, /* 4.5MHz */
    {OT_VO_OUT_320x240_50,   {36, 13136560,  1,  3, 3}, 0}, /* 4.0MHz */
    {OT_VO_OUT_240x320_60,   {36, 12348031,  1,  5, 4}, 0}, /* 5.5MHz */
    {OT_VO_OUT_320x240_60,   {41, 6630356,   1,  5, 4}, 0}, /* 6.2MHz */
    {OT_VO_OUT_800x600_50,   {33, 8858370,   1,  3, 1}, 0}, /* 33.5MHz */
    {OT_VO_OUT_720x1280_60,  {99,   0,       2,  4, 4}, 0}, /* 74.25MHz */
    {OT_VO_OUT_1080x1920_60, {99,   0,       2,  4, 2}, 0}, /* 148.5MHz */
    {} /* user */
};

static vo_pll_param g_vo_pll_param_with_div[OT_VO_OUT_BUTT] = {
    /* index,                fb_div, frac, ref_div, post_div1,post_div2 */
    {OT_VO_OUT_PAL,          {0,    0,  0,  0, 0},                      0}, /* 13.5MHz */
    {OT_VO_OUT_NTSC,         {0,    0,  0,  0, 0},                      0}, /* 13.5MHz */
    {OT_VO_OUT_960H_PAL,     {0,    0,  0,  0, 0},                      0}, /* 36MHz */
    {OT_VO_OUT_960H_NTSC,    {0,    0,  0,  0, 0},                      0}, /* 36MHz */
    {OT_VO_OUT_640x480_60,   {33, 0x920000, 1, 2, 1}, SSC_VDP_DIV_25_TO_50}, /* 25.175MHz */
    {OT_VO_OUT_480P60,       {36,   0,  1,  2, 1},   SSC_VDP_DIV_25_TO_50}, /* 27MHz */
    {OT_VO_OUT_576P50,       {36,   0,  1,  2, 1},   SSC_VDP_DIV_25_TO_50}, /* 27MHz */
    {OT_VO_OUT_800x600_60,   {80,   0,  1,  3, 1},   SSC_VDP_DIV_25_TO_50}, /* 40MHz */
    {OT_VO_OUT_1024x768_60,  {130,  0,  3,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 65MHz */
    {OT_VO_OUT_720P50,       {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_720P60,       {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1280x800_60,  {167,  0,  3,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 83.5MHz */
    {OT_VO_OUT_1280x1024_60, {36,   0,  1,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 108MHz */
    {OT_VO_OUT_1366x768_60,  {57,   0,  1,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 85.5MHz */
    {OT_VO_OUT_1400x1050_60, {121, 0xc00000, 1, 3, 2}, SSC_VDP_DIV_100_TO_200}, /* 121.75MHz */
    {OT_VO_OUT_1440x900_60,  {71,   0,  2,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 106.5MHz */
    {OT_VO_OUT_1680x1050_60, {195,  0,  4,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 146.25MHz */
    {OT_VO_OUT_1080P24,      {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080P25,      {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080P30,      {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080I50,      {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080I60,      {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080P50,      {99,   0,  2,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 148.5MHz */
    {OT_VO_OUT_1080P60,      {99,   0,  2,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 148.5MHz */
    {OT_VO_OUT_1600x1200_60, {54,   0,  1,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 162MHz */
    {OT_VO_OUT_1920x1200_60, {154,  0,  2,  3, 1}, SSC_VDP_DIV_100_TO_200}, /* 154MHz */
    {OT_VO_OUT_1920x2160_30, {99,   0,  2,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 148.5MHz */
    {OT_VO_OUT_2560x1440_30, {40, 0x500000, 1, 2, 1}, SSC_VDP_DIV_100_TO_200}, /* 120.9MHz */
    {OT_VO_OUT_2560x1440_60, {161,  0,  4,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 241.5MHz */
    {OT_VO_OUT_2560x1600_60, {179,  0,  2,  2, 2}, SSC_VDP_DIV_200_TO_340}, /* 268.5MHz */
    {OT_VO_OUT_3840x2160_24, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_3840x2160_25, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_3840x2160_30, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_3840x2160_50, {99,   0,  2,  2, 1}, SSC_VDP_DIV_340_TO_600}, /* 594MHz */
    {OT_VO_OUT_3840x2160_60, {99,   0,  2,  2, 1}, SSC_VDP_DIV_340_TO_600}, /* 594MHz */
    {OT_VO_OUT_4096x2160_24, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_4096x2160_25, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_4096x2160_30, {99,   0,  2,  2, 1}, SSC_VDP_DIV_200_TO_340}, /* 297MHz */
    {OT_VO_OUT_4096x2160_50, {99,   0,  2,  2, 1}, SSC_VDP_DIV_340_TO_600}, /* 594MHz */
    {OT_VO_OUT_4096x2160_60, {99,   0,  2,  2, 1}, SSC_VDP_DIV_340_TO_600}, /* 594MHz */
    {OT_VO_OUT_7680x4320_30, {99,   0,  2,  2, 1},                      0}, /* 1188MHz */
    {OT_VO_OUT_240x320_50,   {36, 1207960,  1,  4, 2},                  0}, /* 4.5MHz */
    {OT_VO_OUT_320x240_50,   {36, 13136560, 1,  3, 3},                  0}, /* 4.0MHz */
    {OT_VO_OUT_240x320_60,   {36, 12348031, 1,  5, 4},                  0}, /* 5.5MHz */
    {OT_VO_OUT_320x240_60,   {41, 6630356,  1,  5, 1},                  0}, /* 6.2MHz */
    {OT_VO_OUT_800x600_50,   {33, 8858370,  1,  3, 1},                  0}, /* 33.5MHz */
    {OT_VO_OUT_720x1280_60,  {99,   0,  2,  2, 1},  SSC_VDP_DIV_50_TO_100}, /* 74.25MHz */
    {OT_VO_OUT_1080x1920_60, {99,   0,  2,  2, 1}, SSC_VDP_DIV_100_TO_200}, /* 148.5MHz */
    {} /* user */
};

static vo_pll_param *vo_drv_get_pll_param_with_div(ot_vo_intf_sync intf_sync)
{
    return &g_vo_pll_param_with_div[intf_sync];
}

static vo_pll_param *vo_drv_get_pll_param_no_div(ot_vo_intf_sync intf_sync)
{
    return &g_vo_pll_param_no_div[intf_sync];
}

static vo_hdmi_ssc_vdp_div_mode vo_drv_get_pll_div(ot_vo_intf_sync intf_sync)
{
    return g_vo_pll_param_with_div[intf_sync].div;
}

td_void vo_drv_get_pll_cfg_no_div(ot_vo_intf_sync intf_sync, ot_vo_pll *pll)
{
    vo_pll_param pll_init = {OT_VO_OUT_1080P60, {99, 0, 2, 4, 2}, 0};
    vo_pll_param *pll_ret = &pll_init;

    if (intf_sync < OT_VO_OUT_USER) {
        pll_ret = vo_drv_get_pll_param_no_div(intf_sync);
    }

    pll->post_div2 = pll_ret->pll.post_div2;
    pll->post_div1 = pll_ret->pll.post_div1;
    pll->frac = pll_ret->pll.frac;
    pll->fb_div = pll_ret->pll.fb_div;
    pll->ref_div = pll_ret->pll.ref_div;
}

td_void vo_drv_get_pll_cfg_with_div(ot_vo_intf_sync intf_sync, ot_vo_pll *pll)
{
    vo_pll_param pll_init = {OT_VO_OUT_1080P60, {99, 0, 2, 2, 1}, SSC_VDP_DIV_100_TO_200};
    vo_pll_param *pll_ret = &pll_init;

    if (intf_sync < OT_VO_OUT_USER) {
        pll_ret = vo_drv_get_pll_param_with_div(intf_sync);
    }

    pll->post_div2 = pll_ret->pll.post_div2;
    pll->post_div1 = pll_ret->pll.post_div1;
    pll->frac = pll_ret->pll.frac;
    pll->fb_div = pll_ret->pll.fb_div;
    pll->ref_div = pll_ret->pll.ref_div;
}

td_void vo_drv_dev_get_pll_cfg(ot_vo_dev dev, ot_vo_pll *pll)
{
    vo_drv_dev *dev_ctx = vo_drv_get_dev_ctx(dev);
    ot_vo_intf_sync intf_sync = dev_ctx->vou_attr.intf_sync;

    if (vo_drv_is_hdmi_intf(dev_ctx->vou_attr.intf_type)) {
        vo_drv_get_pll_cfg_with_div(intf_sync, pll);
    } else {
        vo_drv_get_pll_cfg_no_div(intf_sync, pll);
    }
}

td_void vo_drv_dev_set_pll_cfg(ot_vo_dev dev, ot_vo_pll *pll)
{
    ot_mpp_chn mpp_chn = {0};
    vo_drv_set_dev_mpp_chn(dev, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_FBDIV_SET, &pll->fb_div);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_FRAC_SET, &pll->frac);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_REFDIV_SET, &pll->ref_div);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_POSTDIV1_SET, &pll->post_div1);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_PLL_POSTDIV2_SET, &pll->post_div2);
}

td_s32 vo_drv_check_dev_pll_ref_div(ot_vo_dev dev, td_u32 ref_div)
{
    /* ref_div should be [1, 4] */
    if (ref_div > VO_MAX_PLL_REF_DIV) {
        vo_err_trace("dev(%d) pll param ref_div %d illegal, should be [%d, %d]\n",
            dev, ref_div, VO_MIN_PLL_REF_DIV, VO_MAX_PLL_REF_DIV);
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_u32 vo_drv_dev_get_max_clk(ot_vo_dev dev)
{
    td_u32 max_clk;

    max_clk = (dev == VO_DEV_DHD0) ?  VO_PLL_MAX_CLK_297000 : VO_PLL_MAX_CLK_148500;
    return max_clk;
}

td_bool vo_drv_is_support_hdmi_ssc_vdp_div(td_void)
{
    return TD_TRUE;
}

td_void vo_drv_get_hdmi_ssc_vdp_div(ot_vo_intf_sync intf_sync, td_u32 *hdmi_ssc_vdp_div)
{
    if (intf_sync < OT_VO_OUT_USER) {
        *hdmi_ssc_vdp_div = vo_drv_get_pll_div(intf_sync);
    }
}

static td_void vo_drv_set_hd_clk(ot_vo_dev dev)
{
    ot_vo_pll pll = {0};
    td_u32 hdmi_ssc_vdp_div = 0x0;
    vo_drv_dev *dev_ctx = vo_drv_get_dev_ctx(dev);

    vo_drv_dev_get_pll_cfg(dev, &pll);
    vo_drv_dev_set_pll_cfg(dev, &pll);

    if (vo_drv_is_hdmi_intf(dev_ctx->vou_attr.intf_type)) {
        vo_drv_get_hdmi_ssc_vdp_div(dev_ctx->vou_attr.intf_sync, &hdmi_ssc_vdp_div);
        vo_drv_dev_set_hdmi_ssc_vdp_div(dev, hdmi_ssc_vdp_div);
    }
}

td_void vo_drv_set_dev_clk(ot_vo_dev dev)
{
    vo_drv_dev *dev_ctx = vo_drv_get_dev_ctx(dev);
    if (dev_ctx->vou_attr.intf_sync == OT_VO_OUT_USER) {
        return;
    }

    if (vo_drv_is_phy_dev_uhd(dev) || vo_drv_is_phy_dev_hd(dev)) {
        vo_drv_set_hd_clk(dev);
    }
}

#endif

#if vo_desc("dev clk")

static td_void vo_drv_set_sd_date_crg_clk(td_bool clk_en)
{
    ot_mpp_chn mpp_chn;

    vo_drv_set_dev_mpp_chn(VO_DEV_DSD0, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_DEV_CLK_EN, &clk_en);
}

td_void vo_drv_set_all_crg_clk(td_bool clk_en)
{
    ot_mpp_chn mpp_chn;

    vo_drv_set_dev_mpp_chn(0, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_CFG_CLK_EN, &clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_APB_CLK_EN, &clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BUS_CLK_EN, &clk_en);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_CORE_CLK_EN, &clk_en);

    /* need open sd date crg */
    vo_drv_set_sd_date_crg_clk(clk_en);
}

td_void vo_lpw_bus_reset(td_bool reset)
{
    ot_mpp_chn mpp_chn;

    /* see sys_hal_vo_bus_reset_sel */
    vo_drv_set_dev_mpp_chn(0, &mpp_chn);
    call_sys_drv_ioctrl(&mpp_chn, SYS_VO_BUS_RESET_SEL, &reset);
}

#endif

#if vo_desc("dev intf")

td_bool vo_drv_is_dev_support_rgb(ot_vo_dev dev)
{
    return TD_FALSE;
}

#endif

td_s32 vo_drv_check_dev_clk_src(ot_vo_dev dev, ot_vo_clk_src clk_src)
{
    if ((clk_src != OT_VO_CLK_SRC_PLL) && (clk_src != OT_VO_CLK_SRC_PLL_FOUT4)) {
        vo_err_trace("dev(%d) clk source %d illegal, dev only support pll/pll fout4 clk source.\n",
            dev, clk_src);
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 vo_drv_check_dev_clkvalue(ot_vo_dev dev, const ot_vo_user_sync_info *sync_info)
{
    td_s32 ret;

    if ((dev != VO_DEV_DHD0) && (dev != VO_DEV_DHD1)) {
        return TD_SUCCESS;
    }

    ret = vo_drv_check_dev_clk_src(dev, sync_info->user_sync_attr.clk_src);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return vo_drv_check_dev_pll(dev, sync_info);
}

td_s32 vo_drv_check_intf_user_dev_div(ot_vo_dev dev, td_u32 dev_div)
{
    ot_vo_intf_type intf_type;
    td_u32 intf_dev_div = 0;

    intf_type = vo_drv_get_dev_intf_type(dev);
    if (vo_drv_is_rgb_intf(intf_type)) {
        if ((dev_div != VO_INTF_RGB_DIV_MODE_1) &&
            (dev_div != VO_INTF_RGB_DIV_MODE_3) &&
            (dev_div != VO_INTF_RGB_DIV_MODE_4)) {
            vo_err_trace("vo(%d) div mode %d is illegal, it must be %d/%d/%d when intf type is %d.\n", dev, dev_div,
                VO_INTF_RGB_DIV_MODE_1, VO_INTF_RGB_DIV_MODE_3, VO_INTF_RGB_DIV_MODE_4, intf_type);
            return OT_ERR_VO_ILLEGAL_PARAM;
        }

        return TD_SUCCESS;
    }

    if (intf_type & (OT_VO_INTF_HDMI | OT_VO_INTF_BT1120 | OT_VO_INTF_VGA)) {
        intf_dev_div = VO_INTF_HDMI_DIV_MODE;
    }

    if (intf_type & OT_VO_INTF_CVBS) {
        intf_dev_div = VO_INTF_CVBS_DIV_MODE;
    }

    if (intf_type & OT_VO_INTF_BT656) {
        intf_dev_div = VO_INTF_BT656_DIV_MODE;
    }

    if (dev_div != intf_dev_div) {
        vo_err_trace("vo(%d) dev div mode %d is illegal, it must be %d when intf type is %d.\n", dev, dev_div,
            intf_dev_div, intf_type);
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

#if vo_desc("intf dither")
static td_void vo_drv_get_intf_dither_in_out_mode(vdp_dither_cfg *cfg)
{
    td_u32 i_data_width_dither = 0;
    td_u32 o_data_width_dither = 0;
    td_u32 dither_io_md;
    td_u32 dither_md = 0;

    dither_io_md = cfg->io_mode;
    if (dither_io_md == DITHER_IO_MODE_12_8) { /* rgb24bit */
        dither_md = DITHER_MODE_8BIT;
        i_data_width_dither = DITHER_IWIDTH_MODE_12BIT;
        o_data_width_dither = DITHER_OWIDTH_MODE_8BIT;
    } else if (dither_io_md == DITHER_IO_MODE_10_6) { /* rgb16bit, rgb18bit */
        dither_md = DITHER_MODE_8BIT;
        i_data_width_dither = DITHER_IWIDTH_MODE_10BIT;
        o_data_width_dither = DITHER_OWIDTH_MODE_6BIT;
    }
    cfg->dither_mode = dither_md;
    cfg->i_data_width_dither = i_data_width_dither;
    cfg->o_data_width_dither = o_data_width_dither;
}

static td_void vo_drv_get_intf_dither_sed_and_thr(vdp_dither_cfg *cfg)
{
    const td_u32 sed_y0 = 2147483647; /* 2147483647: typical y0 */
    const td_u32 sed_u0 = 2147482647; /* 2147482647: typical u0 */
    const td_u32 sed_v0 = 2147481647; /* 2147481647: typical v0 */
    const td_u32 sed_w0 = 2147480647; /* 2147480647: typical w0 */
    const td_u32 thr_max = 60000; /* 60000: dither threshold max */
    const td_u32 thr_min = 120; /* 120: dither threshold max */

    cfg->dither_sed.dither_sed_y0 = sed_y0;
    cfg->dither_sed.dither_sed_u0 = sed_u0;
    cfg->dither_sed.dither_sed_v0 = sed_v0;
    cfg->dither_sed.dither_sed_w0 = sed_w0;
    cfg->dither_sed.dither_sed_y1 = sed_y0;
    cfg->dither_sed.dither_sed_u1 = sed_u0;
    cfg->dither_sed.dither_sed_v1 = sed_v0;
    cfg->dither_sed.dither_sed_w1 = sed_w0;
    cfg->dither_sed.dither_sed_y2 = sed_y0;
    cfg->dither_sed.dither_sed_u2 = sed_u0;
    cfg->dither_sed.dither_sed_v2 = sed_v0;
    cfg->dither_sed.dither_sed_w2 = sed_w0;
    cfg->dither_sed.dither_sed_y3 = sed_y0;
    cfg->dither_sed.dither_sed_u3 = sed_u0;
    cfg->dither_sed.dither_sed_v3 = sed_v0;
    cfg->dither_sed.dither_sed_w3 = sed_w0;
    cfg->dither_thr_max = thr_max;
    cfg->dither_thr_min = thr_min;
}

static td_void vo_drv_set_intf_rgb_dither_mode(vdp_dither_cfg *cfg)
{
    /* drv transfer */
    vo_drv_get_intf_dither_in_out_mode(cfg);
    vo_drv_get_intf_dither_sed_and_thr(cfg);
    vo_hal_intf_set_lcd_dither(cfg);
}

static td_void vo_drv_set_intf_rgb_dither(td_u32 in_bit_width, td_u32 out_bit_width)
{
    vdp_dither_cfg dither_cfg = {0};

    /* default setting for rgb24bit */
    dither_cfg.dither_en = 1;
    dither_cfg.dither_round = 1;
    dither_cfg.dither_round_unlim = 1;
    dither_cfg.dither_domain_mode = 1;
    dither_cfg.dither_tap_mode = 1;
    dither_cfg.io_mode = DITHER_IO_MODE_12_8;

    if (in_bit_width == VO_INPUT_BIT_WIDTH_10) {
        if (out_bit_width == VO_OUTPUT_BIT_WIDTH_10) {
            dither_cfg.dither_en = 0;
        } else if (out_bit_width == VO_OUTPUT_BIT_WIDTH_6) { /* for rgb16bit, rgb18bit */
            dither_cfg.dither_round = 0;
            dither_cfg.io_mode = DITHER_IO_MODE_10_6;
        }
    }

    vo_drv_set_intf_rgb_dither_mode(&dither_cfg);
}

td_void vo_drv_set_intf_dither(ot_vo_intf_type intf_type, td_u32 out_bit_width)
{
    if (vo_drv_is_rgb_intf(intf_type)) {
        vo_drv_set_intf_rgb_dither(VO_INPUT_BIT_WIDTH_10, out_bit_width); /* it is always 10bit before dither */
    }
}
#endif

td_s32 vo_drv_check_dev_fixed_clk(ot_vo_dev dev, ot_vo_fixed_clk fixed_clk)
{
    return OT_ERR_VO_NOT_SUPPORT;
}

ot_vo_layer vo_drv_get_gfx_layer(ot_vo_dev dev)
{
    hal_disp_layer hal_layer;

    switch (dev) {
        case VO_DEV_DHD0:
            hal_layer = HAL_DISP_LAYER_GFX0;
            break;
        case VO_DEV_DHD1:
            hal_layer = HAL_DISP_LAYER_GFX1;
            break;
        case VO_DEV_DSD0:
            hal_layer = HAL_DISP_LAYER_GFX3;
            break;
        default:
            hal_layer = HAL_DISP_LAYER_BUTT;
            break;
    }

    return hal_layer;
}

#endif /* #if vo_desc("UBOOT_VO") */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
