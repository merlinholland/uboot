// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_reg_crg.h"
#include "hdmi_product_define.h"

volatile hdmi_reg_crg *g_crg_regs = TD_NULL;

td_void hdmi_reg_crg_init(td_void)
{
    if (g_crg_regs != TD_NULL) {
        return;
    }

    g_crg_regs = (volatile hdmi_reg_crg *)(uintptr_t)(HDMI_CRG_ADDR);

    return;
}

td_void hdmi_reg_crg_deinit(td_void)
{
    if (g_crg_regs != TD_NULL) {
        g_crg_regs = TD_NULL;
    }

    return;
}

td_void hdmi_reg_ssc_in_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.ssc_in_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ssc_bypass_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.ssc_bypass_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ctrl_osc_24m_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_osc_24m_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ctrl_cec_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_cec_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ctrl_os_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_os_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ctrl_as_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_as_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}
td_void hdmi_reg_ctrl_bus_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_bus_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ctrl_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_cec_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ctrl_cec_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ssc_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_ssc_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_ssc_clk_div_set(td_u32 div)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.ssc_clk_div = div;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_pxl_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.hdmitx_pxl_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void reg_hdmi_crg_ssc_bypass_clk_sel_set(td_u32 sel)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8144 peri_crg_8144;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8144.u32);
    peri_crg_8144.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8144.bits.ssc_bypass_clk_sel = sel;
    hdmi_tx_reg_write(reg_addr, peri_crg_8144.u32);

    return;
}

td_void hdmi_reg_hdmirx_phy_tmds_cken_set(td_u32 en)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8152.bits.phy_tmds_cken = en;
    hdmi_tx_reg_write(reg_addr, peri_crg_8152.u32);

    return;
}

td_void hdmi_reg_phy_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8152.bits.hdmitx_phy_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8152.u32);

    return;
}

td_u32 hdmi_reg_phy_srst_req_get(td_void)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);

    return peri_crg_8152.bits.hdmitx_phy_srst_req;
}

td_void hdmi_reg_phy_tmds_srst_req_set(td_u32 req)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8152.bits.phy_tmds_srst_req = req;
    hdmi_tx_reg_write(reg_addr, peri_crg_8152.u32);

    return;
}

td_u32 hdmi_reg_phy_tmds_srst_req_get(td_void)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);

    return peri_crg_8152.bits.phy_tmds_srst_req;
}

td_void hdmi_reg_tmds_clk_div_set(td_u32 div)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);
    peri_crg_8152.bits.tmds_clk_div = div;
    hdmi_tx_reg_write(reg_addr, peri_crg_8152.u32);

    return;
}

td_u32 reg_hdmi_crg_tmds_clk_div_get(td_void)
{
    td_u32 *reg_addr = TD_NULL;
    peri_crg8152 peri_crg_8152;

    reg_addr = (td_u32 *)&(g_crg_regs->crg8152.u32);
    peri_crg_8152.u32 = hdmi_tx_reg_read(reg_addr);

    return peri_crg_8152.bits.tmds_clk_div;
}

