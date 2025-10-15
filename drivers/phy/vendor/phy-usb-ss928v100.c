/*
 * Copyright (c) 2017 Shenshu Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* Attention: For ss928v100 USB3_CTRL_REG_BASE is the drd controler with
 * base address: 0x10320000 ; USB3_CTRL_REG_BASE_1 is the host-only controler
 * with base address: 0x10300000
 */
#include <asm/arch-ss928v100/platform.h>
#include <dm.h>
#include <usb.h>
#include <usb/xhci.h>
#include "phy-usb.h"
#define PINOUT_REG_BASE					(0x10230000)
#define PITOUT_CTRL0_PWREN_OFFSET		(0x44)
#define PITOUT_CTRL1_PWREN_OFFSET		(0X3C)
#define PITOUT_CTRL1_VBUS_OFFSET		(0x38)
#define PINOUT_USB_VAL					(0x1201)

#define SYS_STAT				0x18
#define PCIE_USB3_MODE			(0x3<<0)
#define PCIE_USB3_MODE_OFFSET	16
#define PCIE_X2					0x0
#define USB3_MODE				0x1
#define PORT0U2_PORT1U3_MODE	0x2

#define USB2_PHY_PLLCK_ADDR_OFFSET	0x14
#define USB2_PHY_PLLCK_MASK			0x00000003
#define USB2_PHY_PLLCK_VAL			((0x3 << 0) & USB2_PHY_PLLCK_MASK)

#define USB3_GUSB2PHYCFGN	0xc200
#define USB3_SUSPENDUSB20_PHY		(0x1 << 6)

#define ANA_CFG0_OFFSET					(0x0)
#define TX_DEEMPHASIS_ENABLE			(0x1 << 5)
#define TX_DEEMPHASIS_STRENGTH_MASK		(0xF << 8)
#define TX_DEEMPHASIS_STRENGTH_VAL		(0xC << 8)
#define MBIAS_MASK						(0xF << 0)
#define MBIAS_VAL						(0xB << 0)
#define ANA_CFG2_OFFSET					(0x8)
#define DEEMPHASIS_HALF_BIT_MASK		(0xFF << 20)
#define DEEMPHASIS_HALF_BIT_VAL			(0x2 << 20)
#define DISCONNECT_VREF_MASK			(0x7 << 16)
#define DISCONNECT_VREF_VAL				(0x6 << 16)

