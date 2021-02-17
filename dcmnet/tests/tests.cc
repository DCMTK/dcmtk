/*
 *
 *  Copyright (C) 2012-2020, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmnet_dimseDump_nullByte);
OFTEST_REGISTER(dcmnet_dimseStatusClass);

#ifdef WITH_THREADS
OFTEST_REGISTER(dcmnet_scp_pool);
OFTEST_REGISTER(dcmnet_scp_builtin_verification_support);
OFTEST_REGISTER(dcmnet_scp_fail_on_invalid_association_configuration);
OFTEST_REGISTER(dcmnet_scp_fail_on_disallowed_host);
OFTEST_REGISTER(dcmnet_scp_stop_after_current_association);
OFTEST_REGISTER(dcmnet_scp_stop_after_timeout);
OFTEST_REGISTER(dcmnet_scp_no_stop_wo_request_noblock);
OFTEST_REGISTER(dcmnet_scp_no_stop_wo_request_block);
OFTEST_REGISTER(dcmnet_scp_no_term_notify_without_association);
OFTEST_REGISTER(dcmnet_scp_role_selection);
OFTEST_REGISTER(dcmnet_scu_session_handler);
#endif // WITH_THREADS

OFTEST_MAIN("dcmnet")
