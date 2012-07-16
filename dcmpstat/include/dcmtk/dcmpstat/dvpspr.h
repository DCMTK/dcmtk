/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPrintMessageHandler
 *
 */

#ifndef DVPSPR_H
#define DVPSPR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmnet/dimse.h"

/** pure abstract event handler class for N-EVENT-REPORT.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSPrintEventHandler
{
public:

  /// destructor
  virtual ~DVPSPrintEventHandler() {}

  /** handles an N-EVENT-REPORT-RQ message. This method is called
   *  from DVPSPrintMessageHandler whenever an event report is received.
   *  The event message, information and status detail elements must be copied
   *  if they should remain valid after the end of this method call.
   *  The event handler can react on the event message in any implementation
   *  dependent way, e.g. display the event on-screen.
   *  @param eventMessage the N-EVENT-REPORT-RQ message
   *  @param eventInformation event information dataset. May be NULL if no event information available.
   *  @param statusDetail status detail dataset. ay be NULL if no status detail available.
   *  @return the status code to be sent back as part of the N-EVENT-REPORT-RSP message, usually STATUS_Success.
   */
  virtual DIC_US handleEvent(
    T_DIMSE_N_EventReportRQ& eventMessage,
    DcmDataset *eventInformation,
    DcmDataset *statusDetail)=0;
};

/** representation of an association used for DICOM Basic Grayscale Print.
 */

class DCMTK_DCMPSTAT_EXPORT DVPSPrintMessageHandler
{
public:
  /// default constructor
  DVPSPrintMessageHandler();

  /// destructor
  virtual ~DVPSPrintMessageHandler();

  /** sends an N-CREATE-RQ message and receives response.
   *  Any event report requests incoming before the expected response message are handled.
   *  @param sopclassUID the affected SOP class UID, must be one of the supported print classes.
   *  @param sopinstanceUID the affected SOP instance UID. May be empty in which case the
   *    UID is assigned by the SCP and returned in this parameter.
   *  @param attributeListIn the attribute list dataset, may be NULL.
   *  @param status the response message status code is passed back in this parameter
   *    if the method returns with success status. Content is undefined otherwise.
   *  @param attributeListOut a pointer to the attribute list returned by the SCP is passed back
   *    in this parameter if the method returns with success status. Content is undefined otherwise.
   *    May return NULL if the SCP has not returned an attribute list. If a DICOM dataset is returned,
   *    it must be deleted by the caller.
   *  @return DIMSE_NORMAL if successful (e.g. no protocol failure), an error code otherwise.
   */
  OFCondition createRQ(
    const char *sopclassUID,
    OFString& sopinstanceUID,
    DcmDataset *attributeListIn,
    Uint16& status,
    DcmDataset* &attributeListOut);

