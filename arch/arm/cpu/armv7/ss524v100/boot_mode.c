// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/platform.h>

int get_boot_media(void)
{
	unsigned int reg_val, boot_mode, spi_device_mode;
	int boot_media;

	reg_val = readl(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	boot_mode = GET_SYS_BOOT_MODE(reg_val);

	switch (boot_mode) {
	case BOOT_FROM_SPI:
		spi_device_mode = GET_SPI_DEVICE_TYPE(reg_val);
		if (spi_device_mode)
			boot_media = BOOT_MEDIA_NAND;
		else
			boot_media = BOOT_MEDIA_SPIFLASH;
		break;
	case BOOT_FROM_EMMC:
		boot_media = BOOT_MEDIA_EMMC;
		break;
	default:
		boot_media = BOOT_MEDIA_UNKNOWN;
		break;
	}

	return boot_media;
}

