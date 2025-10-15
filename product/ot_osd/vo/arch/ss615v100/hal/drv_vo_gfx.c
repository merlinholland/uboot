// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include "drv_vo_gfx.h"
#include "ot_common_vo.h"

#include "drv_vo.h"
#if vo_desc("UBOOT_VO")

td_s32 vo_drv_check_gfx_id(ot_vo_layer gfx_layer)
{
    if (gfx_layer == OT_VO_LAYER_G2) {
        return OT_ERR_VO_INVALID_LAYER_ID;
    }

    return TD_SUCCESS;
}

#define VO_GFX_DISP_RECT_W_MAX_OF_GFXX 3840
#define VO_GFX_DISP_RECT_H_MAX_OF_GFXX 3840
#define VO_GFX_DISP_RECT_W_MAX_OF_GFX1 1920
#define VO_GFX_DISP_RECT_H_MAX_OF_GFX1 1920
#define VO_GFX_DISP_RECT_W_MAX_OF_GFX3 720
#define VO_GFX_DISP_RECT_H_MAX_OF_GFX3 720

#if vo_desc("vo gfx")
td_s32 vou_drv_get_gfx_bind_dev(ot_vo_layer layer)
{
    ot_vo_dev bind_dev;
    if (layer == HAL_DISP_LAYER_GFX0) {
        bind_dev = VO_DEV_DHD0;
    } else if (layer == HAL_DISP_LAYER_GFX1) {
        bind_dev = VO_DEV_DHD1;
    } else if (layer == HAL_DISP_LAYER_GFX2) {
        bind_dev = VO_DEV_DHD0;
    } else if (layer == HAL_DISP_LAYER_GFX3) {
        bind_dev = VO_DEV_DSD0;
    } else {
        bind_dev = OT_INVALID_DEV;
    }
    return bind_dev;
}

td_s32 vo_drv_get_hal_gfx_layer(ot_vo_layer gfx_layer, hal_disp_layer *hal_layer)
{
    switch (gfx_layer) {
        case OT_VO_LAYER_G0:
            *hal_layer = HAL_DISP_LAYER_GFX0;
            break;

        case OT_VO_LAYER_G1:
            *hal_layer = HAL_DISP_LAYER_GFX1;
            break;

        case OT_VO_LAYER_G2:
            *hal_layer = HAL_DISP_LAYER_GFX2;
            break;

        case OT_VO_LAYER_G3:
            *hal_layer = HAL_DISP_LAYER_GFX3;
            break;

        default:
            return OT_ERR_VO_INVALID_LAYER_ID;
    }

    return TD_SUCCESS;
}

#endif

#if vo_desc("gfx")

static td_void vo_drv_get_gfx_attr_max_rect(ot_vo_layer layer, td_u32 *width, td_u32 *height)
{
    *width = VO_GFX_DISP_RECT_W_MAX_OF_GFXX;
    *height = VO_GFX_DISP_RECT_H_MAX_OF_GFXX;

    if (layer == VO_HAL_LAYER_G1) {
        *width = VO_GFX_DISP_RECT_W_MAX_OF_GFX1;
        *height = VO_GFX_DISP_RECT_H_MAX_OF_GFX1;
    }

    if (layer == VO_HAL_LAYER_G3) {
        *width = VO_GFX_DISP_RECT_W_MAX_OF_GFX3;
        *height = VO_GFX_DISP_RECT_H_MAX_OF_GFX3;
    }
}

td_s32 vo_drv_check_gfx_attr_display_rect(ot_vo_layer layer, const ot_rect *rect)
{
    td_u32 max_width;
    td_u32 max_height;

    if ((layer >= VO_HAL_LAYER_G0) && (layer <= VO_HAL_LAYER_G3)) {
        vo_drv_get_gfx_attr_max_rect(layer, &max_width, &max_height);
        if ((rect->width > max_width) || (rect->height > max_height)) {
            vo_err_trace("gfx layer (%d) disp rect width(%d) height(%d) can't be larger than %dx%d!\n",
                layer, rect->width, rect->height,
                max_width, max_height);
            return OT_ERR_VO_ILLEGAL_PARAM;
        }
    } else {
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}
#endif
#endif /* #if vo_desc("UBOOT_VO") */