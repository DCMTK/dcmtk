/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    Open Connections GmbH
 *    Stau 33
 *    D-26122 Oldenburg, Germany
 *
 *
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test DcmSCP and DcmSCU classes (only certain aspects so far)
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS

#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"
#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/scu.h"



static OFLogger t_scuscp_logger= OFLog::getLogger("dcmtk.test.tscuscp");

/** Method that ensures that the current thread is actually sleeping for the
 *  defined number of seconds (at least).
 *  The problem with the regular sleep() function called from OFStandard::sleep
 *  is that it might be interrupted by signals or a network timeout (depending
 *  on the operating system). This methods re-executes OFStandard'S sleep method
 *  until the desired number of seconds have elapsed.
 *  @param sleep The number of seconds to sleep (at least)
 */
static void force_sleep(Uint32 sleep)
{
    OFTimer timer;
    double elapsed = timer.getDiff();
    while (elapsed < (double)sleep)
    {
        // Use ceiling since otherwise we could wait too short
        OFStandard::sleep(OFstatic_cast(unsigned int, ceil(sleep-elapsed)));
        elapsed = timer.getDiff();
    }
}


/** SCP derived from DcmSCP in order to test two types of virtual methods:
 *  <ul>
 *    <li> Notifiers: Called for some events happening in DcmSCP. So far
 *    only the notifiers notifyConnectionTimeout() as and notifyAssociationTermination
 *    are tested</li>
 *    <li> Setters: Functions returning a value in order to enforce
 *    a specific DcmSCP behavior. So far, only stopAfterCurrentAssociation() and
 *    stopAfterConnectionTimeout() are tested.
 *  </ul>
 *  Additionally the SCP derives from OFThread in order to construct the
 *  test case (i.e. send data with SCU at the same time and check results).
 */
struct TestSCP: DcmSCP, OFThread
{

    /** Constructor
     */
    TestSCP() :
        DcmSCP(),
        m_listen_result(EC_NotYetImplemented), // value indicating "not set"
        m_set_stop_after_assoc(OFFalse),
        m_set_stop_after_timeout(OFFalse),
        m_set_reject_calling_host(OFFalse),
        m_stop_after_assoc_result(OFFalse),
        m_stop_after_timeout_result(OFFalse),
        m_notify_connection_timeout_result(OFFalse),
        m_notify_assoc_termination_result(OFFalse)
    {
    }

    /** Clear settings
     */
    void clear()
    {
        m_listen_result = EC_NotYetImplemented;
        m_set_stop_after_assoc = OFFalse;
        m_set_stop_after_timeout = OFFalse;
        m_set_reject_calling_host = OFFalse;
        m_stop_after_assoc_result = OFFalse;
        m_stop_after_timeout_result = OFFalse;
        m_notify_connection_timeout_result = OFFalse;
        m_notify_assoc_termination_result = OFFalse;
    }

    /** Overwrite method from DcmSCP in order to test feature to stop after current
     *  association.
     *  @return Returns value of m_set_stop_after_assoc
     */
    virtual OFBool stopAfterCurrentAssociation()
    {
        if (m_set_stop_after_assoc)
        {
            m_stop_after_assoc_result = OFTrue;
        }
        else
        {
            m_stop_after_assoc_result = OFFalse;
        }
        return m_set_stop_after_assoc;
    }

    /** Overwrite method from DcmSCP in order to test feature to stop after
    *   the SCP's connection timeout occurred in non-blocking mode.
    *   @return Returns value of m_set_stop_after_timeout
    */
    virtual OFBool stopAfterConnectionTimeout()
    {
        if (m_set_stop_after_timeout)
        {
            m_stop_after_timeout_result = OFTrue;
        }
        else
        {
            m_stop_after_timeout_result = OFFalse;
        }
        return m_stop_after_timeout_result;
    }

    /** Overwrite method from DcmSCP in order to test feature that SCP reports
     *  connection timeout in non-blocking mode.
     */
    virtual void notifyConnectionTimeout()
    {
        m_notify_connection_timeout_result = OFTrue;
    }

