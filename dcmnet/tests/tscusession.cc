/*
 *
 *  Copyright (C) 2019-2021, OFFIS e.V.
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
 *  Purpose: Test DcmSCU's C-FIND/GET/MOVE session handling
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS

#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftimer.h"

#include <cmath>

static Uint8 NUM_DIMSE_REQUESTS = 5;

/** Method that ensures that the current thread is actually sleeping for the
 *  defined number of seconds (at least).
 *  @param sleep The number of seconds to sleep (at least)
 */
static void force_sleep(Uint32 sleep)
{
    OFTimer timer;
    double elapsed = timer.getDiff();
    while (elapsed < (double)sleep)
    {
        // Use ceiling since otherwise we could wait too short
        OFStandard::sleep(OFstatic_cast(unsigned int, ceil(sleep - elapsed)));
        elapsed = timer.getDiff();
    }
}

/** SCP derived from DcmSCP that has simple handlers for MOVE, GET and FIND
 *  and allows stopping after one association.
 */
struct SessionSCP : public DcmSCP, OFThread
{

    /** Constructor
     */
    SessionSCP()
        : DcmSCP()
    {
    }

    /** Overwrite method from DcmSCP in order to stop after one association.
     *  @return Returns OFTrue (stop after first association)
     */
    virtual OFBool stopAfterCurrentAssociation()
    {
        return OFTrue;
    }

    /** Overwrite DcmSCP method to enable handling of C-FIND and C-MOVE messages (TODO: C-GET)
     *  @param  incomingMsg Incoming DIMSE message
     *  @param  presInfo Presentation Context information
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, const DcmPresentationContextInfo& presInfo)
    {
        OFCondition result;
        DcmDataset* dset = NULL;
        if (incomingMsg->CommandField == DIMSE_C_FIND_RQ)
        {
            // Call our custom FIND handler
            result = handleFIND(incomingMsg->msg.CFindRQ, presInfo.presentationContextID, dset);
        }
        else if (incomingMsg->CommandField == DIMSE_C_MOVE_RQ)
        {
            // Call our custom MOVE handler
            OFString dest = incomingMsg->msg.CMoveRQ.MoveDestination;
            result        = handleMOVE(incomingMsg->msg.CMoveRQ, presInfo.presentationContextID, dset, dest);
        }
        else
            result = DcmSCP::handleIncomingCommand(incomingMsg, presInfo);

        delete dset;
        return result;
    }

    /** Overwrite DcmSCP method to to add specific C-MOVE handler. Sends NUM_DIMSE_REQUESTS
     *  DIMSE responses back, all in status PENDING, last in status SUCCESS.
     *  @param  reqMessage Incoming DIMSE message
     *  @param  presID Presentation Context ID
     *  @param  reqDataset Request dataset
     *  @param  moveDest Move Destination AE Title
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition handleMOVE(T_DIMSE_C_MoveRQ& reqMessage,
                           const T_ASC_PresentationContextID presID,
                           DcmDataset*& reqDataset,
                           OFString& moveDest)
    {
        OFCondition result = DcmSCP::receiveMOVERequest(reqMessage, presID, reqDataset, moveDest);
        if (result.good())
        {
            OFString sop_class, ts;
            OFunique_ptr<DcmDataset> rsp(new DcmDataset);
            *rsp = *reqDataset;
            DcmSCP::findPresentationContext(presID, sop_class, ts);
            Uint16 status = STATUS_MOVE_Pending_SubOperationsAreContinuing;
            // Send back configured number of responses, all but last with status PENDING (last: SUCCESS).
            // All but the last one will return a copy of the request dataset (random choice for this test;
            // last response will not have a dataset.
            for (Uint8 n = 1; n <= NUM_DIMSE_REQUESTS; n++)
            {
                if (n == NUM_DIMSE_REQUESTS)
                {
                    rsp.reset(NULL);
                    status = STATUS_Success;
                }
                result = DcmSCP::sendMOVEResponse(
                    presID, reqMessage.MessageID, sop_class, rsp.get(), status, NULL, NUM_DIMSE_REQUESTS - n, n, 0, 0);
            }
        }
        return result;
    }

    /** Overwrite DcmSCP method to to add specific C-FIND handler. Sends NUM_DIMSE_REQUESTS
     *  DIMSE responses back, all in status PENDING, last in status SUCCESS.
     *  @param  reqMessage Incoming DIMSE message
     *  @param  presID Presentation Context ID
     *  @param  reqDataset Request dataset
     *  @return EC_Normal if successful, error otherwise
     */
    OFCondition
    handleFIND(T_DIMSE_C_FindRQ& reqMessage, const T_ASC_PresentationContextID presID, DcmDataset*& reqDataset)
    {
        OFCondition result = DcmSCP::receiveFINDRequest(reqMessage, presID, reqDataset);
        if (result.good())
        {
            OFString sop_class, ts;
            OFunique_ptr<DcmDataset> rsp(new DcmDataset);
            *rsp = *reqDataset;
            DcmSCP::findPresentationContext(presID, sop_class, ts);
            Uint16 status = STATUS_FIND_Pending_MatchesAreContinuing;
            // Send back configured number of responses, all but last with status PENDING (last: SUCCESS).
            // All but the last one will return a copy of the request dataset (random choice for this test;
            // last response will not have a dataset.
            for (Uint8 n = 1; n <= NUM_DIMSE_REQUESTS; n++)
            {
                if (n == NUM_DIMSE_REQUESTS)
                {
                    rsp.reset(NULL);
                    status = STATUS_Success;
                }
                result = DcmSCP::sendFINDResponse(presID, reqMessage.MessageID, sop_class, rsp.get(), status, NULL);
            }
        }
        return result;
    }

