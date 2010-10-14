/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DVPSPR_H
#define DVPSPR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dimse.h"

/** pure abstract event handler class for N-EVENT-REPORT.
 */

class DVPSPrintEventHandler
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

class DVPSPrintMessageHandler
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

/*
 *  $Log: dvpspr.h,v $
 *  Revision 1.17  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2010-10-07 14:31:36  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved).
 *
 *  Revision 1.15  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.14  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.13  2006-08-15 16:57:01  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.12  2005/12/09 14:48:35  meichel
 *  Added missing virtual destructors
 *
 *  Revision 1.11  2005/12/08 16:03:57  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.10  2003/09/05 14:31:33  meichel
 *  Print SCU now supports TLS connections.
 *
 *  Revision 1.9  2001/10/12 13:46:52  meichel
 *  Adapted dcmpstat to OFCondition based dcmnet module (supports strict mode).
 *
 *  Revision 1.8  2001/06/01 15:50:19  meichel
 *  Updated copyright header
 *
 *  Revision 1.7  2000/06/02 16:00:50  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.6  2000/03/08 16:28:55  meichel
 *  Updated copyright header.
 *
 *  Revision 1.5  1999/10/28 08:18:56  meichel
 *  Print client does not attempt any more to negotiate Presentation LUT or
 *    Annotation Box if config file says that the printer does not support them.
 *
 *  Revision 1.4  1999/10/13 14:10:47  meichel
 *  Now negotiation Basic Annotation Box SOP Class
 *
 *  Revision 1.3  1999/09/24 15:23:47  meichel
 *  Print spooler (dcmprtsv) now logs diagnostic messages in log files
 *    when operating in spool mode.
 *
 *  Revision 1.2  1999/09/17 14:33:58  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.1  1999/07/30 13:34:49  meichel
 *  Added new classes managing Stored Print objects
 *
 *
 */
