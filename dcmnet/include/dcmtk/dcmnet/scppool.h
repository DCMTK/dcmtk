/*
 *
 *  Copyright (C) 2012-2014, OFFIS e.V.
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
 *           worker threads that each are waiting to take over a single incoming
 *           association. Thus, the pool can serve as many associations
 *           simultaneously as the number of threads it is configured to create.
 *
 */

#ifndef SCPPOOL_H
#define SCPPOOL_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#ifdef WITH_THREADS // Without threads this does not make sense...

#include "dcmtk/ofstd/ofthread.h"
#include "dcmtk/dcmnet/scpthrd.h"
#include "dcmtk/dcmnet/scpcfg.h"
#include "dcmtk/dcmnet/assoc.h"

/** Base class for implementing an SCP pool with one thread listening for
 *  incoming TCP/IP connections and spawning a number of SCP worker threads
 *  that handle the incoming DICOM association on that connection. This base
 *  class is abstract.
 */
class DCMTK_DCMNET_EXPORT DcmBaseSCPPool
{
public:

  /** Abstract base class that handles forwarding the configuration and
   *  T_ASC_Association to the actual worker class for each worker thread.
   */
  class DCMTK_DCMNET_EXPORT DcmBaseSCPWorker : public OFThread
  {
    public:

      /** Virtual Destructor
       */
      virtual ~DcmBaseSCPWorker();

      /** Set the association that should be handled by the worker thread.
       *  This must happen *before* actually calling run() (i.e. start()) on
       *  the worker.
       *  @param assoc The association that should be handled by the worker.
       *  @return EC_Normal if OK, error code otherwise. An error may occur
       *          if the the function was called before with a valid
       *          association, or if the given association is NULL.
       */
      virtual OFCondition setAssociation(T_ASC_Association* assoc);

      /** Set SCP configuration that should be used by the worker in order
       *  to handle incoming association requests (presentation contexts, etc.).
       *  @param config A DcmSharedSCPConfig object to be used by this worker.
       *  @return EC_Normal, if configuration is accepted, error code
       *          otherwise.
       */
      virtual OFCondition setSharedConfig(const DcmSharedSCPConfig& config) = 0;

      /** Check whether worker is busy.
       *  @return OFTrue if worker is busy, OFFalse otherwise.
       */
      virtual OFBool busy() = 0;

      /** Ends and exits worker thread. Call will not return.
       */
      virtual void exit();

    protected:

      /** Protected constructor which is called within the friend class
       *  DcmSCPWorkerFactory in order to create a worker.
       *  @param pool Handle to the SCP pool in order to inform pool
       *         about exiting the underlying thread, etc.
       */
      DcmBaseSCPWorker(DcmBaseSCPPool& pool);

      /** Overwrites run() function provided by OFThread. Is automatically
       *  executed when start() is called (also provided by OFThread).
       */
      virtual void run();

      /** Starts listening on the given association.
       *  Note that the underlying TCP connection must be already accepted,
       *  i.e. ASC_receiveAssociation() must have been called already
       *  on the association; after that, this listen() function kicks in
       *  and has to take over full responsibility of the association,
       *  including accepting it, refusing it, handling incoming DIMSE
       *  messages, freeing memory of the T_ASC_Association struct,
       *  and the like.
       *  @param assoc Pointer to the association that should be handled.
       *         Must not be NULL.
       *  @return EC_Normal if association was handled properly (i.e.
       *          was handled, refused, ... Only in case of connection
       *          or messaging errors, an error code will be returned
       *          instead.
       */
      virtual OFCondition workerListen(T_ASC_Association* const assoc) = 0;

      /// Reference to pool in order to notify pool if thread exits, etc.
      DcmBaseSCPPool& m_pool;

      /// Temporarily stores association parameter to be available for the
      /// run() method. run() will set the pointer immediately to NULL; the
      /// deletion takes place inside the actual worker m_worker which starts
      /// its operation afterwards in run().
      T_ASC_Association* m_assoc;
  };

  // Needed to keep MS VC6 happy
  friend class DcmBaseSCPWorker;

  /** Virtual destructor, frees internal memory.
   */
  virtual ~DcmBaseSCPPool();

  /** Set the number of maximum permitted connections, i.e.\ threads/workers.
   *  @param maxWorkers Number of threads permitted to exist within pool.
   */
  virtual void setMaxThreads(const Uint16 maxWorkers);

  /** Get number of maximum permitted connections, i.e.\ threads/workers.
   *  @return Number of threads permitted to exist within pool.
   */
  virtual Uint16 getMaxThreads();

  /** Get number of currently active connections.
   *  @param onlyBusy Return only number of those workers that are busy with a
   *         connection and not idle, if OFTrue.
   *  @return Number of connections currently handled within pool
   */
  virtual size_t numThreads(const OFBool onlyBusy);

  /** Listen for incoming association requests. For each incoming request, a
   *  new thread is started if number of maximum threads is not reached yet.
   *  @return DUL_NOASSOCIATIONREQUEST if no connection is requested during
   *          timeout. Returns other error code if serious error occurs during
   *          listening. Will not return EC_Normal since listens forever if
   *          no timeout occurs.
   */
  virtual OFCondition listen();

  /** Return handle to the SCP configuration that is used to configure how to
   *  handle incoming associations. For the pool, e.g. by providing settings
   *  for TCP connection timeout, and for the workers, e.g. by configuration
   *  presentation contexts and the like.
   *  @return The SCP configuration(s).
   */
  virtual DcmSCPConfig& getConfig();

  /** If enabled, the pool will return from listening for incoming requests
   *  as soon as the last worker is idle, i.e.\ no worker is handling a DICOM
   *  association any more.
   */
  virtual void stopAfterCurrentAssociations();

protected:

