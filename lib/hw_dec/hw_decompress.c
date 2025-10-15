// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#if (defined CONFIG_TARGET_SS919V100) || (defined CONFIG_SS919V100) || \
	(defined CONFIG_TARGET_SS318V100) || (defined CONFIG_TARGET_SS918V100) ||\
	(defined CONFIG_TARGET_SS015V100)  || (defined CONFIG_SS015V100) ||\
	(defined CONFIG_TARGET_SS013V100)  || (defined CONFIG_TARGET_SS928V100) ||\
	(defined CONFIG_TARGET_SS927V100)
#include "hw_decompress_v2.c"
#endif

#if ((defined CONFIG_TARGET_SS313V100) || (defined CONFIG_TARGET_SS312V100) || \
	(defined CONFIG_TARGET_SS813V100) || (defined CONFIG_TARGET_SS815V100) || \
	(defined CONFIG_TARGET_SS812V100) || (defined CONFIG_TARGET_SS101V200) || \
	(defined CONFIG_TARGET_SS101V500) || (defined CONFIG_TARGET_SS101V300) || \
	(defined CONFIG_TARGET_SS101V600) || (defined CONFIG_TARGET_SS012V100)  || \
	(defined CONFIG_TARGET_SS011V100))
#include "hw_decompress_v1.c"
#endif
