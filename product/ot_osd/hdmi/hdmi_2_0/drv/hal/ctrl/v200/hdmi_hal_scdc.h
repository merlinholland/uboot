// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef __HDMI_HAL_SCDC_H__
#define __HDMI_HAL_SCDC_H__

#include "ot_type.h"
#include "drv_hdmi_common.h"

#define SCDC_DEFAULT_SCRAMBLE_TIMEOUT  200
#define SCDC_MAX_SCRAMBLE_TIMEOUT      1000
#define SCDC_DEFAULT_SCRAMBLE_INTERVAL 20

/* HDMI2.1 section 10.4.1.3,OFFSET 0x10 */
typedef union {
    struct {
        td_u8 status_update     : 1;
        td_u8 ced_update        : 1;
        td_u8 rr_test           : 1;
        td_u8 source_test_upate : 1;
        td_u8 frl_start         : 1;
        td_u8 flt_update        : 1;
        td_u8 rsed_update       : 1;
        td_u8 rsvd              : 1;
    } u8;
    td_u8 byte;
} scdc_update0_flag;

/* HDMI2.1 section 10.4.1.4,OFFSET 0x20 */
typedef union {
    struct {
        td_u8 screambling_enable   : 1;
        td_u8 tmds_bit_clock_ratio : 1;
        td_u8 rsvd                 : 6;
    } u8;
    td_u8 byte;
} scdc_tmds_config;

/* HDMI2.1 section 10.4.1.5,OFFSET 0x21 */
typedef union {
    struct {
        td_u8 screambling_status : 1;
        td_u8 rsvd               : 7;
    } u8;
    td_u8 byte;
} scdc_scramble_status;

/* HDMI2.1 section 10.4.1.6,OFFSET 0x31 */
typedef union {
    struct {
        td_u8 frl_rate   : 4;
        td_u8 ffe_levels : 4;
    } u8;
    td_u8 byte;
} scdc_config1;

/* HDMI2.1 section 10.4.1.6,OFFSET 0x35 */
typedef union {
    struct {
        td_u8 rsvd0            : 1;
        td_u8 pre_shoot_only   : 1;
        td_u8 de_emphasis_only : 1;
        td_u8 no_ffe           : 1;
        td_u8 rsvd1            : 1;
        td_u8 flt_no_timeout   : 1;
        td_u8 dsc_frl_max      : 1;
        td_u8 frl_max          : 1;
    } u8;
    td_u8 byte;
} scdc_test_config1;

/* HDMI2.1 section 10.4.1.7,OFFSET 0x40 */
typedef union {
    struct {
        td_u8 colck_detected  : 1;
        td_u8 ch0_locked      : 1;
        td_u8 ch1_locked      : 1;
        td_u8 ch2_locked      : 1;
        td_u8 ch3_locked      : 1;
        td_u8 rsvd            : 1;
        td_u8 flt_ready       : 1;
        td_u8 dsc_decode_fail : 1;
    } u8;
    td_u8 byte;
} scdc_status_flag0;

/* HDMI2.1 section 10.4.1.7,OFFSET 0x41 */
typedef union {
    struct {
        td_u8 ln0_ltp_req : 4;
        td_u8 ln1_ltp_req : 4;
    } u8;
    td_u8 byte;
} scdc_status_flag1;

/* HDMI2.1 section 10.4.1.7,OFFSET 0x42 */
typedef union {
    struct {
        td_u8 ln2_ltp_req : 4;
        td_u8 ln3_ltp_req : 4;
    } u8;
    td_u8 byte;
} scdc_status_flag2;

typedef enum {
    SCDC_OFFSET_SINK_VERSION     = 0x01,
    SCDC_OFFSET_SRC_VERSION      = 0x02,
    SCDC_OFFSET_UPDATE_0         = 0x10,
    SCDC_OFFSET_UPDATE_1         = 0x11,
    SCDC_OFFSET_TMDS_CONFIG      = 0x20,
    SCDC_OFFSET_SCRAMBLER_STATUS = 0x21,
    SCDC_OFFSET_CONFIG_0         = 0x30,
    SCDC_OFFSET_CONFIG_1         = 0x31,
    SCDC_OFFSET_TEST_CONFIG      = 0x35,
    SCDC_OFFSET_STATUS_FLAG_0    = 0x40,
    SCDC_OFFSET_STATUS_FLAG_1    = 0x41,
    SCDC_OFFSET_STATUS_FLAG_2    = 0x42,
    SCDC_OFFSET_DRR_DET_0_L      = 0x50,
    SCDC_OFFSET_DRR_DET_0_H      = 0x51,
    SCDC_OFFSET_DRR_DET_1_L      = 0x52,
    SCDC_OFFSET_DRR_DET_1_H      = 0x53,
    SCDC_OFFSET_DRR_DET_2_L      = 0x54,
    SCDC_OFFSET_DRR_DET_2_H      = 0x55,
    SCDC_OFFSET_DRR_DET_CHKSUM   = 0x56,
    SCDC_OFFSET_TEST_CONFIG_0    = 0xC0,
    SCDC_OFFSET_IEEE_OUI_3TH     = 0xD0,
    SCDC_OFFSET_IEEE_OUI_2ND     = 0xD1,
    SCDC_OFFSET_IEEE_OUI_1ST     = 0xD2,
    SCDC_OFFSET_DEVICE_ID_HEAD   = 0xD3,
    SCDC_OFFSET_DEVICE_ID_TAIL   = 0xDD
} scdc_offset;

typedef struct {
    td_bool sink_read_quest;
    td_bool src_scramble;      /* TD_TRUE indicate enable;TD_FALSE indicate disable. */
    td_bool sink_scramble;     /* TD_TRUE indicate enable;TD_FALSE indicate disable. */
    td_bool tmds_clk_ratio40x; /* TD_TRUE indicate 40x;TD_FALSE indicate 10x. */
    td_u32  scramble_timeout;  /* in unit of ms.for [0,200],force to default 200;or,set the value cfg(>200). */
    /* in unit of ms,range[20,200).for [0,20] or >=200,force to default 20;or,set the value cfg[20,200). */
    td_u32  scramble_interval;
} scdc_attr;

typedef struct {
    td_bool init;
    hdmi_device_id hdmi_id;
} scdc_status;

typedef struct {
    scdc_status status;
    scdc_attr attr;
} scdc_info;

td_void hal_hdmi_scdc_init(hdmi_device_id hdmi_id);

td_void hal_hdmi_scdc_deinit(hdmi_device_id hdmi_id);

td_s32 hal_hdmi_scdc_attr_set(hdmi_device_id hdmi_id, scdc_attr *attr);

td_void hal_hdmi_scdc_attr_get(hdmi_device_id hdmi_id, scdc_attr *attr);

#endif /* __HDMI_HAL_SCDC_H__ */