    /** Overwrite method from DcmSCP in order to test feature that SCP reports
     *  the termination of an association.
     */
    virtual void notifyAssociationTermination()
    {
        m_notify_assoc_termination_result = OFTrue;
    }

    /** Overwrite method from DcmSCP in order to test feature that SCP rejects
     *  a forbidden host.
     *  @param hostOrIP The host name or IP address to be checked
     */
    virtual OFBool checkCallingHostAccepted(const OFString& hostOrIP)
    {
        // In order to test this feature, we do not need to perform actual checking
        (void)hostOrIP;
        if (m_set_reject_calling_host)
        {
            m_calling_host_accepted = OFFalse;
            return OFFalse;
        }
        else
        {
            m_calling_host_accepted = OFTrue;
            return OFTrue;
        }
    }

    /// The result returned my SCP's listen() method
    OFCondition m_listen_result;
    /// If set, the SCP should stop after the currently running association
    OFBool m_set_stop_after_assoc;
    /// If set, the SCP should stop after TCP timeout occurred in non-blocking mode
    OFBool m_set_stop_after_timeout;
    /// If set, the SCP should not allow the calling host (whatever it is)
    OFBool m_set_reject_calling_host;
    /// Indicator whether related virtual function was called and returned accordingly
    OFBool m_stop_after_assoc_result;
    /// Indicator whether related virtual function was called and returned accordingly
    OFBool m_stop_after_timeout_result;
    /// Indicator whether the calling host was accepted
    OFBool m_calling_host_accepted;
    /// Indicator whether related virtual notifier function was called
    OFBool m_notify_connection_timeout_result;
    /// Indicator whether related virtual notifier function was called
    OFBool m_notify_assoc_termination_result;

    /** Method called by OFThread to start SCP operation. Starts listen() loop of DcmSCP.
    */
    virtual void run()
    {
        m_listen_result = listen();
    }

};

// -------------- End of class TestSCP -------------------------


/** Manually configure Verification SOP class on server
 *  @param cfg The SCP configuration to modify
 */
void configure_scp_for_echo(DcmSCPConfig& cfg, T_ASC_SC_ROLE roleOfRequestor = ASC_SC_ROLE_DEFAULT)
{
    cfg.setPort(11112);
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCHECK(cfg.addPresentationContext(UID_VerificationSOPClass, xfers, roleOfRequestor).good());
}


/** Send ECHO to SCP
 *  @param called_ae_title The Called AE Title to be used
 *  @param do_release  If OFTrue, SCU should release the association (otherwise
 *         we expect the SCP to do that)
 *  @param secs_after_echo Seconds to wait after sending echo
 */
void scu_sends_echo(
  const OFString& called_ae_title,
  const OFBool expect_assoc_reject = OFFalse,
  const OFBool do_release = OFTrue,
  const int secs_after_echo = 0)
{
    // make sure server is up
    force_sleep(2);
    DcmSCU scu;
    scu.setAETitle("TEST_SCU");
    scu.setPeerAETitle(called_ae_title);
    scu.setPeerHostName("localhost");
    scu.setPeerPort(11112);
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCondition result = scu.addPresentationContext(UID_VerificationSOPClass, xfers);
    OFCHECK(result.good());
    result = scu.initNetwork();
    OFCHECK(result.good());
    result = scu.negotiateAssociation();
    if (!expect_assoc_reject)
    {
        OFCHECK(result.good());
        result = scu.sendECHORequest(1);
        OFCHECK(result.good());
        force_sleep(secs_after_echo);
        if (do_release)
        {
            result = scu.releaseAssociation();
            OFCHECK(result.good());
        }
    }
    else
    {
        OFCHECK(result == DUL_ASSOCIATIONREJECTED);
    }
    return;
}


