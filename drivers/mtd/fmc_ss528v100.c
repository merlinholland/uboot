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

#define REG_IO_BASE_FMC 0x10ff0000

/* configure io for SPI interface
 * SPI NOR/SPI NAND 3V3 SDR
 *  Signal   |  Register   |   Value    | Level
 *  CLK      | 0x10FF0024  | 0x000002d1 |   3
 *  MOSI_IO0 | 0x10FF001C  | 0x000000d1 |   3
 *  MISO_IO1 | 0x10FF0028  | 0x000000d1	|   3
 *  WP_IO2   | 0x10FF0020  | 0x000000d1	|   3
 *  HOLD_IO3 | 0x10FF002C  | 0x000000d1	|   3
 *  SFC_CS0N | 0x10FF0030  | 0x00000031 |   1
 *  SFC_CS1N | 0x10FF0034  | 0x00000031 |   1
 */
#define REG_SPI_CLK		(REG_IO_BASE_FMC + 0x24)
#define REG_SPI_MOSI_IO0	(REG_IO_BASE_FMC + 0x1C)
#define REG_SPI_MISO_IO1	(REG_IO_BASE_FMC + 0x28)
#define REG_SPI_WP_IO2		(REG_IO_BASE_FMC + 0x20)
#define REG_SPI_HOLD_IO3	(REG_IO_BASE_FMC + 0x2c)
#define REG_SFC_CS0N		(REG_IO_BASE_FMC + 0x30)
#define REG_SFC_CS1N		(REG_IO_BASE_FMC + 0x34)

#define SPI_CLK_VAL		0x000002d1
#define SPI_MOSI_IO0_VAL	0x000000d1
#define SPI_MISO_IO1_VAL	0x000000d1
#define SPI_WP_IO2_VAL		0x000000d1
#define SPI_HOLD_IO3_VAL	0x000000d1
#define SPI_SFC_CS0N_VAL	0x00000031
#define SPI_SFC_CS1N_VAL	0x00000031

static void ss528v100_spi_io_config(void)
{
	static unsigned int io_config_flag = 1;

	if (!io_config_flag)
		return;

	writel(SPI_CLK_VAL, REG_SPI_CLK);
	writel(SPI_MOSI_IO0_VAL, REG_SPI_MOSI_IO0);
	writel(SPI_MISO_IO1_VAL, REG_SPI_MISO_IO1);
	writel(SPI_WP_IO2_VAL, REG_SPI_WP_IO2);
	writel(SPI_HOLD_IO3_VAL, REG_SPI_HOLD_IO3);
	writel(SPI_SFC_CS0N_VAL, REG_SFC_CS0N);
	writel(SPI_SFC_CS1N_VAL, REG_SFC_CS1N);

	io_config_flag = 0;
}

void fmc_set_fmc_system_clock(struct spi_op *op, int clk_en)
{
	unsigned int old_val, regval;

	old_val = regval = readl(CRG_REG_BASE + REG_FMC_CRG);

	regval &= ~FMC_CLK_SEL_MASK;

	if (op && op->clock) {
		regval |= op->clock & FMC_CLK_SEL_MASK;
		fmc_pr(DTR_DB, "\t|||*-get the setting clock value: %#x\n",
				op->clock);
	} else {
		regval |= fmc_clk_sel(FMC_CLK_24M);	/* Default Clock */
		ss528v100_spi_io_config();
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
	unsigned int clk_reg, clk_type;
	const char *str[] = {"12", "37.5", "62.5", "75", "100"};

#define clk_2x(_clk)	(((_clk) + 1) >> 1)
	unsigned int sys_2x_clk[] = {
		clk_2x(24),	fmc_clk_sel(FMC_CLK_24M),
		clk_2x(100),	fmc_clk_sel(FMC_CLK_100M),
		clk_2x(150),	fmc_clk_sel(FMC_CLK_150M),
		clk_2x(200),	fmc_clk_sel(FMC_CLK_200M),
		0,		0,
	};
#undef clk_2x

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
	unsigned int clk_reg, clk_type;
	char* const str[] = {"6", "9.25", "31.25", "37.5", "50",
		"62.5", "75", "100"};

#define clk_4x(_clk)	(((_clk) + 1) >> 2)
	unsigned int sys_4x_clk[] = {
		clk_4x(24),	fmc_clk_sel(FMC_CLK_24M),
		clk_4x(100),	fmc_clk_sel(FMC_CLK_100M),
		clk_4x(150),	fmc_clk_sel(FMC_CLK_150M),
		clk_4x(200),	fmc_clk_sel(FMC_CLK_200M),
		clk_4x(300),	fmc_clk_sel(FMC_CLK_300M),
		clk_4x(400),	fmc_clk_sel(FMC_CLK_400M),
		0,		0,
	};
#undef clk_4x
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
