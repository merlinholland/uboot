// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/platform.h>
#include <fmc_common.h>

#include "fmc_spi_ids.h"

#define REG_IO_BASE 0x10230000

#define REG_NF_DQ6      	(REG_IO_BASE + 0x0)
#define REG_NF_DQ7      	(REG_IO_BASE + 0x4)
#define REG_NF_WEN		(REG_IO_BASE + 0x8)
#define REG_NF_ALE		(REG_IO_BASE + 0xc)
#define REG_NF_CLE		(REG_IO_BASE + 0x10)
#define REG_NF_DQ3      	(REG_IO_BASE + 0x14)
#define REG_NF_DQ0      	(REG_IO_BASE + 0x18)
#define REG_SPI_WP_IO2		(REG_IO_BASE + 0x18)
#define REG_NF_DQ2      	(REG_IO_BASE + 0x1c)
#define REG_SPI_HOLD_IO3	(REG_IO_BASE + 0x1c)
#define REG_NF_DQ1      	(REG_IO_BASE + 0x20)
#define REG_SPI_MISO_IO1	(REG_IO_BASE + 0x20)
#define REG_NF_CSN		(REG_IO_BASE + 0x24)
#define REG_SPI_CLK		(REG_IO_BASE + 0x24)
#define REG_NF_DQ4      	(REG_IO_BASE + 0x28)
#define REG_SPI_MOSI_IO0	(REG_IO_BASE + 0x28)
#define REG_NF_DQ5      	(REG_IO_BASE + 0x2c)
#define REG_NF_REN		(REG_IO_BASE + 0x30)
#define REG_SPI_CS0		(REG_IO_BASE + 0x30)
#define REG_NF_RDY		(REG_IO_BASE + 0x34)
#define REG_SPI_CS1		(REG_IO_BASE + 0x34)

static void ss928v100_nand_io_config(void)
{
	writel(0x1021, REG_NF_CSN);
	writel(0x1041, REG_NF_REN);
	writel(0x1021, REG_NF_WEN);
	writel(0x1021, REG_NF_ALE);
	writel(0x1021, REG_NF_CLE);
	writel(0x1101, REG_NF_RDY);
	writel(0x1021, REG_NF_DQ6);
	writel(0x1021, REG_NF_DQ7);
	writel(0x1021, REG_NF_DQ3);
	writel(0x1021, REG_NF_DQ0);
	writel(0x1021, REG_NF_DQ2);
	writel(0x1021, REG_NF_DQ1);
	writel(0x1021, REG_NF_DQ4);
	writel(0x1021, REG_NF_DQ5);
}

void fmc100_nand_controller_enable(int enable)
{
	unsigned int old_val;
	unsigned int regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	regval &= ~FMC_CLK_SEL_MASK;
	regval |= fmc_clk_sel(FMC_CLK_200M);

	if (enable)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	regval &= ~FMC_SOFT_RST_REQ;
	if (regval != old_val)
		writel(regval, (CRG_REG_BASE + REG_FMC_CRG));

	ss928v100_nand_io_config();
}

static void ss928v100_spi_io_config(void)
{
	static unsigned int io_config_flag = 1;

	if (!io_config_flag)
		return;

	writel(0x1370, REG_SPI_CLK);
	writel(0x1150, REG_SPI_CS0);
	writel(0x1150, REG_SPI_CS1);
	writel(0x1150, REG_SPI_MOSI_IO0);
	writel(0x1050, REG_SPI_MISO_IO1);
	writel(0x1050, REG_SPI_WP_IO2);
	writel(0x1050, REG_SPI_HOLD_IO3);
	io_config_flag = 0;
}

void fmc_set_fmc_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val;
	unsigned int regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	regval &= ~FMC_CLK_SEL_MASK;

	if (op && op->clock) {
		regval |= op->clock & FMC_CLK_SEL_MASK;
		fmc_pr(DTR_DB, "\t|||*-get the setting clock value: %#x\n",
				op->clock);
	} else {
		regval |= fmc_clk_sel(FMC_CLK_24M);	/* Default Clock */
		ss928v100_spi_io_config();
	}

	if (clk_en)
		regval |= FMC_CLK_ENABLE;
	else
		regval &= ~FMC_CLK_ENABLE;

	if (regval != old_val) {
		fmc_pr(DTR_DB, "\t|||*-setting system clock [%#x]%#x\n",
				REG_FMC_CRG, regval);
		writel(regval, (CRG_REG_BASE + REG_FMC_CRG));
	}
}

void fmc_get_fmc_best_2x_clock(unsigned int *clock)
{
	int ix;
	unsigned int clk_reg;
	unsigned int clk_type;
	const char *str[] = {"12", "50", "75", "100"};

	unsigned int sys_2x_clk[] = {
		clk_2x(24), fmc_clk_sel(FMC_CLK_24M),
		clk_2x(100),	fmc_clk_sel(FMC_CLK_100M),
		clk_2x(150),	fmc_clk_sel(FMC_CLK_150M),
		clk_2x(200),	fmc_clk_sel(FMC_CLK_200M),
		0,		0,
	};

	clk_type = FMC_CLK_24M;
	clk_reg = fmc_clk_sel(clk_type);
	fmc_pr(QE_DBG, "\t|||*-matching flash clock %d\n", *clock);
	for (ix = 0; sys_2x_clk[ix]; ix += _2B) {
		if (*clock < sys_2x_clk[ix])
			break;
		clk_reg = sys_2x_clk[ix + 1];
		clk_type = get_fmc_clk_type(clk_reg);
		fmc_pr(QE_DBG, "\t||||-select system clock: %sMHz\n",
				str[clk_type]);
	}
#ifdef CONFIG_DTR_MODE_SUPPORT
	fmc_pr(DTR_DB, "best system clock for SDR.\n");
#endif
	fmc_pr(QE_DBG, "\t|||*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}

#ifdef CONFIG_DTR_MODE_SUPPORT

void fmc_get_fmc_best_4x_clock(unsigned int *clock)
{
	int ix;
	unsigned int clk_reg;
	unsigned int clk_type;
	char* const str[] = {"6", "25", "37.5", "50",
		"62.5", "75", "100"};

	unsigned int sys_4x_clk[] = {
		clk_4x(24), fmc_clk_sel(FMC_CLK_24M),
		clk_4x(100),	fmc_clk_sel(FMC_CLK_100M),
		clk_4x(150),	fmc_clk_sel(FMC_CLK_150M),
		clk_4x(200),	fmc_clk_sel(FMC_CLK_200M),
		clk_4x(250),	fmc_clk_sel(FMC_CLK_250M),
		clk_4x(300),	fmc_clk_sel(FMC_CLK_300M),
		clk_4x(400),	fmc_clk_sel(FMC_CLK_400M),
		0,		0,
	};

	clk_type = FMC_CLK_24M;
	clk_reg = fmc_clk_sel(clk_type);
	fmc_pr(DTR_DB, "\t|||*-matching flash clock %d\n", *clock);
	for (ix = 0; sys_4x_clk[ix]; ix += _2B) {
		if (*clock < sys_4x_clk[ix])
			break;
		clk_reg = sys_4x_clk[ix + 1];
		clk_type = get_fmc_clk_type(clk_reg);
		fmc_pr(DTR_DB, "\t||||-select system clock: %sMHz\n",
				str[clk_type]);
	}
	fmc_pr(DTR_DB, "best system clock for DTR.\n");
	fmc_pr(DTR_DB, "\t|||*-matched best system clock: %sMHz\n",
			str[clk_type]);
	*clock = clk_reg;
}
#endif/* CONFIG_DTR_MODE_SUPPORT */
