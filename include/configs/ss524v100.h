// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef __SS524V100_H
#define __SS524V100_H

#include <linux/sizes.h>
#include <asm/arch/platform.h>

#define CONFIG_SUPPORT_RAW_INITRD

#define CONFIG_BOARD_EARLY_INIT_F

/* Physical Memory Map */

/* CONFIG_SYS_TEXT_BASE needs to align with where ATF loads bl33.bin */
#define CONFIG_SYS_TEXT_BASE_ORI        0x40700000

#define SEC_UBOOT_DATA_ADDR     0x41000000

#define PHYS_SDRAM_1            0x40000000
#define PHYS_SDRAM_1_SIZE       0x20000000

#define CONFIG_SYS_SDRAM_BASE       PHYS_SDRAM_1

#define CONFIG_SYS_INIT_SP_ADDR	    0x0401A000

#define CONFIG_SYS_LOAD_ADDR        (CONFIG_SYS_SDRAM_BASE + 0x80000)
#define CONFIG_SYS_GBL_DATA_SIZE    128

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY       24000000

#define CONFIG_SYS_TIMER_RATE       CFG_TIMER_CLK
#define CONFIG_SYS_TIMER_COUNTER    (CFG_TIMERBASE + REG_TIMER_VALUE)
#define CONFIG_SYS_TIMER_COUNTS_DOWN

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN       (CONFIG_ENV_SIZE + SZ_128K)

/* PL011 Serial Configuration */

#define CONFIG_PL011_CLOCK      24000000

#define CONFIG_PL01x_PORTS  \
	{(void *)UART0_REG_BASE, (void *)UART1_REG_BASE, \
	(void *)UART2_REG_BASE}

#define CONFIG_CUR_UART_BASE	UART0_REG_BASE

#define CONFIG_64BIT

/* Network configuration */
/*-----------------------------------------------------------------------
 * ETH driver
 -----------------------------------------------------------------------*/
/* default is bspeth-switch-fabric */
#ifdef CONFIG_SFV300_ETH
#define INNER_PHY
#define SFV_MII_MODE              0
#define SFV_RMII_MODE             1
#define ETH_MII_RMII_MODE_U       SFV_MII_MODE
#define ETH_MII_RMII_MODE_D       SFV_MII_MODE
#define SFV_PHY_U                 0
#define SFV_PHY_D                 2
#endif /* CONFIG_SFV300_ETH */

#define CONFIG_PHY_GIGE
#ifdef CONFIG_GMACV300_ETH
#define CONFIG_GMAC_NUMS        1
#define CONFIG_GMAC_PHY0_ADDR     1
#define CONFIG_GMAC_PHY0_INTERFACE_MODE	2 /* rgmii 2, rmii 1, mii 0 */
#define CONFIG_GMAC_PHY1_ADDR     3
#define CONFIG_GMAC_PHY1_INTERFACE_MODE	0 /* rgmii 2, rmii 1, mii 0 */
#define CONFIG_GMAC_DESC_4_WORD
#endif

#if defined(CONFIG_SFV300_ETH) || defined(CONFIG_GMACV300_ETH)
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#endif
/* Flash Memory Configuration v100 */
#ifdef CONFIG_FMC
#define CONFIG_FMC_REG_BASE       FMC_REG_BASE
#define CONFIG_FMC_BUFFER_BASE    FMC_MEM_BASE
#define CONFIG_FMC_MAX_CS_NUM     2
#endif

#ifdef CONFIG_FMC_SPI_NOR
#define CONFIG_SPI_NOR_MAX_CHIP_NUM 1
#define CONFIG_SPI_NOR_QUIET_TEST
#endif

#ifdef CONFIG_FMC_SPI_NAND
#define CONFIG_SPI_NAND_MAX_CHIP_NUM    1
#define CONFIG_SYS_MAX_NAND_DEVICE  CONFIG_SPI_NAND_MAX_CHIP_NUM
#define CONFIG_SYS_NAND_MAX_CHIPS   CONFIG_SPI_NAND_MAX_CHIP_NUM
#define CONFIG_SYS_NAND_BASE        FMC_MEM_BASE
#endif

#if (CONFIG_AUTO_UPDATE == 1)
#define CONFIG_AUTO_UPDATE_ADAPTATION   1
#define CONFIG_AUTO_USB_UPDATE	1
#define CONFIG_FS_FAT 1
#define CONFIG_FS_FAT_MAX_CLUSTSIZE 65536
#endif

/* SD/MMC configuration */
#ifdef CONFIG_MMC
/* Open it as you need #define CONFIG_MMC_SDMA */
#define CONFIG_EMMC
#define CONFIG_SUPPORT_EMMC_BOOT
#define CONFIG_GENERIC_MMC
#define CONFIG_SYS_MMC_ENV_DEV  0
#define CONFIG_EXT4_SPARSE
#define CONFIG_SDHCI
#define CONFIG_BSP_SDHCI
#define CONFIG_BSP_SDHCI_MAX_FREQ  148500000
#define CONFIG_FS_EXT4
#define CONFIG_SDHCI_ADMA
#define CONFIG_SUPPORT_EMMC_RPMB
#endif

#define CONFIG_MISC_INIT_R

/* Command line configuration */
#define CONFIG_MENU
/* Open it as you need #define CONFIG_CMD_UNZIP */
#define CONFIG_CMD_ENV

#define CONFIG_MTD_PARTITIONS

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE


/* Initial environment variables */

/*
 * Defines where the kernel and FDT will be put in RAM
 */

/* Assume we boot with root on the seventh partition of eMMC */
#define CONFIG_BOOTARGS "console=ttyAMA0,115200n8 root=/dev/mtdblock2 rw"
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 3
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define BOOT_TARGET_DEVICES(func) \
	func(USB, usb, 0) \
	func(MMC, mmc, 1) \
	func(DHCP, dhcp, na)
#include <config_distro_bootcmd.h>

/* allow change env */
#define  CONFIG_ENV_OVERWRITE

#define CONFIG_COMMAND_HISTORY

#define CONFIG_ENV_VARS_UBOOT_CONFIG

/* kernel parameter list phy addr */
#define CFG_BOOT_PARAMS         (CONFIG_SYS_SDRAM_BASE + 0x0100)

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE       1024    /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE       (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE     CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS      64  /* max command args */

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SKIP_LOWLEVEL_INIT_ONLY
#define CONFIG_CMDLINE_TAG

#define CONFIG_DDR_TRAINING_V2
#define DDR_SCRAMB_ENABLE

/* Open it as you need #define CONFIG_AUDIO_ENABLE */

#define CONFIG_PRODUCTNAME "ss524v100"

#define SECUREBOOT_OTP_REG_BASE_ADDR_PHY            (0x10200000)
#endif /* __SS524V100_H */