  /** sends an N-SET-RQ message and receives response.
   *  Any event report requests incoming before the expected response message are handled.
   *  @param sopclassUID the requested SOP class UID, must be one of the supported print classes.
   *  @param sopinstanceUID the requested SOP instance UID
   *  @param modificationList the modification list dataset, must not be NULL.
   *  @param status the response message status code is passed back in this parameter
   *    if the method returns with success status. Content is undefined otherwise.
   *  @param attributeListOut a pointer to the attribute list returned by the SCP is passed back
   *    in this parameter if the method returns with success status. Content is undefined otherwise.
   *    May return NULL if the SCP has not returned an attribute list. If a DICOM dataset is returned,
   *    it must be deleted by the caller.
   *  @return DIMSE_NORMAL if successful (e.g. no protocol failure), an error code otherwise.
   */
  OFCondition setRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    DcmDataset *modificationList,
    Uint16& status,
    DcmDataset* &attributeListOut);

  /** sends an N-GET-RQ message and receives response.
   *  Any event report requests incoming before the expected response message are handled.
   *  @param sopclassUID the requested SOP class UID, must be one of the supported print classes.
   *  @param sopinstanceUID the requested SOP instance UID
   *  @param attributeIdentifierList the list of DICOM attributes to get, may be NULL.
   *    The attributes should be coded in pairs along the array (e.g. {g,e,g,e,g,e,...}).
   *  @param numShorts the number of Uint16 values in the array (not the number of pairs).
   *  @param status the response message status code is passed back in this parameter
   *    if the method returns with success status. Content is undefined otherwise.
   *  @param attributeListOut a pointer to the attribute list returned by the SCP is passed back
   *    in this parameter if the method returns with success status. Content is undefined otherwise.
   *    May return NULL if the SCP has not returned an attribute list. If a DICOM dataset is returned,
   *    it must be deleted by the caller.
   *  @return DIMSE_NORMAL if successful (e.g. no protocol failure), an error code otherwise.
   */
  OFCondition getRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    const Uint16 *attributeIdentifierList,
    size_t numShorts,
    Uint16& status,
    DcmDataset* &attributeListOut);

  /** sends an N-ACTION-RQ message and receives response.
   *  Any event report requests incoming before the expected response message are handled.
   *  @param sopclassUID the requested SOP class UID, must be one of the supported print classes.
   *  @param sopinstanceUID the requested SOP instance UID
   *  @param actionTypeID the action type ID
   *  @param actionInformation the action information dataset, may be NULL.
   *  @param status the response message status code is passed back in this parameter
   *    if the method returns with success status. Content is undefined otherwise.
   *  @param actionReply a pointer to the action reply dataset returned by the SCP is passed back
   *    in this parameter if the method returns with success status. Content is undefined otherwise.
   *    May return NULL if the SCP has not returned an attribute list. If a DICOM dataset is returned,
   *    it must be deleted by the caller.
   *  @return DIMSE_NORMAL if successful (e.g. no protocol failure), an error code otherwise.
   */
  OFCondition actionRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    Uint16 actionTypeID,
    DcmDataset *actionInformation,
    Uint16& status,
    DcmDataset* &actionReply);

  /** sends an N-DELETE-RQ message and receives response.
   *  Any event report requests incoming before the expected response message are handled.
   *  @param sopclassUID the requested SOP class UID, must be one of the supported print classes.
   *  @param sopinstanceUID the requested SOP instance UID
   *  @param status the response message status code is passed back in this parameter
   *    if the method returns with success status. Content is undefined otherwise.
   *  @return DIMSE_NORMAL if successful (e.g. no protocol failure), an error code otherwise.
   */
  OFCondition deleteRQ(
    const char *sopclassUID,
    const char *sopinstanceUID,
    Uint16& status);

  /** opens a DICOM association to a remote printer.
   *  The Basic Grayscale Print Management Meta SOP Class and Presentation LUT SOP Class
   *  are requested. The association is only accepted if the remote printer supports
   *  at least the Basic Grayscale Print Management Meta SOP Class.
   *  @param tlayer transport layer object, may be NULL.  If present, a transport
   *    layer object for TLS transports is expected.
   *  @param myAEtitle the print client's own application entity title (calling aetitle)
   *  @param peerAEtitle the printer's called aetitle
   *  @param peerHost hostname/IP address of the printer
   *  @param peerPort port number of the printer
   *  @param peerMaxPDU maximum PDU size to negotiate, must be between 8192 and 65536.
   *  @param negotiatePresentationLUT if true, Presentation LUT SOP Class is negotiated
   *  @param negotiateAnnotationBox if true, Basic Annotation Box SOP Class is negotiated
   *  @param implicitOnly if true, only the default implicit VR transfer syntax is proposed,
   *    otherwise all uncompressed transfer syntaxes are proposed.
   *  @return status code that can be checked with the SUCCESS macro. If successful, an association
   *    is established. If unsuccessful, no association is established.
   */
  OFCondition negotiateAssociation(
    DcmTransportLayer *tlayer,
    const char *myAEtitle,
    const char *peerAEtitle,
    const char *peerHost,
    int peerPort,
    long peerMaxPDU,
    OFBool negotiatePresentationLUT,
    OFBool negotiateAnnotationBox,
    OFBool implicitOnly);

  /** releases the current association.
   *  @return ASC_NORMAL or ASC_RELEASECONFIRMED upon success, an error code otherwise.
   */
  OFCondition releaseAssociation();

  /** aborts the current association.
   *  @return a status code that can be checked with the SUCCESS() macro.
   */
  OFCondition abortAssociation();

  /** registers an event handler object for incoming N-EVENT-REPORTs.
   *  @param handler pointer to the new handler object. May be NULL.
   */
  void setEventHandler(DVPSPrintEventHandler *handler) { eventHandler = handler; }

  /** sets the blocking and timeout mode for receive operations.
   *  @param blockMode blocking mode
   *  @param timeout
   */
  void setTimeout(T_DIMSE_BlockingMode blocking, int timeOut) { blockMode=blocking; timeout=timeOut; }

  /** checks if the remote printer supports the Presentation LUT SOP class.
   *  May only be called if association in place.
   *  @return true if presentation context for presentation LUT exists, false otherwise.
   */
  OFBool printerSupportsPresentationLUT();

  /** checks if the remote printer supports the Basic Annotation Box SOP class.
   *  May only be called if association in place.
   *  @return true if presentation context for Basic Annotation Box exists, false otherwise.
   */
  OFBool printerSupportsAnnotationBox();