    /** Method called by OFThread to start SCP operation. Starts listen() loop of DcmSCP.
     */
    virtual void run()
    {
        listen();
    }
};

/** Class derived from DcmSCU that has support for C-FIND/MOVE and GET-related
 *  SOP Classes. Also it has handlers for those DIMSE messages call the original
 *  handlers from DcmSCU and simply count how often they are called.
 *  The goal is to exercise the internal default response handing built into
 *  DcmSCU for handling C-FIND/MOVE and GET-related "sessions".
 */
struct SessionSCU : public DcmSCU
{
    /** Constructor
     */
    SessionSCU()
        : m_session_counter(0)
    {
        OFList<OFString> ts;
        ts.push_back(UID_LittleEndianImplicitTransferSyntax);
        OFCHECK(addPresentationContext(UID_FINDPatientRootQueryRetrieveInformationModel, ts).good());
        OFCHECK(addPresentationContext(UID_MOVEPatientRootQueryRetrieveInformationModel, ts).good());
        OFCHECK(addPresentationContext(UID_GETPatientRootQueryRetrieveInformationModel, ts).good());
    }

    /** Overwrites method from DcmSCU.
     *  Handles incoming C-GET response. Just counts number of calls and then forwards
     *  handling to default DcmSCU handler.
     *  @param  presID Presentation Context ID
     *  @param  response The C-GET response received
     *  @param  continueCGETSession Will receive information whether it should be waited
     *          for further C-GET responses.
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition handleCGETResponse(const T_ASC_PresentationContextID presID,
                                           RetrieveResponse* response,
                                           OFBool& continueCGETSession)
    {
        m_session_counter++;
        return DcmSCU::handleCGETResponse(presID, response, continueCGETSession);
    }

    /** Overwrites method from DcmSCU.
     *  Handles incoming C-MOVE response. Just counts number of calls and then forwards
     *  handling to default DcmSCU handler.
     *  @param  presID Presentation Context ID
     *  @param  response The C-MOVE response received
     *  @param  waitForNextResponse Will receive information whether it should be waited
     *          for further C-MOVE responses.
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition handleMOVEResponse(const T_ASC_PresentationContextID presID,
                                           RetrieveResponse* response,
                                           OFBool& waitForNextResponse)
    {
        m_session_counter++;
        return DcmSCU::handleMOVEResponse(presID, response, waitForNextResponse);
    }

    /** Overwrites method from DcmSCU.
     *  Handles incoming C-FIND response. Just counts number of calls and then forwards
     *  handling to default DcmSCU handler.
     *  @param  presID Presentation Context ID
     *  @param  response The C-FIND response received
     *  @param  waitForNextResponse Will receive information whether it should be waited
     *          for further C-FIND responses.
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition
    handleFINDResponse(const T_ASC_PresentationContextID presID, QRResponse* response, OFBool& waitForNextResponse)
    {
        m_session_counter++;
        OFCondition result = DcmSCU::handleFINDResponse(presID, response, waitForNextResponse);
        return result;
    }

    /** Virtual destructor */
    virtual ~SessionSCU()
    {
    }

