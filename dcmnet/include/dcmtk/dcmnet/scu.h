/*
 *
 *  Copyright (C) 2008-2010, OFFIS e.V.
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
 *  Purpose: Base class for Service Class Users (SCUs)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:22 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SCU_H
#define SCU_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"     /* Covers most common dcmdata classes */
#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmnet/dimse.h"     /* DIMSE network layer */
#include "dcmtk/dcmnet/dcasccff.h"  /* For reading a association config file */
#include "dcmtk/dcmnet/dcasccfg.h"  /* For holding association cfg file infos */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif


/** Different types of closing an association
 */
enum DcmCloseAssociationType
{
  /// Release the current association
  DCMSCU_RELEASE_ASSOCIATION,
  /// Abort the current association
  DCMSCU_ABORT_ASSOCIATION,
  /// Peer requested release (Aborting)
  DCMSCU_PEER_REQUESTED_RELEASE,
  /// Peer aborted the association
  DCMSCU_PEER_ABORTED_ASSOCIATION
};


/** Class representing a single C-FIND response
 */
class FINDResponse
{

public:

  /// Standard constructor
  FINDResponse();

  /// Destructor, cleans up internal memory (datasets if present)
  virtual ~FINDResponse();

  /// Mandatory response field
  Uint16 m_messageIDRespondedTo;

   // Mandatory response field
  OFString m_affectedSOPClassUID;

  /// Conditional response field (NULL if absent)
  DcmDataset *m_dataset;

  /// Mandatory Response Field
  Uint16 m_status;
};


/** Class representing a bunch of FINDResponses from a single C-FIND session
 */
class FINDResponses
{

public:
  FINDResponses();
  virtual ~FINDResponses();
  Uint32 numResults() const;
  void add(FINDResponse *rsp);
  OFListIterator(FINDResponse *) begin();
  OFListIterator(FINDResponse *) end();

private:
  OFList<FINDResponse *> m_responses;
};

/** Base class for implementing DICOM Service Class User functionality. The class offers
 *  support for negotiating associations and sending and receiving arbitrary DIMSE messages
 *  on that connection. DcmSCU has built-in C-ECHO support so derived classes do not have to
 *  implement that capability on their own.
 *  @warning This class is EXPERIMENTAL. Be careful to use it in production environment.
 */
class DcmSCU
{

public:

  /** Constructor, just initializes internal class members
   */
  DcmSCU();

  /** Virtual destructor
    */
  virtual ~DcmSCU();

  /** Add presentation context to be used for association negotiation
   *  @param abstractSyntax [in] Abstract syntax name in UID format
   *  @param xferSyntaxes   [in] List of transfer syntaxes to be added for the given
   *                             abstract syntax
   *  @return EC_Normal if adding was successful, otherwise error code
   */
  OFCondition addPresentationContext(const OFString &abstractSyntax,
                                     const OFList<OFString> &xferSyntaxes);

  /** Initialize network, i.e.\ prepare for association negotiation
   *  @return EC_Normal if initialization was successful, otherwise error code
   */
  virtual OFCondition initNetwork();

  /** Negotiate association by using presentation contexts and parameters as defined by
   *  earlier function calls. If negotiation fails, there is no need to close the
   *  association or to do anything else with this class.
   *  @return EC_Normal if negotiation was successful, otherwise error code
   */
  virtual OFCondition negotiateAssociation();

   /** After negotiation association, this call returns the first usable presentation
    *  context given the desired abstract syntax and transfer syntax
    *  @param abstractSyntax [in] The abstract syntax (UID) to look for
    *  @param transferSyntax [in] The transfer syntax (UID) to look for.
    *                             If empty, the transfer syntax is not checked.
    *  @return Adequate Presentation context ID that can be used. 0 if none found.
    */
  T_ASC_PresentationContextID findPresentationContextID(const OFString &abstractSyntax,
                                                        const OFString &transferSyntax);

  /** This function sends a C-ECHO command via network to another DICOM application
   *  @param presID [in] Presentation context ID to use. A value of 0 lets SCP class tries
   *                     to choose one on its own.
   *  @return EC_Normal if echo was successful, an error code otherwise
   *
   */
  virtual OFCondition sendECHORequest(const T_ASC_PresentationContextID presID);

