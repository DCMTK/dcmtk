/*
 *
 *  Copyright (C) 2009-2010, OFFIS e.V.
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
 *  Purpose: General SCP class that can be used to implement derived SCP
 *           applications.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:22 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SCP_H
#define SCP_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dctk.h"     /* Covers most common dcmdata classes */
#include "dcmtk/dcmnet/dimse.h"     /* DIMSE network layer */
#include "dcmtk/dcmnet/dcasccff.h"  /* For reading a association config file */
#include "dcmtk/dcmnet/dcasccfg.h"  /* For holding association cfg file infos */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif


/** Structure representing single process in multi-process mode
 */
struct DcmProcessSlotType
{
  /// Name of peer
  DIC_NODENAME peerName;
  /// Calling AE title
  DIC_AE callingAETitle;
  /// Called AE title
  DIC_AE calledAETitle;
  /// Process ID
  int processId;
  /// Start time
  time_t startTime;
  /// Indicator if process has storage ability
  OFBool hasStorageAbility;
};

/** Action codes that can be given to DcmSCP to control behaviour during SCP's operation.
 *  Different hooks permit jumping into different phases of SCP operation.
 */
enum DcmSCPActionType
{
  /// No action defined
  DCMSCP_ACTION_UNDEFINED,
  /// Tell SCP to refuse association
  DCMSCP_ACTION_REFUSE_ASSOCIATION
};

/** Codes denoting a reason for refusing an association
 */
enum DcmRefuseReasonType
{
  /// Too many associations (SCP cannot handle a further association)
  DCMSCP_TOO_MANY_ASSOCIATIONS,
  /// Forking a new SCP process failed
  DCMSCP_CANNOT_FORK,
  /// Refusing association because of bad application context name
  DCMSCP_BAD_APPLICATION_CONTEXT_NAME,
  /// Refusing association because of unacceppted AE name
  DCMSCP_BAD_APPLICATION_ENTITY_SERVICE,
  /// Refusing association because SCP was forced to do so
  DCMSCP_FORCED,
  /// Refusing association because of missing Implementation Class UID
  DCMSCP_NO_IMPLEMENTATION_CLASS_UID,
  /// Refusing association because proposed Presentation Context is supported
  DCMSCP_NO_PRESENTATION_CONTEXTS,
  /// Refusing association because of internal error
  DCMSCP_INTERNAL_ERROR
};

/** Structure representing a single Presentation Context. Fields "reserved" and "result"
 *  not included from DUL_PRESENTATIONCONTEXT which served as the blueprint for this
 *  structure.
 */
struct DcmPresentationContextInfo
{
  /// Presentation Context ID as proposed by SCU
  Uint8 presentationContextID;
  /// Abstract Syntax name (UID) as proposed by SCU
  OFString abstractSyntax;
  /// SCP role as proposed from SCU
  Uint8 proposedSCRole;
  /// Role acccepted by SCP for this Presentation Context
  Uint8 acceptedSCRole;
  /// Transfer Syntax accepted for this Presentation Context (UID)
  OFString acceptedTransferSyntax;
  // Fields "reserved" and "result" not included from DUL_PRESENTATIONCONTEXT
};


/** Base class for implementing a DICOM Service Class Provider (SCP). Derived classes can
 *  add the presentation contexts they want to support, set further parameters (port, peer
 *  hostname, etc. as desired) and then call DcmSCP's listen() method to start the server.
 *  For incoming associations and DIMSE messages, a derived class can define the behaviour
 *  of the server. The DcmSCP base class is capable of responding to C-ECHO requests
 *  (Verification SOP Class).
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 */
class DcmSCP
{

public:

  /** Constructor. Initializies internal member variables.
   */
  DcmSCP();

  /** Virtual destructor, frees internal memory.
   */
  virtual ~DcmSCP();

#ifdef _WIN32
  /** Marks this SCP as being a forked child under Windows, i.e.\ it handles an association
   *  received by the parent.
   *  @return EC_Normal if marking was successful, an error code otherwise
   */
  OFCondition markAsForkedChild();
#endif

  /** Starts providing the implemented services to SCUs.
   *  After calling this method the SCP is listening for connection requests.
   *  @return The result. Function usually only returns in case of errors.
   */
  virtual OFCondition listen();


