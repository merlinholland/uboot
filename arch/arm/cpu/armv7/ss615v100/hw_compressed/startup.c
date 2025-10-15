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

const unsigned long image_entry = (CONFIG_SYS_TEXT_BASE);

#define error(_s)			   uart_early_puts(_s)
#define putstr(_s)			  uart_early_puts(_s)

#define large_malloc(_n)		malloc(_n)
#define large_free(_n)		  free(_n)

/*
 * this value will be (_prepare_hw - CONFIG_SYS_MALLOC_LEN)
 */
static long malloc_start_lzma;

static void *malloc(unsigned int size)
{
	void *ptr = (void *)malloc_start_lzma;
	malloc_start_lzma += size;

	if (malloc_start_lzma >= ((unsigned long)&_prepare_hw))
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

#ifdef CONFIG_BSP_NAND_SPL
void *image_to_ddr(void *dst, void *src, int len)
{
	const char *s = src;
	char *d = dst;
	while (len--)
		*d++ = *s++;
	return dst;
}

#define _BIT_OFFSET_32_ 	32
#define _BIT_OFFSET_16_ 	16
#define _BIT_OFFSET_8_	 	8
#define _BIT_OFFSET_4_	 	4
#define _BIT_OFFSET_2_	 	2
#define _BIT_OFFSET_1_	 	2
int fls(int x)
{
	/* 32 bits */
	int r = _BIT_OFFSET_32_;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		/* check low 16 bits */
		x <<= _BIT_OFFSET_16_;
		r -= _BIT_OFFSET_16_;
	}
	if (!(x & 0xff000000u)) {
		/* check low 8 bits */
		x <<= _BIT_OFFSET_8_;
		r -= _BIT_OFFSET_8_;
	}
	if (!(x & 0xf0000000u)) {
		/* check low 4 bits */
		x <<= _BIT_OFFSET_4_;
		r -= _BIT_OFFSET_4_;
	}
	if (!(x & 0xc0000000u)) {
		/* check low 2 bits */
		x <<= _BIT_OFFSET_2_;
		r -= _BIT_OFFSET_2_;
	}
	if (!(x & 0x80000000u)) {
		/* check low 1 bits */
		x <<= _BIT_OFFSET_1_;
		r -= _BIT_OFFSET_1_;
	}
	return r;
}

#define read_reg(_addr)	({unsigned int (_v) = *(volatile unsigned int *)(_addr); dmb(); _v;})
#define FMC_REG_BASE		0x10000000
#define SYS_CTRL_REG_BASE   	0x11020000
#define FMC_CFG			0
#define REG_SYSSTAT         	0x0018
#define MASK_VALUE		3
#define PAGE_SHIFT		3
#define BLOCK_SHIFT		8
#define FIRST_IMAGE		0
#define BLOCK_NUM_64    	64UL
#define BLOCK_NUM_128   	128UL
#define PAGE_2K			2048UL
#define PAGE_4K			4096UL
#define PART_SIZE_256K		0x40000UL
#define PART_SIZE_512K		0x80000UL
#define _1M			0x100000UL
#define _64K			0x10000UL
#define SPI_NOR			0
#define SPI_NAND		1
#define EMMC			3
#define BOOT_MEDIA_SHIFT	2
#define CRC_NUMBER_OFFSET	20
#define CRC_STR_LEN		8
typedef unsigned long int       uintptr_t;

unsigned int get_flash_info(void)
{
	unsigned int reg;
	unsigned int page_num;
	unsigned int page_size;

	reg = read_reg(FMC_REG_BASE + FMC_CFG);
	page_num = (((reg >> BLOCK_SHIFT) & MASK_VALUE) ? BLOCK_NUM_128 : BLOCK_NUM_64);
	page_size = (((reg >> PAGE_SHIFT) & MASK_VALUE) ? PAGE_4K : PAGE_2K);
	return (page_num * page_size);
}
#define _HEXADECIMAL_	16
#define _DECIMAL_	10
unsigned int get_crc32_num(unsigned int crc_addr)
{
	int i;
	unsigned char *src = (unsigned char *)(uintptr_t)crc_addr;
	unsigned int value = 0;
	unsigned char str[CRC_STR_LEN] = {0};
	for (i = 0; i < CRC_STR_LEN; i++) {
		str[i] = *src++;
		if (str[i] >= '0' && str[i] <= '9') {
			/* Converting a hexadecimal numeric string into a hexadecimal
			 *  number requires multiplying the number by 16
			 */
			value = _HEXADECIMAL_ * value + str[i] - '0';
		} else if ((str[i] >= 'a' && str[i] <= 'z')) {
			/* To convert a hexadecimal string to a hexadecimal number,
			 * multiply by 16 and add 10
			 */
			value = _HEXADECIMAL_ * value + _DECIMAL_ + str[i] - 'a';
		}
	}
	return value;
}

int image_crc32_check(unsigned int *image_data_len)
{
	unsigned int total_num;
	unsigned int crc_number;
	unsigned int block_size;
	unsigned int offset;
	unsigned int reg;
	unsigned int boot_media;
	unsigned int cur_image_idx = 0;
	unsigned int crc_check;

	offset = ((uintptr_t)input_data) - ((uintptr_t)&_prepare_hw);

	reg = read_reg(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	boot_media = (reg >> BOOT_MEDIA_SHIFT) & MASK_VALUE;
	if (boot_media == SPI_NAND)
		block_size = get_flash_info();
	else
		return 0;
	total_num = 1 << (fls(_1M) - fls(block_size));
	crc_check = get_crc32_num(_blank_crc_start);
	do {
		/* the first u-boot image had already copy */
		if (cur_image_idx != FIRST_IMAGE) {
			image_to_ddr((void *)input_data, (void *)
				((uintptr_t)FMC_MEM_BASE + cur_image_idx * block_size + offset), *image_data_len);
			uart_early_puts("copy image finished\n");
		}
		crc_number = crc32(0, (const char *)input_data, *image_data_len);
		cur_image_idx++;
		if (crc_number == crc_check)
			uart_early_puts("crc32 check ok\n");
		else
			uart_early_puts("crc32 check failed\n");
	} while ((cur_image_idx < total_num) && (crc_number != crc_check));
	if (crc_number != crc_check) {
		uart_early_puts("can't get a complete uboot\n");
		return -1;
	}

	return 0;
}
#endif

void start_armboot(void)
{
	unsigned char *pdst = (unsigned char *)(uintptr_t)image_entry;
	unsigned int image_data_len = input_data_end - input_data;

	malloc_start_lzma = (((uintptr_t)&_prepare_hw) - CONFIG_SYS_MALLOC_LEN);
	uart_early_init();
#ifdef CONFIG_BSP_NAND_SPL
	if (image_crc32_check(&image_data_len))
		return;
#endif
	if (input_data[0] == 0x5D) {
		uart_early_puts("\nUncompress");
		decompress(input_data, image_data_len, pdst);
		uart_early_puts("Ok\r\n");
	} else {
#ifdef CONFIG_BSP_NAND_SPL
		image_to_ddr((void *)pdst, (void *)input_data, image_data_len);
#else
		int *s = (int *)input_data;
		int *d = (int *)pdst;
		unsigned int len = ((image_data_len + 3) >> 2);
		while (len--)
			*d++ = *s++;
#endif
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
