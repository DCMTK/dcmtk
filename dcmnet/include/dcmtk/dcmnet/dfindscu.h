/*
 *
 *  Copyright (C) 1994-2015, OFFIS e.V.
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
 *  Author:  Marco Eichelberg / Andrew Hewett
 *
 *  Purpose: Classes for Query/Retrieve Service Class User (C-FIND operation)
 *
 */

#ifndef DFINDSCU_H
#define DFINDSCU_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"    /* for class OFCondition */
#include "dcmtk/dcmdata/dcxfer.h"  /* for E_TransferSyntax */
#include "dcmtk/dcmnet/dimse.h"    /* for T_DIMSE_BlockingMode */

// forward declarations of classes and structs
class DcmDataset;
class DcmTransportLayer;
class OFConsoleApplication;
struct T_ASC_Association;
struct T_ASC_Parameters;
struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_FindRSP;

/** Abstract base class for Find SCU callbacks. During a C-FIND operation, the
 *  callback() method of a callback handler object derived from this class is
 *  called once for each incoming C-FIND-RSP message. The callback method has
 *  access to the original C-FIND-RQ message (but not the request dataset), the
 *  current C-FIND-RSP message including its dataset, the number of the current
 *  request, the association over which the request is received and the
 *  presentation context ID. The callback is needed to process the incoming
 *  message (e.g., display on screen, add to some list, store to file). The
 *  callback may also issue a C-FIND-CANCEL message if needed. Implementations
 *  may provide their own callbacks, which must be derived from this base
 *  class.
 */
class DCMTK_DCMNET_EXPORT DcmFindSCUCallback
{
public:

  /// default constructor
  DcmFindSCUCallback();

  /// destructor
  virtual ~DcmFindSCUCallback() {}

  /** callback method that is called once for each incoming C-FIND-RSP message.
   *  @param request DIMSE command of the original C-FIND request
   *  @param responseCount number of current response
   *  @param rsp DIMSE command of incoming C-FIND response
   *  @param responseIdentifiers dataset of incoming C-FIND response
   */
  virtual void callback(
        T_DIMSE_C_FindRQ *request,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers) = 0;

  /** assigns a value to member variable assoc_. Used by FindSCU code
   *  (class DcmFindSCU) to store a pointer to the current association
   *  before the callback object is used.
   *  @param assoc pointer to current association
   */
  void setAssociation(T_ASC_Association *assoc);

  /** assigns a value to member variable presId_. Used by FindSCU code
   *  (class DcmFindSCU) to store the current presentation context ID
   *  before the callback object is used.
   *  @param presId current presentation context ID
   */
  void setPresentationContextID(T_ASC_PresentationContextID presId);

protected: /* the two member variables are protected and can be accessed from derived classes */

   /// pointer to current association. Will contain valid value when callback() is called.
   T_ASC_Association *assoc_;

   /// current presentation context ID. Will contain valid value when callback() is called.
   T_ASC_PresentationContextID presId_;

private:

  /// private undefined copy constructor
  DcmFindSCUCallback(const DcmFindSCUCallback& other);

  /// private undefined assignment operator
  DcmFindSCUCallback& operator=(const DcmFindSCUCallback& other);

};


/** Default implementation of FindSCU callback class. This implementation is
 *  used when no explicit callback is passed by the user, e.g. in the findscu tool.
 */
class DCMTK_DCMNET_EXPORT DcmFindSCUDefaultCallback: public DcmFindSCUCallback
{
public:
  /** constructor
   *  @param extractResponsesToFile if true, C-FIND-RSP datasets will be stored as DICOM files
   *  @param cancelAfterNResponses if non-negative, a C-FIND-CANCEL will be issued after the
   *    given number of incoming C-FIND-RSP messages
   *  @param outputDirectory directory used to store the output files (e.g. response messages).
   *    If NULL, the current directory is used.
   */
  DcmFindSCUDefaultCallback(
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    const char *outputDirectory = NULL);

  /// destructor
  virtual ~DcmFindSCUDefaultCallback() {}

