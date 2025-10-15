#DRV_PATH := $(srctree)/product/ot_osd/hdmi

HDMI_VER := hdmi_2_0
ifeq ($(CONFIG_PRODUCTNAME), "ss919v100")
CHIP_TYPE := ss919v100
CTRL      := v200
PHY       := v200
cflags-y  += -DHDMI_PRODUCT_SS919V100
cflags-y  += -DCHIP_SS919V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss015v100")
CHIP_TYPE := ss919v100
CTRL      := v200
PHY       := v200
cflags-y  += -DHDMI_PRODUCT_SS919V100
cflags-y  += -DCHIP_SS919V100

else ifeq ($(CONFIG_PRODUCTNAME), "ss918v100")
CHIP_TYPE := ss918v100
CTRL      := v100
PHY       := v200
cflags-y  += -DHDMI_PRODUCT_SS918V100
cflags-y  += -DCHIP_SS918V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss318v100")
CHIP_TYPE := ss918v100
CTRL      := v100
PHY       := v200
cflags-y  += -DHDMI_PRODUCT_SS918V100
cflags-y  += -DCHIP_SS918V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss013v100")
CHIP_TYPE := ss918v100
CTRL      := v100
PHY       := v200
cflags-y  += -DHDMI_PRODUCT_SS918V100
cflags-y  += -DCHIP_SS918V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss813v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss313v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss312v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss011v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss012v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss815v100")
CHIP_TYPE := ss812v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS812V100
cflags-y  += -DCHIP_TYPE_SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), "ss528v100")
VO_ARCH_NAME = ss528v100
VO_SUB_ARCH_NAME = ss528v100
CHIP_TYPE := ss528v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS528V100
cflags-y  += -DCHIP_TYPE_SS528V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
else ifeq ($(CONFIG_PRODUCTNAME), "ss625v100")
VO_ARCH_NAME = ss528v100
VO_SUB_ARCH_NAME = ss625v100
CHIP_TYPE := ss528v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS625V100
cflags-y  += -DCHIP_TYPE_SS625V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
cflags-y  += -DHDMI_SUPPORT_1_4
else ifeq ($(CONFIG_PRODUCTNAME), "ss524v100")
VO_ARCH_NAME = ss524v100
CHIP_TYPE := ss524v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS524V100
cflags-y  += -DCHIP_TYPE_SS524V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
cflags-y  += -DHDMI_SUPPORT_1_4
else ifeq ($(CONFIG_PRODUCTNAME), "ss522v101")
VO_ARCH_NAME = ss524v100
CHIP_TYPE := ss524v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS524V100
cflags-y  += -DCHIP_TYPE_SS524V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
cflags-y  += -DHDMI_SUPPORT_1_4
else ifeq ($(CONFIG_PRODUCTNAME), "ss522v100")
VO_ARCH_NAME = ss524v100
CHIP_TYPE := ss524v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS524V100
cflags-y  += -DCHIP_TYPE_SS524V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
cflags-y  += -DHDMI_SUPPORT_1_4
else ifeq ($(CONFIG_PRODUCTNAME), "ss615v100")
VO_ARCH_NAME = ss615v100
CHIP_TYPE := ss524v100
CTRL      := v100
PHY       := v100
cflags-y  += -DHDMI_PRODUCT_SS615V100
cflags-y  += -DCHIP_TYPE_SS615V100
cflags-y  += -DPHY_CRAFT_S28
cflags-y  += -DHDMI_PLATFORM_FMT
cflags-y  += -DHDMI_SUPPORT_1_4
else ifeq ($(CONFIG_PRODUCTNAME), "ss928v100")
VO_ARCH_NAME = ss928v100
VO_SUB_ARCH_NAME = ss928v100
CHIP_TYPE := ss928v100
CTRL      := v100
PHY       := v300
cflags-y  += -DHDMI_PRODUCT_SS928V100
cflags-y  += -DCHIP_TYPE_ss928v100
cflags-y  += -DHDMI_PLATFORM_FMT
else ifeq ($(CONFIG_PRODUCTNAME), "ss927v100")
VO_ARCH_NAME = ss928v100
VO_SUB_ARCH_NAME = ss928v100
CHIP_TYPE := ss928v100
CTRL      := v100
PHY       := v300
cflags-y  += -DHDMI_PRODUCT_SS928V100
cflags-y  += -DCHIP_TYPE_ss928v100
cflags-y  += -DHDMI_PLATFORM_FMT
endif

CTRL_DIR    := $(srctree)/hal/ctrl/$(CTRL)
PHY_DIR     := $(srctree)/hal/phy/$(PHY)
PRODUCT_DIR := $(srctree)/product/$(CHIP_TYPE)

cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal/ctrl/$(CTRL)
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal/ctrl/$(CTRL)/regs
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal/emi
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal/phy/$(PHY)
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/hal/phy/$(PHY)/regs
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/product/$(CHIP_TYPE)
cflags-y     += -I$(srctree)/product/ot_osd/hdmi/$(HDMI_VER)/drv/product/$(CHIP_TYPE)/regs
cflags-y     += -I$(srctree)/include
cflags-y     += -I$(PRODUCT_DIR)
cflags-y     += -I$(PRODUCT_DIR)/regs
cflags-y     += -I$(srctree)/product/ot_osd/vo/arch/$(VO_ARCH_NAME)/include
cflags-y     += -I$(srctree)/product/ot_osd/vo/arch/include
cflags-y     += -I$(srctree)/product/ot_osd/vo/include
cflags-y     += -I$(srctree)/product/ot_osd/vo/arch/$(VO_ARCH_NAME)/include/$(VO_SUB_ARCH_NAME)
cflags-y     += -I$(srctree)/product/ot_osd/include

cflags-y += -DHDMI_SUPPORT_LOGIC_V100
cflags-y += -DCONFIG_HDMI_BSP_SDK -DDVI_SUPPORT
cflags-y += -DDVI_SUPPORT
cflags-y += -DHDMI_SCDC_SUPPORT

ifeq ($(FPGA),y)
cflags-y  += -DHDMI_FPGA_SUPPORT
endif

ccflags-y  += $(cflags-y)
HOSTCFLAGS += $(cflags-y)
CPPFLAGS   += $(cflags-y)

#HDMI_EMI_ENABLE

OBJ    := ../boot_hdmi_intf.o
OBJ    += drv_hdmi_common.o
OBJ    += drv_hdmi_infoframe.o
OBJ    += $(CTRL_DIR)/hdmi_hal_intf.o

OBJ    +=   $(CTRL_DIR)/regs/hdmi_reg_aon.o \
            $(CTRL_DIR)/regs/hdmi_reg_ctrl.o \
            $(CTRL_DIR)/regs/hdmi_reg_tx.o \
            $(CTRL_DIR)/regs/hdmi_reg_video_path.o

OBJ += $(PRODUCT_DIR)/regs/hdmi_reg_crg.o

OBJ    +=   $(CTRL_DIR)/hdmi_hal_intf.o \
            $(CTRL_DIR)/hdmi_hal_ddc.o \
            $(CTRL_DIR)/hdmi_hal_ctrl.o \
            $(CTRL_DIR)/hdmi_hal_scdc.o

OBJ += $(PHY_DIR)/hdmi_hal_phy.o
OBJ += $(PHY_DIR)/regs/hdmi_reg_dphy.o
OBJ += $(PRODUCT_DIR)/hdmi_product_define.o

obj-y += $(OBJ)

#########################################################################