// Test case that checks whether server returns if it is configured to stop
// after current association
OFTEST_FLAGS(dcmnet_scp_stop_after_current_association, EF_Slow)
{
    // Configure SCP for Verification and start it. First we check that the
    // server does *not* return after association without configuring it
    // accordingly
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);
    config.setAETitle("STOP_AFTER_ASSOC");
    config.setConnectionBlockingMode(DUL_BLOCK);
    scp.start();

    // Send ECHO, and wait to be sure SCP has time to exit
    scu_sends_echo("STOP_AFTER_ASSOC");
    // Make sure server would have time to return
    force_sleep(2);

    // Check whether all test variables have the correct values
    OFCHECK(scp.m_listen_result == EC_NotYetImplemented); // still listening
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue); // SCU released association

    // Tell SCP to stop after association and run SCU again
    scp.clear();
    scp.m_set_stop_after_assoc = OFTrue;
    scu_sends_echo("STOP_AFTER_ASSOC");
    force_sleep(2);

    // Check whether all test variables have the correct values
    OFCHECK(scp.m_listen_result == NET_EC_StopAfterAssociation);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue); // SCU released association

    scp.join();
}


// Test case that checks whether server returns with the correct error code if
// configured with an invalid configuration (here: no presentation contexts)
OFTEST_FLAGS(dcmnet_scp_fail_on_invalid_association_configuration, EF_Slow)
{
    // Configure SCP but do not add any presentation contexts
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    config.setPort(11112);;
    config.setAETitle("TEST_INVALID_CFG");
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    // Ensure that if the server starts listen loop (it should not), the
    // related thread returns anyway.
    config.setConnectionTimeout(3);
    scp.m_set_stop_after_timeout = OFTrue;
    // Start server and check that it returns with the expected return value.
    // In this case we did not configure any presentation contexts, so we
    // expect the related error code.
    scp.start();
    scp.join();
    OFCHECK(scp.m_listen_result == NET_EC_InvalidSCPAssociationProfile);
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFFalse);
}


// Test case that checks whether server checks for allowed/disallowed host
// names, i.e. it refuses the association in case the connecting host
// is not accepted by the related virtual method.
OFTEST_FLAGS(dcmnet_scp_fail_on_disallowed_host, EF_Slow)
{
    // Configure SCP for Verification
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);
    config.setAETitle("REJECT_HOST");
    config.setConnectionBlockingMode(DUL_BLOCK);
    // Make sure SCP always returns
    scp.m_set_stop_after_assoc = OFTrue;
    // Tell SCP to reject calling host
    scp.m_set_reject_calling_host = OFTrue;
    scp.start();

    scu_sends_echo("REJECT_HOST", OFTrue /* expect that association is being refused */);
    force_sleep(2);  // make sure server would have time to return

    // Check whether all test variables have the correct values
    OFCHECK(scp.m_listen_result == NET_EC_StopAfterAssociation);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue);
    // Check whether the calling host has been the reason for
    // refusing the association
    OFCHECK(scp.m_calling_host_accepted == OFFalse);
    scp.join();
}


// Test case that checks the SCP's builtin Verification support
// works and serves C-ECHO
OFTEST_FLAGS(dcmnet_scp_builtin_verification_support, EF_Slow)
{
    // Configure SCP and enable built-in Verification SOP Class Handler
    TestSCP scp;
    scp.setEnableVerification();
    scp.getConfig().setPort(11112);
    DcmSCPConfig& config = scp.getConfig();
    config.setAETitle("TEST_BUILTIN_VER");
    config.setConnectionBlockingMode(DUL_BLOCK);
    // Make sure server stops after the SCU connection
    scp.m_set_stop_after_assoc = OFTrue;
    scp.start();

    // Send echo and receive response
    scu_sends_echo("TEST_BUILTIN_VER");
    // make sure server would have time to return
    force_sleep(1);

    // Check whether all test variables have the correct values
    OFCHECK(scp.m_listen_result == NET_EC_StopAfterAssociation);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue); // SCU released association

    scp.join();
}

