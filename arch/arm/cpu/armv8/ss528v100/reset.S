// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <config.h>

.global reset_cpu
reset_cpu:
	ldr	x1, rstctl			/*  get addr for global reset */
						    /*  reg */
	mov	w3, #0x2			/*  full reset pll + mpu */
	str	w3, [x1]			/*  force reset */
	mov	x0, x0

_loop_forever:
	b	_loop_forever
.align 3
rstctl:
	.quad  SYS_CTRL_REG_BASE + REG_SC_SYSRES
