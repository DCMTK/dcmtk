/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Author:  Andrew Hewett, Uli Schlachter
 *
 *  Purpose:
 *    This file contains the routines for printing the
 *    contents of parsed DIMSE commands.
 *
 *    Module Prefix: DIMSE_
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:28 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dimse.h"         /* always include the module header */
#include "dcmtk/dcmdata/dcuid.h"

static void DIMSE_dumpMessage_start(OFString &str, enum DIMSE_direction dir)
{
    if (dir == DIMSE_INCOMING)
      str = "===================== INCOMING DIMSE MESSAGE ====================\n";
    else
      str = "===================== OUTGOING DIMSE MESSAGE ====================\n";
}

static void DIMSE_dumpMessage_end(OFString &str, DcmItem *dataset = NULL)
{
    str += '\n';
    if (dataset)
    {
      str += "-----------------------------------------------------------------";
      OFOStringStream stream;
      stream << DcmObject::PrintHelper(*dataset, DCMTypes::PF_shortenLongTagValues) << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(stream, result)
      str += result;
      OFSTRINGSTREAM_FREESTR(result)
    }
    str += "======================= END DIMSE MESSAGE =======================";
}

static void DIMSE_printNStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  switch(status)
  {
    case STATUS_Success:
      dumpStream << "0x0000: Success";
      break;
    case STATUS_N_Cancel:
      dumpStream << "0xFE00: Cancel";
      break;
    case STATUS_N_AttributeListError:
      dumpStream << "0x0107: Attribute list error";
      break;
    case STATUS_N_SOPClassNotSupported:
      dumpStream << "0x0122: SOP class not supported";
      break;
    case STATUS_N_ClassInstanceConflict:
      dumpStream << "0x0119: Class/instance conflict";
      break;
    case STATUS_N_DuplicateSOPInstance:
      dumpStream << "0x0111: Duplicate SOP instance";
      break;
    case STATUS_N_DuplicateInvocation:
      dumpStream << "0x0210: Duplicate invocation";
      break;
    case STATUS_N_InvalidArgumentValue:
      dumpStream << "0x0115: Invalid argument value";
      break;
    case STATUS_N_InvalidAttributeValue:
      dumpStream << "0x0106: Invalid attribute value";
      break;
    case STATUS_N_InvalidObjectInstance:
      dumpStream << "0x0117: Invalid object instance";
      break;
    case STATUS_N_MissingAttribute:
      dumpStream << "0x0120: Missing attribute";
      break;
    case STATUS_N_MissingAttributeValue:
      dumpStream << "0x0121: Missing attribute value";
      break;
    case STATUS_N_MistypedArgument:
      dumpStream << "0x0212: Mistyped argument";
      break;
    case STATUS_N_NoSuchArgument:
      dumpStream << "0x0114: No such argument";
      break;
    case STATUS_N_NoSuchAttribute:
      dumpStream << "0x0105: No such attribute";
      break;
    case STATUS_N_NoSuchEventType:
      dumpStream << "0x0113: No such event type";
      break;
    case STATUS_N_NoSuchObjectInstance:
      dumpStream << "0x0112: No such object instance";
      break;
    case STATUS_N_NoSuchSOPClass:
      dumpStream << "0x0118: No such SOP class";
      break;
    case STATUS_N_ProcessingFailure:
      dumpStream << "0x0110: Processing failure";
      break;
    case STATUS_N_ResourceLimitation:
      dumpStream << "0x0213: Resource limitation";
      break;
    case STATUS_N_UnrecognizedOperation:
      dumpStream << "0x0211: Unrecognized operation";
      break;
    case STATUS_N_PRINT_BFS_Warn_MemoryAllocation:
      dumpStream << "0xB600: Basic film session warning - Memory allocation";
      break;
    case STATUS_N_PRINT_BFS_Warn_NoSessionPrinting:
      dumpStream << "0xB601: Basic film session warning - No session printing";
      break;
    case STATUS_N_PRINT_BFS_Warn_EmptyPage:
      dumpStream << "0xB602: Basic film session warning - Empty page";
      break;
    case STATUS_N_PRINT_BFB_Warn_EmptyPage:
      dumpStream << "0xB603: Basic film box warning - Empty page";
      break;
    case STATUS_N_PRINT_BFS_Fail_NoFilmBox:
      dumpStream << "0xC600: Basic film session failure - No film box";
      break;
    case STATUS_N_PRINT_BFS_Fail_PrintQueueFull:
      dumpStream << "0xC601: Basic film session failure - Print queue full";
      break;
    case STATUS_N_PRINT_BSB_Fail_PrintQueueFull:
      dumpStream << "0xC602: Basic film box failure - Print queue full";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_ImageSize:
      dumpStream << "0xC603: Basic film session/box failure - Image size";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_PositionCollision:
      dumpStream << "0xC604: Basic film session/box failure - Position collision";
      break;
    case STATUS_N_PRINT_IB_Fail_InsufficientMemory:
      dumpStream << "0xC605: Image box failure - Insufficient memory";
      break;
    case STATUS_N_PRINT_IB_Fail_MoreThanOneVOILUT:
      dumpStream << "0xC606: Image box failure - More than one VOI LUT";
      break;
    default:
      dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
          << status << ": Unknown Status Code";
      break;
  }
}

