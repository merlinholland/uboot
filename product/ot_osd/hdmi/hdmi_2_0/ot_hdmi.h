/*
* Copyright (c) 2020 Shenshu Technologies Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef  __OT_HDMI_H__
#define  __OT_HDMI_H__

#include "ot_type.h"

#define OT_HDMI_HW_PARAM_NUM 4

typedef enum {
    OT_HDMI_VIDEO_FMT_1080P_60,
    OT_HDMI_VIDEO_FMT_1080P_50,
    OT_HDMI_VIDEO_FMT_1080P_30,
    OT_HDMI_VIDEO_FMT_1080P_25,
    OT_HDMI_VIDEO_FMT_1080P_24,
    OT_HDMI_VIDEO_FMT_1080I_60,
    OT_HDMI_VIDEO_FMT_1080I_50,
    OT_HDMI_VIDEO_FMT_720P_60,
    OT_HDMI_VIDEO_FMT_720P_50,
    OT_HDMI_VIDEO_FMT_576P_50,
    OT_HDMI_VIDEO_FMT_480P_60,
    OT_HDMI_VIDEO_FMT_PAL,                  /* B D G H I PAL */
    OT_HDMI_VIDEO_FMT_PAL_N,                /* (N)PAL        */
    OT_HDMI_VIDEO_FMT_PAL_NC,               /* (Nc)PAL       */
    OT_HDMI_VIDEO_FMT_NTSC,                 /* (M)NTSC       */
    OT_HDMI_VIDEO_FMT_NTSC_J,               /* NTSC-J        */
    OT_HDMI_VIDEO_FMT_NTSC_PAL_M,           /* (M)PAL        */
    OT_HDMI_VIDEO_FMT_SECAM_SIN,            /* SECAM_SIN */
    OT_HDMI_VIDEO_FMT_SECAM_COS,            /* SECAM_COS */
    OT_HDMI_VIDEO_FMT_861D_640X480_60,
    OT_HDMI_VIDEO_FMT_VESA_800X600_60,
    OT_HDMI_VIDEO_FMT_VESA_1024X768_60,
    OT_HDMI_VIDEO_FMT_VESA_1280X720_60,
    OT_HDMI_VIDEO_FMT_VESA_1280X800_60,
    OT_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    OT_HDMI_VIDEO_FMT_VESA_1366X768_60,
    OT_HDMI_VIDEO_FMT_VESA_1440X900_60,
    OT_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    OT_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    OT_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    OT_HDMI_VIDEO_FMT_VESA_1680X1050_60,
    OT_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    OT_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    OT_HDMI_VIDEO_FMT_VESA_2048X1152_60,    /* Currently, not support */
    OT_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE, /* Currently, not support */
    OT_HDMI_VIDEO_FMT_BUTT
} ot_hdmi_video_fmt;

typedef enum {
    OT_HDMI_VIDEO_MODE_RGB444,
    OT_HDMI_VIDEO_MODE_YCBCR422,
    OT_HDMI_VIDEO_MODE_YCBCR444,
    OT_HDMI_VIDEO_MODE_BUTT
} ot_hdmi_video_mode;

typedef enum {
    OT_HDMI_ID_0,
    OT_HDMI_ID_BUTT
} ot_hdmi_id;

/* the config parameter of HDMI interface */
typedef struct {
    td_bool            enable_hdmi;
    td_u32             disp_fmt;
    ot_hdmi_video_mode vid_out_mode;
    ot_hdmi_video_mode vid_in_mode;
} ot_drv_hdmi_attr;

/* the config parameter of HDMI interface */
typedef struct {
    td_bool enable_hdmi;
    td_u32 disp_fmt;
    ot_hdmi_video_mode vid_out_mode;
    ot_hdmi_video_mode vid_in_mode;
} ot_unf_hdmi_attr;

typedef struct {
    td_u32 i_de_main_clk;
    td_u32 i_de_main_data;
    td_u32 i_main_clk;
    td_u32 i_main_data;
    td_u32 ft_cap_clk;
    td_u32 ft_cap_data;
} ot_hdmi_hw_param;

typedef struct {
    ot_hdmi_hw_param hw_param[OT_HDMI_HW_PARAM_NUM];
} ot_hdmi_hw_spec;

typedef enum {
    OT_HDMI_TRACE_LEN_0,
    OT_HDMI_TRACE_LEN_1,
    OT_HDMI_TRACE_LEN_2,
    OT_HDMI_TRACE_LEN_3,
    OT_HDMI_TRACE_LEN_4,
    OT_HDMI_TRACE_LEN_5,
    OT_HDMI_TRACE_LEN_6,
    OT_HDMI_TRACE_LEN_7,
    OT_HDMI_TRACE_LEN_8,
    OT_HDMI_TRACE_DEFAULT,
    OT_HDMI_TRACE_BUTT
} ot_hdmi_trace_len;

typedef struct {
    td_bool emi_en; /* hdmi emi enable */
    ot_hdmi_trace_len trace_len; /* Phy parameters selection. */
} ot_hdmi_mod_param;

td_s32 ot_drv_hdmi_init(td_void);
td_s32 ot_drv_hdmi_de_init(td_void);
td_s32 ot_drv_hdmi_open(ot_hdmi_id hdmi_id);
td_s32 ot_drv_hdmi_set_attr(ot_hdmi_id hdmi_id, const ot_drv_hdmi_attr *attr);
td_s32 ot_drv_hdmi_start(ot_hdmi_id hdmi_id);
td_s32 ot_drv_hdmi_close(ot_hdmi_id hdmi_id);
td_s32 hdmi_display(td_u32 sync, td_u32 input_colorspace, td_u32 output_colorspace);
td_void hdmi_stop(td_void);
#endif

