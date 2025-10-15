// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

/* CRG register */
#define PERI_CRG_MMC_CLK      (CRG_REG_BASE + 0x34c0)
#define  CRG_CLK_SEL_SHIFT    24
#define  CRG_CLK_SEL_MASK     (0x7 << CRG_CLK_SEL_SHIFT)
#define  CRG_DLL_SRST_REQ     (1 << 16)
#define  CRG_CLK_EN           (1 << 0)
#define  CRG_AHB_CLK_EN       (2 << 0)

#define PERI_CRG_MMC_P4_DLL     (CRG_REG_BASE + 0x34c4)
#define  CRG_P4_DLL_SRST_REQ    (1 << 1)

#define PERI_CRG_MMC_DRV_DLL        (CRG_REG_BASE + 0x34c8)
#define  CRG_DRV_PHASE_SEL_SHIFT    15
#define  CRG_DRV_PHASE_SEL_MASK     (0x1F << 15)

#define PERI_CRG_MMC_STAT     (CRG_REG_BASE + 0x34d8)
#define  CRG_SAM_DLL_READY    (1 << 12)
#define  CRG_DS_DLL_READY     (1 << 10)
#define  CRG_P4_DLL_LOCKED    (1 << 9)

#define REG_EMMC_SAMPL_DLL_STATUS      PERI_CRG_MMC_STAT
#define REG_SDIO0_SAMPL_DLL_STATUS     PERI_CRG_MMC_STAT
#define  SDIO_SAMPL_DLL_SLAVE_READY    CRG_SAM_DLL_READY

#define REG_EMMC_DRV_DLL_CTRL       PERI_CRG_MMC_DRV_DLL
#define REG_SDIO0_DRV_DLL_CTRL      PERI_CRG_MMC_DRV_DLL
#define  SDIO_DRV_PHASE_SEL_MASK    CRG_DRV_PHASE_SEL_MASK
#define  sdio_drv_sel(phase)        ((phase) << CRG_DRV_PHASE_SEL_SHIFT)

#define REG_EMMC_SAMPLB_DLL_CTRL    0x34d4
#define SDIO_SAMPLB_DLL_CLK_MASK    0xf
#define sdio_samplb_sel(phase)      (((phase) & 0xf) << 0)

/* eMMC io reg */
#define REG_IO_CFG_BASE    0x10FF0000
#define REG_MMC_CLK_IO     0x10
#define REG_MMC_CMD_IO     0x14
#define REG_MMC_D0_IO      0x00
#define REG_MMC_D1_IO      0x04
#define REG_MMC_D2_IO      0x08
#define REG_MMC_D3_IO      0x0c
#define REG_MMC_D4_IO      0x1c
#define REG_MMC_D6_IO      0x20
#define REG_MMC_D5_IO      0x28
#define REG_MMC_D7_IO      0x2C
#define REG_MMC_DQS_IO     0x30
#define REG_MMC_RST_IO     0x18

 /* IO CFG */
#define IO_CFG_DRV_STR_MASK      (0xf << 4)
#define  IO_CFG_DRV_STR_SEL(str) ((str) << 4)
#define  IO_CFG_PULL_UP          (0x1 << 8)
#define  IO_CFG_PULL_DOWN        (0x1 << 9)
#define  IO_CFG_SR               (0x1 << 10)
#define  IO_CFG_SDIO_MASK        (IO_CFG_DRV_STR_MASK | IO_CFG_PULL_UP |\
    IO_CFG_PULL_DOWN | IO_CFG_SR)

#define MMC_IO_MUX              0x2
#define IO_MUX_MASK             0x3
#define MMC_BUS_WIDTH_4_BIT     4
#define MMC_BUS_WIDTH_8_BIT     8
#define BOOT_FROM_NAND_FLAG     (0x1  << 2)
#define BOOT_FLAG_MASK          (0x3  << 2)
#define EMMC_BOOT_8BIT          (0x1  << 11)

#define IO_CLK              0
#define IO_CMD              1
#define IO_DATA             2
#define IO_RST              3
#define IO_DS               4
#define EMMC_IO_TYPE_NUM    5

static unsigned int reg_data_io[] = {
	REG_MMC_D0_IO, REG_MMC_D1_IO,
	REG_MMC_D2_IO, REG_MMC_D3_IO,
	REG_MMC_D4_IO, REG_MMC_D5_IO,
	REG_MMC_D6_IO, REG_MMC_D7_IO
};

static u32 bsp_mmc_io_cfg[][EMMC_IO_TYPE_NUM] = {
	[MMC_LEGACY] = {
		IO_CFG_DRV_STR_SEL(0xd) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP
	},
	[MMC_HS] = {
		IO_CFG_DRV_STR_SEL(0xd) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP
	},
	[MMC_HS_52] = {
		IO_CFG_DRV_STR_SEL(0xd) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xe) | IO_CFG_PULL_UP
	},
	[MMC_HS_200] = {
		IO_CFG_DRV_STR_SEL(0x8) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xc) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xc) | IO_CFG_PULL_UP
	},
	[MMC_HS_400] = {
		IO_CFG_DRV_STR_SEL(0x8) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xb) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xb) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0x3) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0x3) | IO_CFG_PULL_DOWN
	},
	[MMC_HS_400_ES] = {
		IO_CFG_DRV_STR_SEL(0x8) | IO_CFG_PULL_DOWN,
		IO_CFG_DRV_STR_SEL(0xb) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0xb) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0x3) | IO_CFG_PULL_UP,
		IO_CFG_DRV_STR_SEL(0x3) | IO_CFG_PULL_DOWN
	}
};