  /** callback method that is called once for each incoming C-FIND-RSP message.
   *  @param request DIMSE command of the original C-FIND request
   *  @param responseCount number of current response
   *  @param rsp DIMSE command of incoming C-FIND response
   *  @param responseIdentifiers dataset of incoming C-FIND response
   */
  virtual void callback(
        T_DIMSE_C_FindRQ *request,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers);

private:

   /// if true, C-FIND-RSP datasets will be stored as DICOM files
   OFBool extractResponsesToFile_;

   /// if non-negative, a C-FIND-CANCEL will be issued after the given number of incoming C-FIND-RSP messages
   int cancelAfterNResponses_;

   /// directory used to store the output files (e.g. response messages)
   OFString outputDirectory_;
};


/** This class implements a complete DICOM C-FIND SCU, including association set-up, execution of the
 *  C-FIND transaction including processing of results, and closing of the association. By default,
 *  incoming C-FIND-RSP messages will be displayed on console and, optionally, also stored in files.
 *  By providing a user-defined callback, other types of processing are possible.
 */
class DCMTK_DCMNET_EXPORT DcmFindSCU
{
public:

  /// constructor, does not execute any network-related code
  DcmFindSCU();

  /// destructor. Destroys network structure if not done already.
  virtual ~DcmFindSCU();

