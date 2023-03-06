/*
 *
 *  Copyright (C) 2018-2023, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmtls
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    OpenSSL feature test macros
 *
 */

#ifndef TLSFMAC_H
#define TLSFMAC_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

BEGIN_EXTERN_C
#include <openssl/ssl.h>
#include <openssl/tls1.h>
END_EXTERN_C

/* check if we fulfil all requirements for implementing the 
 * Modified BCP 195 RFC 8996 TLS Profile
 */
#if defined(HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305) \
  && defined(HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8) \
  && defined(HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384) \
  && defined(HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_CHACHA20_POLY1305_SHA256) \
  && defined(HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_128_CCM_8_SHA256)

/* all feature tests except Camellia in GCM mode are successful
 * We now check if we either have Camellia in GCM more or have been
 * instructed to ignore this requirement
 */
#if defined(HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384) \
  || defined(DCMTK_IGNORE_BCP195M_CAMELLIA_GCM_REQUIREMENT)

#define DCMTK_Modified_BCP195_RFC8996_TLS_Profile_Supported
#endif
#endif

#endif /* WITH_OPENSSL */
#endif /* TLSFMAC_H */