  /* ************************************************************* */
  /*             Set methods for configuring SCP behaviour         */
  /* ************************************************************* */

  /** Add abstract syntax to presentation contexts the SCP is able to negotiate with SCUs.
   *  @param abstractSyntax [in] The UID of the abstract syntax (e.g. SOP class) to add
   *  @param xferSyntaxes   [in] List of transfer syntaxes (UIDs) that should be supported
   *                             for the given abstract syntax name
   *  @param role           [in] The role to be negotiated
   *  @param profile        [in] The profile the abstract snytax should be added to. The
   *                             default is to add it to the DcmSCP's internal standard
   *                             profile called "DEFAULT".
   *  @return EC_Normal if adding was successful, an error code otherwise
   */
  virtual OFCondition addPresentationContext(const OFString &abstractSyntax,
                                             const OFList<OFString> xferSyntaxes,
                                             const T_ASC_SC_ROLE role = ASC_SC_ROLE_DEFAULT,
                                             const OFString &profile = "DEFAULT");

  /** Set SCP's TCP/IP listening port
   *  @param port [in] The port number to listen on. Note that usually on Unix-like systems
   *                   only root user is permitted to open ports below 1024.
   */
  void setPort(const Uint16 port);

  /** Set AETitle of the server
   *  @param aetitle [in] The AETitle of the server. By default, all SCU association requests
   *                      calling another AETitle will be rejected. This behaviour can be
   *                      changed by using the setRespondWithCalledAETitle() method.
   */
  void setAETitle(const OFString &aetitle);

  /** Set SCP to use the called AETitle from the SCU request for the response, i.e.\ the SCP
   *  will always respond with setting it's own name to the one the SCU used for calling.
   *  Overrides any AETitle eventually set with setAETitle().
   *  @param useCalled [in] If OFTrue, the SCP will use the called AE title from the request
   *                        for responding. DcmSCP's default is OFFalse.
   */
  void setRespondWithCalledAETitle(const OFBool useCalled);

  /** Loads association configuration file
   *  @param assocFile [in] The filename of the association configuration to be loaded. The
   *                        association configuration file must be valid for an SCP.
   *  @return EC_Normal if loading was successful, error otherwise
   */
  virtual OFCondition loadAssociationCfgFile(const OFString &assocFile);

  /** If an association profile should be selected, either by loading an associaton
   *  configuration file or using the addAbstractSyntax() function, one of those can be
   *  selected and checked for validity using this method.
   *  @param profileName [in] The name of the association profile which must be configured
   *                          before being selected here
   *  @return EC_Normal if selecting/checking was successful, an error code otherwise
   */
  virtual OFCondition setAndCheckAssociationProfile(const OFString &profileName);

  /** Force every association request to be refused by SCP, no matter what the SCU is
   *  offering
   *  @param doRefuse [in] If OFTrue, every association is being refused. DcmSCP's default
   *                       is not to refuse every association.
   */
  void forceAssociationRefuse(const OFBool doRefuse);

  /** Set maximum PDU size the SCP is able to receive. This size is sent in associaton
   *  response message to SCU.
   *  @param maxRecPDU [in] The maximum PDU size to use in bytes
   */
  void setMaxReceivePDULength(const Uint32 maxRecPDU);

  /** Enable multi-process mode for SCP. For every incoming association, a new process is
   *  started. For Unix this is done with the fork() command that creates a copy of the
   *  process running and continues executing the code after the fork() called seamlessy.
   *  For Windows CreateProcess() is used given the commandline specified in the function's
   *  parameters. The created process is handled a copy of the open TCP/IP connection and
   *  then is responsible for handling the association. After process fork/creation, DcmSCP
   *  is ready for listening to new connection requests.
   *  @param argc [in] Number of commandline arguments (only Windows)
   *  @param argv [in] Command line (only Windows)
   *  @return EC_Normal, if single process mode could be enabled, an error otherwise
   */
  OFCondition enableMultiProcessMode(int argc = 0,
                                     char *argv[] = NULL);

  /** Set number of maximum simultanous associations. Only works for Unix and with
   *  multi-process mode enabled.
   *  @param maxAssocs [in] Maximum number of simultanous associations
   */
  void setMaxAssociations(const Uint16 maxAssocs);

