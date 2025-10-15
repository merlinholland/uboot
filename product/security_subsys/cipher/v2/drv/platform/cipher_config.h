// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#ifndef CIPHER_CONFIG_H
#define CIPHER_CONFIG_H

#define RSA_ENABLE
#define OTP_SUPPORT
#define CIPHER_HASH_SUPPORT
#define CIPHER_EFUSE_SUPPORT
#define CIPHER_KLAD_SUPPORT

#define CIPHER_IRQ_NUMBER                       59

#if defined(CHIP_TYPE_SS919V100)
#define CIPHER_RNG_REG_BASE_ADDR_PHY            0x10230000
#define CIPHER_ENFUSE_REG_BASE_ADDR_PHY         (0x10250000)
#define CIPHER_CIPHER_REG_BASE_ADDR_PHY         (0x10200000)
#define CIPHER_RSA_REG_BASE_ADDR_PHY            (0x10220000)

#define CIPHER_RSA_CRG_ADDR_PHY                 (0x12010194)
#define RSA_CRG_CLOCK_BIT                       (0x01 << 15)
#define RSA_CRG_RESET_BIT                       (0x01 << 14)
#define CIPHER_SPACC_CRG_ADDR_PHY               (0x1201016C)
#define SPACC_CRG_CLOCK_BIT                     (0x01 << 1)
#define SPACC_CRG_RESET_BIT                     (0x01 << 0)
#define CIPHER_RNG_CRG_ADDR_PHY                 (0x12010194)
#define RNG_CRG_CLOCK_BIT                       (0x01 << 13)
#define RNG_CRG_RESET_BIT                       (0x01 << 12)

#define CIPHER_KLAD_REG_BASE_ADDR_PHY           (0x10210000)
#define CIPHER_OTP_REG_BASE_ADDR_PHY            (0x10240000)
#define CIPHER_KLAD_CRG_ADDR_PHY                (0x12010194)

#define KLAD_CRG_CLOCK_BIT                      (0x01 << 11)
#define KLAD_CRG_RESET_BIT                      (0x01 << 10)
#elif defined(CHIP_TYPE_SS918V100)
#define CIPHER_RNG_REG_BASE_ADDR_PHY            (0x04090000)
#define CIPHER_CIPHER_REG_BASE_ADDR_PHY         (0x04060000)
#define CIPHER_RSA_REG_BASE_ADDR_PHY            (0x04088000)

#define CIPHER_RSA_CRG_ADDR_PHY                 (0x04510194)
#define RSA_CRG_CLOCK_BIT                       (0x01 << 23)
#define RSA_CRG_RESET_BIT                       (0x01 << 22)
#define CIPHER_SPACC_CRG_ADDR_PHY               (0x0451016c)
#define SPACC_CRG_CLOCK_BIT                     (0x01 << 1)
#define SPACC_CRG_RESET_BIT                     (0x01 << 0)
#define CIPHER_RNG_CRG_ADDR_PHY                 (0x04510194)
#define RNG_CRG_CLOCK_BIT                       (0x01 << 13)
#define RNG_CRG_RESET_BIT                       (0x01 << 12)

#define CIPHER_KLAD_REG_BASE_ADDR_PHY           (0x04070000)
#define CIPHER_OTP_REG_BASE_ADDR_PHY            (0x040A0000)
#define CIPHER_KLAD_CRG_ADDR_PHY                (0x04510194)

#define KLAD_CRG_CLOCK_BIT                      (0x01 << 11)
#define KLAD_CRG_RESET_BIT                      (0x01 << 10)
#elif defined(CHIP_TYPE_SS812V100)
#define CIPHER_RNG_REG_BASE_ADDR_PHY            (0x10090000)
#define CIPHER_CIPHER_REG_BASE_ADDR_PHY         (0x100C0000)
#define CIPHER_RSA_REG_BASE_ADDR_PHY            (0x100D0000)