// Check whether SCP stops after provided timeout
OFTEST_FLAGS(dcmnet_scp_stop_after_timeout, EF_Slow)
{
    // Configure SCP with Verification, and make sure it returns after
    // a timeout occurs, i.e. no connection is received. For testing, a connection
    // is performed, i.e. timeout occurs after that connection.
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);
    config.setAETitle("STOP_ON_TIMEOUT");
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setConnectionTimeout(5);
    scp.m_set_stop_after_timeout = OFTrue;
    scp.start();
    force_sleep(1);
    scu_sends_echo("STOP_ON_TIMEOUT");
    // Wait for server to return after connection has been served and timeout
    // has been reached
    scp.join();

    // Check whether all test variables have the correct values
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFTrue);                 // timeout has been checked
    OFCHECK(scp.m_notify_connection_timeout_result == OFTrue);          // timeout notifier method has been called
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue);           // association ended
    OFCHECK(scp.m_listen_result == NET_EC_StopAfterConnectionTimeout);  // SCP returns, using specific return code
}


// Check whether SCP continues after timeout elapsed if timeout handler does
// not tell SCP to stop
OFTEST_FLAGS(dcmnet_scp_no_stop_wo_request_noblock, EF_Slow)
{
    // Configure SCP for Verification, set timeout but do not tell
    // SCP to stop after timeout occurs, and start SCP
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);

    config.setAETitle("NO_STOP_NOBLOCK");
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setConnectionTimeout(1);
    scp.m_set_stop_after_timeout = OFFalse;
    scp.start();

    // Check whether all test variables have the correct values, especially that
    // timeout occurred but did not lead to a stop
    force_sleep(5);
    OFCHECK(scp.m_listen_result == EC_NotYetImplemented);      // still listening
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFTrue); // Timeout occurred, but we did not stop
    OFCHECK(scp.m_notify_assoc_termination_result == OFFalse); // SCU released association

    // Stop (already tested in former test cases)
    scp.m_set_stop_after_timeout = OFTrue;
    scp.join();

}


// Check whether SCP does not stop without request in blocking mode
OFTEST_FLAGS(dcmnet_scp_no_stop_wo_request_block, EF_Slow)
{
    // Configure SCP for Verification and to do not ask to exit for any reason
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);
    config.setAETitle("NO_STOP_BLOCK");
    config.setConnectionBlockingMode(DUL_BLOCK);
    scp.start();
    scu_sends_echo("NO_STOP_BLOCK");

    // Give some time to SCP so it could return
    force_sleep(3);

    // Check whether all test variables have the correct values
    // SCP did not return yet (check whether it is still connected)
    OFCHECK(scp.m_listen_result == EC_NotYetImplemented); // still listening
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_connection_timeout_result == OFFalse);
    OFCHECK(scp.m_notify_assoc_termination_result == OFTrue);  // SCU released association

    // Stop (already tested in former test cases), therefore, send another ECHO
    scp.m_set_stop_after_assoc = OFTrue;
    scu_sends_echo("NO_STOP_BLOCK");
    scp.join();
}


// Check association termination notifier does not fire when there was no
// association at all
OFTEST_FLAGS(dcmnet_scp_no_term_notify_without_association, EF_Slow)
{
    // Configure SCP for Verification and tell it stop after 3 seconds.
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config);
    config.setAETitle("NO_TERM_WO_ASSOC");
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setConnectionTimeout(3);
    scp.m_set_stop_after_timeout = OFTrue;
    scp.start();
    scp.join();
    // Check whether all test variables have the correct values.
    OFCHECK(scp.m_stop_after_assoc_result == OFFalse);
    OFCHECK(scp.m_stop_after_timeout_result == OFTrue);
    OFCHECK(scp.m_notify_connection_timeout_result == OFTrue);
    OFCHECK(scp.m_notify_assoc_termination_result == OFFalse); // no notification
    OFCHECK(scp.m_listen_result == NET_EC_StopAfterConnectionTimeout); // SCP ran into timeout
}