  /** This function sends a C-STORE request on the currently opened association and receives
   *  the corresponding response then
   *  @param presID          [in]  Contains in the end the ID of the presentation context which
   *                               was specified in the DIMSE command. If 0 is given, the
   *                               function tries to find an approriate presentation context
   *                               itself.
   *  @param dicomFile       [in]  The filename of the DICOM file to be sent. Alternatively, a
   *                               dataset can be given in the next parameter. If both are
   *                               given the dataset from the file name is used.
   *  @param dset            [in]  The dataset to be sent. Alternatively, a filename can be
   *                               specified in the previous parameter. If both are given the
   *                               dataset from the file name is used.
   *  @param rspCommandSet   [out] If this parameter is not NULL it will return a copy of the
   *                               DIMSE command which was received from the other DICOM
   *                               application.
   *  @param rspStatusDetail [out] If a non-NULL value is passed this variable will in the end
   *                               contain detailed information with regard to the status
   *                               information which is captured in the status element
   *                               (0000,0900). Note that the value for element (0000,0900) is
   *                               not contained in this return value but in internal msg. For
   *                               details on the structure of this object, see DICOM standard
   *                               part 7, annex C).
   *  @param rspStatusCode   [out] The response status code received. 0 means success,
   *                               others can be found in the DICOM standard.
   *  @return EC_Normal if request could be issued and response was received successfully,
   *          error code otherwise. That means that if the receiver sends a response
   *          denoting failure of the storage request, EC_Normal will be returned.
   */
  virtual OFCondition sendSTORERequest(const T_ASC_PresentationContextID presID,
                                       const OFString &dicomFile,
                                       DcmDataset *dset,
                                       DcmDataset *&rspCommandSet,
                                       DcmDataset *&rspStatusDetail,
                                       Uint16 &rspStatusCode);

  /** Sends a C-FIND Request on given presentation context and receives list of responses.
   *  The function receives the first response and then calls the function handleFINDResponse
   *  which gets the relevant presentation context together with the response dataset and
   *  status information. Then it waits again for the next response, if there are more to
   *  come (i.e. response status is PENDING). In the end, after receiving all responses, the
   *  full list of responses is returned to the caller. If he is not interested, he just sets
   *  responses=NULL when calling the function.
   *  This function can be overwritten by actual SCU implementations but just should work fine
   *  for most people.
   *  @param TODO
   *  @return TODO
   */
  virtual OFCondition sendFINDRequest(T_ASC_PresentationContextID presID,
                                      DcmDataset *queryKeys,
                                      FINDResponses *responses);

  /** This is the standard handler for C-FIND message responses: It just adds up all
   *  responses it receives and prints a DEBUG message. Therefore, it is called by for
   *  each response received in sendFINDRequest(). The idea is of course to overwrite
   *  this function in a derived, actual SCU implementation if required. Thus, after
   *  each response, the caller of sendFINDRequest() can decide on its own whether he
   *  wants to cancel the C-FIND session, terminate the association, do something useful
   *  or whatever. That way this is a more object oriented kind of callback.
   *  @param TODO
   *  @return TODO
   */
  virtual OFCondition handleFINDResponse(Uint16 presContextID,
                                         FINDResponse *response,
                                         OFBool &waitForNextResponse);

  /** Send C-FIND-CANCEL and, therefore, ends the C-FIND session, i.e.\ no further responses
   *  will be handled. A call to this function only makes sense if an association is open,
   *  the given presentation context represents a valid C-FIND-enabled SOP class and usually
   *  only, if the last command send on that presentation context was a C-FIND message.
   *  @param TODO
   *  @return TODO
   */
  virtual OFCondition sendCANCELRequest(Uint16 presContextID);

