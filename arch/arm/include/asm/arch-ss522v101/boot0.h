// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */


#ifndef __BOOT0_H
#define __BOOT0_H

#define ARM_SOC_BOOT0_HOOK

/* BOOT0 header information */
#define VENDOR_BOOT0_HOOK   \
	__blank_zone_start: \
	.fill 1024 * 8, 1, 0;   \
	__blank_zone_end:

#include <../mach-vendor/boot0_hook.S>
#endif /* __BOOT0_H */