/* avaliable frequency */
#define CLK_400K        400000
#define CLK_24P75M      24750000
#define CLK_49P5M       49500000
#define CLK_99M         99000000
#define CLK_148P5M      148500000
#define CLK_175M        175000000
#define CLK_196P5M      196500000

/* sample drive phase */
#define DRIVE             0
#define SAMPLE            1
#define PHASE_TYPE_NUM    2

static u32 phase_cfg[][PHASE_TYPE_NUM] = {
	[MMC_LEGACY]     = { 16, 0 },
	[MMC_HS]     = { 16, 4 },
	[MMC_HS_52]  = { 16, 4 },
	[MMC_HS_200]  = { 22, 0 },
	[MMC_HS_400]  = {  9, 0 },
	[MMC_HS_400_ES]  = {  9, 0 }
};

unsigned int get_ocr_from_bootrom(void)
{
	return readl(REG_SAVE_HCS);
}

void wait_drv_dll_lock(const struct sdhci_host *host)
{
	/* doing nothing */
}

void enable_sampl_dll_slave(struct sdhci_host *host)
{
	/* doing nothing */
}

static void dll_reset_assert(void)
{
	const uintptr_t crg_addr =  PERI_CRG_MMC_P4_DLL;
	unsigned int reg;

	reg = readl(crg_addr);
	reg |= CRG_P4_DLL_SRST_REQ;
	writel(reg, crg_addr);
}

static void dll_reset_deassert(void)
{
	const uintptr_t crg_addr = PERI_CRG_MMC_P4_DLL;
	unsigned int reg;

	reg = readl(crg_addr);
	reg &= ~CRG_P4_DLL_SRST_REQ;
	writel(reg, crg_addr);
}

static void wait_p4_dll_lock(void)
{
	const uintptr_t reg_addr = PERI_CRG_MMC_STAT;
	unsigned int timeout = 20;
	unsigned int reg;

	do {
		reg = readl(reg_addr);
		if (reg & CRG_P4_DLL_LOCKED)
			return;

		udelay(1000); /* delay 1000 us */
		timeout--;
	} while (timeout > 0);

	printf("sdhci-bspsi: P4 DLL master not locked.\n");
}

static void wait_ds_dll_ready(void)
{
	const uintptr_t reg_addr = PERI_CRG_MMC_STAT;
	unsigned int timeout = 20;
	unsigned int reg;

	do {
		reg = readl(reg_addr);
		if (reg & CRG_DS_DLL_READY)
			return;

		udelay(1000); /* delay 1000 us */
		timeout--;
	} while (timeout > 0);

	printf("sdhci-bspsi: DS DLL slave not ready.\n");
}

static void bsp_mmc_priv_init(struct sdhci_host *host)
{
	unsigned int reg;

	reg = sdhci_readl(host, SDHCI_AXI_MBIIU_CTRL);
	reg &= ~SDHCI_UNDEFL_INCR_EN;
	sdhci_writel(host, reg, SDHCI_AXI_MBIIU_CTRL);

	reg = sdhci_readl(host, SDHCI_EMMC_CTRL);
	reg |= SDHCI_CARD_IS_EMMC;
	sdhci_writel(host, reg, SDHCI_EMMC_CTRL);
}

static unsigned int get_mmc_bus_width(void)
{
	unsigned int sys_stat;
	unsigned int bus_width;

	sys_stat = readl(SYS_CTRL_REG_BASE + REG_SYSSTAT);
	if ((sys_stat & BOOT_FLAG_MASK) == BOOT_FROM_NAND_FLAG) {
		/* up to 4 bit mode support when spi nand start up */
		bus_width = MMC_BUS_WIDTH_4_BIT;
	} else {
		bus_width =  (sys_stat & EMMC_BOOT_8BIT) ?
			MMC_BUS_WIDTH_8_BIT : MMC_BUS_WIDTH_4_BIT;
	}

	return bus_width;
}

static void set_drv_str(unsigned int offset, unsigned int drv_str)
{
	unsigned int reg;
	const uintptr_t io_addr = REG_IO_CFG_BASE + offset;

	reg = readl(io_addr);
	reg &= ~IO_CFG_SDIO_MASK;
	reg |= drv_str;
	writel(reg, io_addr);
}

