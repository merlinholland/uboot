
OTP_CFLAGS += -I$(OTP_BASE_DIR)/arch/$(INTER_DRV)/

DRV_OBJS += arch/$(INTER_DRV)/otp_data.o

OT_OTP_VERSION := v100

# cfg cpu type, ree cpu maybe read tee data, tee cpu can operate ree data
OTP_CFLAGS += -DOT_OTP_REE_CPU
