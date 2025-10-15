// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <config.h>
#include <linux/kconfig.h>
#include <asm/io.h>
#include <compiler.h>
#include <cpu_common.h>

void mmu_pagedir_init(unsigned long);
void mmu_pagedir_cached_range(unsigned long, unsigned long, unsigned long);
void mmu_startup(unsigned long);
void dcache_flush_all(void);
void dcache_inv_all(void);
void mmu_turnoff(void);

const uintptr_t image_entry = (CONFIG_SYS_TEXT_BASE);

#define error(_s)			   uart_early_puts(_s)
#define putstr(_s)			  uart_early_puts(_s)

#define large_malloc(_n)		malloc(_n)
#define large_free(_n)		  free(_n)

/*
 * this value will be (_start - CONFIG_SYS_MALLOC_LEN)
 */
static long malloc_start_lzma;

static void *malloc(unsigned int size)
{
	void *ptr = (void *)malloc_start_lzma;
	malloc_start_lzma += size;

	if (malloc_start_lzma >= ((unsigned long)&_start))
		return NULL;

	return ptr;
}

static void free(const void *ptr)
{
}

#include "unlzma.c"

#ifndef CONFIG_SYS_ICACHE_OFF
/* Invalidate entire I-cache and branch predictor array */
static void invalidate_icache_all(void)
{
	/*
	 * Invalidate all instruction caches to PoU.
	 * Also flushes branch target cache.
	 */
	asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));

	/* Invalidate entire branch predictor array */
	asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));

	/* Full system DSB - make sure that the invalidation is complete */
	dsb();

	/* ISB - make sure the instruction stream sees it */
	isb();
}
#else
static void invalidate_icache_all(void)
{
}
#endif

#ifdef CONFIG_EARLY_DCACHE_ENABLE
static int early_dcache_enable(void)
{
	unsigned long pdt = CONFIG_SYS_SDRAM_BASE + SZ_64K;
	mmu_pagedir_init(pdt);
	mmu_pagedir_cached_range(pdt, CONFIG_SYS_SDRAM_BASE, SZ_256M);
	mmu_startup(pdt);

	return 0;
}

static int early_dcache_disable(void)
{
	int i = 0;

	dcache_flush_all();

	/* mem barrier to sync up things */
	asm("mcr p15, 0, %0, c7, c10, 4" : : "r"(i));

	dcache_inv_all();

	mmu_turnoff();

	return 0;
}
#endif

void start_armboot(void)
{
	unsigned char *pdst = (unsigned char *)image_entry;
	unsigned int image_data_len = input_data_end - input_data;

	malloc_start_lzma = (((uintptr_t)&_start) - CONFIG_SYS_MALLOC_LEN);
	uart_early_init();

	if (input_data[0] == 0x5D) {
		uart_early_puts("\nUncompress");
		decompress(input_data, image_data_len, pdst);
		uart_early_puts("Ok\r\n");
	} else {
		int *s = (int *)input_data;
		int *d = (int *)pdst;
		unsigned int len = ((image_data_len + 3) >> 2);
		while (len--)
			*d++ = *s++;
	}

	void (*uboot)(void);
	uboot = (void (*))CONFIG_SYS_TEXT_BASE;
	invalidate_icache_all();
	uboot();
}

void hang(void)
{
	uart_early_puts("### ERROR ### Please RESET the board ###\n");
	for (;;)
		;
}


void do_bad_sync(void)
{
	uart_early_puts("bad sync abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_sync(void)
{
	uart_early_puts("sync abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_bad_error(void)
{
	uart_early_puts("bad error\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_error(void)
{
	uart_early_puts("error\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_bad_fiq(void)
{
	uart_early_puts("bad fast interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_bad_irq(void)
{
	uart_early_puts("bad interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_fiq(void)
{
	uart_early_puts("fast interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}

void do_irq(void)
{
	uart_early_puts("interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	reset_cpu(0);
}
