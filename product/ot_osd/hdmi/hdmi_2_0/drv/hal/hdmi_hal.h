// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_HAL_H__
#define __HDMI_HAL_H__

#include "drv_hdmi_common.h"
#include "hdmi_hal_phy.h"
#include "hdmi_hal_intf.h"

typedef enum {
    HDMI_DEBUG_CMD_COLOR_BAR,
    HDMI_DEBUG_CMD_SW_RESET,
    HDMI_DEBUG_CMD_RGB2YUV,
    HDMI_DEBUG_CMD_YUV2RGB,
    HDMI_DEBUG_CMD_DITHER,
    HDMI_DEBUG_CMD_BYPASS,
    HDMI_DEBUG_CMD_DDC_FREQ,
    HDMI_DEBUG_CMD_PHY_DEFAULT_GET,
    HDMI_DEBUG_CMD_PHY_PARA_SET,
    HDMI_DEBUG_CMD_DUMP,
#if defined (HDMI_SUPPORT_LOGIC_V100)
    HDMI_DEBUG_CMD_PROC_MACH,
    HDMI_DEBUG_CMD_PROC_SCDC,
    HDMI_DEBUG_CMD_PROC_HDCP14,
    HDMI_DEBUG_CMD_PROC_HDCP22,
    HDMI_DEBUG_CMD_PROC_DDC,
    HDMI_DEBUG_CMD_PROC_CECTX,
    HDMI_DEBUG_CMD_PROC_CECRX,
    HDMI_DEBUG_CMD_DBG_VIDEO_GET,
    HDMI_DEBUG_CMD_DBG_VIDEO_SET,
    HDMI_DEBUG_CMD_SSC,
    HDMI_DEBUG_CMD_FRL,
#endif
} hdmi_hal_debug_cmd;

typedef struct {
    td_void           *hdmi_hw;
    td_void           *hdmi_dev;
    td_u32             hdmi_id;
    hdmi_tx_capability tx_capability;
    hdmi_callback      callback;
    hdmi_video_config  video_cfg;
    td_char            *base_addr;
    td_char            *phy_addr;
} hdmi_hal_context;

typedef struct {
    td_u32          disp_fmt;
    td_u32          pix_clk;
    hdmi_colorspace colorspace;
    hdmi_deep_color deep_color;
} hdmi_hal_base_param;

typedef struct _hdmi_hal_ {
    hdmi_hal_context hal_ctx;
    td_void (*hal_hdmi_hardware_init)(const struct _hdmi_hal_ *hal);
    td_void (*hal_hdmi_tmds_mode_set)(const struct _hdmi_hal_ *hal, hdmi_tmds_mode tmds_mode);
    td_void (*hal_hdmi_avmute_set)(const struct _hdmi_hal_ *hal, td_bool avmute);
    td_void (*hal_hdmi_infoframe_set)(const struct _hdmi_hal_ *hal, hdmi_infoframe_id id, const td_u8 in_buffer[]);
    td_void (*hal_hdmi_infoframe_enable_set)(const struct _hdmi_hal_ *hal, hdmi_infoframe_id id, td_bool enable);
    td_s32  (*hal_hdmi_video_path_set)(const struct _hdmi_hal_ *hal, const hdmi_video_config *video_cfg);
    td_void (*hal_hdmi_phy_output_enable_set)(const struct _hdmi_hal_ *hal, td_bool enable);
    td_void (*hal_hdmi_phy_power_enable_set)(const struct _hdmi_hal_ *hal, td_bool enable);
    td_void (*hal_hdmi_tx_capability_get)(const struct _hdmi_hal_ *hal, hdmi_tx_capability *tx_cap);
    td_void (*hal_hdmi_csc_param_set)(struct _hdmi_hal_ *hal, const hdmi_video_config *video_cfg);
    td_void (*hal_hdmi_phy_set)(const struct _hdmi_hal_ *hal, const hdmi_phy_cfg *phy_cfg);
    td_void (*hal_hdmi_ctrl_reset)(const struct _hdmi_hal_ *hal);
#ifdef HDMI_SCDC_SUPPORT
    td_void (*hal_hdmi_scdc_status_get)(const struct _hdmi_hal_ *hal, hdmi_scdc_status *status);
    td_void (*hal_hdmi_scdc_status_set)(const struct _hdmi_hal_ *hal, const hdmi_scdc_status *status);
    td_s32 (*hal_hdmi_scdc_process)(struct _hdmi_hal_ *hal, scdc_cmd cmd, td_void *data);
#endif
#ifdef PHY_CRAFT_S28
    td_s32 (*hal_hdmi_phy_hw_spec_get)(const struct _hdmi_hal_ *hal, hdmi_hw_spec *hw_spec);
    td_s32 (*hal_hdmi_phy_hw_spec_set)(const struct _hdmi_hal_ *hal, hdmi_hal_hw_param param);
#endif
} hdmi_hal;

td_s32 hal_hdmi_open(const hdmi_hal_init *hal_init, hdmi_hal **hal_handle);

td_void hal_hdmi_close(hdmi_hal **hal);
#endif

