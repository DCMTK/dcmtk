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
 *  Author:  Michael Onken
 *
 *  Purpose: Class listening for association requests and managing a pool of
 *  worker threads that each are waiting to take over a single incoming
 *  association. Thus, the pool can serve as many associations
 *  simultaneously as the number of threads it is configured to create.
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS // Without threads pool does not make sense...

#include "dcmtk/dcmnet/scppool.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmtls/tlslayer.h"

// ----------------------------------------------------------------------------

DcmBaseSCPPool::DcmBaseSCPPool()
  : m_criticalSection(),
    m_workersBusy(),
    m_workersIdle(),
    m_cfg(),
    m_maxWorkers(5),
    m_runMode( LISTEN )
    // not implemented yet: m_workersBusyTimeout(60),
    // not implemented yet: m_waiting(),
{
}

// ----------------------------------------------------------------------------

DcmBaseSCPPool::~DcmBaseSCPPool()
{
}

// ----------------------------------------------------------------------------

OFCondition DcmBaseSCPPool::listen()
{
  m_runMode = LISTEN;

  /* Copy the config to a shared config that is shared by all workers. */
  DcmSharedSCPConfig sharedConfig(m_cfg);

  /* Initialize network, i.e. create an instance of T_ASC_Network*. */
  T_ASC_Network *network = NULL;
  OFCondition cond = initializeNework(&network);
  if(cond.bad())
    return cond;

  /* As long as all is fine (or we have been to busy handling last connection request) keep listening */
  while ( m_runMode == LISTEN && ( cond.good() || (cond == NET_EC_SCPBusy) ) )
  {
    // Reset status
    cond = EC_Normal;
    // Every incoming connection is handled in a new association object
    T_ASC_Association *assoc = NULL;
    OFBool useSecureLayer = m_cfg.transportLayerEnabled();

    // Listen to a socket for timeout seconds for an association request, accepts TCP connection.
    cond = ASC_receiveAssociation( network, &assoc, m_cfg.getMaxReceivePDULength(), NULL, NULL, useSecureLayer,
        m_cfg.getConnectionBlockingMode(), OFstatic_cast(int, m_cfg.getConnectionTimeout()) );

    /* If we have a connection request, try to find/create a worker to handle it */
    if (cond.good())
    {
      cond = runAssociation(assoc, sharedConfig);

      /* If anything goes wrong running association: Refuse it */
      if (cond.bad())
      {
        if (cond == NET_EC_SCPBusy)
        {
          rejectAssociation(assoc, ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED);
        }
        else
        {
          rejectAssociation(assoc, ASC_REASON_SP_PRES_TEMPORARYCONGESTION);
        }
        dropAndDestroyAssociation(assoc);
      }
    }

    /* If error occurred while receiving association, clean up */
    else
    {
      /* Handle timeout and errors differently */
      if ( cond == DUL_NOASSOCIATIONREQUEST )
      {
        ASC_destroyAssociation( &assoc );
      }
      else
      {
        dropAndDestroyAssociation(assoc);
        DCMNET_ERROR("DcmBaseSCPPool: Error receiving association: " << cond.text());
      }
      // ... and keep listening ...
      cond = EC_Normal;
    }
  }

  m_criticalSection.lock();
  m_runMode = SHUTDOWN;

  // iterate over all busy workers, join their threads and delete them.
  for
  (
    OFListIterator( DcmBaseSCPPool::DcmBaseSCPWorker* ) it = m_workersBusy.begin();
    it != m_workersBusy.end();
    ++it
  )
  {
    m_criticalSection.unlock();
    (*it)->join();
    delete *it;
    m_criticalSection.lock();
  }

  m_workersBusy.clear();
  m_criticalSection.unlock();

  /* In the end, clean up the rest of the memory and drop network */
  ASC_dropNetwork(&network);

  return EC_Normal;
}

void DcmBaseSCPPool::stopAfterCurrentAssociations()
{
  m_criticalSection.lock();
  if (m_runMode == LISTEN )
    m_runMode = STOP;
  m_criticalSection.unlock();
}

// ----------------------------------------------------------------------------

Uint16 DcmBaseSCPPool::getMaxThreads()
{
  return m_maxWorkers;
}

// ----------------------------------------------------------------------------

