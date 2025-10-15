// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __X509_PARSER_H__
#define __X509_PARSER_H__

#include "rsa_public.h"

/**
 * parse x509 certification to rsa public key.
 */
int x509_convert_to_rsa_pub_key(const unsigned char *x509_cert_content, unsigned int cert_len, rsa_pub_key *pub_key);
#endif
