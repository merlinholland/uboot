// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#if defined(CONFIG_TARGET_SS919V100) || defined(CONFIG_TARGET_SS015V100)
#include "phy-usb-ss919v100.c"
#endif
#if defined(CONFIG_TARGET_SS318V100)
#include "phy-usb-ss318v100.c"
#endif
#if defined(CONFIG_TARGET_SS918V100) || defined(CONFIG_TARGET_SS013V100)
#include "phy-usb-ss918v100.c"
#endif
#if defined(CONFIG_TARGET_SS812V100)
#include "phy-usb-ss812v100.c"
#endif
#if (defined(CONFIG_TARGET_SS813V100) || defined(CONFIG_TARGET_SS815V100))
#include "phy-usb-ss813v100.c"
#endif
#if defined(CONFIG_TARGET_SS312V100)
#include "phy-usb-ss312v100.c"
#endif
#if defined(CONFIG_TARGET_SS313V100)
#include "phy-usb-ss313v100.c"
#endif
#if defined(CONFIG_TARGET_SS011V100)
#include "phy-usb-ss313v100.c"
#endif
#if defined(CONFIG_TARGET_SS012V100)
#include "phy-usb-ss313v100.c"
#endif
#ifdef CONFIG_TARGET_SS101V200
#include "phy-ss101v200-usb.c"
#endif
#ifdef CONFIG_TARGET_SS101V500
#include "phy-ss101v500-usb.c"
#endif
#ifdef CONFIG_TARGET_SS101V300
#include "phy-ss101v300-usb.c"
#endif
#ifdef CONFIG_TARGET_SS101V600
#include "phy-ss101v600-usb.c"
#endif
#if defined(CONFIG_TARGET_SS528V100)
#include "phy_usb_ss528v100.c"
#endif
#if defined(CONFIG_TARGET_SS625V100)
#include "phy_usb_ss528v100.c"
#endif
#if defined(CONFIG_TARGET_SS928V100) || defined(CONFIG_TARGET_SS927V100)
#include "phy-usb-ss928v100.c"
#endif

#if (defined(CONFIG_TARGET_SS524V100) || defined(CONFIG_TARGET_SS522V101) || \
	defined(CONFIG_TARGET_SS615V100)) || defined(CONFIG_TARGET_SS522V100)
#include "phy_usb_ss524v100.c"
#endif
