# INTER_DRV defined before include arch/build.mak
ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss919v100" "ss319v100" "ss015v100"))
INTER_DRV := SS919V100
else ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss918v100" "ss318v100" "ss013v100"))
INTER_DRV := SS918V100
else ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss812v100" "ss813v100" "ss815v100" "ss312v100" \
                                                                  "ss313v100" "ss011v100" "ss012v100"))
INTER_DRV := SS812V100
else ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss101v200" "ss101v500" "ss101v600" "ss101v300"))
INTER_DRV := SS101V200
else ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss528v100" "ss625v100"))
INTER_DRV := SS528V100
else ifeq ($(CONFIG_PRODUCTNAME), $(filter $(CONFIG_PRODUCTNAME), "ss524v100" "ss522v101" "ss522v100" "ss615v100"))
INTER_DRV := SS524V100
endif

CIPHER_BASE_DIR := $(srctree)/product/security_subsys/cipher/v2
CIPHER_PREFIX := v2

cflags-y += -DCHIP_TYPE_$(INTER_DRV)
cflags-y += -I$(srctree)/product/security_subsys/ext_inc

include $(CIPHER_BASE_DIR)/api/build.mak
include $(CIPHER_BASE_DIR)/drv/build.mak

ccflags-y  += $(cflags-y)
HOSTCFLAGS += $(cflags-y)
CPPFLAGS   += $(cflags-y)