  /** Set whether DIMSE messaging should be blocking or non-blocking. In non-blocking mode,
   *  the networking routines will wait for DIMSE messages for the specified DIMSE timeout
   *  time, see setDIMSETimeout() function. In blocking mode, no timeout is set but the
   *  operating system's network routines will be used to read from the socket for new data.
   *  In the worst case, this may be a long time until that call returns. The default of
   *  DcmSCP is blocking mode.
   *  @param blockingMode [in] Either DIMSE_BLOCKING for blocking mode or DIMSE_NONBLOCKING
   *                           for non-blocking mode
   */
  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode);

  /** Set the timeout to be waited for incoming DIMSE message packets. This is only relevant
   *  for DIMSE blocking mode messaging (see also setDIMSEBlockingMode().
   *  @param dimseTimeout [in] DIMSE receive timeout in seconds
   */
  void setDIMSETimeout(const Uint32 dimseTimeout);

  /** Set the timeout used during ACSE messaging protocol.
   *  @param acseTimeout [in] ACSE timeout in seconds.
   */
  void setACSETimeout(const Uint32 acseTimeout);

  /** Set whether to show presentation contexts in verbose or debug mode
   *  @param mode [in] Show presentation contexts in verbose mode if OFTrue. By default, the
   *                   presentation contexts are shown in debug mode.
   */
  void setVerbosePCMode(const OFBool mode);

  /* Get methods for SCP settings */

  /** Returns TCP/IP port number SCP listens for new connection requests
   *  @return The port number
   */
  Uint16 getPort() const;

  /** Returns SCP's own AE title. Only used if the SCP is not configured to respond with the
   *  called AE Title the SCU uses for association negotiation, see setRespondWithCalledAETitle().
   *  @return The configured AETitle
   */
  const OFString &getAETitle() const;

  /** Returns whether SCP uses the called AE Title from SCU requests to respond to connection
   *  requests instead of a configured AE Title
   *  @return OFTrue, if the SCU's calling AE Title is utilized, OFFalse otherwise
   */
  OFBool getRespondWithCalledAETitle() const;

  /** Returns whether SCP should refuse any association request no matter what the SCU proposes
   *  @return OFTrue, if SCP is configured to refuse every association
   */
  OFBool getRefuseAssociation() const;

  /** Returns maximum PDU length configured to be received by SCP
   *  @return Maximum PDU length in bytes
   */
  Uint32 getMaxReceivePDULength() const;

  /** Returns whether SCP is running in single or multi-process mode
   *  @return OFTrue, if SCP is running in single process mode, otherwise returns OFFalse
   */
  OFBool getSingleProcess() const;

  /** Returns number of maximum simultanous connections permitted. Only makes sense for Unix
   *  operating systems. For Windows, the number of maximum connections is either "unlimited"
   *  or 1. Use getSingleProcess() in that case to find out if SCP is permitting more than
   *  one connection.
   *  @return Under Unix-like systems, returns number of maximum associations (=processes)
   *          permitted
   */
  Uint16 getMaxAssociations() const;

  /** Returns whether receiving of DIMSE messages is done in blocking or unblocking mode
   *  @return DIMSE_BLOCKING if in blocking mode, otherwise DIMSE_NONBLOCKING
   */
  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  /** Returns DIMSE timeout (only applicable in blocking mode)
   *  @return DIMSE timeout in seconds
   */
  Uint32 getDIMSETimeout() const;

  /** Returns ACSE timeout
   *  @return ACSE timeout in seconds
   */
  Uint32 getACSETimeout() const;

  /** Returns the verbose presentation context mode configured specifying whether details on
   *  the presentation contexts (negotiated during association setup) should be shown in
   *  verbose or debug mode. The latter is the default.
   *  @return The verbose presentation context mode configured
   */
  OFBool getVerbosePCMode() const;

  /* Get information about current association */

  /** Returns whether SCP is currently connected. If in multi-process mode, the "father"
   *  process should always return false here, because connection is always handled by child
   *  process.
   *  @return OFTrue, if SCP is currently connected to calling SCU
   */
  OFBool isConnected() const;

  /** Returns number of associations currently running. Only applicable in Unix-like
   *  operating systems. Can only be greater than one when running in multi-process mode.
   *  @return Number of currently running associations
   */
  Uint16 numAssociations() const;

  /** Returns AE Title the SCU used as called AE Title in associaton request
   *  @return AE Title the SCP was called with. Empty string if SCP is currently not
   *          connected.
   */
  OFString getCalledAETitle() const;

  /** Returns AE Title (calling AE Title) the SCU used for association request
   *  @return Calling AE Title of SCU. Empty string if SCP is currently not connected.
   */
  OFString getPeerAETitle() const;

  /** Returns IP address of connected SCU
   *  @return IP address of connected SCU. Empty string if SCP is currently not connected.
   */
  OFString getPeerIP() const;

  /** Returns maximum PDU size the communication peer (i.e.\ the SCU) is able to receive
   *  @return Maximum PDU size the SCU is able to receive. Returns zero if SCP is currently
   *          not connected.
   */
  Uint32 getPeerMaxPDULength() const;


