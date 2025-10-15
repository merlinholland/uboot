// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_reg_aon.h"
#include "hdmi_product_define.h"

volatile tx_aon_regs *g_tx_aon_all_reg = NULL;

td_s32 hdmi_tx_aon_regs_init(td_void)
{
    g_tx_aon_all_reg = (volatile tx_aon_regs *)(HDMI_TX_BASE_ADDR + (HDMI_TX_BASE_ADDR_AON));
    return TD_SUCCESS;
}

td_void hdmi_tx_aon_regs_deinit(td_void)
{
    if (g_tx_aon_all_reg != TD_NULL) {
        g_tx_aon_all_reg = TD_NULL;
    }
    return;
}

td_void hdmi_dcc_man_en_set(td_u32 dcc_man_en)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_ctrl mst_ctrl;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_mst_ctrl.u32);
    mst_ctrl.u32 = hdmi_tx_reg_read(reg_addr);
    mst_ctrl.bits.dcc_man_en = dcc_man_en;
    hdmi_tx_reg_write(reg_addr, mst_ctrl.u32);

    return;
}

td_void hdmi_ddc_sda_oen_set(td_u32 ddc_sda_oen)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl man_ctrl;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_man_ctrl.u32);
    man_ctrl.u32 = hdmi_tx_reg_read(reg_addr);
    man_ctrl.bits.ddc_sda_oen = ddc_sda_oen;
    hdmi_tx_reg_write(reg_addr, man_ctrl.u32);

    return;
}

td_void hdmi_ddc_scl_oen_set(td_u32 ddc_scl_oen)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl man_ctrl;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_man_ctrl.u32);
    man_ctrl.u32 = hdmi_tx_reg_read(reg_addr);
    man_ctrl.bits.ddc_scl_oen = ddc_scl_oen;
    hdmi_tx_reg_write(reg_addr, man_ctrl.u32);

    return;
}

td_u32 hdmi_ddc_i2c_no_ack_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_state mst_state;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_mst_state.u32);
    mst_state.u32 = hdmi_tx_reg_read(reg_addr);
    return mst_state.bits.ddc_i2c_no_ack;
}

td_u32 hdmi_ddc_i2c_bus_low_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_state mst_state;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_mst_state.u32);
    mst_state.u32 = hdmi_tx_reg_read(reg_addr);
    return mst_state.bits.ddc_i2c_bus_low;
}

td_u32 hdmi_ddc_sda_st_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl man_ctrl;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_man_ctrl.u32);
    man_ctrl.u32 = hdmi_tx_reg_read(reg_addr);
    return man_ctrl.bits.ddc_sda_st;
}

td_u32 hdmi_ddc_scl_st_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl man_ctrl;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->reg_ddc_man_ctrl.u32);
    man_ctrl.u32 = hdmi_tx_reg_read(reg_addr);
    return man_ctrl.bits.ddc_scl_st;
}

