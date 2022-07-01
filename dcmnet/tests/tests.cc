/*
 *
 *  Copyright (C) 2012-2022, OFFIS e.V.
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

OFTEST_REGISTER(dcmnet_scu_setConectionTimeout_does_not_change_global_dcmConnectionTimeout_parameter);
OFTEST_REGISTER(dcmnet_scu_getConectionTimeout_returns_scu_tcp_connection_timeout);

OFTEST_REGISTER(dcmnet_scu_sendNCREATERequest_succeeds_when_optional_createdinstance_is_null);
OFTEST_REGISTER(dcmnet_scu_sendNCREATERequest_fails_when_affectedsopinstance_is_empty);
OFTEST_REGISTER(dcmnet_scu_sendNCREATERequest_creates_instance_when_association_was_accepted);
OFTEST_REGISTER(dcmnet_scu_sendNCREATERequest_succeeds_and_sets_responsestatuscode_from_scp_when_scp_sets_error_status);

OFTEST_REGISTER(dcmnet_scu_sendNSETRequest_fails_when_requestedsopinstance_is_empty);
OFTEST_REGISTER(dcmnet_scu_sendNSETRequest_succeeds_and_modifies_instance_when_scp_has_instance);
OFTEST_REGISTER(dcmnet_scu_sendNSETRequest_succeeds_and_sets_responsestatuscode_from_scp_when_scp_sets_error_status);

#endif // WITH_THREADS

OFTEST_MAIN("dcmnet")