static void bsp_mmc_set_ioconfig(struct sdhci_host *host)
{
	unsigned int selected_mode = host->mmc->selected_mode;
	unsigned int bus_width;
	unsigned int i;

	bus_width = get_mmc_bus_width();

	set_drv_str(REG_MMC_CLK_IO, bsp_mmc_io_cfg[selected_mode][IO_CLK]);
	set_drv_str(REG_MMC_CMD_IO, bsp_mmc_io_cfg[selected_mode][IO_CMD]);
	for (i = 0; i < bus_width; i++)
		set_drv_str(reg_data_io[i],
				 bsp_mmc_io_cfg[selected_mode][IO_DATA]);
	set_drv_str(REG_MMC_RST_IO, bsp_mmc_io_cfg[selected_mode][IO_RST]);

	if (selected_mode == MMC_HS_400 ||
	    selected_mode == MMC_HS_400_ES)
		set_drv_str(REG_MMC_DQS_IO,
				 bsp_mmc_io_cfg[selected_mode][IO_DS]);

	sdhci_set_uhs_timing(host);
}

static void set_phase(struct sdhci_host *host)
{
	unsigned int drv_phase, sample_phase;
	unsigned int selected_mode = host->mmc->selected_mode;

	if (selected_mode == MMC_HS_400 ||
	    selected_mode == MMC_HS_200)
		sample_phase = host->tuning_phase;
	else
		sample_phase = phase_cfg[selected_mode][SAMPLE];

	drv_phase = phase_cfg[selected_mode][DRIVE];

	set_drv_phase(host, drv_phase);
	enable_sample(host);
	set_sampl_phase(host, sample_phase);

	udelay(25); /* delay 25us */
}

static void set_crg(struct sdhci_host *host, unsigned int clk)
{
	const uintptr_t crg_addr = PERI_CRG_MMC_CLK;
	unsigned int sel, reg;
	unsigned int clk_mux[] = {
		CLK_400K, CLK_24P75M, CLK_49P5M,
		CLK_99M, CLK_148P5M, CLK_175M, CLK_196P5M
	};

	for (sel = 0x6; sel > 0; sel--) {
		if (clk >= clk_mux[sel])
			break;
	}

	reg = readl(crg_addr);
	reg &= ~CRG_CLK_SEL_MASK;
	reg |= mmc_clk_sel(sel);
	writel(reg, crg_addr);
}

static int bsp_mmc_set_clk(struct sdhci_host *host, unsigned int clk)
{
	disable_card_clk(host);
	udelay(25); /* delay 25 us */
	disable_internal_clk(host);

	if (clk == 0)
		return 0;

	set_crg(host, clk);
	set_phase(host);

	udelay(5); /* delay 5 us */

	enable_internal_clk(host);

	if ((host->mmc->selected_mode == MMC_HS_400) ||
	    (host->mmc->selected_mode == MMC_HS_400_ES) ||
	    (host->mmc->selected_mode == MMC_HS_200)) {
		dll_reset_assert();
		dll_reset_deassert();
		wait_p4_dll_lock();
		wait_sampl_dll_slave_ready(host);
	}

	if ((host->mmc->selected_mode == MMC_HS_400) ||
	    (host->mmc->selected_mode == MMC_HS_400_ES))
		wait_ds_dll_ready();

	enable_card_clk(host);
	udelay(75); /* delay 75 us */

	return 0;
}

static void set_pin_mux(unsigned int offset, unsigned int pin_mux)
{
	unsigned int reg;
	const uintptr_t io_addr = REG_IO_CFG_BASE + offset;

	reg = readl(io_addr);
	reg &= ~IO_MUX_MASK;
	reg |= pin_mux;
	writel(reg, io_addr);
}

static void emmc_pin_mux_config(void)
{
	unsigned int bus_width, i;

	bus_width = get_mmc_bus_width();

	set_pin_mux(REG_MMC_CLK_IO, MMC_IO_MUX);
	set_pin_mux(REG_MMC_CMD_IO, MMC_IO_MUX);
	for (i = 0; i < bus_width; i++)
		set_pin_mux(reg_data_io[i], MMC_IO_MUX);
	set_pin_mux(REG_MMC_RST_IO, MMC_IO_MUX);

	if (bus_width == MMC_BUS_WIDTH_8_BIT)
		set_pin_mux(REG_MMC_DQS_IO, MMC_IO_MUX);
}

static void bsp_mmc_crg_init(void)
{
	unsigned int reg;

	/* eMMC clk enable */
	reg = readl(PERI_CRG_MMC_CLK);
	reg |= CRG_CLK_EN | CRG_AHB_CLK_EN;
	writel(reg, PERI_CRG_MMC_CLK);

	/* eMMC reset assert */
	reg = readl(PERI_CRG_MMC_CLK);
	reg |= CRG_DLL_SRST_REQ;
	writel(reg, PERI_CRG_MMC_CLK);
	udelay(25); /* delay 25us */

	/* eMMC reset deassert */
	reg = readl(PERI_CRG_MMC_CLK);
	reg &= ~CRG_DLL_SRST_REQ;
	writel(reg, PERI_CRG_MMC_CLK);
	udelay(1); /* delay 1us */
}

static int emmc_hardware_init(void)
{
	emmc_pin_mux_config();
	if (!get_ocr_from_bootrom())
		bsp_mmc_crg_init();
	return 0;
}

static int sd_hardware_init(void)
{
	printf("sdhci-bspsi: sd card not support, should not be here !\n");
	return 0;
}