static long ctrl_base = 0;
static long u2_phy_base = 0;
static long ctrl_crg_base = 0;
static long u2_phy_crg_base = 0;
static long u3_phy_crg_base = 0;
int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	if (index == 0)
		ctrl_base = USB3_CTRL_REG_BASE;
	if (index == 1)
		ctrl_base = USB3_CTRL_REG_BASE_1;

	*hccr = (struct xhci_hccr *)(ctrl_base);
	*hcor = (struct xhci_hcor *)((long) *hccr
			+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void usb3_eye_config(void)
{
	int reg;

	reg = readl(USB2_PHY_BASE + ANA_CFG0_OFFSET);
	reg |= TX_DEEMPHASIS_ENABLE;
	reg &= ~(TX_DEEMPHASIS_STRENGTH_MASK | MBIAS_MASK);
	reg |= (TX_DEEMPHASIS_STRENGTH_VAL | MBIAS_VAL);
	writel(reg, USB2_PHY_BASE + ANA_CFG0_OFFSET);
	udelay(U_LEVEL6);

	reg = readl(USB2_PHY_BASE + ANA_CFG2_OFFSET);
	reg &= ~(DEEMPHASIS_HALF_BIT_MASK | DISCONNECT_VREF_MASK);
	reg |= (DEEMPHASIS_HALF_BIT_VAL | DISCONNECT_VREF_VAL);
	writel(reg, USB2_PHY_BASE + ANA_CFG2_OFFSET);
	udelay(U_LEVEL6);

	reg = readl(USB2_PHY_BASE_1 + ANA_CFG0_OFFSET);
	reg |= TX_DEEMPHASIS_ENABLE;
	reg &= ~(TX_DEEMPHASIS_STRENGTH_MASK | MBIAS_MASK);
	reg |= (TX_DEEMPHASIS_STRENGTH_VAL | MBIAS_VAL);
	writel(reg, USB2_PHY_BASE_1 + ANA_CFG0_OFFSET);
	udelay(U_LEVEL6);

	reg = readl(USB2_PHY_BASE_1 + ANA_CFG2_OFFSET);
	reg &= ~(DEEMPHASIS_HALF_BIT_MASK | DISCONNECT_VREF_MASK);
	reg |= (DEEMPHASIS_HALF_BIT_VAL | DISCONNECT_VREF_VAL);
	writel(reg, USB2_PHY_BASE_1 + ANA_CFG2_OFFSET);
	udelay(U_LEVEL6);
}

void phy_usb_init(int index)
{
	unsigned long flags;
	unsigned int reg, cbp_mode;

	writel(PINOUT_USB_VAL, PINOUT_REG_BASE + PITOUT_CTRL0_PWREN_OFFSET);
	writel(PINOUT_USB_VAL, PINOUT_REG_BASE + PITOUT_CTRL1_PWREN_OFFSET);
	writel(PINOUT_USB_VAL, PINOUT_REG_BASE + PITOUT_CTRL1_VBUS_OFFSET);
	udelay(U_LEVEL6);

	local_irq_save(flags);

	if (index == 0) {
		ctrl_base = USB3_CTRL_REG_BASE;
		u2_phy_base = USB2_PHY_BASE;
		ctrl_crg_base = USB3_CTRL_CRG;
		u2_phy_crg_base = USB2_PHY_CRG;
		u3_phy_crg_base = USB3_PHY_CRG;
	} else if (index == 1) {
		ctrl_base = USB3_CTRL_REG_BASE_1;
		u2_phy_base = USB2_PHY_BASE_1;
		ctrl_crg_base = USB3_CTRL_CRG_1;
		u2_phy_crg_base = USB2_PHY_CRG_1;
		u3_phy_crg_base = USB3_PHY_CRG_1;
	} else {
		return;
	}

	/* judgement pcie usb3 mode */
	cbp_mode = readl(SYS_CTRL_REG_BASE + SYS_STAT);
	cbp_mode = cbp_mode >> PCIE_USB3_MODE_OFFSET;
	cbp_mode &= PCIE_USB3_MODE;

	/* write default crg value */
	writel(USB3_CTRL_CRG_DEFAULT_VALUE, ctrl_crg_base);
	writel(USB2_PHY_CRG_DEFAULT_VALUE, u2_phy_crg_base);
	writel(USB3_PHY_CRG_DEFAULT_VALUE, u3_phy_crg_base);
	udelay(U_LEVEL6);

	/* phy crg setting */
	reg = readl(u2_phy_crg_base);
	reg &= ~(USB2_PHY_CRG_APB_SREQ);
	writel(reg, u2_phy_crg_base);
	udelay(U_LEVEL6);

	/* ctrl crg setting */
	/* usb3 occ pclk sel */
	reg = readl(ctrl_crg_base);
	if (cbp_mode == PCIE_X2)
		reg |= USB3_CRG_PCLK_OCC_SEL;
	else if (cbp_mode == PORT0U2_PORT1U3_MODE && index == 1)
		reg |= USB3_CRG_PCLK_OCC_SEL;
	else
		reg &= ~(USB3_CRG_PCLK_OCC_SEL);

	reg |= (USB3_CRG_PIPE_CKEN |
			USB3_CRG_UTMI_CKEN |
			USB3_CRG_SUSPEND_CKEN |
			USB3_CRG_REF_CKEN |
			USB3_CRG_BUS_CKEN);
	writel(reg, ctrl_crg_base);
	udelay(U_LEVEL6); // delay 200us

	/* U3 phy TPOR &POR reset */
	reg = readl(u3_phy_crg_base);
	reg &= ~(USB3_PHY_CRG_TREQ | USB3_PHY_CRG_REQ);
	writel(reg, u3_phy_crg_base);
	udelay(U_LEVEL6); // delay 200us

	/* U2 phy POR reset */
	reg = readl(u2_phy_crg_base);
	reg &= ~USB2_PHY_CRG_REQ;
	writel(reg, u2_phy_crg_base);

	reg = readl(u2_phy_base + USB2_PHY_PLLCK_ADDR_OFFSET);
	reg &= ~USB2_PHY_PLLCK_MASK;
	reg |= USB2_PHY_PLLCK_VAL;
	writel(reg, u2_phy_base + USB2_PHY_PLLCK_ADDR_OFFSET);
	udelay(U_LEVEL10); // delay 2ms

	/* U2 phy TPOR reset */
	reg = readl(u2_phy_crg_base);
	reg &= ~USB2_PHY_CRG_TREQ;
	writel(reg, u2_phy_crg_base);
	udelay(U_LEVEL6); // delay 200us

	/* ctrl crg reset release*/
	reg = readl(ctrl_crg_base);
	reg &= ~USB3_CRG_SRST_REQ;
	writel(reg, ctrl_crg_base);
	udelay(U_LEVEL6); // delay 200us

	/* CTRL set */
	reg = readl(ctrl_base + USB3_GUSB2PHYCFGN);
	if (cbp_mode == PCIE_X2)
		reg &= ~(USB3_SUSPENDUSB20_PHY);
	else if (cbp_mode == PORT0U2_PORT1U3_MODE && index == 1)
		reg &= ~(USB3_SUSPENDUSB20_PHY);
	else
		reg |= (USB3_SUSPENDUSB20_PHY);
	writel(reg, ctrl_base + USB3_GUSB2PHYCFGN);
	udelay(U_LEVEL6);

	/* Host mode */
	reg = readl(ctrl_base + REG_GUSB3PIPECTL0);
	reg |= PCS_SSP_SOFT_RESET;
	writel(reg, ctrl_base + REG_GUSB3PIPECTL0);
	udelay(U_LEVEL6);
	reg = readl(ctrl_base + REG_GCTL);
	reg &= ~PORT_CAP_DIR_MASK;
	reg |= PORT_CAP_DIR_HOST;
	writel(reg, ctrl_base + REG_GCTL);
	udelay(U_LEVEL6); // delay 200us

	reg = readl(ctrl_base + REG_GUSB3PIPECTL0);
	reg &= ~PCS_SSP_SOFT_RESET;
	reg &= ~SUSPEND_ENABLE;
	writel(reg, ctrl_base + REG_GUSB3PIPECTL0);
	udelay(U_LEVEL6); // delay 200us

	reg &= CLEAN_USB3_GTXTHRCFG;
	reg |= USB_TXPKT_CNT_SEL;
	reg |= USB_TXPKT_CNT;
	reg |= USB_MAXTX_BURST_SIZE;
	writel(reg, ctrl_base + GTXTHRCFG);
	udelay(U_LEVEL6); // delay 200us
	writel(reg, ctrl_base + GRXTHRCFG);
	udelay(U_LEVEL6); // delay 200us

	usb3_eye_config();

	local_irq_restore(flags);
}

void xhci_hcd_stop(int index)
{
	if (index == 0) {
		ctrl_base = USB3_CTRL_REG_BASE;
		u2_phy_base = USB2_PHY_BASE;
		ctrl_crg_base = USB3_CTRL_CRG;
		u2_phy_crg_base = USB2_PHY_CRG;
		u3_phy_crg_base = USB3_PHY_CRG;
	} else if (index == 1) {
		ctrl_base = USB3_CTRL_REG_BASE_1;
		u2_phy_base = USB2_PHY_BASE_1;
		ctrl_crg_base = USB3_CTRL_CRG_1;
		u2_phy_crg_base = USB2_PHY_CRG_1;
		u3_phy_crg_base = USB3_PHY_CRG_1;
	} else {
		return;
	}

	/* write default crg value */
	writel(USB3_CTRL_CRG_DEFAULT_VALUE, ctrl_crg_base);
	writel(USB2_PHY_CRG_DEFAULT_VALUE, u2_phy_crg_base);
	writel(USB3_PHY_CRG_DEFAULT_VALUE, u3_phy_crg_base);
	udelay(U_LEVEL6); // delay 200us
}