size_t DcmBaseSCPPool::numThreads(const OFBool onlyBusy)
{
  size_t result = 0;
  m_criticalSection.lock();
  if (!onlyBusy)
  {
    result = m_workersBusy.size() + m_workersIdle.size();
  }
  else
    result = m_workersBusy.size();
  m_criticalSection.unlock();
  return result;
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::setMaxThreads(const Uint16 maxWorkers)
{
  m_maxWorkers = maxWorkers;
}

// ----------------------------------------------------------------------------

OFCondition DcmBaseSCPPool::runAssociation(T_ASC_Association *assoc,
                                           const DcmSharedSCPConfig& sharedConfig)
{
  /* Try to find idle worker thread */
  OFCondition result = EC_Normal;
  DcmBaseSCPWorker *chosen = NULL;

  /* Do we have idle worker threads that can handle the association? */
  m_criticalSection.lock();
  if (m_workersIdle.empty())
  {
    if (m_workersBusy.size() >= m_maxWorkers)
    {
      /* No idle workers and maximum of busy workers reached? Return busy */
      result = NET_EC_SCPBusy;
    }
    else /* Else we can produce another worker */
    {
      DCMNET_DEBUG("DcmBaseSCPPool: Starting new DcmSCP worker thread");
      DcmBaseSCPWorker* const worker = createSCPWorker();
      if (!worker) /* Oops, we cannot allocate a new worker thread */
      {
        result = EC_MemoryExhausted;
      }
      else /* Configure worker thread */
      {
        m_workersBusy.push_back(worker);
        worker->setSharedConfig(sharedConfig);
        chosen = worker;
      }
    }
  }
  /* Else we have idle workers, use one of them */
  else
  {
    chosen = m_workersIdle.front();
    m_workersIdle.pop_front();
    m_workersBusy.push_back(chosen);
    result = EC_Normal;
  }
  m_criticalSection.unlock();

  /* Hand association to worker */
  if (result.good())
  {
    result = chosen->setAssociation(assoc);
  }
  /* Start the thread */
  if (result.good())
  {
     if (chosen->start() != 0)
     {
       result = NET_EC_CannotStartSCPThread;
     }
  }
  /* Return to listen loop */
  return result;
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::rejectAssociation(T_ASC_Association *assoc,
                                       const T_ASC_RejectParametersReason& reason)
{
  T_ASC_RejectParameters rej;
  rej.result = ASC_RESULT_REJECTEDTRANSIENT;
  rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
  rej.reason = reason;
  ASC_rejectAssociation( assoc, &rej );
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::dropAndDestroyAssociation(T_ASC_Association *assoc)
{
  if (assoc)
  {
    ASC_dropAssociation( assoc );
    ASC_destroyAssociation( &assoc );
  }
  assoc = NULL;
}

// ----------------------------------------------------------------------------

DcmSCPConfig& DcmBaseSCPPool::getConfig()
{
  return m_cfg;
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::notifyThreadExit(DcmBaseSCPPool::DcmBaseSCPWorker* thread,
                                      OFCondition result)
{
  m_criticalSection.lock();
  if( m_runMode != SHUTDOWN )
  {
    DCMNET_DEBUG("DcmBaseSCPPool: Worker thread #" << thread->threadID() << " exited with error: " << result.text());
    m_workersBusy.remove(thread);
    delete thread;
    thread = NULL;
  }
  m_criticalSection.unlock();
}

// ----------------------------------------------------------------------------

OFCondition DcmBaseSCPPool::initializeNework(T_ASC_Network** network)
{
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, OFstatic_cast(int, m_cfg.getPort()), m_cfg.getACSETimeout(), network);
    if (cond.good())
    {
      if (m_cfg.transportLayerEnabled())
      {
        cond = ASC_setTransportLayer(*network, m_cfg.getTransportLayer(), 0 /* Do not take over ownership */);
        if (cond.bad())
        {
          DCMNET_ERROR("DcmBaseSCPPool: Error setting secured transport layer: " << cond.text());
          ASC_dropNetwork(network);
        }
      }
    }
    return cond;
}

/* *********************************************************************** */
/*                        DcmBaseSCPPool::BaseSCPWorker class              */
/* *********************************************************************** */

DcmBaseSCPPool::DcmBaseSCPWorker::DcmBaseSCPWorker(DcmBaseSCPPool& pool)
  : m_pool(pool),
    m_assoc(NULL)
{
}

// ----------------------------------------------------------------------------

DcmBaseSCPPool::DcmBaseSCPWorker::~DcmBaseSCPWorker()
{
  // do nothing
}

// ----------------------------------------------------------------------------

OFCondition DcmBaseSCPPool::DcmBaseSCPWorker::setAssociation(T_ASC_Association* assoc)
{
  if (busy())
    return NET_EC_AlreadyConnected;

  if ( (m_assoc != NULL) || (assoc == NULL) )
    return DIMSE_ILLEGALASSOCIATION;

  m_assoc = assoc;
  return EC_Normal;
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::DcmBaseSCPWorker::run()
{
  OFCondition result;
  if(!m_assoc)
  {
    DCMNET_ERROR("DcmBaseSCPPool: Worker thread #" << threadID() << " received run command but has no association, exiting");
    m_pool.notifyThreadExit(this, ASC_NULLKEY);
    thread_exit();
  }
  else
  {
    T_ASC_Association *param = m_assoc;
    m_assoc = NULL;
    result = workerListen(param);
    DCMNET_DEBUG("DcmBaseSCPPool: Worker thread #" << threadID() << " returns with code: " << result.text() );
  }
  m_pool.notifyThreadExit(this, result);
  thread_exit();
  return;
}

// ----------------------------------------------------------------------------

void DcmBaseSCPPool::DcmBaseSCPWorker::exit()
{
  thread_exit();
}

#endif // WITH_THREADS