/** Helper function for testing role selection, test "dcmnet_scp_role_selection".
 *  @param  r_req The role selection setting from the association requestor
 *  @param  r_acc The role selection setting configured for the association acceptor
 *  @param  expected_result The expected result of the role negotiation, i.e. what the
 *          acceptor will return to the client.
 *  @param  expect_assoc_reject If OFTrue, it is expected that the negotiation fails,
 *          default is OFFalse
 */
void test_role_selection(const T_ASC_SC_ROLE r_req,
                         const T_ASC_SC_ROLE r_acc,
                         const T_ASC_SC_ROLE expected_result,
                         const OFBool expect_assoc_reject = OFFalse,
                         const OFBool acceptDefaultInsteadOfSCP = OFFalse)
{
    // Make it possible to loop over roles, used for checking the
    // negotiated presentation contexts.
    static OFVector<T_ASC_SC_ROLE> roles;
    if (roles.empty())
    {
        roles.push_back(ASC_SC_ROLE_DEFAULT);
        roles.push_back(ASC_SC_ROLE_NONE);
        roles.push_back(ASC_SC_ROLE_SCP);
        roles.push_back(ASC_SC_ROLE_SCU);
        roles.push_back(ASC_SC_ROLE_SCUSCP);
    }

    // Configure SCP for Verification with the desired role, and start it
    TestSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_echo(config, r_acc);
    config.setAETitle("ACCEPTOR");
    config.setConnectionBlockingMode(DUL_NOBLOCK);
    config.setConnectionTimeout(4);
    config.setAlwaysAcceptDefaultRole(acceptDefaultInsteadOfSCP);
    scp.start();

    // Ensure server is up and listening
    force_sleep(1);

    // Configure SCP and run it against SCP with the desired role
    DcmSCU scu;
    scu.setPeerAETitle("ACCEPTOR");
    scu.setAETitle("REQUESTOR");
    scu.setPeerHostName("localhost");
    scu.setPeerPort(11112);
    OFList<OFString> ts;
    ts.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCHECK(scu.addPresentationContext(UID_VerificationSOPClass, ts, r_req).good());
    OFCHECK(scu.initNetwork().good());
    if (!expect_assoc_reject)
        OFCHECK(scu.negotiateAssociation().good());
    else
    {
        OFCHECK(scu.negotiateAssociation() == DUL_ASSOCIATIONREJECTED);
    }
    // Loop over roles and check for each role whether has been negotiated or not.
    // Only a single role (the expected_result) should be successfully negotiated.
    OFVector<T_ASC_SC_ROLE>::iterator it = roles.begin();
    OFCondition result;
    while (it != roles.end())
    {
        T_ASC_PresentationContextID id = scu.findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianImplicitTransferSyntax, (*it));
        if ( ((*it) == expected_result) && !expect_assoc_reject)
        {
            if (id == 0)
            {
                OFCHECK_FAIL("Error while testing requestor role " << ASC_role2String(r_req)
                    << " versus acceptor role " << ASC_role2String(r_acc)
                    << ", expected result: " << ASC_role2String(expected_result) << ", but did not find related presentation context with that role");
            }
        }
        else
        {
            if (id != 0)
            {
                OFCHECK_FAIL("Error while testing requestor role " << ASC_role2String(r_req)
                    << " versus acceptor role " << ASC_role2String(r_acc)
                    << ", expected result: " << ASC_role2String(expected_result) << ", but found unexpected presentation context for role " << ASC_role2String(*it));
            }
        }
        it++;
    }
    scp.m_set_stop_after_assoc = OFTrue;
    scp.m_set_stop_after_timeout = OFTrue; // also handles the association rejection case
    // Only release association if we're connected (could happen we're not in case
    // of test failures of if expect_assoc_reject is true)
    if (scu.isConnected())
        OFCHECK(scu.releaseAssociation().good());
    scp.join();
}

