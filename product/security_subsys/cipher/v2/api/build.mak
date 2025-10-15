cflags-y     += -I$(CIPHER_BASE_DIR)/api

obj-y   += $(CIPHER_PREFIX)/api/ree_mpi_cipher.o
obj-y   += $(CIPHER_PREFIX)/api/ree_mpi_hash.o
obj-y   += $(CIPHER_PREFIX)/api/ree_mpi_rsa.o
obj-y   += $(CIPHER_PREFIX)/api/ree_mpi_rng.o
