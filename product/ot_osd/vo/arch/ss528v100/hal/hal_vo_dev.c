// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "hal_vo_dev.h"
#include "hal_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#if vo_desc("UBOOT_VO")

td_void hal_sys_set_outstanding(td_void)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_mac_outstanding mac_outstanding;

    mac_outstanding.u32 = hal_read_reg((td_u32 *)(td_uintptr_t)&(vo_reg->mac_outstanding.u32));
    mac_outstanding.bits.mstr0_routstanding = 0xf;  /* 0xf: master 0 read outstanding */
    mac_outstanding.bits.mstr0_woutstanding = 0x6;  /* 0x6: master 0 write outstanding */
    hal_write_reg((td_u32 *)(td_uintptr_t)&(vo_reg->mac_outstanding.u32), mac_outstanding.u32);
}

#if vo_desc("dev intf")

td_void vo_hal_intf_set_cvbs_dac_cfg(td_void)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_vo_dac_ctrl vo_dac_ctrl;
    volatile reg_vo_dac_otp vo_dac_otp;

    vo_dac_ctrl.u32 = vo_reg->vo_dac_ctrl.u32;
    vo_dac_otp.u32 = vo_reg->vo_dac_otp.u32;
    vo_dac_ctrl.bits.dac_reg_rev = 0xF;       /* 0xF: CVBS hardware parameter */
    vo_dac_otp.bits.dac_otp_reg = 0x7900;   /* 0x7900: CVBS hardware parameter */
    vo_reg->vo_dac_ctrl.u32 = vo_dac_ctrl.u32;
    vo_reg->vo_dac_otp.u32 = vo_dac_otp.u32;
}

td_void vo_hal_intf_set_date_cvbs_gain(td_u32 data)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    vo_reg->date_coeff57.u32 = data;
}

td_void vo_hal_intf_set_date_cvbs_burst_start(td_void)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_date_coeff61 date_coeff61;

    date_coeff61.u32 = vo_reg->date_coeff61.u32;
    date_coeff61.bits.burst_start_ovrd = 0x1; /* 0x1: refresh */
    vo_reg->date_coeff61.u32 = date_coeff61.u32;
}

td_void hal_disp_set_hdmi_mode(ot_vo_dev dev, td_u32 color_space)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_dhd0_ctrl dhd0_ctrl;
    volatile td_ulong addr_reg;

    vo_hal_check_phy_dev_return(dev);

    addr_reg = vou_get_chn_abs_addr(dev, (td_uintptr_t)&(vo_reg->dhd0_ctrl.u32));
    dhd0_ctrl.u32 = hal_read_reg((td_u32 *)(td_uintptr_t)addr_reg);
    dhd0_ctrl.bits.hdmi_mode = color_space;
    hal_write_reg((td_u32 *)(td_uintptr_t)addr_reg, dhd0_ctrl.u32);
}

static td_void vo_hal_intf_set_mux_sel_hd(ot_vo_dev dev, ot_vo_intf_type intf)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_vo_mux vo_mux;

    vo_mux.u32 = vo_reg->vo_mux.u32;

    if (intf == OT_VO_INTF_HDMI) {
        vo_mux.bits.hdmi_sel = dev;
    } else if (intf == OT_VO_INTF_VGA) {
        vo_mux.bits.vga_sel = dev;
    }  else if (intf == OT_VO_INTF_BT1120) {
        vo_mux.bits.digital_sel = 0;
        vo_mux.bits.bt_sel = dev;
    }
    vo_reg->vo_mux.u32 = vo_mux.u32;
}

static td_void vo_hal_intf_set_mux_sel_sd(ot_vo_intf_type intf)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_vo_mux vo_mux;

    vo_mux.u32 = vo_reg->vo_mux.u32;
    if (intf == OT_VO_INTF_CVBS) {
        vo_mux.bits.sddate_sel = 2;   /* 2: DSD SDATE */
    }
    vo_reg->vo_mux.u32 = vo_mux.u32;
}

static td_bool vo_hal_is_valid_sd_dev(ot_vo_dev dev)
{
    if (dev == VO_DEV_DSD0) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_void vo_hal_intf_set_mux_sel(ot_vo_dev dev, ot_vo_intf_type intf)
{
    vo_hal_check_phy_dev_return(dev);

    if (vo_hal_is_valid_sd_dev(dev)) {
        vo_hal_intf_set_mux_sel_sd(intf);
        return;
    }

    vo_hal_intf_set_mux_sel_hd(dev, intf);
}

td_void vo_hal_intf_set_csc_enable(ot_vo_intf_type intf, td_bool enable)
{
    volatile reg_vdp_regs *vo_reg = vo_hal_get_reg();
    volatile reg_hdmi_csc_idc hdmi_csc_idc;
    volatile reg_vga_csc_idc vga_csc_idc;

    if (intf == OT_VO_INTF_HDMI) {
        hdmi_csc_idc.u32 = vo_reg->hdmi_csc_idc.u32;
        hdmi_csc_idc.bits.csc_en = enable;
        vo_reg->hdmi_csc_idc.u32 = hdmi_csc_idc.u32;
    } else if (intf == OT_VO_INTF_VGA) {
        vga_csc_idc.u32 = vo_reg->vga_csc_idc.u32;
        vga_csc_idc.bits.csc_en = enable;
        vo_reg->vga_csc_idc.u32 = vga_csc_idc.u32;
    }
}

td_void vo_hal_intf_set_csc_cfg(ot_vo_intf_type intf, const csc_coef *csc_cfg)
{
    const vdp_csc_dc_coef *csc_dc_coef = (vdp_csc_dc_coef *)(&csc_cfg->csc_in_dc0);
    const vdp_csc_coef *coef = (vdp_csc_coef *)(&csc_cfg->csc_coef00);

    if (intf == OT_VO_INTF_HDMI) {
        vo_hal_intf_set_hdmi_csc_dc_coef(csc_dc_coef);
        vo_hal_intf_set_hdmi_csc_coef(coef);
    } else if (intf == OT_VO_INTF_VGA) {
        vo_hal_intf_set_vga_csc_dc_coef(csc_dc_coef);
        vo_hal_intf_set_vga_csc_coef(coef);
    }
}

#endif

#endif /* #if vo_desc("UBOOT_VO") */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