protected:

  /* ***********************************************************************
   *  Functions particularly interesting for overwriting in derived classes
   * ***********************************************************************
   */

  /** Handle incoming command set and react accordingly, e.g.\ sending response via
   *  DIMSE_sendXXXResponse(). The standard handler only knows how to handle an Echo request
   *  by calling handleEchoRequest(). This function is most likely to be implemented by a
   *  derived class implementing a specific SCP behaviour.
   *  @param TODO
   *  @return TODO
   */
  virtual OFCondition handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                            const DcmPresentationContextInfo &presContextInfo);

  /** Overwrite this function to be notified about an incoming association request.
   *  The standard handler only outputs some information to the logger.
   *  @param TODO
   */
  virtual void notifyAssociationRequest(const T_ASC_Parameters &params,
                                        DcmSCPActionType &desiredAction);

  /** Overwrite this function to be notified about an incoming association request.
   *  The standard handler only outputs some information to the logger.
   */
  virtual void notifyAssociationAcknowledge();

  /** Overwrite this function to be notified about an incoming association release request.
   *  The standard handler only outputs some information to the logger.
   */
  virtual void notifyReleaseRequest();

  /** Overwrite this function to be notified about an incoming association abort request.
   *  The standard handler only outputs some information to the logger.
   */
  virtual void notifyAbortRequest();

  /** Overwrite this function to be notified when an association is terminated.
   *  The standard handler only outputs some information to the logger.
   */
  virtual void notifyAssociationTermination();

  /** Overwrite this function to be notified when an association is terminated.
   *  The standard handler only outputs some information to the logger.
   *  @param TODO
   */
  virtual void notifyDIMSEError(const OFCondition &cond);

  /** Overwrite this function to change the behavior of the listen() method. As long as no
   *  severe error occurs and this method returns OFFalse, the listen() method will wait
   *  for incoming associations in an infinite loop.
   *  @return The standard handler always returns OFFalse
   */
  virtual OFBool stopAfterCurrentAssociation();

  /** Respond to storage request
   *  @param presID       [in] The presentation context ID to respond to
   *  @param reqMessage   [in] The C-STORE request that is responded to
   *  @param rspMessage   [in] The C-STORE response to be sent
   *  @param statusDetail [in  The status detail to be sent
   *  @return EC_Normal, if responding was successful, an error code otherwise
   */
  virtual OFCondition sendSTOREResponse(T_ASC_PresentationContextID presID,
                                        T_DIMSE_C_StoreRQ &reqMessage,
                                        T_DIMSE_C_StoreRSP &rspMessage,
                                        DcmDataset *statusDetail);

  /** Standard handler for Verification Service Class (DICOM Echo). Returns echo response
   *  (i.e. whether C-ECHO could be responded to with status success).
   *  @param reqMessage [in] The C-ECHO request message that was received
   *  @param presID     [in] The presentation context to be used. By default, the presentation
   *                         context of the request is used.
   *  @return OFCondition value denoting success or error
   */
  virtual OFCondition handleECHORequest(T_DIMSE_C_EchoRQ &reqMessage,
                                        T_ASC_PresentationContextID presID);

  /** Receives N-EVENT-REPORT request on the currently opened association and sends a
   *  corresponding response. Calls checkEVENTREPORTRequest() in order to determine the
   *  DIMSE status code to be used for the N-EVENT-REPORT response.
   *  @param reqMessage  [in]  The N-EVENT-REPORT request message that was received
   *  @param presID      [in]  The presentation context to be used. By default, the
   *                           presentation context of the request is used.
   *  @param reqDataset  [out] Pointer to the dataset received
   *  @param eventTypeID [out] Event Type ID from the command set received
   *  @return status, EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition handleEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &reqMessage,
                                               T_ASC_PresentationContextID presID,
                                               DcmDataset *&reqDataset,
                                               Uint16 &eventTypeID);

  /** Check given N-EVENT-REPORT request and dataset for validity. This method is called by
   *  handleEVENTREPORTRequest() before sending the response in order to determine the
   *  DIMSE status code to be used for the response message.
   *  @param reqMessage [in] The N-EVENT-REPORT request message data structure
   *  @param reqDataset [in] The N-EVENT-REPORT request dataset received. Might be NULL.
   *  @return DIMSE status code to be used for the N-EVENT-REPORT response.
   *          Always returns STATUS_Success (0). Derived classes should, therefore,
   *          overwrite this method and return a more appropriate value based on the
   *          result of the checks performed.
   */
  virtual Uint16 checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &reqMessage,
                                         DcmDataset *reqDataset);

  /** Function that checks for each association request, whether the combination of calling
   *  and called AE title proposed by the SCU is accepted. The standard behaviour is to
   *  either accept any AE title (if the corresponding option is set) or to only accept the
   *  one set with setAETitle(). In the former case, the function also adopts the SCP's
   *  AETitle to the one used by the SCU. The method can be overwritten to have full control
   *  about which AE title combinations are accepted by the SCP.
   *  @param callingAE [in] The AE title the SCU uses as Calling AE Title
   *  @param calledAE  [in] The AE title the SCU uses as Called AE Title
   *  @return OFTrue if the given AE title is going to be accepted, OFFalse otherwise
   */
  virtual OFBool calledAETitleAccepted(const OFString &callingAE,
                                       const OFString &calledAE);


  /* *********************************************************************
   *  Further functions and member variables
   * *********************************************************************
   */

  /** This function takes care of receiving, negotiating and accepting/refusing an
   *  association request. Additionally, if negotiation was successful, it handles any
   *  incoming DIMSE commands by calling handleAssociation(). An error is only returned, if
   *  something goes wrong. Therefore, refusing an association because of wrong application
   *  context name or no common presentation contexts with the SCU does NOT lead to an error.
   *  @param network [in] Contains network parameters
   *  @return EC_Normal, if everything went fine, an error code otherwise
   */
  virtual OFCondition waitForAssociation(T_ASC_Network *network);

    /** This function takes care of removing items referring to (terminated) subprocess from
     *  the table which stores all subprocess information. This function does not make sense
     *  for Windows operating systems where the SCP does not have any control over
     *  additionally created processes.
     */
  virtual void cleanChildren();

  /** This function checks all presentation contexts proposed by the SCU whether they are
   *  supported or not. It is not an error if no common presentation context could be
   *  identified with the SCU; only issues like problems in memory management etc. are
   *  reported as an error. This function does not send a response message to the SCU. This
   *  is done in other functions.
   *  @return EC_Normal if negotiation was successfully done, an error code otherwise
   */
  virtual OFCondition negotiateAssociation();

  /** This function adds a process to the table that stores process information (only
   *  relevant for multi-process mode under Unix-like systems)
   *  @param pid [in] The process ID of the sub-process which was just started
   */
  virtual void addProcessToTable(int pid);

  /** This function removes one particular item from the table which stores all subprocess
   *  information. The corresponding process item to be deleted will be identified by its
   *  process ID. This function is only applicable for multi-process mode under Unix-like
   *  systems.
   *  @param pid [in] Process ID.
   */
  virtual void removeProcessFromTable(int pid);

  /** This function takes care of refusing an assocation request
   *  @param reason [in] The reason why the association request will be refused and that
   *                     will be reported to the SCU.
   */
  virtual void refuseAssociation(DcmRefuseReasonType reason);

  /** This function takes care of handling the other DICOM application's request. After
   *  having accomplished all necessary steps, the association will be dropped and destroyed.
   */
  virtual void handleAssociation();

  /** Sends a DIMSE command and possibly also a dataset from a data object via network to
   *  another DICOM application
   *  @param presID          [in]  Presentation context ID to be used for message
   *  @param msg             [in]  Structure that represents a certain DIMSE command which
   *                               shall be sent
   *  @param dataObject      [in]  The instance data which shall be sent to the other DICOM
   *                               application; NULL, if there is none
   *  @param callback        [in]  Pointer to a function which shall be called to indicate
   *                               progress
   *  @param callbackContext [in]  Pointer to data which shall be passed to the progress
   *                               indicating function
   *  @param commandSet      [out] If this parameter is not NULL it will return a copy of the
   *                               DIMSE command which is sent to the other DICOM application
   *  @return Returns EC_Normal if sending request was successful, an error code otherwise
   */
  OFCondition sendDIMSEMessage(const T_ASC_PresentationContextID presID,
                               T_DIMSE_Message *msg,
                               DcmDataset *dataObject,
                               DIMSE_ProgressCallback callback,
                               void *callbackContext,
                               DcmDataset **commandSet = NULL);

  /** Receive DIMSE command (excluding dataset!) over the currently open association
   *  @param presID       [out] Contains in the end the ID of the presentation context
   *                            which was specified in the DIMSE command received
   *  @param msg          [out] The message received
   *  @param statusDetail [out] If a non-NULL value is passed this variable will in the end
   *                            contain detailed information with regard to the status
   *                            information which is captured in the status element
   *                            (0000,0900). Note that the value for element (0000,0900) is
   *                            not contained in this return value but in internal msg. For
   *                            details on the structure of this object, see DICOM standard
   *                            part 7, annex C).
   *  @param commandSet   [out] If this parameter is not NULL, it will return a copy of the
   *                            DIMSE command which was received from the other DICOM
   *                            application. The caller is responsible to de-allocate the
   *                            returned object!
   *  @param timeout      [in]  If this parameter is not 0, it specifies the timeout (in
   *                            seconds) to be used for receiving the DIMSE command.
   *                            Otherwise, the default timeout value is used (see
   *                            setDIMSETimeout()).
   *  @return EC_Normal if command could be received successfully, an error code otherwise
   */
  OFCondition receiveDIMSECommand(T_ASC_PresentationContextID *presID,
                                  T_DIMSE_Message *msg,
                                  DcmDataset **statusDetail,
                                  DcmDataset **commandSet = NULL,
                                  const Uint32 timeout = 0);

  /** Receives one dataset (of instance data) via network from another DICOM application
   *  @param presID          [out] Contains in the end the ID of the presentation context
   *                               which was used in the PDVs that were received on the
   *                               network. If the PDVs show different presentation context
   *                               IDs, this function will return an error.
   *  @param dataObject      [out] Contains in the end the information which was received
   *                               over the network
   *  @param callback        [in]  Pointer to a function which shall be called to indicate
   *                               progress
   *  @param callbackContext [in]  Pointer to data which shall be passed to the progress
   *                               indicating function
   *  @return EC_Normal if dataset could be received successfully, an error code otherwise
   */
  OFCondition receiveDIMSEDataset(T_ASC_PresentationContextID *presID,
                                  DcmDataset **dataObject,
                                  DIMSE_ProgressCallback callback,
                                  void *callbackContext);

