/*
 *
 *  Copyright (C) 2009, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmppscu
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Base class for Service Class Users (SCUs)
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-12-17 09:12:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/dcmtk/dcmnet/scu.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcompat.h"
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

class DcmSCU
{

public:

  /** Constructor, just initializes internal class members
   */
  DcmSCU();

  /** Virtual desctructor 
    */
  virtual ~DcmSCU();

  /** Add presentation context to be used for association negotiation
   *  @param abstractSyntax [in] Abstract syntax name in UID format.
   *  @param tSyntaxes [in] List of transfer syntaxes to be added
   *                   for the given abstract syntax
   *  @return EC_Normal if adding was successful, otherwise error code
   */
  OFCondition addPresentationContext(const OFString& abstractSyntax,
                                     const OFList<OFString>& tSyntaxes);

  /** Initialize network, i.e. prepare for association negotiation.
   *  @return EC_Normal if initialization was successful, otherwise error code
   */
  OFCondition initNetwork();

  /** Negotiate association by using presentation contexts and parameters
   *  as defined by earlier function calls.
   *  @return EC_Normal if negotiation was successful, otherwise error code
   */
  OFCondition negotiateAssociation();

   /** After negotiation association, this call returns a usable presentation
    *  context given the desired abstract syntax and transfer syntax.
    *  @param abstractSyntax [in] The abstract syntax (UID) to look for 
    *  @param transferSyntax [in] The transfer syntax (UID) to look for
    *  @return Adequate Presentation context ID that can be used. 0 if not found.
    */
  T_ASC_PresentationContextID findPresContID(const OFString& abstractSyntax,
                                             const OFString& transferSyntax);

  /**
   * This function sends a C-ECHO command via network
   *  to another DICOM application.
   *  @param presID [in] Presentation context ID to use. A value of 0 lets SCP class
   *                tries to choose one on its own.
   *  @return EC_Normal if echo was successful, error code otherwise
   *           
   */
  OFCondition sendECHORequest( const T_ASC_PresentationContextID& presID );

  /**
   * This function sends a DIMSE command and possibly also a dataset
   * from a data object via network to another DICOM application.
   *
   * @param presID [in] Presentation context ID to be used for message.
   * @param msg [in] tructure that represents a certain DIMSE command which
   *            shall be sent.
   * @param dataObject [in] The instance data which shall be sent to the
   *                   other DICOM application, NULL, if there is none
   * @param callback [in] Pointer to a function which shall be called to
   *                 indicate progress.
   * @param callbackContext [in] Pointer to data which shall be passed to the
   *                        progress indicating function
   * @param commandSet [out] If this parameter is not NULL it will return a
   *                   copy of the DIMSE command which is sent to the other
   *                   DICOM application.
   * @return Returns EC_Normal if sending request was successful, error code
   *         otherwise.
   */
  OFCondition sendDIMSERequest( const T_ASC_PresentationContextID& presID,
                                T_DIMSE_Message *msg,
                                /*DcmDataset *statusDetail,*/ // Brauche ich nur bei RSP nachrichten oder?
                                DcmDataset *dataObject,
                                DIMSE_ProgressCallback callback,
                                void *callbackContext,
                                DcmDataset **commandSet = NULL);

  /** Receive DIMSE command (excluding dataset!) over the currently
   *  open association.
   *  @param presId [out] Contains in the end the ID of the presentation
   *                context which was specified in the DIMSE command.
   *  @param msg [out] Contains in the end information which represents the DIMSE
   *             command which was received.
   *  @param statusDetail [out] If a non-NULL value is passed this variable
   *                            will in the end contain detailed information
   *                            with regard to the status information which
   *                            is captured in the status element (0000,0900).
   *                            Note that the value for element (0000,0900)
   *                            is not contained in this return value but in
   *                            msg. For details on the structure of this
   *                            object, see DICOM standard (year 2000) part
   *                            7, annex C) (or the corresponding section in a
   *                            later version of the standard.)
   *  @param commandSet [out] If this parameter is not NULL
   *                          it will return a copy of the DIMSE command which
   *                          was received from the other DICOM application.
   *  @return EC_Normal if response was received successfully, error code
   *          otherwise
   */
  OFCondition receiveDIMSEResponse( T_ASC_PresentationContextID *presID,
                                    T_DIMSE_Message *msg,
                                    DcmDataset **statusDetail,
                                    DcmDataset **commandSet = NULL);
  
  /** This function revceives one data set (of instance data) via network
   *  from another DICOM application.
   *  @param presID          - [out] Contains in the end the ID of the presentation context which was used in the PDVs
   *                           that were received on the network. If the PDVs show different presentation context
   *                           IDs, this function will return an error.
   *  @param dataObject      - [out] Contains in the end the information which was received over the network.
   *                           Note that this function assumes that either imageFileName or imageDataSet does not equal NULL.
   *  @param callback        - [in] Pointer to a function which shall be called to indicate progress.
   *  @param callbackContext - [in] Pointer to data which shall be passed to the progress indicating function
   */
  OFCondition receiveDIMSEDataset( T_ASC_PresentationContextID *presID,
                                   DcmDataset **dataObject,
                                   DIMSE_ProgressCallback callback,
                                   void *callbackContext);

  /** Closes the association of this SCU. As parameter it needs information
   *  whether the remote peer wants to release/abort the association. Allowed values:
   *  EC_Normal: Let SCU release its association
   *  DUL_PEERABORTEDASSOCIATION: Also releases association on peer's request
   *  DUL_PEERREQUESTEDRELEASE: Aborts associaton on peer's request
   *  Other cases: SCU aborts association.
   *  @param abortOrReleaseRequested [in] Define whether to release or abort
   *                                      the association.
   */
  void closeAssociation(const OFCondition& abortOrReleaseRequested);

  /* Set methods */

  /** Set maximum PDU length (to be received by SCU).
   *  @param maxRecPDU [in] The maximum PDU size to use in bytes
   */
  void setMaxReceivePDULength(const unsigned long& maxRecPDU);

  /** Set whether to send in DIMSE blocking or non-blocking mode.
   *  @param blockingMode [in] Either blocking or non-blocking mode
   */
  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode& blockingMode);

  /** Set SCU's AETitle to be used in association negotiation.
   *  @param myAETtitle [in] The SCU's AETitle to be used
   */
  void setAETitle(const OFString& myAETtitle);

  /** Set SCP's host (hostname or IP address) to talk to in association negotiation.
   *  @param peerHostName [in] The SCP's hostname or IP address to be used
   */  
  void setPeerHostName(const OFString& peerHostName);

  /** Set SCP's AETitle to talk to in association negotiation.
   *  @param peerAETitle [in] The SCP's AETitle to be used
   */  
  void setPeerAETitle(const OFString& peerAETitle);

  /** Set SCP's port number to connect to for association negotiation.
   *  @param peerAETitle [in] The SCP's port number.
   */  
  void setPeerPort(const unsigned long& peerPort);

  /** Set timeout for receiving DIMSE messages.
   *  @param dimseTimeout [in] DIMSE Timeout for receiving data. If the
   *                      blocking mode is DIMSE_NONBLOCKING and we are
   *                      trying to read data from the incoming socket
   *                      stream and no data has been received.
   */  
  void setDIMSETimeout(const int& dimseTimeout);

  /** Set timeout for receiving ACSE messages.
   *  @param dimseTimeout [in] ACSE Timeout used by timer for message timeouts
   *                      during association negotiation(?). Defined in seconds.
   */    
  void setACSETimeout(const int& acseTimeout);

  /** Set an association configuration file and profile to be used.
   *  @param filename [in] File name of the association configuration file
   *  @param profile  [in] Profile inside the association negotiation file
   *                       during association negotiation(?).
   *                       Defined in seconds.
   */    
  void setAssocConfigFileAndProfile(const OFString& filename,
                                    const OFString& profile);

  /* Get methods */

  /** Get current connection status.
   *  @return OFTrue if SCU is currently connected, OFFalse otherwise.
   */    
  OFBool isConnected() const;

  /** Returns maximum PDU length configured to be received by SCU
   *  @return Maximum PDU length in bytes
   */
  Uint32 getMaxReceivePDULength() const;

  /** Returns whether DIMSE messaging is configured to be blocking or
   *  unblocking.
   *  @return The blocking mode configured.
   */
  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  /** Returns the SCU's own configured AETitle.
   *  @return The AETitle configured for this SCU.
   */
  OFString getAETitle() const;

  /** Returns the SCP's (peer's) host name configured.
   *  @return The hostname (or IP) configured to be talked to.
   */
  OFString getPeerHostName() const;

  /** Returns the SCP's (peer's) AETitle configured.
   *  @return The AETitle configured to be talked to.
   */  
  OFString getPeerAETitle() const;

  /** Returns the SCP's (peer's) TCP/IP port configured.
   *  @return The port configured to talked to.
   */ 
  unsigned int getPeerPort() const;

  /** Returns the DIMSE timeout configured defining how long SCU will wait
   *  for DIMSE responses.
   *  @return The DIMSE timeout configured.
   */ 
  int getDIMSETimeout() const;

  /** Returns the timeout configured defining how long SCU will wait
   *  for messages during ACSE messaging (association negotiation).
   *  @return The ACSE timeout configured.
   */ 
  int getACSETimeout() const;

  /** Returns whether SCU is configured to create a TLS connection
   *  with the SCP.
   *  @return OFTrue if a TLS connection shall be used for messaging,
   *          OFFalse otherwise
   */ 
  OFBool getSecureConnectionMode() const;