static void DIMSE_printCStoreStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status;

  if ((status & 0xff00) == STATUS_STORE_Refused_OutOfResources)
  {
    dumpStream << ": Error: Refused - Out of resources";
  }
  else if ((status & 0xff00) == STATUS_STORE_Refused_SOPClassNotSupported)
  {
    dumpStream << ": Error: Refused - SOP Class not supported";
  }
  else if ((status & 0xff00) == STATUS_STORE_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << ": Error: Refused - Data Set does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_STORE_Error_CannotUnderstand)
  {
    dumpStream << ": Error: Cannot understand";
  }
  else if (status == STATUS_STORE_Warning_CoersionOfDataElements)
  {
    dumpStream << ": Warning: Coersion of data elements";
  }
  else if (status == STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << ": Warning: Data Set does not match SOP Class";
  }
  else if (status == STATUS_STORE_Warning_ElementsDiscarded)
  {
    dumpStream << ": Warning: Elements discarded";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << ": Success";
  }
  else
  {
    dumpStream << ": Unknown Status Code";
  }
}

static void DIMSE_printCFindStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status;

  if ((status & 0xff00) == STATUS_FIND_Refused_OutOfResources)
  {
    dumpStream << ": Error: Refused - Out of resources";
  }
  else if ((status & 0xff00) == STATUS_FIND_Refused_SOPClassNotSupported)
  {
    dumpStream << ": Error: Refused - SOP Class not supported";
  }
  else if ((status & 0xff00) == STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass)
  {
    dumpStream << ": Error: Failed - Identifier does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_FIND_Failed_UnableToProcess)
  {
    dumpStream << ": Error: Failed - Unable to process";
  }
  else if (status == STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest)
  {
    dumpStream << ": Cancel: Matching terminated due to Cancel Request";
  }
  else if (status == STATUS_FIND_Pending_WarningUnsupportedOptionalKeys)
  {
    dumpStream << ": Pending: Warning - Unsupported optional keys";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << ": Success";
  }
  else
  {
    dumpStream << ": Unknown Status Code";
  }
}

static void DIMSE_printCGetMoveStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status;

  if ((status & 0xf000) == STATUS_MOVE_Failed_UnableToProcess)
  {
    dumpStream << ": Error: Failed - Unable to process";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches)
  {
    dumpStream << ": Error: Refused - Out of resources - Number of matches";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesSubOperations)
  {
    dumpStream << ": Error: Refused - Out of resources - Suboperations";
  }
  else if (status == STATUS_MOVE_Failed_SOPClassNotSupported)
  {
    dumpStream << ": Failed: SOP Class not supported";
  }
  else if (status == STATUS_MOVE_Failed_MoveDestinationUnknown)
  {
    dumpStream << ": Failed: Move Destination unknown";
  }
  else if (status == STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass)
  {
    dumpStream << ": Failed: Identifier does not match SOP Class";
  }
  else if (status == STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication)
  {
    dumpStream << ": Cancel: Suboperations terminated due to Cancel Indication";
  }
  else if (status == STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures)
  {
    dumpStream << ": Warning: Suboperations complete, one or more failures";
  }
  else if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << ": Success";
  }
  else
  {
    dumpStream << ": Unknown Status Code";
  }
}

