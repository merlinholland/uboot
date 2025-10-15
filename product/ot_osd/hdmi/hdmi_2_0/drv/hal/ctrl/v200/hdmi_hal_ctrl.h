// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_HAL_CTRL_H__
#define __HDMI_HAL_CTRL_H__

#include "drv_hdmi_common.h"
#include "drv_hdmi_infoframe.h"
#include "ot_type.h"

#define HDMI_INFOFRMAE_MAX_SIZE 31

typedef enum {
    HDMI_COLORMETRY_BT709,
    HDMI_COLORMETRY_BT601,
    HDMI_COLORMETRY_BT2020,
    HDMI_COLORMETRY_BT2020_CONST,
    HDMI_COLORMETRY_BUTT
} hdmi_colormetry;

typedef struct {
    td_bool h_pol_invert;
    td_bool v_pol_invert;
    td_bool de_invert;
} hdmi_sync_pol;

typedef struct {
    hdmi_video_timing timing;
    td_u32            in_pixel_clk;
    td_u32            out_tmds_clk;
    hdmi_colorspace   in_colorspace;
    hdmi_colorspace   out_colorspace;
    hdmi_colormetry   in_colormetry;
    hdmi_colormetry   out_colormetry;
    hdmi_deep_color   in_deep_color;
    hdmi_deep_color   out_deep_color;
    hdmi_sync_pol     out_hv_sync_pol;
    hdmi_quantization in_quantization;
    hdmi_quantization out_quantization;
} hdmi_video_path;

typedef struct {
    /* VDP hw timing */
    td_bool sync_sw_enable;
    td_bool v_sync_polarity;
    td_bool h_sync_polarity;
    td_bool progressive;
    td_u32  h_sync_total;
    td_u32  h_active_cnt;
    td_u32  v_sync_total;
    td_u32  v_active_cnt;
} ctrl_timing_detect;

typedef struct {
    td_u32              out_tmds_clk;
    hdmi_colorspace     in_colorspace;
    hdmi_colorspace     out_colorspace;
    hdmi_colormetry     in_colormetry;
    hdmi_colormetry     out_colormetry;
    hdmi_deep_color     in_deep_color;
    hdmi_deep_color     out_deep_color;
    hdmi_sync_pol       out_hv_sync_pol;
    hdmi_quantization   in_quantization;
    hdmi_quantization   out_quantization;
    td_bool             csc_enable;
    td_bool             y422_enalbe;
    td_bool             y420_enalbe;
    td_bool             dwsm_hori_enalbe;
    td_bool             dwsm_vert_enalbe;
    hdmi_video_dither   dither_mode;
    ctrl_timing_detect timing;
} ctrl_video_status;

typedef struct {
    td_bool         init;
    td_u32          mach_id;
    hdmi_tmds_mode  tmds_mode;
    td_bool         avmute;
    td_bool         audio_mute;
    td_bool         video_mute;
    td_bool         if_enable[HDMI_INFOFRAME_TYPE_BUTT - HDMI_INFOFRAME_TYPE_VENDOR];
    td_bool         if_data[HDMI_INFOFRAME_TYPE_BUTT - HDMI_INFOFRAME_TYPE_VENDOR][HDMI_INFOFRMAE_MAX_SIZE];
    hdmi_video_path video_path;
    td_void         *event_data;
    hdmi_callback   event_callback;
    td_bool         hpd;
    td_bool         rsen;
} hdmi_ctrl_info;

typedef struct {
    td_bool          init;
    hdmi_tmds_mode   tmds_mode;
    td_bool          avmute;
    td_bool          audio_mute;
    td_bool          video_mute;
    td_bool          if_enable[HDMI_INFOFRAME_TYPE_BUTT - HDMI_INFOFRAME_TYPE_VENDOR];
    td_bool          if_data[HDMI_INFOFRAME_TYPE_BUTT - HDMI_INFOFRAME_TYPE_VENDOR][HDMI_INFOFRMAE_MAX_SIZE];
    hdmi_video_path  video_path;
} hdmi_ctrl_status;

td_s32 hal_hdmi_ctrl_init(hdmi_device_id hdmi_id, const hdmi_hal_init *hal_init);

td_void hal_hdmi_ctrl_deinit(hdmi_device_id hdmi_id);

td_s32 hal_hdmi_ctrl_avmute_set(hdmi_device_id hdmi_id, const hdmi_avmute_cfg *avmute_cfg);

td_void hal_hdmi_ctrl_videopath_set(hdmi_device_id hdmi_id, hdmi_video_path *video_path);

td_s32 hal_hdmi_ctrl_tmds_mode_set(hdmi_device_id hdmi_id, hdmi_tmds_mode tmds_mode);

td_s32 hal_hdmi_ctrl_infoframe_en_set(hdmi_device_id hdmi_id, hdmi_infoframe_id infoframe_id, td_bool enable);

td_s32 hal_hdmi_ctrl_infoframe_data_set(hdmi_device_id hdmi_id, hdmi_infoframe_id infoframe_id, const td_u8 *if_data);

td_void hal_hdmi_ctrl_csc_set(hdmi_device_id hdmi_id, const hdmi_video_config *video_cfg);

td_void hal_hdmi_ctrl_data_reset(hdmi_device_id hdmi_id);

#endif /* __HDMI_HAL_CTRL_H__ */

