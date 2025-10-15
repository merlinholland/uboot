// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <config.h>

.global reset_cpu
reset_cpu:
	ldr	r1, rstctl			@ get addr for global reset
						@ reg
	mov	r3, #0x2			@ full reset pll + mpu
	str	r3, [r1]			@ force reset
	mov	r0, r0

_loop_forever:
	b	_loop_forever
.align 2
rstctl:
	.word  SYS_CTRL_REG_BASE + REG_SC_SYSRES
