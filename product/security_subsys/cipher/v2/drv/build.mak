cflags-y += -I$(CIPHER_BASE_DIR)/drv/platform
cflags-y += -I$(CIPHER_BASE_DIR)/drv/rng
cflags-y += -I$(CIPHER_BASE_DIR)/drv/rsa
cflags-y += -I$(CIPHER_BASE_DIR)/drv/compat
cflags-y += -I$(CIPHER_BASE_DIR)/drv/sm2
cflags-y += -I$(CIPHER_BASE_DIR)/drv/spacc
cflags-y += -I$(CIPHER_BASE_DIR)/drv/include

obj-y += $(CIPHER_PREFIX)/drv/platform/cipher_adapt.o
obj-y += $(CIPHER_PREFIX)/drv/drv_cipher_intf.o
obj-y += $(CIPHER_PREFIX)/drv/rng/drv_rng.o
obj-y += $(CIPHER_PREFIX)/drv/rsa/drv_rsa.o
obj-y += $(CIPHER_PREFIX)/drv/spacc/spacc_body.o
obj-y += $(CIPHER_PREFIX)/drv/spacc/spacc_intf.o
obj-y += $(CIPHER_PREFIX)/drv/compat/hal_otp.o
obj-y += $(CIPHER_PREFIX)/drv/compat/drv_klad.o
obj-y += $(CIPHER_PREFIX)/drv/compat/ot_drv_compat.o
