// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_REG_CRG_H__
#define __HDMI_REG_CRG_H__

#include "ot_type.h"

typedef union {
    struct {
        td_u32 hdmitx_ctrl_bus_srst_req : 1;  /* [0] */
        td_u32 hdmitx_ctrl_srst_req     : 1;  /* [1] */
        td_u32 hdmitx_ctrl_cec_srst_req : 1;  /* [2] */
        td_u32 hdmitx_ssc_srst_req      : 1;  /* [3] */
        td_u32 ssc_in_cken              : 1;  /* [4] */
        td_u32 ssc_bypass_cken          : 1;  /* [5] */
        td_u32 hdmitx_ctrl_osc_24m_cken : 1;  /* [6] */
        td_u32 hdmitx_ctrl_cec_cken     : 1;  /* [7] */
        td_u32 hdmitx_ctrl_os_cken      : 1;  /* [8] */
        td_u32 hdmitx_ctrl_as_cken      : 1;  /* [9] */
        td_u32 hdmitx_pxl_cken          : 1;  /* [10] */
        td_u32 reserved_0               : 13; /* [11..23] */
        td_u32 ssc_clk_div              : 4;  /* [24..27] */
        td_u32 ssc_bypass_clk_sel       : 1;  /* [28] */
        td_u32 reserved_1               : 3;  /* [29..31] */
    } bits;
    td_u32 u32;
} peri_crg8144;

typedef union {
    struct {
        td_u32 hdmitx_phy_srst_req : 1;  /* [0] */
        td_u32 phy_tmds_srst_req   : 1;  /* [1] */
        td_u32 reserved_0          : 2;  /* [2..3] */
        td_u32 phy_tmds_cken       : 1;  /* [4] */
        td_u32 reserved_1          : 19; /* [5..23] */
        td_u32 tmds_clk_div        : 3;  /* [24..26] */
        td_u32 reserved_2          : 5;  /* [27..31] */
    } bits;
    td_u32 u32;
} peri_crg8152;

typedef struct {
    volatile peri_crg8144 crg8144; /* 0x7F40 */
    volatile td_u32 reserved[7];   /* 0x7F44~0x0x7F5C */
    volatile peri_crg8152 crg8152; /* 0x7F60 */
} hdmi_reg_crg;

td_void hdmi_reg_crg_init(td_void);
td_void hdmi_reg_crg_deinit(td_void);
td_void hdmi_reg_ssc_in_cken_set(td_u32 en);
td_void hdmi_reg_ssc_bypass_cken_set(td_u32 en);
td_void hdmi_reg_ctrl_osc_24m_cken_set(td_u32 en);
td_void hdmi_reg_ctrl_cec_cken_set(td_u32 en);
td_void hdmi_reg_ctrl_os_cken_set(td_u32 en);
td_void hdmi_reg_ctrl_as_cken_set(td_u32 en);
td_void hdmi_reg_ctrl_bus_srst_req_set(td_u32 req);
td_void hdmi_reg_ctrl_srst_req_set(td_u32 req);
td_void hdmi_reg_cec_srst_req_set(td_u32 req);
td_void hdmi_reg_ssc_srst_req_set(td_u32 req);
td_void hdmi_reg_ssc_clk_div_set(td_u32 div);
td_void hdmi_reg_pxl_cken_set(td_u32 en);
td_void hdmi_reg_hdmirx_phy_tmds_cken_set(td_u32 en);
td_void hdmi_reg_phy_srst_req_set(td_u32 req);
td_u32 hdmi_reg_phy_srst_req_get(td_void);
td_void hdmi_reg_phy_tmds_srst_req_set(td_u32 req);
td_u32 hdmi_reg_phy_tmds_srst_req_get(td_void);
td_u32 hdmi_reg_phy_srst_req_get(td_void);
td_void hdmi_reg_tmds_clk_div_set(td_u32 div);
#endif /* __HDMI_REG_CRG_H__ */