    /** Counter for number of DIMSE messages handled
     */
    size_t m_session_counter;
};

// -------------- End of class SessionSCP -------------------------

/** Manually configure Verification SOP class on server
 *  @param cfg The SCP configuration to modify
 *  @param sop_class SOP class to be supported
 *  @param roleOfRequestor role to be negotiated
 */
void configure_scp_for_sop_class(DcmSCPConfig& cfg,
                                 const OFString& sop_class,
                                 T_ASC_SC_ROLE roleOfRequestor = ASC_SC_ROLE_DEFAULT)
{
    cfg.setPort(11112);
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCHECK(cfg.addPresentationContext(sop_class, xfers, roleOfRequestor).good());
}

/** Initiate SCU and perform session with SCP using given SOP class.
 *  @param called_ae_title The Called AE Title to be used
 *  @param sop_class The SOP class to negotiate
 */
void scu_session(const OFString& called_ae_title, const OFString& sop_class)
{
    // Make sure server is up
    force_sleep(1);
    // Basic configuration
    SessionSCU scu;
    scu.setAETitle("TEST_SCU");
    scu.setPeerAETitle(called_ae_title);
    scu.setPeerHostName("localhost");
    scu.setPeerPort(11112);
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    OFCondition result = scu.addPresentationContext(sop_class, xfers);
    OFCHECK(result.good());
    result = scu.initNetwork();
    OFCHECK(result.good());

    // Start connection
    result = scu.negotiateAssociation();
    OFCHECK(result.good());

    // Build simple query
    DcmDataset query;
    query.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "PATIENT");
    query.putAndInsertOFStringArray(DCM_PatientID, "0815");
    // Send FIND, GET (TODO), or MOVE
    if (sop_class == UID_FINDPatientRootQueryRetrieveInformationModel)
    {
        result = scu.sendFINDRequest(1, &query, NULL);
    }
    else if (sop_class == UID_MOVEPatientRootQueryRetrieveInformationModel)
    {
        result = scu.sendMOVERequest(3, "TEST_SCU", &query, NULL);
    }
    // Check whether session worked by comparing response count
    OFCHECK(scu.m_session_counter == NUM_DIMSE_REQUESTS);
    OFCHECK(result.good());
    // Release association
    result = scu.releaseAssociation();
    OFCHECK(result.good());
    return;
}

// Test case to check SCU session handling for C-FIND, C-MOVE and C-GET
OFTEST_FLAGS(dcmnet_scu_session_handler, EF_Slow)
{
    // Configure SCP and start it.
    SessionSCP scp;
    DcmSCPConfig& config = scp.getConfig();
    configure_scp_for_sop_class(config, UID_FINDPatientRootQueryRetrieveInformationModel);
    configure_scp_for_sop_class(config, UID_GETPatientRootQueryRetrieveInformationModel);
    configure_scp_for_sop_class(config, UID_MOVEPatientRootQueryRetrieveInformationModel);
    config.setConnectionBlockingMode(DUL_BLOCK);
    scp.start();

    // Send FIND, and wait to be sure SCP has time to exit
    scu_session("FIND_SESSION", UID_FINDPatientRootQueryRetrieveInformationModel);
    force_sleep(1);
    scp.join();

    // Send MOVE, and wait to be sure SCP has time to exit
    scp.start();
    scu_session("MOVE_SESSION", UID_MOVEPatientRootQueryRetrieveInformationModel);
    force_sleep(1);
    scp.join();

    // TODO: Test C-GET
}

#endif // WITH_THREADS