private:

  /** Private undefined copy constructor. Shall never be called.
   *  @param src Source object
   */
  DcmSCP(const DcmSCP &src);

  /** Private undefined assignment operator. Shall never be called.
   *  @param src Source object
   *  @return Reference to this
   */
  DcmSCP &operator=(const DcmSCP &src);

  /// Current association run by this SCP
  T_ASC_Association *m_assoc;

  /// Association configuration. May be filled from association configuration file or by
  /// adding presentation contexts by calling addPresentationContext() (or both)
  DcmAssociationConfiguration *m_assocConfig;

  /// Profile in association configuration that should be used. By default, a profile
  /// called "DEFAULT" is used.
  OFString m_assocCfgProfileName;

  /// Port on which the SCP is listening for association requests. The default port is 104.
  Uint16 m_port;

  /// AETitle to be used for responding to SCU (default: DCMTK_SCP). This value is not
  /// evaluated if the the SCP is configured to respond to any assocation requests with the
  /// name the SCU used as Called AE Title (which is the SCP's default behaviour); see
  /// setRespondWithCalledAETitle().
  OFString m_aetitle;

  /// Indicates if the application shall refuse any association attempt regardless of what
  /// the SCU proposes.
  OFBool m_refuseAssociation;

  /// Maximum PDU size the SCP is able to receive. This value is sent to the SCU during
  /// association negotiation.
  Uint32 m_maxReceivePDULength;

  /// Indicates if SCP is run in single process mode or not. In multi-process mode, the SCP
  /// starts a new process for any incoming association request. The association is then
  /// completetly handled by the "child" process while the parent process keeps listening for
  /// new associations. Under Unix, multi-process mode uses the fork command to spawn a
  /// process which is an exact copy of the parent and continues code execution after the
  /// fork command. For Windows, the programmer of the derived class is responsible for
  /// setting the command line needed for the internal "CreateProcess" call. The commandline
  /// arguments are specified with function enableMultiProcessMode().
  OFBool m_singleProcess;

  /// Indicates, that this process was spawn as child from a parent process needed for
  /// multiprocess mode under Windows operating systems.
  OFBool m_forkedChild;