// OPENSSL stuff
#ifdef WITH_OPENSSL

  /** Tells SCU to enable/disable secure TLS messaging
   *  @param secureMode [in] If OFTrue, SCU will try to establish secure
   *                    connection. For unsecure connection, set to OFFalse.
   */
  void setSecureConnectionMode(const OFBool& secureMode);

  /** XXX
   *  @param XXX
   */
  void addTrustedCertFile(const OFString& str);
  
  /** XXX
   *  @param XXX
   */
  void addTrustedCertDir(const OFString& certDir);

  /** XXX
   *  @param XXX
   */
  void setKeyFileFormat(const int& keyFileFormat);

  /** XXX
   *  @param XXX
   */  
  void setAuthenticationEnabled(const OFBool& authMode);

  /** XXX
   *  @param XXX
   */  
  void setPrivateKeyFile(const OFString& keyFile);

  /** XXX
   *  @param XXX
   */  
  void setCertificateFile(const OFString& certFile);

  /** XXX
   *  @param XXX
   */  
  void setPassword(const char* passwd);

  /** XXX
   *  @param XXX
   */  
  void setCiphersuites(const OFString& cs);

  /** XXX
   *  @param XXX
   */
  void setReadSeedFile(const OFString& seedFile);

  /** XXX
   *  @param XXX
   */
  void setWriteSeedFile(const OFString& seedFile);

  /** XXX
   *  @param XXX
   */
  void setCertVerification(const DcmCertificateVerification& cert);

  /** XXX
   *  @param XXX
   */
  void setDHParam(const OFString& dhParam);

  /** XXX
   *  @param XXX
   */  
  int getKeyFileFormat() const;

  /** XXX
   *  @param XXX
   */
  OFBool getAuthenticationEnabled() const;

  /** XXX
   *  @param XXX
   */
  OFString getPrivateKeyFile() const;

  /** XXX
   *  @param XXX
   */
  void getTrustedCertFiles(OFList<OFString>& files /*out*/) const;

  /** XXX
   *  @param XXX
   */
  void getTrustedCertDirs(OFList<OFString>& trustedDirs /*out*/) const;

  /** XXX
   *  @param XXX
   */
  OFString getCertificateFile() const;

  /** XXX
   *  @param XXX
   */
  OFBool getPassword(OFString& passwd /*out*/) const;

  /** XXX
   *  @param XXX
   */
  OFString getCiphersuites() const;

  /** XXX
   *  @param XXX
   */
  OFString getReadSeedFile() const;

  /** XXX
   *  @param XXX
   */
  OFString getWriteSeedFile() const;

  /** XXX
   *  @param XXX
   */
  DcmCertificateVerification getCertVerification() const;

  /** XXX
   *  @param XXX
   */
  OFString getDHParam() const;