  /** This function sends a N-ACTION request on the currently opened association and receives
   *  the corresponding response then
   *  @param presID         [in]  The ID of the presentation context to be used for sending
   *                              the request message. Should not be 0.
   *  @param sopInstanceUID [in]  The requested SOP Instance UID
   *  @param actionTypeID   [in]  The action type ID to be used
   *  @param reqDataset     [in]  The dataset to be sent
   *  @param rspStatusCode  [out] The response status code received. 0 means success,
   *                              others can be found in the DICOM standard.
   *  @return EC_Normal if request could be issued and response was received successfully,
   *          an error code otherwise. That means that if the receiver sends a response
   *          denoting failure of the storage request, EC_Normal will be returned.
   */
  virtual OFCondition sendACTIONRequest(const T_ASC_PresentationContextID presID,
                                        const OFString &sopInstanceUID,
                                        const Uint16 actionTypeID,
                                        DcmDataset *reqDataset,
                                        Uint16 &rspStatusCode);

  /** Receives N-EVENT-REPORT request on the currently opened association and sends a
   *  corresponding response. Calls checkEVENTREPORTRequest() in order to determine the
   *  DIMSE status code to be used for the N-EVENT-REPORT response.
   *  @param reqDataset  [out] Pointer to the dataset received
   *  @param eventTypeID [out] Event Type ID from the command set received
   *  @param timeout     [in]  Optional timeout in seconds for receiving data. This value
   *                           (if not 0) overwrites the standard DIMSE timeout and also
   *                           enables the non-blocking mode for receiving the request.
   *  @return status, EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition handleEVENTREPORTRequest(DcmDataset *&reqDataset,
                                               Uint16 &eventTypeID,
                                               const int timeout = 0);

  /** Closes the association created by this SCU. Also resets the current association.
   *  @param closeType [in] Define whether to release or abort the association
   */
  virtual void closeAssociation(const DcmCloseAssociationType closeType);

  /* Set methods */

  /** Set maximum PDU length (to be received by SCU)
   *  @param maxRecPDU [in] The maximum PDU size to use in bytes
   */
  void setMaxReceivePDULength(const unsigned long maxRecPDU);

  /** Set whether to send in DIMSE blocking or non-blocking mode
   *  @param blockingMode [in] Either blocking or non-blocking mode
   */
  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode);

  /** Set SCU's AETitle to be used in association negotiation
   *  @param myAETtitle [in] The SCU's AETitle to be used
   */
  void setAETitle(const OFString &myAETtitle);

  /** Set SCP's host (hostname or IP address) to talk to in association negotiation
   *  @param peerHostName [in] The SCP's hostname or IP address to be used
   */
  void setPeerHostName(const OFString &peerHostName);

  /** Set SCP's AETitle to talk to in association negotiation
   *  @param peerAETitle [in] The SCP's AETitle to be used
   */
  void setPeerAETitle(const OFString &peerAETitle);

  /** Set SCP's port number to connect to for association negotiation
   *  @param peerPort [in] The SCP's port number
   */
  void setPeerPort(const Uint16 peerPort);

  /** Set timeout for receiving DIMSE messages
   *  @param dimseTimeout [in] DIMSE Timeout in seconds for receiving data. If the blocking
   *                           mode is DIMSE_NONBLOCKING and we are trying to read data from
   *                           the incoming socket stream and no data has been received.
   */
  void setDIMSETimeout(const Uint32 dimseTimeout);

  /** Set timeout for receiving ACSE messages
   *  @param acseTimeout [in] ACSE Timeout in seconds used by timer for message timeouts
   *                          during association negotiation
   */
  void setACSETimeout(const Uint32 acseTimeout);

  /** Set an association configuration file and profile to be used
   *  @param filename [in] File name of the association configuration file
   *  @param profile  [in] Profile inside the association negotiation file
   */
  void setAssocConfigFileAndProfile(const OFString &filename,
                                    const OFString &profile);

  /** Set whether to show presentation contexts in verbose or debug mode
   *  @param mode [in] Show presentation contexts in verbose mode if OFTrue. By default, the
   *                   presentation contexts are shown in debug mode.
   */
  void setVerbosePCMode(const OFBool mode);

  /* Get methods */

  /** Get current connection status
   *  @return OFTrue if SCU is currently connected, OFFalse otherwise
   */
  OFBool isConnected() const;

  /** Returns maximum PDU length configured to be received by SCU
   *  @return Maximum PDU length in bytes
   */
  Uint32 getMaxReceivePDULength() const;

  /** Returns whether DIMSE messaging is configured to be blocking or unblocking
   *  @return The blocking mode configured
   */
  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  /** Returns the SCU's own configured AETitle
   *  @return The AETitle configured for this SCU
   */
  const OFString &getAETitle() const;

  /** Returns the SCP's (peer's) host name configured
   *  @return The hostname (or IP) configured to be talked to
   */
  const OFString &getPeerHostName() const;

  /** Returns the SCP's (peer's) AETitle configured
   *  @return The AETitle configured to be talked to
   */
  const OFString &getPeerAETitle() const;

  /** Returns the SCP's (peer's) TCP/IP port configured
   *  @return The port configured to talked to
   */
  Uint16 getPeerPort() const;

  /** Returns the DIMSE timeout configured defining how long SCU will wait for DIMSE
   *  responses
   *  @return The DIMSE timeout configured
   */
  Uint32 getDIMSETimeout() const;

  /** Returns the timeout configured defining how long SCU will wait for messages
   *  during ACSE messaging (association negotiation)
   *  @return The ACSE timeout configured
   */
  Uint32 getACSETimeout() const;

  /** Returns the verbose presentation context mode configured specifying whether
   *  details on the presentation contexts (negotiated during association setup)
   *  should be shown in verbose or debug mode. The latter is the default.
   *  @return The verbose presentation context mode configured
   */
  OFBool getVerbosePCMode() const;

  /** Returns whether SCU is configured to create a TLS connection with the SCP
   *  @return OFFalse for this class but may be overriden by derived classes
   */
  OFBool getTLSEnabled() const;