#ifdef _WIN32
  /// Number of arguments in commandline, needed for multiprocess mode on Windows sytems
  int m_cmd_argc;

  /// Complete command line, needed for multiprocess mode on Windows sytems
  char **m_cmd_argv;
#endif

  /// Maximum number of association for multi-process mode. This member is only evaluated
  /// under Unix. For Windows there is no mechanism to restrict the number of simultanous
  /// associations in multi-process mode, thus only permitting "unlimited" associations.
  Uint16 m_maxAssociations;

  /// Blocking mode for DIMSE operations. If DIMSE non-blocking mode is enabled, the SCP is
  /// wating for new DIMSE data a specific (m_dimseTimeout) amount of time and then returns
  /// if not data arrives. In blocking mode the SCP is calling the underlying operating
  /// system function for receiving data from the socket directly, which may return after a
  /// very long time, depending on the system's network configuration.
  T_DIMSE_BlockingMode m_blockMode;

  /// Timeout for DIMSE operations in seconds. Maximum time in DIMSE non-blocking mode to
  /// wait for incoming DIMSE data.
  Uint32 m_dimseTimeout;

  /// Timeout for ACSE operations in seconds. Maximum time during association negotiation
  /// which is given for the SCU to follow the ACSE protocol.
  Uint32 m_acseTimeout;

  /// Verbose PC mode. Flags specifying whether details on the presentation contexts
  /// (negotiated during association setup) should be shown in verbose or debug mode.
  OFBool m_verbosePCMode;

  /// Table of processes for non-single process mode. This member is only applicable when
  /// the SCP is running under Unix and multi-process mode.
  OFList<DcmProcessSlotType *> m_processTable;

  /// If set, the AE Title as received in the request (called AE Title) is used in response
  /// (default: OFTrue).
  OFBool m_respondWithCalledAETitle;

  /** Drops association and clears internal structures to free memory
   */
  void dropAndDestroyAssociation();

};

