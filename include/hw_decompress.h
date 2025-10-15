// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifndef HW_DECOMPRESS_H
#define HW_DECOMPRESS_H

#define GZIP_MAX_LEN    0xffffff /* (16MB - 1Byte) */

typedef enum {
	HW_DECOMPRESS_OP_START = 0, /* decompress operation start */
	HW_DECOMPRESS_OP_CONTINUE,  /* decompress operation continue */
	HW_DECOMPRESS_OP_END,       /* decompress operation end */
	HW_DECOMPRESS_OP_ONCE,      /* decompress operation just once */
} hw_decompress_op_type;

extern unsigned int hw_dec_type;
void hw_dec_init(void);
void hw_dec_uinit(void);

#if (defined CONFIG_TARGET_SS919V100) || (defined CONFIG_SS919V100)        || \
	(defined CONFIG_TARGET_SS318V100) || (defined CONFIG_TARGET_SS918V100) || \
	(defined CONFIG_TARGET_SS015V100)	|| (defined CONFIG_SS015V100)         || \
	(defined CONFIG_TARGET_SS013V100)
int hw_dec_decompress(const unsigned char *dst_h32, const unsigned char *dst_l32,
	int* const dstlen, const unsigned char *src_h32,
	const unsigned char *src_l32, int srclen, const void *unused);
#endif

#if ((defined CONFIG_TARGET_SS313V100) || (defined CONFIG_TARGET_SS312V100)  || \
	(defined CONFIG_TARGET_SS813V100) || (defined CONFIG_TARGET_SS815V100) || \
	(defined CONFIG_TARGET_SS812V100) || (defined CONFIG_TARGET_SS101V200) || \
	(defined CONFIG_TARGET_SS101V500) || (defined CONFIG_TARGET_SS101V300) || \
	(defined CONFIG_TARGET_SS101V600) || (defined CONFIG_TARGET_SS011V100)  || \
	(defined CONFIG_TARGET_SS012V100))
int hw_dec_decompress(const unsigned char *dst, int *dstlen,
	const unsigned char *src, int srclen, const void *unused);
#endif

/*
 * Support decompressing gzip file by sections
 * op_type: operation type
 * dst: addr of uncompressed file
 * dstlen: len of uncompressed file
 * src: addr of compressed file
 * srclen: len of compressed file
 *
 * note: only support v1 version now for sections feature
 */
int hw_dec_decompress_ex(hw_decompress_op_type op_type, const unsigned char *dst,
	int *dstlen, const unsigned char *src, int srclen);
#endif