private:

  /// private undefined copy constructor
  DVPSPrintMessageHandler(const DVPSPrintMessageHandler& copy);
  /// private undefined assignment operator
  DVPSPrintMessageHandler& operator=(const DVPSPrintMessageHandler& copy);

  /** sends a DIMSE-N request (any type except N-EVENT-REPORT) and waits for a response.
   *  Any event report requests incoming before the expected DIMSE-N-RSP are handled.
   *  If an event handler is registered, it is called for each received event report request.
   *  @param presId presentation context ID
   *  @param request DIMSE-N request message to be sent. All fields except DataSetType must be filled in.
   *  @param rqDataSet DICOM dataset to be sent with the request message, e.g. N-Set modification list,
   *    N-Action action information, or N-Create attribute list. Not applicable to N-Get or N-Delete.
   *  @param response the received response is passed back in this parameter if the method returns
   *    with success status. Content is undefined otherwise.
   *  @param statusDetail the status detail dataset of the received response is passed back in this
   *    parameter if the method returns with success status. Content is undefined otherwise.
   *  @param rspDataset the received response dataset is passed back in this
   *    parameter if the method returns with success status. Content is undefined otherwise.
   *  @return DIMSE_NORMAL if successful, an error code otherwise.
   */
  OFCondition sendNRequest(
    T_ASC_PresentationContextID presId,
    T_DIMSE_Message &request,
    DcmDataset *rqDataSet,
    T_DIMSE_Message &response,
    DcmDataset* &statusDetail,
    DcmDataset* &rspDataset);

  /** looks for an accepted presentation context for the given SOP class.
   *  If the SOP class is one of the Basic Grayscale Print Management Meta classes,
   *  looks for a presentation context for the meta SOP class.
   *  @param sopclassuid SOP Class UID for which a presentation context is searched
   *  @return presentation context if found, 0 otherwise.
   */
  T_ASC_PresentationContextID findAcceptedPC(const char *sopclassuid);

  /** dumps the given message to the dump stream if it exists.
   *  @param msg message to be dumped, should be DIMSE-N
   *  @param dataset option dataset to be dumped, may be NULL
   *  @param outgoing OFTrue if message is outgoing, OFFalse if incoming.
   */
  void dumpNMessage(T_DIMSE_Message &msg, DcmItem *dataset, OFBool outgoing);

  /// the association to be used for message communication. Can be NULL.
  T_ASC_Association *assoc;

  /// the network used for establishing associations. Can be NULL.
  T_ASC_Network *net;

  /// the current event handler. Can be NULL.
  DVPSPrintEventHandler *eventHandler;

  /// blocking mode for receive
  T_DIMSE_BlockingMode blockMode;

  /// timeout for receive
  int timeout;
};

#endif
