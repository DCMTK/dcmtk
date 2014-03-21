/*
 *
 *  Copyright (C) 2013-2014, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Test DcmSCPPool class, including DcmSCP and DcmSCU interaction
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmnet/scppool.h"
#include "dcmtk/dcmnet/scu.h"

struct TestSCU : DcmSCU, OFThread
{
    OFCondition result;
protected:
    void run()
    {
        negotiateAssociation();
        result = sendECHORequest(0);
        releaseAssociation();
    }
};

struct TestPool : DcmSCPPool<>, OFThread
{
    OFCondition result;
protected:
    void run()
    {
        result = listen();
    }
};


/* Test starts pool with a maximum of 20 SCP workers. All workers are
 * configured to respond to C-ECHO (Verification SOP Class). 20 SCU
 * threads are created and connect simultaneously to the pool, send
 * C-ECHO messages and release the association.
 */
OFTEST_FLAGS(dcmnet_scp_pool, EF_Slow)
{
    TestPool pool;
    DcmSCPConfig& config = pool.getConfig();

    config.setAETitle("PoolTestSCP");
    config.setPort(11112);
    config.setConnectionBlockingMode(DUL_NOBLOCK);

    // Dead time during which the pool is unable to respond to
    // stopAfterCurrentAssociations().
    config.setConnectionTimeout(1);

    pool.setMaxThreads(20);
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    config.addPresentationContext(UID_VerificationSOPClass, xfers);

    pool.start();

    OFVector<TestSCU*> scus(20);
    for (OFVector<TestSCU*>::iterator it1 = scus.begin(); it1 != scus.end(); ++it1)
    {
        *it1 = new TestSCU;
        (*it1)->setAETitle("PoolTestSCU");
        (*it1)->setPeerAETitle("PoolTestSCP");
        (*it1)->setPeerHostName("localhost");
        (*it1)->setPeerPort(11112);
        (*it1)->addPresentationContext(UID_VerificationSOPClass, xfers);
        (*it1)->initNetwork();
    }

    // "ensure" the pool is initialized before any SCU starts connecting to it. The initialization
    // can take a couple of seconds on older systems, e.g. debian i368.
    OFStandard::sleep(5);

    for (OFVector<TestSCU*>::const_iterator it2 = scus.begin(); it2 != scus.end(); ++it2)
        (*it2)->start();

    for (OFVector<TestSCU*>::iterator it3 = scus.begin(); it3 != scus.end(); ++it3)
    {
        (*it3)->join();
        OFCHECK((*it3)->result.good());
        delete *it3;
    }

    // Request shutdown.
    pool.stopAfterCurrentAssociations();
    pool.join();

    OFCHECK(pool.result.good());
}

#endif // WITH_THREADS
