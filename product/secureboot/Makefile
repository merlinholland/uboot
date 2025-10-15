sinclude $(TOPDIR)/config.mk

CONFIG_SECUREBOOT_DEBUG_ENABLE	:= n
CONFIG_SECUREBOOT_TIMER_ENABLE	:= n

# Debug
ifeq ($(CONFIG_SECUREBOOT_DEBUG_ENABLE), y)
cflags-y	+= -DCONFIG_SECUREBOOT_DEBUG_ENABLE
endif
ifeq ($(CONFIG_SECUREBOOT_TIMER_ENABLE), y)
cflags-y	+= -DCONFIG_SECUREBOOT_TIMER_ENABLE
endif

cflags-y    += -I$(srctree)/product/secureboot
cflags-y    += -I$(srctree)/product/secureboot/include
cflags-y    += -I$(srctree)/product/secureboot/cipher
cflags-y    += -I$(srctree)/product/secureboot/hal
cflags-y    += -I$(srctree)/product/secureboot/vbmeta
cflags-y    += -I$(srctree)/product/secureboot/user
cflags-y    += -I$(srctree)/product/secureboot/utils
cflags-y    += -I$(srctree)/product/security_subsys/cipher/v2/api
cflags-y    += -I$(srctree)/product/security_subsys/ext_inc
ccflags-y   += $(cflags-y)

obj-$(CONFIG_SECURE_BOOT_SUPPORT) += sec_boot.o

obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./cipher/cipher_adaptor.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./hal/otp_user_hal.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./vbmeta/vbmeta.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./vbmeta/x509_parser.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./user/user_adaptor.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./utils/rsa_public.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += ./command_debug.o
obj-$(CONFIG_SECURE_BOOT_SUPPORT) += secureboot_log.o
