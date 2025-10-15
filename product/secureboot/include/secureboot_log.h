// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef SECUREBOOT_LOG_H
#define SECUREBOOT_LOG_H

#include <common.h>

#include "security_boot_struct.h"
#define se_log(fmt, ...)   printf("[%s:%d] " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define HEX_BASE        16
#define MAX_DUMP_LEN    64
void dump_data(const char *name, const unsigned char *data, unsigned int data_len);
void image_header_dump(const image_head_struct *image_head);

#endif