// Test case that checks whether server returns after association if enabled
OFTEST_FLAGS(dcmnet_scp_role_selection, EF_Slow)
{
    // The following role selection behavior should be implemented and
    // is exercised in this test (copied from dul.h):
    //  *  +--------------------+------------------+---------+
    //  *  | Requestor Proposal | Acceptor Setting | Result  |
    //  *  +--------------------+------------------+---------+
    //  *  | SCU                | SCP              | NONE    |
    //  *  | SCU                | SCU              | SCU     |
    //  *  | SCU                | SCU/SCP          | SCU     |
    //  *  | SCU                | DEFAULT          | DEFAULT |
    //  *  | SCP                | SCP              | SCP     |
    //  *  | SCP                | SCU              | NONE    |
    //  *  | SCP                | SCU/SCP          | SCP     |
    //  *  | SCP                | DEFAULT          | DEFAULT |
    //  *  | SCU/SCP            | SCP              | SCP     |
    //  *  | SCU/SCP            | SCU              | SCU     |
    //  *  | SCU/SCP            | SCU/SCP          | SCU/SCP |
    //  *  | SCU/SCP            | DEFAULT          | DEFAULT |
    //  *  | DEFAULT            | SCP              | Reject  |
    //  *  | DEFAULT            | SCU              | DEFAULT |
    //  *  | DEFAULT            | SCU/SCP          | DEFAULT |
    //  *  | DEFAULT            | DEFAULT          | DEFAULT |
    //  *  +--------------------+------------------+---------+
    //
    //  The Reject case can be turned into returning DEFAULT role when
    //  setting a specific option also being tested below.

    test_role_selection(ASC_SC_ROLE_SCU, ASC_SC_ROLE_SCP,     ASC_SC_ROLE_NONE);
    test_role_selection(ASC_SC_ROLE_SCU, ASC_SC_ROLE_SCU,     ASC_SC_ROLE_SCU);
    test_role_selection(ASC_SC_ROLE_SCU, ASC_SC_ROLE_SCUSCP,  ASC_SC_ROLE_SCU);
    test_role_selection(ASC_SC_ROLE_SCU, ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_DEFAULT);

    test_role_selection(ASC_SC_ROLE_SCP, ASC_SC_ROLE_SCP,     ASC_SC_ROLE_SCP);
    test_role_selection(ASC_SC_ROLE_SCP, ASC_SC_ROLE_SCU,     ASC_SC_ROLE_NONE);
    test_role_selection(ASC_SC_ROLE_SCP, ASC_SC_ROLE_SCUSCP,  ASC_SC_ROLE_SCP);
    test_role_selection(ASC_SC_ROLE_SCP, ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_DEFAULT);
    test_role_selection(ASC_SC_ROLE_SCUSCP, ASC_SC_ROLE_SCP,     ASC_SC_ROLE_SCP);
    test_role_selection(ASC_SC_ROLE_SCUSCP, ASC_SC_ROLE_SCU,     ASC_SC_ROLE_SCU);
    test_role_selection(ASC_SC_ROLE_SCUSCP, ASC_SC_ROLE_SCUSCP,  ASC_SC_ROLE_SCUSCP);
    test_role_selection(ASC_SC_ROLE_SCUSCP, ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_DEFAULT);

    test_role_selection(ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_SCP,     ASC_SC_ROLE_NONE /* not evaluated */, OFTrue /* expect rejection */);
    // Repeat the test but this time work around faulty clients by also accepting Default role
    // instead of the originally configured SCP role.
    test_role_selection(ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_SCP,     ASC_SC_ROLE_DEFAULT, OFFalse, OFTrue);
    test_role_selection(ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_SCU,     ASC_SC_ROLE_DEFAULT);
    test_role_selection(ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_SCUSCP,  ASC_SC_ROLE_DEFAULT);
    test_role_selection(ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_DEFAULT, ASC_SC_ROLE_DEFAULT);

}

#endif // WITH_THREADS