static void DIMSE_printCEchoStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status;
  if (DICOM_WARNING_STATUS(status))
  {
    dumpStream << ": Warning";
  }
  else if (DICOM_PENDING_STATUS(status))
  {
    dumpStream << ": Pending";
  }
  else if (status == STATUS_Success)
  {
    dumpStream << ": Success";
  }
  else
  {
    dumpStream << ": Unknown Status Code";
  }
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-ECHO RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_ECHO_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : C-ECHO RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (msg.opts & O_ECHO_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;

    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCEchoStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-STORE RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Affected SOP Instance UID     : " << msg.AffectedSOPInstanceUID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Priority                      : ";
    switch (msg.Priority)
    {
      case DIMSE_PRIORITY_LOW:
        stream << "low";
        break;
      case DIMSE_PRIORITY_MEDIUM:
        stream << "medium";
        break;
      case DIMSE_PRIORITY_HIGH:
        stream << "high";
        break;
    }
    if (msg.opts & O_STORE_MOVEORIGINATORAETITLE)
    {
        stream  << OFendl << "Move Originator AE Title      : " << msg.MoveOriginatorApplicationEntityTitle;
    }
    if (msg.opts & O_STORE_MOVEORIGINATORID)
    {
        stream  << OFendl << "Move Originator ID            : " << msg.MoveOriginatorID;
    }

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_STORE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : C-STORE RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (msg.opts & O_STORE_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_STORE_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCStoreStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-GET RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Priority                      : ";
    switch (msg.Priority)
    {
      case DIMSE_PRIORITY_LOW:
        stream << "low";
        break;
      case DIMSE_PRIORITY_MEDIUM:
        stream << "medium";
        break;
      case DIMSE_PRIORITY_HIGH:
        stream << "high";
        break;
    }

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_GET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : C-GET RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (msg.opts & O_GET_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Remaining Suboperations       : ";
    if (msg.opts & O_GET_NUMBEROFREMAININGSUBOPERATIONS) stream << msg.NumberOfRemainingSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Completed Suboperations       : ";
    if (msg.opts & O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS) stream << msg.NumberOfCompletedSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Failed Suboperations          : ";
    if (msg.opts & O_GET_NUMBEROFFAILEDSUBOPERATIONS) stream << msg.NumberOfFailedSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Warning Suboperations         : ";
    if (msg.opts & O_GET_NUMBEROFWARNINGSUBOPERATIONS) stream << msg.NumberOfWarningSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCGetMoveStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-MOVE RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Priority                      : ";
    switch (msg.Priority)
    {
      case DIMSE_PRIORITY_LOW:
        stream << "low";
        break;
      case DIMSE_PRIORITY_MEDIUM:
        stream << "medium";
        break;
      case DIMSE_PRIORITY_HIGH:
        stream << "high";
        break;
    }
    stream << OFendl << "Move Destination              : " << msg.MoveDestination;

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_MOVE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : C-MOVE RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (msg.opts & O_MOVE_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Remaining Suboperations       : ";
    if (msg.opts & O_MOVE_NUMBEROFREMAININGSUBOPERATIONS) stream << msg.NumberOfRemainingSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Completed Suboperations       : ";
    if (msg.opts & O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS) stream << msg.NumberOfCompletedSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Failed Suboperations          : ";
    if (msg.opts & O_MOVE_NUMBEROFFAILEDSUBOPERATIONS) stream << msg.NumberOfFailedSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Warning Suboperations         : ";
    if (msg.opts & O_MOVE_NUMBEROFWARNINGSUBOPERATIONS) stream << msg.NumberOfWarningSubOperations << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCGetMoveStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-FIND RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Priority                      : ";
    switch (msg.Priority)
    {
      case DIMSE_PRIORITY_LOW:
        stream << "low";
        break;
      case DIMSE_PRIORITY_MEDIUM:
        stream << "medium";
        break;
      case DIMSE_PRIORITY_HIGH:
        stream << "high";
        break;
    }

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_FIND_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : C-FIND RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (msg.opts & O_FIND_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCFindStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_CancelRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-CANCEL RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-EVENT-REPORT RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Affected SOP Instance UID     : " << msg.AffectedSOPInstanceUID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Event Type ID                 : " << msg.EventTypeID;

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-EVENT-REPORT RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Event Type ID                 : ";
    if (msg.opts & O_NEVENTREPORT_EVENTTYPEID) stream << msg.EventTypeID << OFendl;
    else stream << "none" << OFendl;
    stream << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-GET RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
           << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Attribute Identifier List     : ";
    if (msg.ListCount == 0) stream << "none";
    else for (int cList = 0; cList < msg.ListCount; cList += 2)
    {
      stream << "(" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
             << msg.AttributeIdentifierList[cList]
             << "," << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
             << msg.AttributeIdentifierList[cList + 1] << ") ";
    }

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NGET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-GET RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NGET_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NGET_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-SET RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
           << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NSET_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-SET RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NSET_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NSET_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-ACTION RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
           << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
           << "Action Type ID                : " << msg.ActionTypeID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NACTION_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-ACTION RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NACTION_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NACTION_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "Action Type ID                : ";
    if (msg.opts & O_NACTION_ACTIONTYPEID) stream << msg.ActionTypeID << OFendl;
    else stream << "none" << OFendl;
    stream << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-CREATE RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Affected SOP Class UID        : " << (uid ? uid : msg.AffectedSOPClassUID) << OFendl
           << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NCREATE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-CREATE RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NCREATE_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRQ &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = dcmFindNameOfUID(msg.RequestedSOPClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : N-DELETE RQ" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID                    : " << msg.MessageID << OFendl
           << "Requested SOP Class UID       : " << (uid ? uid : msg.RequestedSOPClassUID) << OFendl
           << "Requested SOP Instance UID    : " << msg.RequestedSOPInstanceUID << OFendl
           << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present");

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRSP &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    OFOStringStream stream;
    const char *uid = NULL;

    DIMSE_dumpMessage_start(str, dir);
    if (msg.opts & O_NDELETE_AFFECTEDSOPCLASSUID) uid = dcmFindNameOfUID(msg.AffectedSOPClassUID);
    stream << "Message Type                  : N-DELETE RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : " ;
    if (msg.opts & O_NDELETE_AFFECTEDSOPCLASSUID) stream << (uid ? uid : msg.AffectedSOPClassUID) << OFendl;
    else stream << "none" << OFendl;
    stream << "Affected SOP Instance UID     : ";
    if (msg.opts & O_NDELETE_AFFECTEDSOPINSTANCEUID) stream << msg.AffectedSOPInstanceUID << OFendl;
    else stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printNStatusString(stream, msg.DimseStatus);

    OFSTRINGSTREAM_GETSTR(stream, result)
    str += result;
    OFSTRINGSTREAM_FREESTR(stream)

    DIMSE_dumpMessage_end(str, dataset);

    return str;
}

OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_Message &msg, enum DIMSE_direction dir, DcmItem *dataset, T_ASC_PresentationContextID presID)
{
    switch (msg.CommandField) {
    case DIMSE_NOTHING:
        DIMSE_dumpMessage_start(str, dir);
        str += "Undefined Request/Response";
        DIMSE_dumpMessage_end(str, dataset);
        break;
    case DIMSE_C_STORE_RQ:
        DIMSE_dumpMessage(str, msg.msg.CStoreRQ, dir, dataset, presID);
        break;
    case DIMSE_C_STORE_RSP:
        DIMSE_dumpMessage(str, msg.msg.CStoreRSP, dir, dataset, presID);
        break;
    case DIMSE_C_GET_RQ:
        DIMSE_dumpMessage(str, msg.msg.CGetRQ, dir, dataset, presID);
        break;
    case DIMSE_C_GET_RSP:
        DIMSE_dumpMessage(str, msg.msg.CGetRSP, dir, dataset, presID);
        break;
    case DIMSE_C_FIND_RQ:
        DIMSE_dumpMessage(str, msg.msg.CFindRQ, dir, dataset, presID);
        break;
    case DIMSE_C_FIND_RSP:
        DIMSE_dumpMessage(str, msg.msg.CFindRSP, dir, dataset, presID);
        break;
    case DIMSE_C_MOVE_RQ:
        DIMSE_dumpMessage(str, msg.msg.CMoveRQ, dir, dataset, presID);
        break;
    case DIMSE_C_MOVE_RSP:
        DIMSE_dumpMessage(str, msg.msg.CMoveRSP, dir, dataset, presID);
        break;
    case DIMSE_C_ECHO_RQ:
        DIMSE_dumpMessage(str, msg.msg.CEchoRQ, dir, dataset, presID);
        break;
    case DIMSE_C_ECHO_RSP:
        DIMSE_dumpMessage(str, msg.msg.CEchoRSP, dir, dataset, presID);
        break;
    case DIMSE_C_CANCEL_RQ:
        DIMSE_dumpMessage(str, msg.msg.CCancelRQ, dir, dataset, presID);
        break;
    case DIMSE_N_EVENT_REPORT_RQ:
        DIMSE_dumpMessage(str, msg.msg.NEventReportRQ, dir, dataset, presID);
        break;
    case DIMSE_N_EVENT_REPORT_RSP:
        DIMSE_dumpMessage(str, msg.msg.NEventReportRSP, dir, dataset, presID);
        break;
    case DIMSE_N_GET_RQ:
        DIMSE_dumpMessage(str, msg.msg.NGetRQ, dir, dataset, presID);
        break;
    case DIMSE_N_GET_RSP:
        DIMSE_dumpMessage(str, msg.msg.NGetRSP, dir, dataset, presID);
        break;
    case DIMSE_N_SET_RQ:
        DIMSE_dumpMessage(str, msg.msg.NSetRQ, dir, dataset, presID);
        break;
    case DIMSE_N_SET_RSP:
        DIMSE_dumpMessage(str, msg.msg.NSetRSP, dir, dataset, presID);
        break;
    case DIMSE_N_ACTION_RQ:
        DIMSE_dumpMessage(str, msg.msg.NActionRQ, dir, dataset, presID);
        break;
    case DIMSE_N_ACTION_RSP:
        DIMSE_dumpMessage(str, msg.msg.NActionRSP, dir, dataset, presID);
        break;
    case DIMSE_N_CREATE_RQ:
        DIMSE_dumpMessage(str, msg.msg.NCreateRQ, dir, dataset, presID);
        break;
    case DIMSE_N_CREATE_RSP:
        DIMSE_dumpMessage(str, msg.msg.NCreateRSP, dir, dataset, presID);
        break;
    case DIMSE_N_DELETE_RQ:
        DIMSE_dumpMessage(str, msg.msg.NDeleteRQ, dir, dataset, presID);
        break;
    case DIMSE_N_DELETE_RSP:
        DIMSE_dumpMessage(str, msg.msg.NDeleteRSP, dir, dataset, presID);
        break;
    default:
        DIMSE_dumpMessage_start(str, dir);
        str += "DIMSE_printCommand: Bad msg->CommandField";
        DIMSE_dumpMessage_end(str, dataset);
        break;
    }

    return str;
}

/*
 * CVS Log
 * $Log: dimdump.cc,v $
 * Revision 1.17  2010-10-14 13:14:28  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.16  2010-10-13 08:08:15  joergr
 * Replaced copyright header since source code has been rewritten completely.
 * Previously: "Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM"
 *
 * Revision 1.15  2010-09-30 09:50:53  joergr
 * Added missing call to DIMSE_dumpMessage_start() for C-GET-RQ message.
 *
 * Revision 1.14  2010-06-14 16:00:21  joergr
 * Slightly modified output of DIMSE_dumpMessage_end().
 *
 * Revision 1.13  2009-12-08 16:44:25  joergr
 * Slightly modified some log messages.
 *
 * Revision 1.12  2009-11-18 11:53:59  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.11  2009-08-04 10:05:58  joergr
 * Added optional parameter to printCStore() functions that allows for printing
 * the Presentation Context ID.
 *
 * Revision 1.10  2009-04-07 09:09:22  joergr
 * Fixed issue with unknown UID in uid2name().
 *
 * Revision 1.9  2006/08/15 16:04:29  meichel
 * Updated the code in module dcmnet to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.8  2005/12/08 15:44:40  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2003/03/12 17:35:04  meichel
 * Updated DcmObject::print() flags
 *
 * Revision 1.6  2002/11/27 13:04:39  meichel
 * Adapted module dcmnet to use of new header file ofstdinc.h
 *
 * Revision 1.5  2000/06/07 08:57:54  meichel
 * dcmnet DIMSE routines now allow to retrieve raw command sets as DcmDataset
 *   objects, e.g. for logging purposes. Added enhanced message dump functions.
 *
 * Revision 1.4  2000/02/23 15:12:32  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.3  2000/02/01 10:24:09  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.2  1998/08/10 08:53:42  meichel
 * renamed member variable in DIMSE structures from "Status" to
 *   "DimseStatus". This is required if dcmnet is used together with
 *   <X11/Xlib.h> where Status is #define'd as int.
 *
 * Revision 1.1.1.1  1996/03/26 18:38:45  hewett
 * Initial Release.
 *
 *
 */
