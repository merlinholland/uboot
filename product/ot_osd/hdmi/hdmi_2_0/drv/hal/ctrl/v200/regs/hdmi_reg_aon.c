// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include "hdmi_reg_aon.h"
#include "hdmi_product_define.h"

volatile tx_aon_reg_type *g_tx_aon_all_reg = NULL;

td_s32 hdmi_tx_aon_reg_type_init(td_void)
{
    g_tx_aon_all_reg = (volatile tx_aon_reg_type *)(HDMI_TX_BASE_ADDR + (HDMI_TX_BASE_ADDR_AON));
    return TD_SUCCESS;
}

td_void hdmi_tx_aon_reg_type_deinit(td_void)
{
    if (g_tx_aon_all_reg != NULL) {
        g_tx_aon_all_reg = NULL;
    }
    return;
}

td_s32 hdmi_dcc_man_en_set(td_u32 dcc_man_en)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->mst_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.dcc_man_en = dcc_man_en;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ddc_sda_oen_set(td_u32 dcc_sda_oen)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->man_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.ddc_sda_oen = dcc_sda_oen;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_s32 hdmi_ddc_scl_oen_set(td_u32 dcc_scl_oen)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->man_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    tmp.bits.ddc_scl_oen = dcc_scl_oen;
    hdmi_tx_reg_write(reg_addr, tmp.u32);

    return TD_SUCCESS;
}

td_u32 hdmi_ddc_i2c_no_ack_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_state tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->mst_state.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.ddc_i2c_no_ack;
}

td_u32 hdmi_ddc_i2c_bus_low_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_mst_state tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->mst_state.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.ddc_i2c_bus_low;
}

td_u32 hdmi_ddc_sda_st_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->man_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.ddc_sda_st;
}

td_u32 hdmi_ddc_scl_st_get(td_void)
{
    td_u32 *reg_addr = NULL;
    ddc_man_ctrl tmp;

    reg_addr = (td_u32 *)&(g_tx_aon_all_reg->man_ctrl.u32);
    tmp.u32 = hdmi_tx_reg_read(reg_addr);
    return tmp.bits.ddc_scl_st;
}