#define CIPHER_RSA_CRG_ADDR_PHY                 (0x120101A0)
#define RSA_CRG_CLOCK_BIT                       (0x01 << 7)
#define RSA_CRG_RESET_BIT                       (0x01 << 6)
#define CIPHER_SPACC_CRG_ADDR_PHY               (0x120101A0)
#define SPACC_CRG_CLOCK_BIT                     (0x01 << 9)
#define SPACC_CRG_RESET_BIT                     (0x01 << 8)
#define CIPHER_RNG_CRG_ADDR_PHY                 (0x120101A0)
#define RNG_CRG_CLOCK_BIT                       (0x01 << 3)
#define RNG_CRG_RESET_BIT                       (0x01 << 2)

#define CIPHER_KLAD_REG_BASE_ADDR_PHY           (0x10070000)
#define CIPHER_OTP_REG_BASE_ADDR_PHY            (0x100B0000)
#define CIPHER_KLAD_CRG_ADDR_PHY                (0x120101A0)

#define KLAD_CRG_CLOCK_BIT                      (0x01 << 1)
#define KLAD_CRG_RESET_BIT                      (0x01 << 0)
#elif defined(CHIP_TYPE_SS101V200)
#define CIPHER_RNG_REG_BASE_ADDR_PHY            (0x10080000)
#define CIPHER_CIPHER_REG_BASE_ADDR_PHY         (0x10050000)
#define CIPHER_RSA_REG_BASE_ADDR_PHY            (0x10070000)

#define CIPHER_RSA_CRG_ADDR_PHY                 (0x120101A0)
#define RSA_CRG_CLOCK_BIT                       (0x01 << 5)
#define RSA_CRG_RESET_BIT                       (0x01 << 4)
#define CIPHER_SPACC_CRG_ADDR_PHY               (0x120101A0)
#define SPACC_CRG_CLOCK_BIT                     (0x01 << 9)
#define SPACC_CRG_RESET_BIT                     (0x01 << 8)
#define CIPHER_RNG_CRG_ADDR_PHY                 (0x120101A0)
#define RNG_CRG_CLOCK_BIT                       (0x01 << 3)
#define RNG_CRG_RESET_BIT                       (0x01 << 2)

#define CIPHER_KLAD_REG_BASE_ADDR_PHY           (0x10060000)
#define CIPHER_OTP_REG_BASE_ADDR_PHY            (0x10090000)
#define CIPHER_KLAD_CRG_ADDR_PHY                (0x120101A0)

#define KLAD_CRG_CLOCK_BIT                      (0x01 << 1)
#define KLAD_CRG_RESET_BIT                      (0x01 << 0)
#elif (defined(CHIP_TYPE_SS528V100) || defined(CHIP_TYPE_SS524V100))
#define CIPHER_RNG_REG_BASE_ADDR_PHY            (0x10130000)
#define CIPHER_CIPHER_REG_BASE_ADDR_PHY         (0x10100000)
#define CIPHER_RSA_REG_BASE_ADDR_PHY            (0x10120000)

#define CIPHER_RSA_CRG_ADDR_PHY                 (0x11012D00)
#define RSA_CRG_CLOCK_BIT                       (0x01 << 4)
#define RSA_CRG_RESET_BIT                       (0x01 << 0)
#define CIPHER_SPACC_CRG_ADDR_PHY               (0x11012C80)
#define SPACC_CRG_CLOCK_BIT                     (0x01 << 4)
#define SPACC_CRG_RESET_BIT                     (0x01 << 0)
#define CIPHER_RNG_CRG_ADDR_PHY                 (0x11012D80)
#define RNG_CRG_CLOCK_BIT                       (0x01 << 4)
#define RNG_CRG_RESET_BIT                       (0x01 << 0)

#define CIPHER_KLAD_REG_BASE_ADDR_PHY           (0x10110000)
#define CIPHER_OTP_REG_BASE_ADDR_PHY            (0x10200000)
#define CIPHER_KLAD_CRG_ADDR_PHY                (0x11012CC0)

#define KLAD_CRG_CLOCK_BIT                      (0x01 << 4)
#define KLAD_CRG_RESET_BIT                      (0x01 << 0)
#endif

#endif /* CIPHER_CONFIG_H */
