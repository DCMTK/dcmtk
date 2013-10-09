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
 *  Author:  Michael Onken
 *
 *  Purpose: Class for implementing a threaded Service Class Provider worker.
 *
 */

#ifndef SCPTHRD_H
#define SCPTHRD_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/scp.h"


/** Base class for implementing a DICOM Service Class Provider (SCP) that can serve
 *  as a worker in a thread pool, by offering functionality to run an association
 *  from an already accepted TCP/IP connection.
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 */
class DCMTK_DCMNET_EXPORT DcmThreadSCP : public DcmSCP
{

public:

  /** Constructor. Initializes internal member variables.
   */
  DcmThreadSCP();

  /** Virtual destructor, frees internal memory.
   */
  virtual ~DcmThreadSCP();

  /** Handle an already established (on TCP/IP level) connection.
   *  This function is used when in a thread pool context where a central
   *  thread receives TCP/IP connections and starts a DcmSCP worker in order
   *  to handle the rest of the connection (DICOM negotiation and DIMSE
   *  messaging).
   *  @param incomingAssoc the association of the connection.
   *  @return If listening fails, e.g. because the given association is not
   *          valid or any serious network error occurs, an error is reported.
   *          In all other cases, e.g. no presentation contexts could be
   *          negotiated with the requesting SCU, then EC_Normal is returned.
   */
  virtual OFCondition run(T_ASC_Association* incomingAssoc);

  /** Get access to the DcmSharedSCPConfig object. The shared configuration can be used
   *  to provide other SCPs with the same configuration without the need to copy it.
   *  @return a reference to the DcmSharedSCPConfig object used by this DcmSCP object.
   */
  virtual DcmSharedSCPConfig& getSharedConfig();

  /** Set the DcmSharedSCPConfig object to use for configuring this DcmSCP object.
   *  @param config Set the shared configuration to be used (no deep copy is performed).
   *  @return EC_Normal if configuration can be used, error code otherwise.
   */
  virtual OFCondition setSharedConfig(const DcmSharedSCPConfig& config);

private:

  /** Private undefined copy constructor. Shall never be called.
   *  @param src Source object
   */
  DcmThreadSCP(const DcmThreadSCP &src);

  /** Private undefined assignment operator. Shall never be called.
   *  @param src Source object
   *  @return Reference to this
   */
  DcmThreadSCP &operator=(const DcmThreadSCP &src);

};

#endif // SCPTHRD_H
