/*
 *
 *  Copyright (C) 2019-2020, OFFIS e.V.
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
 *  Module:  dcmtls
 *
 *  Authors:  Michel Amat, Damien Lerat
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftest.h"

#ifdef WITH_THREADS
#ifdef WITH_OPENSSL
OFTEST_REGISTER(dcmtls_scp_tls);
OFTEST_REGISTER(dcmtls_scp_pool_tls);
#endif // WITH_OPENSSL
#endif // WITH_THREADS

OFTEST_MAIN("dcmtls")