  /** initialize the network structure. This should be done only once.
   *  @param acse_timeout timeout for ACSE operations, in seconds
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition initializeNetwork(int acse_timeout);

  /** enable user-defined transport layer. This method is needed when
   *  the network association should use a non-default transport layer
   *  (e.g. a TLS connection). In this case a fully initialized transport
   *  layer object must be passed with this call after a call to
   *  initializeNetwork(), but prior to any call to performQuery().
   *  The transport layer object will not be deleted by this class and
   *  must remain alive until this object is deleted or a new transport
   *  layer is set.
   *  @param tLayer pointer to transport layer object
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition setTransportLayer(DcmTransportLayer *tLayer);

  /** destroy network struct. This should be done only once.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition dropNetwork();

  /** main worker method that negotiates an association, executes one or more
   *  C-FIND-RQ transactions, processes the responses and closes the association
   *  once everything is finished (or an error has occurred).
   *  @param peer hostname or IP address of peer SCP host
   *  @param port TCP port number of peer SCP host
   *  @param ourTitle calling AE title
   *  @param peerTitle called AE title
   *  @param abstractSyntax SOP Class UID or Meta SOP Class UID of service
   *  @param preferredTransferSyntax May be Unknown, Implicit Little Endian, or any of the
   *    two uncompressed explicit VR transfer syntaxes. By default (unknown), local endian
   *    explicit VR is proposed first, followed by opposite endian explicit VR, followed by
   *    implicit VR. This behavior can be modified by explicitly specifying the preferred
   *    explicit VR transfer syntax. With Little Endian Implicit, only Implicit VR is proposed.
   *  @param blockMode DIMSE blocking mode
   *  @param dimse_timeout timeout for DIMSE operations (in seconds)
   *  @param maxReceivePDULength limit the maximum PDU size for incoming PDUs to the given value.
   *    This value should be less than or equal to ASC_DEFAULTMAXPDU, and is usually identical
   *    to ASC_DEFAULTMAXPDU (other values are only useful for debugging purposes).
   *  @param secureConnection this flag, if true, requests a secure TLS connection to be used
   *    instead of a normal connection. This will only work if DCMTK has been compiled with
   *    OpenSSL support (WITH_OPENSSL) and if a transport layer object supporting secure
   *    connections has been set with setTransportLayer() prior to this call.
   *  @param abortAssociation abort association instead of releasing it (for debugging purposes)
   *  @param repeatCount number of times this query should be repeated
   *    (for debugging purposes, works only with default callback)
   *  @param extractResponsesToFile if true, extract incoming response messages to file
   *    (works only with default callback)
   *  @param cancelAfterNResponses issue C-FIND-CANCEL after given number of responses
   *    (works only with default callback)
   *  @param overrideKeys list of keys/paths that override those in the query files, if any.
   *    Either the list of query files or override keys or both should be non-empty, because the
   *    query dataset will be empty otherwise. For path syntax see DcmPath.
   *  @param callback user-provided non-default callback handler object.
   *    For default callback, pass NULL.
   *  @param fileNameList list of query files. Each file is expected to be a DICOM file
   *    containing a dataset that is used as a query, possibly modified by override keys, if any.
   *    This parameter, if non-NULL, points to a list of filenames (paths).
   *  @param outputDirectory directory used to store the output files (e.g. response messages).
   *    If NULL, the current directory is used.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition performQuery(
    const char *peer,
    unsigned int port,
    const char *ourTitle,
    const char *peerTitle,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    Uint32 maxReceivePDULength,
    OFBool secureConnection,
    OFBool abortAssociation,
    unsigned int repeatCount,
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback = NULL,
    OFList<OFString> *fileNameList = NULL,
    const char *outputDirectory = NULL);

  /** static helper function that writes the content of the given dataset
   *  into a DICOM file (using the DICOM file format with metaheader).
   *  This method produces a temporary copy of the dataset and should, therefore,
   *  not be used with very large datasets.
   *  @param ofname filename to write
   *  @param dataset dataset to store in file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFBool writeToFile(const char* ofname, DcmDataset *dataset);

private:

  /** add presentation context for given abstract syntax and given preferred transfer syntax
   *  to the ACSE parameter struct.
   *  @param params ACSE parameter struct to be modified
   *  @param abstractSyntax SOP Class UID or Meta SOP Class UID of service
   *  @param preferredTransferSyntax May be Unknown, Implicit Little Endian, or any of the
   *    two uncompressed explicit VR transfer syntaxes. By default (unknown), local endian
   *    explicit VR is proposed first, followed by opposite endian explicit VR, followed by
   *    implicit VR. This behavior can be modified by explicitly specifying the preferred
   *    explicit VR transfer syntax. With Little Endian Implicit, only Implicit VR is proposed.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition addPresentationContext(
    T_ASC_Parameters *params,
    const char *abstractSyntax,
    E_TransferSyntax preferredTransferSyntax);

  /** perform a single C-FIND transaction on association that is already open.
   *  @param assoc DIMSE association
   *  @param fname file name for the query file to be used with this query. May be NULL.
   *  @param repeatCount number of times this query should be repeated
   *    (for debugging purposes, works only with default callback)
   *  @param abstractSyntax SOP Class UID or Meta SOP Class UID of service
   *  @param blockMode DIMSE blocking mode
   *  @param dimse_timeout timeout for DIMSE operations (in seconds)
   *  @param extractResponsesToFile if true, extract incoming response messages to file
   *    (works only with default callback)
   *  @param cancelAfterNResponses issue C-FIND-CANCEL after given number of responses
   *    (works only with default callback)
   *  @param overrideKeys list of keys/paths that override those in the query files, if any.
   *    Either the list of query files or override keys or both should be non-empty, because
   *    the query dataset will be empty otherwise. For path syntax see DcmPath.
   *  @param callback user-provided non-default callback handler object.
   *    For default callback, pass NULL.
   *  @param outputDirectory directory used to store the output files (e.g. response messages).
   *    If NULL, the current directory is used.
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition findSCU(
    T_ASC_Association * assoc,
    const char *fname,
    int repeatCount,
    const char *abstractSyntax,
    T_DIMSE_BlockingMode blockMode,
    int dimse_timeout,
    OFBool extractResponsesToFile,
    int cancelAfterNResponses,
    OFList<OFString> *overrideKeys,
    DcmFindSCUCallback *callback = NULL,
    const char *outputDirectory = NULL) const;

  /// Private undefined copy constructor
  DcmFindSCU(const DcmFindSCU& other);

  /// Private undefined assignment operator
  DcmFindSCU& operator=(const DcmFindSCU& other);

private:

  /// pointer to network structure
  T_ASC_Network *net_;

};

#endif
