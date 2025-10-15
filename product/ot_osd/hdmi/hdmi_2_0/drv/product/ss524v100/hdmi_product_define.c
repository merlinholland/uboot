// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_product_define.h"
#include "hdmi_reg_dphy.h"
#include "hdmi_reg_crg.h"

#define HDMI_IO_CFG_FUNCTION1_SEL 0x1

td_s32 hdmi_tx_reg_write(td_u32 *reg_addr, td_u32 value)
{
    hdmi_if_null_return(reg_addr, TD_FAILURE);
    *(volatile td_u32 *)reg_addr = value;
    return TD_SUCCESS;
}

td_u32 hdmi_tx_reg_read(const td_u32 *reg_addr)
{
    hdmi_if_null_return(reg_addr, TD_FAILURE);
    return *(volatile td_u32 *)(reg_addr);
}

td_void hdmi_reg_write_u32(td_u32 reg_addr, td_u32 value)
{
    volatile td_u32 *addr = TD_NULL;

    addr = (volatile td_u32 *)(uintptr_t)reg_addr;
    if (addr != TD_NULL) {
        *addr = value;
    } else {
        hdmi_err("addr=0x%x err!\n", reg_addr);
    }

    return;
}

td_u32 hdmi_reg_read_u32(td_u32 reg_addr)
{
    td_u32 value = 0;
    volatile td_u32 *addr = TD_NULL;

    addr = (volatile td_u32 *)(uintptr_t)reg_addr;
    if (addr != TD_NULL) {
        value = *addr;
    } else {
        hdmi_err("addr=0x%x\n err!\n", reg_addr);
    }

    return value;
}

td_void drv_hdmi_prod_io_cfg_set(td_void)
{
    hdmi_if_fpga_return_void();

    hdmi_reg_write_u32(HDMI_ADDR_IO_CFG_HOTPLUG, HDMI_IO_CFG_FUNCTION1_SEL);
    hdmi_reg_write_u32(HDMI_ADDR_IO_CFG_SDA, HDMI_IO_CFG_FUNCTION1_SEL);
    hdmi_reg_write_u32(HDMI_ADDR_IO_CFG_SCL, HDMI_IO_CFG_FUNCTION1_SEL);

    return;
}

td_void drv_hdmi_prod_crg_gate_set(td_bool enable)
{
    hdmi_if_fpga_return_void();
    /* gate */
    hdmi_reg_ssc_in_cken_set(enable);
    hdmi_reg_ssc_bypass_cken_set(enable);
    hdmi_reg_ctrl_osc_24m_cken_set(enable);
    hdmi_reg_ctrl_cec_cken_set(enable);
    hdmi_reg_ctrl_os_cken_set(enable);
    hdmi_reg_ctrl_as_cken_set(enable);
    hdmi_reg_pxl_cken_set(enable);
    hdmi_reg_hdmirx_phy_tmds_cken_set(enable);

    return;
}

td_void drv_hdmi_prod_crg_all_reset_set(td_bool enable)
{
    hdmi_if_fpga_return_void();

    hdmi_reg_ctrl_bus_srst_req_set(enable);
    hdmi_reg_ctrl_srst_req_set(enable);
    hdmi_reg_cec_srst_req_set(enable);
    hdmi_reg_ssc_srst_req_set(enable);
    hdmi_reg_phy_srst_req_set(enable);
    hdmi_reg_phy_tmds_srst_req_set(enable);
    enable = !enable;
    /* delay 1us, to insure ctrl reset success. */
    udelay(1);
    hdmi_reg_ctrl_bus_srst_req_set(enable);
    hdmi_reg_ctrl_srst_req_set(enable);
    hdmi_reg_cec_srst_req_set(enable);
    hdmi_reg_ssc_srst_req_set(enable);
    hdmi_reg_phy_srst_req_set(enable);
    hdmi_reg_phy_tmds_srst_req_set(enable);

    return;
}

td_void drv_hdmi_prod_crg_phy_reset_set(td_bool enable)
{
    hdmi_if_fpga_return_void();
    hdmi_reg_phy_srst_req_set(enable);
    hdmi_reg_phy_tmds_srst_req_set(enable);
    enable = !enable;
    hdmi_reg_phy_srst_req_set(enable);
    hdmi_reg_phy_tmds_srst_req_set(enable);

    return;
}

td_void drv_hdmi_prod_crg_phy_reset_get(td_bool *enable)
{
    hdmi_if_fpga_return_void();
    hdmi_if_null_return_void(enable);

    if (hdmi_reg_phy_srst_req_get() || hdmi_reg_phy_tmds_srst_req_get()) {
        *enable = TD_TRUE;
    } else {
        *enable = TD_FALSE;
    }

    return;
}

td_void drv_hdmi_prod_crg_div_set(const hdmi_crg_cfg *crg_cfg)
{
    td_u32 tmp;

    hdmi_if_fpga_return_void();
    if (crg_cfg != TD_NULL && crg_cfg->ssc_bypass_div >= 1 && crg_cfg->tmds_clk_div >= 1) {
        tmp = crg_cfg->ssc_bypass_div - 1;
        hdmi_reg_ssc_clk_div_set(tmp);
        tmp = crg_cfg->tmds_clk_div - 1;
        hdmi_reg_tmds_clk_div_set(tmp);
        /* use phy reg to rest dphy, don't use CRG phy rst reg */
        hdmi_hdmitx_dphy_rst_reg_rstset(TD_TRUE);
        hdmi_hdmitx_dphy_rst_reg_rstset(TD_FALSE);
    }

    return;
}

td_void drv_hdmi_prod_reg_defaut_cfg(td_void)
{
    return;
}

td_void drv_hdmi_prod_crg_init(td_void)
{
    hdmi_if_fpga_return_void();
    drv_hdmi_prod_io_cfg_set();
    drv_hdmi_prod_crg_gate_set(TD_TRUE);
    drv_hdmi_prod_crg_all_reset_set(TD_TRUE);

    return;
}

td_void drv_hdmi_low_power_set(td_bool enable)
{
    hdmi_if_fpga_return_void();
    enable = !enable;
    hdmi_reg_ctrl_os_cken_set(enable);
    hdmi_reg_ctrl_as_cken_set(enable);

    return;
}

td_void drv_hdmi_hardware_reset(td_void)
{
    hdmi_if_fpga_return_void();

    hdmi_reg_crg_init();
    hdmi_reg_ctrl_bus_srst_req_set(TD_TRUE);
    hdmi_reg_ctrl_srst_req_set(TD_TRUE);
    hdmi_reg_cec_srst_req_set(TD_TRUE);
    hdmi_reg_ssc_srst_req_set(TD_TRUE);
    hdmi_reg_phy_srst_req_set(TD_TRUE);
    hdmi_reg_phy_tmds_srst_req_set(TD_TRUE);
    hdmi_reg_ssc_in_cken_set(TD_FALSE);
    hdmi_reg_ssc_bypass_cken_set(TD_FALSE);
    hdmi_reg_ctrl_osc_24m_cken_set(TD_FALSE);
    hdmi_reg_ctrl_cec_cken_set(TD_FALSE);
    hdmi_reg_ctrl_os_cken_set(TD_FALSE);
    hdmi_reg_ctrl_as_cken_set(TD_FALSE);
    hdmi_reg_pxl_cken_set(TD_FALSE);

    return;
}

