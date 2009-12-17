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
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: General SCP class that can be used to implement derived SCP
 *           applications.
 *
 *  Last Update:      $Author: onken $
 *  Update Date:      $Date: 2009-12-17 09:02:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/dcmtk/dcmnet/scp.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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


/// structure for non-single process mode
struct DcmProcessSlotType
{
  /// name of peer
  DIC_NODENAME peerName;
  /// calling AE title
  DIC_AE callingAETitle;
  /// called AE title
  DIC_AE calledAETitle;
  /// process id
  int processId;
  /// start time
  time_t startTime;
  /// indicator if process has storage ability
  OFBool hasStorageAbility;
};

enum DcmSCPActionType
{
  DCMSCP_ACTION_UNDEFINED,
  DCMSCP_ACTION_REFUSE_ASSOCIATION
};

enum DcmRefuseReasonType
{
  DCMSCP_TOO_MANY_ASSOCIATIONS,
  DCMSCP_CANNOT_FORK,
  DCMSCP_BAD_APP_CONTEXT,
  DCMSCP_BAD_AE_SERVICE,
  DCMSCP_FORCED,
  DCMSCP_NO_IC_UID,
  DCMSCP_NO_PRES_CONTEXTS,
  DCMSCP_INTERNAL_ERROR
};

struct DcmPresentationContextInfo
{
  OFString abstractSyntax;
  Uint8 acceptedSCRole;
  OFString acceptedTransferSyntax;
  Uint8 presentationContextID;
  Uint8 proposedSCRole;
  // fields "reserved" and "result" not included from DUL_PRESENTATIONCONTEXT
};

class DcmSCP
{

public:

  /** constructor.
   */
  DcmSCP();

#ifdef _WIN32    
   /* Marks this SCP as being a forked child under windows, ie. it handles
   *  an association received by the parent.
   */
  OFCondition markAsForkedChild();
#endif

  /** Starts providing the implemented service for calling SCUs.
   *  @return The result. Function usually only returns in case of errors.
   */
  virtual OFCondition listen();

  /* Set methods for configuring SCP behaviour */

  OFCondition addAbstractSyntax(const OFString& abstractSyntaxUID,
                                const OFList<OFString> transferSyntaxUIDs,
                                const OFString& profile);

  void setPort(const Uint16& port);
  
  /** Sets AETitle of SCP used for answering requests.
   */
  void setAETitle(const OFString& aetitle);

  void setRespondWithCalledAETitle(const OFBool useCalled);

  OFCondition loadAssociationCfgFile(const OFString& assocFile);

  OFCondition setAndCheckAssociationProfile(const OFString& profileName);

  void setRefuseAssociation(const OFBool doRefuse);

  void setMaxReceivePDU(const Uint32 maxPDU);

  OFCondition enableMultiProcessMode(int argc = 0, 
                                     char *argv[] = NULL);

  void setMaxAssociations(const Uint16 maxAssocs);

  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode);

  void setDIMSETimeout (const int dimseTimeout);

  void setACSETimeout (const int acseTimeout);

  /* Get methods for SCP settings */

  Uint16 getPort() const;

  OFString getAETitle() const;

  OFBool getRespondWithCalledAETitle() const;

  OFBool getRefuseAssociation() const;

  Uint32 getMaxReceivePDU() const;

  OFBool getSingleProcess() const;

  Uint16 getMaxAssociations() const;

  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  int getDIMSETimeout () const;

  int getACSETimeout () const;

  /* Get information about current association */

  Uint16 numAssociations() const;

  OFBool isConnected() const;

  OFCondition getCalledAETitle(OFString& calledAE) const;
  
  OFCondition getPeerAETitle(OFString &aetitle) const;

  OFCondition getPeerIP(OFString &ipOrHostname) const;

  OFCondition getPeerMaxPDU(Uint32 &maxPeerPDU) const;

  /** Destructor
   */
  virtual ~DcmSCP();
  