protected:

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

  /** Returns SOP Class UID, SOP Instance UID and original transfer syntax for a given dataset
   *  @param dataset        [in]  The dataset to read from
   *  @param sopClassUID    [out] The value of attribute SOP Class UID if present
   *  @param sopInstanceUID [out] The value of attribute SOP Instance UID if present
   *  @param transferSyntax [out] The value of transfer syntax that originally was read from
   *                              disk
   *  @return Error if dataset is NULL, otherwise EC_Normal. All return parameters will be
   *          empty if not found (transferSyntax will be set to EXS_Unknown) in the file.
   */
  OFCondition getDatasetInfo(DcmDataset *dataset,
                             OFString &sopClassUID,
                             OFString &sopInstanceUID,
                             E_TransferSyntax &transferSyntax);


  /** Tells DcmSCU to use a secure TLS connection described by the given TLS layer
   *  @param tlayer [in] The TLS transport layer including all TLS parameters
   *  @return EC_Normal if given transport layer is ok, an error code otherwise
   */
  OFCondition useSecureConnection(DcmTransportLayer *tlayer);

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

   /** After negotiation association, this call returns the presentation context belonging
    *  to the given presentation context ID
    *  @param presID         [in]  The presentation context ID to look for
    *  @param abstractSyntax [out] The abstract syntax (UID) for that ID.
    *                              Empty, if such a presentation context does not exist.
    *  @param transferSyntax [out] The transfer syntax (UID) for that ID.
    *                              Empty, if such a presentation context does not exist.
    */
  void findPresentationContext(const T_ASC_PresentationContextID presID,
                               OFString &abstractSyntax,
                               OFString &transferSyntax);

  /** Check given N-EVENT-REPORT request and dataset for validity. This method is called by
   *  handleEVENTREPORTRequest() before sending the response in order to determine the
   *  DIMSE status code to be used for the response message.
   *  @param request    [in] The N-EVENT-REPORT request message data structure
   *  @param reqDataset [in] The N-EVENT-REPORT request dataset received. Might be NULL.
   *  @return DIMSE status code to be used for the N-EVENT-REPORT response.
   *          Always returns STATUS_Success (0). Derived classes should, therefore,
   *          overwrite this method and return a more appropriate value based on the
   *          result of the checks performed.
   */
  virtual Uint16 checkEVENTREPORTRequest(T_DIMSE_N_EventReportRQ &request,
                                         DcmDataset *reqDataset);

