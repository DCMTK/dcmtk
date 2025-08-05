/*
 *
 *  Copyright (C) 2018-2025, OFFIS e.V.
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
 * Modified BCP 195 RFC 8996 TLS Profile. With DICOM CP 2311
 * making support for Camellia in GCM mode optional, this is now rather simple.
 */
#ifdef HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8
#define DCMTK_Modified_BCP195_RFC8996_TLS_Profile_Supported
#endif

#endif /* WITH_OPENSSL */
#endif /* TLSFMAC_H */