protected:

  /** Handle incoming commandset and react accordingly, e. g. sending 
   *  response via DIMSE_sendXXXResponse(). The standard handler only
   *  knows how to handle an Echo request by calling handleEchoRequest().
   *  This function is most likely to be implemented by a derived class
   *  implementing a specific SCP behaviour.
   */
  virtual OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg, 
                                            const DcmPresentationContextInfo& presContextInfo);

  /** Overwrite this function to be notified about an incoming association request.
   *  The standard handler just prints debug information.
   */
  virtual void notifyAssociationRequest(const T_ASC_Parameters& params,
                                        DcmSCPActionType& desiredAction);

  /** Overwrite this function to be notified about an incoming association request.
   *  The standard handler prints debug information.
   */
  virtual void notifyAssociationAcknowledge();

  /** Overwrite this function to be notified about an incoming
   *  association release request.
   *  The standard handler prints debug information.
   */
  virtual void notifyReleaseRequest();

  /** Overwrite this function to be notified about an incoming
   *  association abort request.
   *  The standard handler prints debug information.
   */
  virtual void notifyAbortRequest();

  /** Overwrite this function to be notified when an association
   *  is terminated.
   *  The standard handler prints debug information.
   */
  virtual void notifyAssociatonTermination();

  /** Overwrite this function to be notified when an association
   *  is terminated.
   *  The standard handler prints debug information.
   */
  virtual void notifyDIMSEError(const OFCondition& cond);

  /** Receive dataset over existing association.
   */
  virtual OFCondition receiveDataset(DcmDataset **dataObject,
		                                 DIMSE_ProgressCallback callback,
		                                 void *callbackContext);

  /** Standard handler for Verification Service Class (DICOM Echo). Returns
   *  echo response (ie. success).
   *  @param req    The DIMSE C-ECHO-RQ message that was received.
   *  @return OFCondition value denoting success or error.
   */
  virtual OFCondition handleEchoRequest(T_DIMSE_C_EchoRQ *req, 
                                        T_ASC_PresentationContextID presId);

  virtual OFBool calledAETitleAccepted(const OFString& callingAE,
                                       const OFString& calledAE);


  /* *********************************************************************
   * Members and further functions
   * ********************************************************************* 
   */

  /// the current association run by this SCP
  T_ASC_Association *m_assoc;

  /// association configuration
  DcmAssociationConfiguration* m_assocConfig;

  /// profile in association configuration to use
  OFString m_assocCfgProfileName;

  /// port on which the application is listening
  Uint16 m_port;

  /// AETitle to be used. If empty, SCP responds with
  /// the AETitle as expected by the calling SCU
  OFString m_aetitle;
  
  /// indicates if the application shall refuse any association
  OFBool m_refuseAssociation;
  
  /// max pdu size
  Uint32 m_maxPDU;
  
  /// indicates if the application is run in single process mode or not
  OFBool m_singleProcess;
  
  /// indicates, that this process was spawn as child from a parent process
  /// needed for multiprocess mode on WIN32
  OFBool m_forkedChild;
  
  /// number of arguments in commandline, needed for multiprocess mode on WIN32
  int m_cmd_argc;
  
  /// complete command line, needed for multiprocess mode on WIN32
  char **m_cmd_argv;
  
  /// maximum number of association for non-single process mode
  Uint16 m_maxAssociations;
  
  /// blocking mode for DIMSE operations
  T_DIMSE_BlockingMode m_blockMode;
  
  /// timeout for DIMSE operations
  int m_dimse_timeout;
  
  /// timeout for ACSE operations
  int m_acse_timeout;
  
  /// table of processes for non-single process mode
  OFList<DcmProcessSlotType*> m_processTable;

  // If set, the AE Title as received in the request (called AE Title) is used in response.
  // Default ist true;
  OFBool m_respondWithCalledAETitle;

  /// The presentation context of the current request
  DcmPresentationContextInfo m_pcInfo;

  /** This function takes care of receiving, negotiating and accepting/refusing an
   *  association request. Additionally, it handles the request the association
   *  requesting application transmits after a connection is established.
   *  @param net Contains network parameters.
   */
  OFCondition waitForAssociation(T_ASC_Network* network);

    /** This function takes care of removing items referring to (terminated) subprocess
     *  from the table which stores all subprocess information. Three different versions
     *  for three different platforms are implemented.
     */
  void cleanChildren();

  /** This function negotiates a presentation context which will be used by this application
   *  and the other DICOM appliation that requests an association.
   *  @param assoc The association (network connection to another DICOM application).
   */
  OFCondition negotiateAssociation(); 

  /** This function adds a process to the table that stores process information.
   *  @param pid   the process id of the sub-process which was just started.
   *  @param assoc The association (network connection to another DICOM application).
   */
  void addProcessToTable( int pid); 

  /** This function removes one particular item from the table which stores all subprocess
   *  information. The item which shall be deleted will be identified by its process id.
   *  @param pid process id.
   */
  void removeProcessFromTable( int pid );

  /** This function takes care of refusing an assocation request.
   *  @param assoc  The association (network connection to another DICOM application).
   *  @param reason The reason why the association request will be refused.
   */
  void refuseAssociation( DcmRefuseReasonType reason );

  /** This function takes care of handling the other DICOM application's request. After
   *  having accomplished all necessary steps, the association will be dropped and destroyed.
   *  @param assoc The association (network connection to another DICOM application).
   */
  void handleAssociation();

  /** Protected undefined copy-constructor. Shall never be called.
   *  @param Src Source object.
   */
  DcmSCP( const DcmSCP &src );

  /** Protected undefined operator=. Shall never be called.
   *  @param Src Source object.
   *  @return Reference to this.
   */
  DcmSCP &operator=( const DcmSCP &Src );

private:

  void dropAndDestroyAssociation();

  const OFString DCMSCP_TS_KEY;

  const OFString DCMSCP_PC_KEY;

  const OFString DCMSCP_PROFILE_KEY;;

};


#endif // SCP_H

/*
 *  CVS/RCS Log:
 *  $Log: scp.h,v $
 *  Revision 1.1  2009-12-17 09:02:43  onken
 *  Added base classes for SCU and SCP implementations.
 *
 */