  /** Constructor. Initializes internal member variables.
   */
  DcmBaseSCPPool();

  /** Create SCP worker.
   *  @return The worker created
   */
  virtual DcmBaseSCPWorker* createSCPWorker() = 0;

  /** Try to find worker to run the association. If worker could be found, a
   *  side effect is that assoc is set to NULL.
   *  @param assoc The association to be run. Must be not NULL.
   *  @param sharedConfig A DcmSharedSCPConfig object to be used by the worker.
   *  @return EC_Normal if worker could be found and runs the association,
   *          an error code otherwise.
   */
  OFCondition runAssociation(T_ASC_Association* assoc,
                             const DcmSharedSCPConfig& sharedConfig);

  /** Drops association and clears internal structures to free memory
   *  @param assoc The association to free
   */
  virtual void dropAndDestroyAssociation(T_ASC_Association* assoc);

  /** Reject association using the given reason, e.g.\ because maximum number
   *  of connections is currently already served.
   *  @param assoc The association to reject
   *  @param reason The rejection reason
   */
  void rejectAssociation(T_ASC_Association* assoc,
                         const T_ASC_RejectParametersReason& reason);

  /** Used by thread to tell pool it has terminated
   *  @param thread The thread that is calling this function and is about to
   *                exit.
   *  @param result The final result of the thread.
   */
  void notifyThreadExit(DcmBaseSCPWorker* thread,
                        OFCondition result);

private:

  /// Possible run modes of pool
  enum runmode
  {
    /// Listen for new connections
    LISTEN,
    /// Reserved for later use
    STOP,
    /// Shutting down worker threads
    SHUTDOWN
  };

  /// Mutex that guards the list of busy and idle workers
  OFMutex m_criticalSection;
  /// List of all workers running a connection
  OFList<DcmBaseSCPWorker*> m_workersBusy;
  /// List of all workers being idle, i.e.\ not running a connection
  OFList<DcmBaseSCPWorker*> m_workersIdle;

  /// SCP configuration to be used by pool and all workers
  DcmSCPConfig m_cfg;
  /// Maximum number of workers that can exist at a time. Thus limits the
  /// maximum number of connections for the pool since every worker serves
  /// one connection at a time.
  Uint16 m_maxWorkers;

  // Not implemented yet: Can be helpful if all workers are busy but incoming
  // associations should then not be rejected immediately but only after a
  // specific timeout
  // Uint16 m_workersBusyTimeout;

  // Not implemented yet: list of associations that are waiting for a worker
  // becoming available
  // OFList<T_ASC_Association*> m_waiting;

  /// Current run mode of pool
  runmode m_runMode;
};

/** Implementation of DICOM SCP server pool. The pool waits for incoming
 *  TCP/IP connection requests, accepts them on TCP/IP level and hands the
 *  connection to a worker thread. The maximum number of worker threads, i.e.
 *  simultaneous connections, is configurable. The default is 5. At the moment,
 *  if no free worker slots are available, an incoming request is rejected with
 *  the error "local limit exceeded", i.e. those requests are not queued. This
 *  behaviour might change in the future.
 *  @tparam SCP the service class provider to be instantiated for each request,
 *    should follow the @ref SCPThread_Concept.
 *  @tparam SCPPool the base SCP pool class to use. Use this parameter if you
 *    want to use a different implementation (probably derived from
 *    DcmBaseSCPPool) as base class for implementing the SCP pool.
 *  @tparam BaseSCPWorker the base SCP worker class to use. Use this parameter
 *    if you want to use a different implementation of the SCP worker, e.g. for
 *    using processes instead of threads or sharing a single thread for
 *    multiple workers.
 */
template<typename SCP = DcmThreadSCP, typename SCPPool = DcmBaseSCPPool, typename BaseSCPWorker = OFTypename SCPPool::DcmBaseSCPWorker>
class DcmSCPPool : public SCPPool
{
public:

    /** Default construct a DcmSCPPool object.
     */
    DcmSCPPool() : SCPPool()
    {
    }

private:

    /** Helper class to use any class as an SCPWorker as long as it is a model
     *  of the @ref SCPThread_Concept.
     */
    struct SCPWorker : public BaseSCPWorker
                     , private SCP
    {
        /** Construct a SCPWorker for being used by the given DcmSCPPool.
         *  @param pool the DcmSCPPool object this Worker belongs to.
         */
        SCPWorker(DcmSCPPool& pool)
          : BaseSCPWorker(pool)
          , SCP()
        {
        }

        /** Set the shared configuration for this worker.
         *  @param config a DcmSharedSCPConfig object to be used by this worker.
         *  @return the result of the underlying SCP implementation.
         */
        virtual OFCondition setSharedConfig(const DcmSharedSCPConfig& config)
        {
            return SCP::setSharedConfig(config);
        }

        /** Determine if the Worker is currently handling any request.
         *  @return OFTrue if the underlying SCP implementation is currently
         *          handling a request, OFFalse otherwise.
         */
        virtual OFBool busy()
        {
            return SCP::isConnected();
        }

        /** Perform SCP's duties on an already accepted (TCP/IP) connection.
         *  @param assoc The association to be run
         *  @return Returns EC_Normal if negotiation could take place and no
         *          serious network error has occurred or the given association
         *          is invalid. Error code otherwise.
         */
        virtual OFCondition workerListen(T_ASC_Association* const assoc)
        {
            return SCP::run(assoc);
        }
    };

    /** Create a worker to be used for handling a request.
     *  @return a pointer to a newly created SCP worker.
     */
    virtual BaseSCPWorker* createSCPWorker()
    {
        return new SCPWorker(*this);
    }
};


#endif // WITH_THREADS

#endif // SCPPOOL_H
