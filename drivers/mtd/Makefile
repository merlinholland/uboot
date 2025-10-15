# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2007
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

mtd-$(CONFIG_MTD) += mtdcore.o mtd_uboot.o
mtd-$(CONFIG_DM_MTD) += mtd-uclass.o
mtd-$(CONFIG_MTD_PARTITIONS) += mtdpart.o
mtd-$(CONFIG_MTD_CONCAT) += mtdconcat.o
mtd-$(CONFIG_ALTERA_QSPI) += altera_qspi.o
mtd-$(CONFIG_FLASH_CFI_DRIVER) += cfi_flash.o
mtd-$(CONFIG_FLASH_CFI_MTD) += cfi_mtd.o
mtd-$(CONFIG_FLASH_CFI_LEGACY) += jedec_flash.o
mtd-$(CONFIG_MW_EEPROM) += mw_eeprom.o
mtd-$(CONFIG_FLASH_PIC32) += pic32_flash.o
mtd-$(CONFIG_ST_SMI) += st_smi.o
mtd-$(CONFIG_STM32_FLASH) += stm32_flash.o
mtd-$(CONFIG_RENESAS_RPC_HF) += renesas_rpc_hf.o
mtd-$(CONFIG_HBMC_AM654) += hbmc-am654.o
obj-$(CONFIG_FMC) += fmc_common.o
obj-$(CONFIG_TARGET_SS919V100) += fmc_ss919v100.o
obj-$(CONFIG_TARGET_SS015V100) += fmc_ss919v100.o
obj-$(CONFIG_TARGET_SS318V100) += fmc_ss318v100.o
obj-$(CONFIG_TARGET_SS918V100) += fmc_ss918v100.o
obj-$(CONFIG_TARGET_SS013V100) += fmc_ss918v100.o
obj-$(CONFIG_TARGET_SS812V100) += fmc_ss812v100.o
obj-$(CONFIG_TARGET_SS312V100) += fmc_ss312v100.o
obj-$(CONFIG_TARGET_SS313V100) += fmc_ss313v100.o
obj-$(CONFIG_TARGET_SS011V100) += fmc_ss313v100.o
obj-$(CONFIG_TARGET_SS012V100) += fmc_ss313v100.o
obj-$(CONFIG_TARGET_SS813V100) += fmc_ss813v100.o
obj-$(CONFIG_TARGET_SS815V100) += fmc_ss815v100.o
obj-$(CONFIG_TARGET_SS101V200) += fmc_ss101v200.o
obj-$(CONFIG_TARGET_SS101V500) += fmc_ss101v500.o
obj-$(CONFIG_TARGET_SS101V300) += fmc_ss101v300.o
obj-$(CONFIG_TARGET_SS101V600) += fmc_ss101v600.o
obj-$(CONFIG_TARGET_SS528V100) += fmc_ss528v100.o
obj-$(CONFIG_TARGET_SS625V100) += fmc_ss528v100.o
obj-$(CONFIG_TARGET_SS524V100) += fmc_ss524v100.o
obj-$(CONFIG_TARGET_SS615V100) += fmc_ss524v100.o
obj-$(CONFIG_TARGET_SS522V100) += fmc_ss522v100.o
obj-$(CONFIG_TARGET_SS522V101) += fmc_ss522v101.o
obj-$(CONFIG_TARGET_SS928V100) += fmc_ss928v100.o
obj-$(CONFIG_TARGET_SS927V100) += fmc_ss928v100.o

# U-Boot build
ifeq ($(CONFIG_SPL_BUILD)$(CONFIG_TPL_BUILD),)

ifneq ($(mtd-y),)
obj-y += mtd.o
endif
obj-y += nand/
obj-y += onenand/
obj-y += spi/
obj-$(CONFIG_MTD_UBI) += ubi/

#SPL/TPL build
else

ifneq ($(mtd-y),)
obj-$(CONFIG_SPL_MTD_SUPPORT) += mtd.o
endif
obj-$(CONFIG_$(SPL_TPL_)NAND_SUPPORT) += nand/
obj-$(CONFIG_SPL_ONENAND_SUPPORT) += onenand/
obj-$(CONFIG_$(SPL_TPL_)SPI_FLASH_SUPPORT) += spi/
obj-$(CONFIG_SPL_UBI) += ubispl/

endif
