/*
 *
 *  Copyright (C) 2013, OFFIS e.V.
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
        result = sendECHORequest( 0 );
        closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
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


/* Test starts pool with a maximum of 5 SCP workers. All workers are
   configured to respond to C-ECHO (Verification  SOP Class). 5 SCU
   threads are created and connect simultanously to the pool, send
   C-ECHO messages are relase the association.
   Currently the pool ends itself after 3 seconds without connection
   request. This can be changed to a "shutDown()" call on the pool
   once it is implemented
*/
OFTEST(dcmnet_scp_pool)
{
    TestPool pool;
    DcmSCPConfig& config = pool.getConfig();

    config.setAETitle( "PoolTestSCP" );
    config.setPort( 11112 );
    config.setConnectionBlockingMode( DUL_NOBLOCK );

    /* Stop listening after 3 seconds. This makes sure the
       server pool exits 3 seconds after the last connection request
       of the SCUs. As soon as it is possible to shut down the pool
       via API (currently under test), this should be done instead of
       exiting via connection timeout.
     */
    config.setConnectionTimeout( 3 );

    pool.setMaxThreads( 20 );
    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    config.addPresentationContext(UID_VerificationSOPClass,xfers);

    OFVector<TestSCU*> scus( 20 );
    for( OFVector<TestSCU*>::iterator it = scus.begin(); it != scus.end(); ++it )
    {
        *it = new TestSCU;
        (*it)->setAETitle( "PoolTestSCU" );
        (*it)->setPeerAETitle( "PoolTestSCP" );
        (*it)->setPeerHostName( "localhost" );
        (*it)->setPeerPort( 11112 );
        (*it)->addPresentationContext(UID_VerificationSOPClass,xfers);
        (*it)->initNetwork();
    }

    pool.start();

    for( OFVector<TestSCU*>::const_iterator it = scus.begin(); it != scus.end(); ++it )
        (*it)->start();

    for( OFVector<TestSCU*>::iterator it = scus.begin(); it != scus.end(); ++it )
    {
        (*it)->join();
        OFCHECK( (*it)->result.good() );
        delete *it;
    }

    // enable as soon as available (currently being tested)
    // pool.stopAfterCurrentAssociations();
    pool.join();

    OFCHECK( pool.result.good() );
}

#endif // WITH_THREADS