#endif

protected:

private:

  /// The DICOM network the association is based on
  T_ASC_Network* m_net;

  /// Association parameters
  T_ASC_Parameters *m_params;

  /// Associaton of this SCU. This class only handles 1 association at a time.
  T_ASC_Association *m_assoc;

  /// Configuration file for presentation contexts (optional)
  OFString m_assocConfigFilename;

  /// Profile in configuration file that should be used (optional)
  OFString m_assocConfigProfile;

  /// Defines presentation context, consisting of one abstract syntax name
  /// and a list of transfer syntaxes for this abstract syntax
  struct DcmSCUPresContext {
    OFString abstractSyntaxName;
    OFList<OFString> transferSyntaxes;
  };

  /// List of presentation contexts that should be negotiated
  OFList<DcmSCUPresContext> m_presContexts;

  /// Configuration file containing association parameters.
  OFString opt_assocConfigFile;

  /// The ID of the next presentation context to be added (starting with 1)
  T_ASC_PresentationContextID m_currPresID;

  /// The last DIMSE successfully sent, unresponded DIMSE request
  T_DIMSE_Message* m_openDIMSERequest;

  /// maximum PDU size
  Uint32 m_maxReceivePDULength;

  /// if true, openssl encryption is used for the connection
  OFBool m_secureConnection;

  /// DIMSE blocking mode
  T_DIMSE_BlockingMode m_blockMode;

  /// AEtitle of this application
  OFString m_ourAETitle;

  /// peer hostname
  OFString m_peer;

  /// AEtitle of remote application
  OFString m_peerAETitle;

  /// port of remote application entity
  Uint32 m_peerPort;

  /// DIMSE timeout
  int m_dimse_timeout;

  /// ACSE timeout
  int m_acse_timeout;

#ifdef WITH_OPENSSL
    /// TODO
    DcmTLSTransportLayer *m_tLayer;

    /// TODO
    int m_keyFileFormat;

    /// TODO
    OFBool m_doAuthenticate;

    /// TODO
    OFList<OFString> m_trustedCertDirs;

    /// TODO
    OFList<OFString> m_trustedCertFiles;

    /// TODO
    OFString m_privateKeyFile ;

    /// TODO
    OFString m_certificateFile;

    /// TODO
    char* m_passwd;

    /// TODO
    OFString m_ciphersuites;;

    /// TODO
    OFString m_readSeedFile;

    /// TODO
    OFString m_writeSeedFile;

    /// TODO
    DcmCertificateVerification m_certVerification;

    /// TODO
    OFString m_dhparam;
#endif

  /** Returns next available message ID free to be used by SCU.
   *  @return Next free message ID
   */
  unsigned short nextMessageID();

};

/*
** CVS Log
** $Log: scu.h,v $
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
**
**
*/
