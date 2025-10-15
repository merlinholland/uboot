// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef CIPHER_ADAPT_H
#define CIPHER_ADAPT_H

#ifndef OT_MINIBOOT_SUPPORT
#include <common.h>
#else
#include "delay.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#endif
#include "malloc.h"

#include <asm/io.h>
#include "ot_type.h"
#include "ot_drv_cipher.h"
#include "cipher_config.h"
#include "cipher_osal.h"

/**************************** M A C R O ****************************/
#define OT_ERR_CIPHER_NOT_INIT                     (td_s32)(0x804D0001)
#define OT_ERR_CIPHER_INVALID_HANDLE               (td_s32)(0x804D0002)
#define OT_ERR_CIPHER_INVALID_POINT                (td_s32)(0x804D0003)
#define OT_ERR_CIPHER_INVALID_PARAM                (td_s32)(0x804D0004)
#define OT_ERR_CIPHER_FAILED_INIT                  (td_s32)(0x804D0005)
#define OT_ERR_CIPHER_FAILED_GETHANDLE             (td_s32)(0x804D0006)
#define OT_ERR_CIPHER_FAILED_RELEASEHANDLE         (td_s32)(0x804D0007)
#define OT_ERR_CIPHER_FAILED_CONFIGAES             (td_s32)(0x804D0008)
#define OT_ERR_CIPHER_FAILED_CONFIGDES             (td_s32)(0x804D0009)
#define OT_ERR_CIPHER_FAILED_ENCRYPT               (td_s32)(0x804D000A)
#define OT_ERR_CIPHER_FAILED_DECRYPT               (td_s32)(0x804D000B)
#define OT_ERR_CIPHER_BUSY                         (td_s32)(0x804D000C)
#define OT_ERR_CIPHER_NO_AVAILABLE_RNG             (td_s32)(0x804D000D)

#define OT_ID_CIPHER                        100

#define CIPHER_IOR                          _IOWR
#define CIPHER_IOW                          _IOW
#define CIPHER_IOWR                         _IOWR

#define CIPHER_IOC_DIR                      _IOC_DIR
#define CIPHER_IOC_TYPE                     _IOC_TYPE
#define CIPHER_IOC_NR                       _IOC_NR
#define CIPHER_IOC_SIZE                     _IOC_SIZE
#define u32_to_point(addr)                  ((td_void*)((td_size_t)(addr)))
#define point_to_u32(addr)                  ((td_u32)((td_size_t)(addr)))

#define hal_cipher_read_reg(addr, val)      (*(val) = readl(addr))
#define hal_cipher_write_reg(addr, val)     writel(val, addr)

#define hal_set_bit(src, bit)               ((src) |= (1 << (bit)))
#define hal_clear_bit(src, bit)             ((src) &= ~(1 << (bit)))

/**************************** S T D L I B ****************************/
#define cipher_ioremap_nocache(addr, size)  (td_void*)(addr)
#define cipher_iounmap(x)

#define CIPHER_QUEUE_HEAD                   td_void *
#define cipher_queue_init(x)
#define cipher_queue_wait_up(x)
#define cipher_queue_wait_timeout(head, con, time)

#define cipher_request_irq(irq, func, name)
#define cipher_free_irq(irq, name)
#define CRYPTO_IRQRETURN_T                  td_s32
#define CIPHER_IRQ_HANDLED                  1

#define cipher_udelay(usec)                 udelay(usec)

void hex2str(char buf[2], td_u8 val); /* 2 buf size */

td_s32  cipher_mmz_alloc_remap(td_char *name, cipher_mmz_buf_t *cipher_mmz);
td_void cipher_mmz_release_unmap(cipher_mmz_buf_t *cipher_mmz);
td_s32  cipher_mmz_map(cipher_mmz_buf_t *cipher_mmz);
td_void cipher_mmz_unmap(const cipher_mmz_buf_t *cipher_mmz);

#endif /* CIPHER_ADAPT_H */
