// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */
#ifdef CONFIG_TARGET_SS528V100
#include <asm/arch-ss528v100/platform.h>
#endif
#ifdef CONFIG_TARGET_SS625V100
#include <asm/arch-ss625v100/platform.h>
#endif
#include <dm.h>
#include <usb.h>
#include <usb/xhci.h>
#include "phy-usb.h"

static uintptr_t xhci_base = 0;

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	if (index == 0)
		xhci_base = USB3_CTRL_REG_BASE;
	if (index == 1)
		xhci_base = USB2_CTRL_REG_BASE;

	*hccr = (struct xhci_hccr *)(xhci_base);
	*hcor = (struct xhci_hcor *)((uintptr_t)*hccr
			+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void usb2_crg_config(void)
{
	unsigned int reg;

	/* ctrl1 rst*/
	reg = readl(USB2_CTRL1_CFG);
	reg |= USB2_1_SRST_REQ;
	writel(reg, USB2_CTRL1_CFG);
	udelay(U_LEVEL6);

	/* usb2 phy1 rst */
	reg = readl(USB2_PHY1_CFG);
	reg |= (USB2_PHY1_REQ | USB2_PHY1_TREQ | USB2_PHY1_APB_SRST_REQ);
	writel(reg, USB2_PHY1_CFG);
	udelay(U_LEVEL6);

	/* open usb2 phy1 clk */
	reg = readl(USB2_PHY1_CFG);
	reg |= USB2_PHY1_XTAL_CKEN;
	writel(reg, USB2_PHY1_CFG);
	udelay(U_LEVEL6);

	/* cancel usb2 phy1 rst */
	reg = readl(USB2_PHY1_CFG);
	reg &= ~(USB2_PHY1_REQ | USB2_PHY1_TREQ | USB2_PHY1_APB_SRST_REQ);
	writel(reg, USB2_PHY1_CFG);
	mdelay(U_LEVEL10);

	/* open utmi/ref/bus clk */
	reg = readl(USB2_CTRL1_CFG);
	reg |= USB2_1_BUS_CKEN;
	reg |= USB2_1_REF_CKEN;
	reg |= USB2_1_UTMI_CKEN;
	writel(reg, USB2_CTRL1_CFG);
	udelay(U_LEVEL6);

	/* cancel ctrl1 rst*/
	reg = readl(USB2_CTRL1_CFG);
	reg &= ~USB2_1_SRST_REQ;
	writel(reg, USB2_CTRL1_CFG);
	udelay(U_LEVEL6);
}

static void usb2_phy_eye_config(void)
{
	unsigned int reg;
	unsigned int trim_val;

	/* adeust the hstx mbias deen de */
	reg = readl(USB2_PHY1_BASE + U2_ANA_CFG0);
	reg = ana_cfg0_val(reg);
	reg |= U2_1_HSTX_MBIAS;
	writel(reg, USB2_PHY1_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);

#ifdef CONFIG_TARGET_SS528V100
	reg = readl(USB2_PHY1_BASE + U2_ANA_CFG0);
	reg |= U2_1_HSTX_DEEN;
	reg &= ~HSTX_DE_MASK;
	reg |= U2_1_HSTX_DE;
	writel(reg, USB2_PHY1_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);
#endif

	/* vdiscref sel and test tx set */
	reg = readl(USB2_PHY1_BASE + U2_ANA_CFG2);
	reg = ana_cfg2_val(reg);
	reg |= U2_1_VDISCREF_SEL;
	reg |= U2_TEST_TX;
#ifdef CONFIG_TARGET_SS528V100
	reg |= U2_TEST_TX_HALT_DEEN;
#endif
	writel(reg, USB2_PHY1_BASE + U2_ANA_CFG2);
	udelay(U_LEVEL5);

	/* OTP usb2 phy1*/
	trim_val = readl(USB_SYS_CTRL_BASE);
	trim_val = usb2_1_trim_val(trim_val);
	if ((trim_val >= U2_TRIM_VAL_MIN) && (trim_val <= U2_TRIM_VAL_MAX)) {
		reg = readl(USB2_PHY1_BASE + U2_ANA_CFG2);
		reg = usb2_rt_trim_clr(reg);
		reg |= usb2_rt_trim_set(trim_val);
		writel(reg, USB2_PHY1_BASE + U2_ANA_CFG2);
		udelay(U_LEVEL5);
	}

	/* ATOP TEST bit */
	reg = readl(USB2_PHY1_BASE + U2_ANA_CFG3);
	reg = ana_cfg3_val(reg);
	reg |= U2_1_SLEW_RATE_OPT;
	writel(reg, USB2_PHY1_BASE + U2_ANA_CFG3);
	udelay(U_LEVEL5);

	/* vtxref sel==>430mV, enable fls edge mode */
	reg = readl(USB2_PHY1_BASE + U2_ANA_CFG4);
	reg = ana_cfg4_val(reg);
	reg |= U2_VTXREF_SEL;
	reg |= U2_FLS_EDGE_MODE;
	writel(reg, USB2_PHY1_BASE + U2_ANA_CFG4);
	udelay(U_LEVEL5);
}

void usb2_phy_config(void)
{
	unsigned int reg;

	/* usb2 phy1 pll enable */
	reg = readl(USB2_PHY1_BASE + PHY_PLL_OFFSET);
	reg |= PHY_PLL_ENABLE;
	writel(reg, USB2_PHY1_BASE + PHY_PLL_OFFSET);
	udelay(U_LEVEL5);
}

void usb3_crg_config(void)
{
	unsigned int reg;

	/* USB3 disable */
	reg = readl(MISC_REG_BASE + USB_CTRL6);
	reg |= U3_PORT_DISABLE;
	writel(reg, MISC_REG_BASE + USB_CTRL6);
	udelay(U_LEVEL6);

	/* usb2 phy0 rst */
	reg = readl(USB2_PHY0_CFG);
	reg |= (USB2_PHY0_REQ | USB2_PHY0_TREQ | USB2_PHY0_APB_SRST_REQ);
	writel(reg, USB2_PHY0_CFG);
	udelay(U_LEVEL6);

	/* usb2 phy2 rst */
	reg = readl(USB2_PHY2_CFG);
	reg |= (USB2_PHY2_REQ | USB2_PHY2_TREQ | USB2_PHY2_APB_SRST_REQ);
	writel(reg, USB2_PHY2_CFG);
	udelay(U_LEVEL6);

	/* open usb2 phy0 clk */
	reg = readl(USB2_PHY0_CFG);
	reg |= USB2_PHY0_XTAL_CKEN;
	writel(reg, USB2_PHY0_CFG);
	udelay(U_LEVEL6);

	/* open usb2 phy2 clk */
	reg = readl(USB2_PHY2_CFG);
	reg |= USB2_PHY2_XTAL_CKEN;
	writel(reg, USB2_PHY2_CFG);
	udelay(U_LEVEL6);

	/* cancel usb2 phy0 rst */
	reg = readl(USB2_PHY0_CFG);
	reg &= ~(USB2_PHY0_REQ | USB2_PHY0_TREQ | USB2_PHY0_APB_SRST_REQ);
	writel(reg, USB2_PHY0_CFG);
	mdelay(U_LEVEL10);

	/* cancel usb2 phy2 rst */
	reg = readl(USB2_PHY2_CFG);
	reg &= ~(USB2_PHY2_REQ | USB2_PHY2_TREQ | USB2_PHY2_APB_SRST_REQ);
	writel(reg, USB2_PHY2_CFG);
	mdelay(U_LEVEL10);

	/* open combphy0 clk */
	reg = readl(USB3_COMBPHY_CFG);
	reg |= COMBPHY0_REF_CKEN;
	writel(reg, USB3_COMBPHY_CFG);
	udelay(U_LEVEL6);

	/* cancel combphy0 rst */
	reg = readl(USB3_COMBPHY_CFG);
	reg &= ~COMBPHY0_SRST_REQ;
	writel(reg, USB3_COMBPHY_CFG);
	udelay(U_LEVEL5);

	/* open pipe/suspend/ref/bus clk */
	reg = readl(USB3_PHY_CFG);
	reg |= USB3_BUS_CKEN;
	reg |= USB3_REF_CKEN;
	reg |= USB3_SUSPEND_CKEN;
	reg |= USB3_UTMI_CKEN;
	reg |= USB3_PIPE_CKEN;
	writel(reg, USB3_PHY_CFG);
	udelay(U_LEVEL6);

	/* open utmi clk */
	reg = readl(USB2_CTRL0_CFG);
	reg |= USB2_0_UTMI_CKEN;
	writel(reg, USB2_CTRL0_CFG);
	udelay(U_LEVEL6);

	/* cancel ctrl0 rst */
	reg = readl(USB3_PHY_CFG);
	reg &= ~USB3_SRST_REQ;
	writel(reg, USB3_PHY_CFG);
	udelay(U_LEVEL6);
}

static void usb2_phy0_eye_config(void)
{
	unsigned int reg;
	unsigned int trim_val;

	/* adjust the hstx mbias deen de */
	reg = readl(USB2_PHY0_BASE + U2_ANA_CFG0);
	reg = ana_cfg0_val(reg);
	reg |= U2_0_HSTX_MBIAS;
	writel(reg, USB2_PHY0_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);

#ifdef CONFIG_TARGET_SS528V100
	reg = readl(USB2_PHY0_BASE + U2_ANA_CFG0);
	reg |= U2_0_HSTX_DEEN;
	reg &= ~HSTX_DE_MASK;
	reg |= U2_0_HSTX_DE;
	writel(reg, USB2_PHY0_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);
#endif

	/* vdiscref sel and test tx set */
	reg = readl(USB2_PHY0_BASE + U2_ANA_CFG2);
	reg = ana_cfg2_val(reg);
	reg |= U2_0_VDISCREF_SEL;
	reg |= U2_TEST_TX;
#ifdef CONFIG_TARGET_SS528V100
	reg |= U2_TEST_TX_HALT_DEEN;
#endif
	writel(reg, USB2_PHY0_BASE + U2_ANA_CFG2);
	udelay(U_LEVEL5);

	/* OTP usb2 phy0*/
	trim_val = readl(USB_SYS_CTRL_BASE);
	trim_val = usb2_0_trim_val(trim_val);
	if ((trim_val >= U2_TRIM_VAL_MIN) && (trim_val <= U2_TRIM_VAL_MAX)) {
		reg = readl(USB2_PHY0_BASE + U2_ANA_CFG2);
		reg = usb2_rt_trim_clr(reg);
		reg |= usb2_rt_trim_set(trim_val);
		writel(reg, USB2_PHY0_BASE + U2_ANA_CFG2);
		udelay(U_LEVEL5);
	}

	/* ATOP TEST bit */
	reg = readl(USB2_PHY0_BASE + U2_ANA_CFG3);
	reg = ana_cfg3_val(reg);
	reg |= U2_0_SLEW_RATE_OPT;
	writel(reg, USB2_PHY0_BASE + U2_ANA_CFG3);
	udelay(U_LEVEL5);

	/* vtxref sel==>430mV, enable fls edge mode */
	reg = readl(USB2_PHY0_BASE + U2_ANA_CFG4);
	reg = ana_cfg4_val(reg);
	reg |= U2_VTXREF_SEL;
	reg |= U2_FLS_EDGE_MODE;
	writel(reg, USB2_PHY0_BASE + U2_ANA_CFG4);
	udelay(U_LEVEL5);
}

void usb2_phy0_config(void)
{
	unsigned int reg;

	/* usb2 phy0 pll enable */
	reg = readl(USB2_PHY0_BASE + PHY_PLL_OFFSET);
	reg |= PHY_PLL_ENABLE;
	writel(reg, USB2_PHY0_BASE + PHY_PLL_OFFSET);
	udelay(U_LEVEL5);
}

static void usb2_phy2_eye_config(void)
{
	unsigned int reg;
	unsigned int trim_val;

	/* adjust the hstx mbias deen de */
	reg = readl(USB2_PHY2_BASE + U2_ANA_CFG0);
	reg = ana_cfg0_val(reg);
	reg |= U2_2_HSTX_MBIAS;
	writel(reg, USB2_PHY2_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);

#ifdef CONFIG_TARGET_SS528V100
	reg = readl(USB2_PHY2_BASE + U2_ANA_CFG0);
	reg |= U2_2_HSTX_DEEN;
	reg &= ~HSTX_DE_MASK;
	reg |= U2_2_HSTX_DE;
	writel(reg, USB2_PHY2_BASE + U2_ANA_CFG0);
	udelay(U_LEVEL5);
#endif

	/* vdiscref sel and test tx set */
	reg = readl(USB2_PHY2_BASE + U2_ANA_CFG2);
	reg = ana_cfg2_val(reg);
	reg |= U2_2_VDISCREF_SEL;
	reg |= U2_TEST_TX;
#ifdef CONFIG_TARGET_SS528V100
	reg |= U2_TEST_TX_HALT_DEEN;
#endif
	writel(reg, USB2_PHY2_BASE + U2_ANA_CFG2);
	udelay(U_LEVEL5);

	/* OTP usb2 phy2*/
	trim_val = readl(USB_SYS_CTRL_BASE);
	trim_val = usb2_2_trim_val(trim_val);
	if ((trim_val >= U2_TRIM_VAL_MIN) && (trim_val <= U2_TRIM_VAL_MAX)) {
		reg = readl(USB2_PHY2_BASE + U2_ANA_CFG2);
		reg = usb2_rt_trim_clr(reg);
		reg |= usb2_rt_trim_set(trim_val);
		writel(reg, USB2_PHY2_BASE + U2_ANA_CFG2);
		udelay(U_LEVEL5);
	}

	/* ATOP TEST bit */
	reg = readl(USB2_PHY2_BASE + U2_ANA_CFG3);
	reg = ana_cfg3_val(reg);
	reg |= U2_2_SLEW_RATE_OPT;
	writel(reg, USB2_PHY2_BASE + U2_ANA_CFG3);
	udelay(U_LEVEL5);

	reg = readl(USB2_PHY2_BASE + U2_ANA_CFG4);
	reg = ana_cfg4_val(reg);
	reg |= U2_VTXREF_SEL;
	reg |= U2_FLS_EDGE_MODE;
	writel(reg, USB2_PHY2_BASE + U2_ANA_CFG4);
	udelay(U_LEVEL5);
}

void usb2_phy2_config(void)
{
	unsigned int reg;

	/* usb2 phy2 pll enable */
	reg = readl(USB2_PHY2_BASE + PHY_PLL_OFFSET);
	reg |= PHY_PLL_ENABLE;
	writel(reg, USB2_PHY2_BASE + PHY_PLL_OFFSET);
	udelay(U_LEVEL5);
}

void usb3_phy_config(void)
{
	usb2_phy0_config();
	usb2_phy0_eye_config();
	usb2_phy2_config();
	usb2_phy2_eye_config();

	/* PI_CURRENT_TRIM ==>2'b00 to 2'b01 */
	writel(PI_CURRENT_TRIM_ENABLE, USB_MISC_REG_BASE);
	writel(PI_CURRENT_TRIM_VAL, USB_MISC_REG_BASE);
	writel(PI_CURRENT_TRIM_ENABLE, USB_MISC_REG_BASE);
	udelay(U_LEVEL5);

	/* TX_SWING_COMP ==>4'b1000 to 4b'1100 */
	writel(TX_SWING_COMP_ENABLE, USB_MISC_REG_BASE);
	writel(TX_SWING_COMP_VAL, USB_MISC_REG_BASE);
	writel(TX_SWING_COMP_ENABLE, USB_MISC_REG_BASE);
	udelay(U_LEVEL5);
}

void phy_usb_init(int index)
{
	unsigned long flags;

	local_irq_save(flags);

	switch (index) {
	case 0:
		usb3_crg_config();
		usb3_phy_config();
		break;
	case 1:
		usb2_crg_config();
		usb2_phy_config();
		usb2_phy_eye_config();
		break;
	default:
			break;
	}

	local_irq_restore(flags);
}

void usb2_disable(void)
{
	unsigned int reg;

	/* ctrl1 rst */
	reg = readl(USB2_CTRL1_CFG);
	reg |= USB2_1_SRST_REQ;
	writel(reg, USB2_CTRL1_CFG);
	udelay(U_LEVEL6);

	/* usb2 phy1 rst */
	reg = readl(USB2_PHY1_CFG);
	reg |= (USB2_PHY1_REQ | USB2_PHY1_TREQ);
	writel(reg, USB2_PHY1_CFG);
	udelay(U_LEVEL6);
}

void usb3_disable(void)
{
	unsigned int reg;

	/* ctrl0 rst */
	reg = readl(USB3_PHY_CFG);
	reg |= USB3_SRST_REQ;
	writel(reg, USB3_PHY_CFG);
	udelay(U_LEVEL6);

	/* usb2 phy0 rst */
	reg = readl(USB2_PHY0_CFG);
	reg |= (USB2_PHY0_REQ | USB2_PHY0_TREQ);
	writel(reg, USB2_PHY0_CFG);
	udelay(U_LEVEL6);

	/* usb2 phy2 rst */
	reg = readl(USB2_PHY2_CFG);
	reg |= (USB2_PHY2_REQ | USB2_PHY2_TREQ);
	writel(reg, USB2_PHY2_CFG);
	udelay(U_LEVEL6);
}

void xhci_hcd_stop(int index)
{
	switch (index) {
	case 0:
		usb3_disable();
		break;
	case 1:
		usb2_disable();
		break;
	default:
		break;
	}
}