private:

  /** Private undefined copy-constructor. Shall never be called.
   *  @param src Source object
   */
  DcmSCU(const DcmSCU &src);

  /** Private undefined operator=. Shall never be called.
   *  @param src Source object
   *  @return Reference to this
   */
  DcmSCU &operator=(const DcmSCU &src);

  /// Associaton of this SCU. This class only handles 1 association at a time.
  T_ASC_Association *m_assoc;

  /// The DICOM network the association is based on
  T_ASC_Network *m_net;

  /// Association parameters
  T_ASC_Parameters *m_params;

  /// Configuration file for presentation contexts (optional)
  OFString m_assocConfigFilename;

  /// Profile in configuration file that should be used (optional)
  OFString m_assocConfigProfile;

  /// Defines presentation context, consisting of one abstract syntax name
  /// and a list of transfer syntaxes for this abstract syntax
  struct DcmSCUPresContext {
    /// Abstract Syntax Name of Presentation Context
    OFString abstractSyntaxName;
    /// List of Transfer Syntaxes for Presentation Context
    OFList<OFString> transferSyntaxes;
  };

  /// List of presentation contexts that should be negotiated
  OFList<DcmSCUPresContext> m_presContexts;

  /// Configuration file containing association parameters
  OFString m_assocConfigFile;

  /// The last DIMSE successfully sent, unresponded DIMSE request
  T_DIMSE_Message *m_openDIMSERequest;

  /// Maximum PDU size
  Uint32 m_maxReceivePDULength;

  /// DIMSE blocking mode
  T_DIMSE_BlockingMode m_blockMode;

  /// AEtitle of this application
  OFString m_ourAETitle;

  /// Peer hostname
  OFString m_peer;

  /// AEtitle of remote application
  OFString m_peerAETitle;

  /// Port of remote application entity
  Uint16 m_peerPort;

  /// DIMSE timeout
  Uint32 m_dimseTimeout;

  /// ACSE timeout
  Uint32 m_acseTimeout;

  /// Verbose PC mode
  OFBool m_verbosePCMode;

  /** Returns next available message ID free to be used by SCU
   *  @return Next free message ID
   */
  Uint16 nextMessageID();

};

#endif // SCU_H


/*
** CVS Log
** $Log: scu.h,v $
** Revision 1.13  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2010-10-07 12:54:07  joergr
** Fixed minor Doxygen API documentation issues (added backslash in order to
** avoid that the short description ends at the first period).
**
** Revision 1.11  2010-06-24 09:21:54  joergr
** Revised comment to make clear that the parameter "presID" shall never be 0
** for the sendACTIONRequest() method.
**
** Revision 1.10  2010-06-22 15:45:27  joergr
** Introduced new enumeration type to be used for closeAssociation().
** Further code cleanup. Renamed some methods, variables, types and so on.
**
** Revision 1.9  2010-06-17 17:11:27  joergr
** Added preliminary support for N-EVENT-REPORT to DcmSCU. Some further code
** cleanups and enhancements. Renamed some methods. Revised documentation.
**
** Revision 1.8  2010-06-09 16:09:01  joergr
** Added preliminary support for N-ACTION to DcmSCU. Some further code cleanups
** and enhancements.
**
** Revision 1.7  2010-06-08 17:54:12  onken
** Added C-FIND functionality to DcmSCU. Some code cleanups. Fixed
** memory leak sometimes occuring during association configuration.
**
** Revision 1.6  2010-04-29 16:13:28  onken
** Made SCU class independent from dcmtls, i.e. outsourced TLS API. Added
** direct API support for sending C-STORE requests. Further API changes and
** some bugs fixed.
**
** Revision 1.5  2009-12-21 17:00:32  onken
** Fixed API documentation to keep doxygen quiet.
**
** Revision 1.4  2009-12-21 15:33:55  onken
** Added documentation and refactored / enhanced some code.
**
** Revision 1.3  2009-12-17 09:12:10  onken
** Fixed other scu and scp base class compile issues.
**
** Revision 1.2  2009-12-17 09:05:15  onken
** Fixed typo resulting in build failure.
**
** Revision 1.1  2009-12-17 09:02:44  onken
** Added base classes for SCU and SCP implementations.
**
** Revision 1.2  2009-12-02 14:26:05  uli
** Stop including dcdebug.h which was removed.
**
** Revision 1.1  2008-09-29 13:51:52  onken
** Initial checkin of module dcmppscu implementing an MPPS commandline client.
**
*/
