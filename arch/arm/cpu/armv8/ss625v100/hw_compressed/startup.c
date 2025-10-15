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

/*
 * SCTLR_EL1/SCTLR_EL2/SCTLR_EL3 bits definitions
 */
#define CR_M            (1 << 0)        /* MMU enable                   */
#define CR_A            (1 << 1)        /* Alignment abort enable       */
#define CR_C            (1 << 2)        /* Dcache enable                */
#define CR_SA           (1 << 3)        /* Stack Alignment Check Enable */
#define CR_I            (1 << 12)       /* Icache enable                */
#define CR_WXN          (1 << 19)       /* Write Permision Imply XN     */
#define CR_EE           (1 << 25)       /* Exception (Big) Endian       */

#define EXCEPTION_LEVEL1   	1
#define EXCEPTION_LEVEL2   	2

const uintptr_t image_entry = (CONFIG_SYS_TEXT_BASE);

#define error(_s)               uart_early_puts(_s)
#define putstr(_s)              uart_early_puts(_s)

#define large_malloc(_n)        malloc(_n)
#define large_free(_n)          free(_n)

/*
 * this value will be (_armboot_start - CONFIG_SYS_MALLOC_LEN)
 */
static long malloc_start_lzma;

static void *malloc(unsigned int size)
{
	void *ptr = (void *)(uintptr_t)malloc_start_lzma;
	malloc_start_lzma += size;

	if (malloc_start_lzma >= ((uintptr_t)&_start))
		return NULL;

	return ptr;
}

static void free(const void *ptr)
{
}

#include "unlzma.c"

void invalidate_icache_all(void);
void dcache_enable(void);
void dcache_disable(void);

void start_armboot(void)
{
	unsigned char *pdst = (unsigned char *)image_entry;
	unsigned int image_data_len = input_data_end - input_data;

	malloc_start_lzma = (((uintptr_t)&_start) - CONFIG_SYS_MALLOC_LEN);
	uart_early_init();

	dcache_enable();

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

	dcache_disable();

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

static unsigned int current_el(void)
{
	unsigned int el;
	asm volatile("mrs %0, CurrentEL" : "=r" (el) : : "cc");
	return el >> 2; /* Move Left 2bit */
}

void set_sctlr(unsigned int val)
{
	unsigned int el;

	el = current_el();
	if (el == EXCEPTION_LEVEL1)
		asm volatile("msr sctlr_el1, %0" : : "r" (val) : "cc");
	else if (el == EXCEPTION_LEVEL2)
		asm volatile("msr sctlr_el2, %0" : : "r" (val) : "cc");
	else
		asm volatile("msr sctlr_el3, %0" : : "r" (val) : "cc");

	asm volatile("isb");
}

unsigned int get_sctlr(void)
{
	unsigned int el, val;

	el = current_el();
	if (el == EXCEPTION_LEVEL1)
		asm volatile("mrs %0, sctlr_el1" : "=r" (val) : : "cc");
	else if (el == EXCEPTION_LEVEL2)
		asm volatile("mrs %0, sctlr_el2" : "=r" (val) : : "cc");
	else
		asm volatile("mrs %0, sctlr_el3" : "=r" (val) : : "cc");

	return val;
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