#endif // SCP_H


/*
 *  CVS/RCS Log:
 *  $Log: scp.h,v $
 *  Revision 1.9  2010-10-14 13:17:22  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.8  2010-10-07 12:54:07  joergr
 *  Fixed minor Doxygen API documentation issues (added backslash in order to
 *  avoid that the short description ends at the first period).
 *
 *  Revision 1.7  2010-06-22 15:44:55  joergr
 *  Added support for handling N-EVENT-REPORT request.
 *  Added support for stopping after the current association is finished.
 *  Further code cleanup. Renamed some methods, variables, types and so on.
 *
 *  Revision 1.6  2010-06-18 14:50:33  joergr
 *  Added support for the SCP/SCU role selection negotiation.
 *
 *  Revision 1.5  2010-06-17 17:06:30  joergr
 *  Aligned SCP class with existing SCU class. Some further code cleanups.
 *  Changed default profile from "Default" to "DEFAULT". Revised documentation.
 *
 *  Revision 1.4  2010-04-29 16:14:59  onken
 *  Added function for responding to storage requests to SCP class.
 *
 *  Revision 1.3  2009-12-21 17:00:32  onken
 *  Fixed API documentation to keep doxygen quiet.
 *
 *  Revision 1.1  2009-12-17 09:02:43  onken
 *  Added base classes for SCU and SCP implementations.
 *
 */
