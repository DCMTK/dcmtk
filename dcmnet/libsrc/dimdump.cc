/*
 *
 *  Copyright (C) 1993-2021, OFFIS e.V.
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
 */


/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

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
      OFSTRINGSTREAM_GETOFSTRING(stream, result);
      str += result;
    }
    str += "======================= END DIMSE MESSAGE =======================";
}

static void DIMSE_printStatusClassString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* try to determine the Status Class of the DIMSE Status Code */
  if (DICOM_SUCCESS_STATUS(status))
    dumpStream << "Success";
  else if (DICOM_FAILURE_STATUS(status))
    dumpStream << "Failure";
  else if (DICOM_WARNING_STATUS(status))
    dumpStream << "Warning";
  else if (DICOM_CANCEL_STATUS(status))
    dumpStream << "Cancel";
  else if (DICOM_PENDING_STATUS(status))
    dumpStream << "Pending";
  else if (DICOM_STANDARD_STATUS(status))
    dumpStream << "Unknown Status Code";
  else
    dumpStream << "Unknown Status Code (non-standard)";
}

static void DIMSE_printNStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  switch(status)
  {
    /* General DIMSE-N Codes */
    case STATUS_N_Success:
      dumpStream << "Success";
      break;
    case STATUS_N_Refused_NotAuthorized:
      dumpStream << "Refused: Not authorized";
      break;
    case STATUS_N_Cancel:
      dumpStream << "Cancel";
      break;
    case STATUS_N_AttributeListError:
      dumpStream << "Attribute list error";
      break;
    case STATUS_N_SOPClassNotSupported:
      dumpStream << "SOP Class not supported";
      break;
    case STATUS_N_ClassInstanceConflict:
      dumpStream << "Class-Instance conflict";
      break;
    case STATUS_N_DuplicateSOPInstance:
      dumpStream << "Duplicate SOP Instance";
      break;
    case STATUS_N_DuplicateInvocation:
      dumpStream << "Duplicate invocation";
      break;
    case STATUS_N_InvalidArgumentValue:
      dumpStream << "Invalid argument value";
      break;
    case STATUS_N_InvalidAttributeValue:
      dumpStream << "Invalid attribute value";
      break;
    case STATUS_N_AttributeValueOutOfRange:
      dumpStream << "Attribute value out of range";
      break;
    case STATUS_N_InvalidSOPInstance:
      dumpStream << "Invalid SOP Instance";
      break;
    case STATUS_N_MissingAttribute:
      dumpStream << "Missing attribute";
      break;
    case STATUS_N_MissingAttributeValue:
      dumpStream << "Missing attribute value";
      break;
    case STATUS_N_MistypedArgument:
      dumpStream << "Mistyped argument";
      break;
    case STATUS_N_NoSuchAction:
      dumpStream << "No such action";
      break;
    case STATUS_N_NoSuchArgument:
      dumpStream << "No such argument";
      break;
    case STATUS_N_NoSuchAttribute:
      dumpStream << "No such attribute";
      break;
    case STATUS_N_NoSuchEventType:
      dumpStream << "No such Event Type";
      break;
    case STATUS_N_NoSuchSOPInstance:
      dumpStream << "No such SOP Instance";
      break;
    case STATUS_N_NoSuchSOPClass:
      dumpStream << "No such SOP Class";
      break;
    case STATUS_N_ProcessingFailure:
      dumpStream << "Processing failure";
      break;
    case STATUS_N_ResourceLimitation:
      dumpStream << "Resource limitation";
      break;
    case STATUS_N_UnrecognizedOperation:
      dumpStream << "Unrecognized operation";
      break;

    /* Print Management Service Class Specific Codes */
    case STATUS_N_PRINT_BFS_Warn_MemoryAllocation:
      dumpStream << "Basic film session warning - Memory allocation";
      break;
    case STATUS_N_PRINT_BFS_Warn_NoSessionPrinting:
      dumpStream << "Basic film session warning - No session printing";
      break;
    case STATUS_N_PRINT_BFS_Warn_EmptyPage:
      dumpStream << "Basic film session warning - Empty page";
      break;
    case STATUS_N_PRINT_BFB_Warn_EmptyPage:
      dumpStream << "Basic film box warning - Empty page";
      break;
    case STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageDemagnified:
      dumpStream << "Basic film session/box or image box warning - Image demagnified";
      break;
    case STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageCropped:
      dumpStream << "Basic film session/box or image box warning - Image cropped";
      break;
    case STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageDecimated:
      dumpStream << "Basic film session/box or image box warning - Image decimated";
      break;
    case STATUS_N_PRINT_BFS_Fail_NoFilmBox:
      dumpStream << "Basic film session failure - No film box";
      break;
    case STATUS_N_PRINT_BFS_Fail_PrintQueueFull:
      dumpStream << "Basic film session failure - Print queue full";
      break;
    case STATUS_N_PRINT_BFB_Fail_PrintQueueFull:
      dumpStream << "Basic film box failure - Print queue full";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_ImageSize:
      dumpStream << "Basic film session/box failure - Image size";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_PositionCollision:    // retired
      dumpStream << "Basic film session/box failure - Position collision (retired)";
      break;
    case STATUS_N_PRINT_BFS_BFB_Fail_CombinedImageSize:
      dumpStream << "Basic film session/box failure - Combined image size";
      break;
    case STATUS_N_PRINT_IB_Warn_MinMaxDensity:
      dumpStream << "Image box warning - Min/Max density";
      break;
    case STATUS_N_PRINT_IB_Fail_InsufficientMemory:
      dumpStream << "Image box failure - Insufficient memory";
      break;
    case STATUS_N_PRINT_IB_Fail_MoreThanOneVOILUT:
      dumpStream << "Image box failure - More than one VOI LUT";
      break;

    /* Modality Performed Procedure Step Retrieve SOP Class Specific Codes */
    case STATUS_N_MPPS_Warning_RequestedOptionalAttributesNotSupported:
      dumpStream << "MPPS retrieve warning: Requested optional attributes not supported";
      break;

    /* Application Event Logging Service Class Specific Codes */
    case STATUS_N_LOG_Failure_ProceduralLoggingNotAvailable:
      dumpStream << "Event logging failure - Procedural logging not available";
      break;
    case STATUS_N_LOG_Failure_EventInformationDoesNotMatchTemplate:
      dumpStream << "Event logging failure - Event information does not match template";
      break;
    case STATUS_N_LOG_Failure_CannotMatchEventToCurrentStudy:
      dumpStream << "Event logging failure - Cannot match event to a current study";
      break;
    case STATUS_N_LOG_Failure_IDsInconsistentInMatchingCurrentStudy:
      dumpStream << "Event logging failure - IDs inconsistent in matching a current study - Event not logged";
      break;
    case STATUS_N_LOG_Warning_SynchronizationFrameOfReferenceDoesNotMatch:
      dumpStream << "Event logging warning - Synchronization Frame of Reference does not match";
      break;
    case STATUS_N_LOG_Warning_StudyInstanceUIDCoercion:
      dumpStream << "Event logging warning - Study Instance UID coercion";
      break;
    case STATUS_N_LOG_Warning_IDsInconsistentInMatchingCurrentStudy:
      dumpStream << "Event logging warning - IDs inconsistent in matching a current study - Event logged";
      break;

    /* Media Creation Management Service Class Specific Codes */
    case STATUS_N_MEDIA_Failed_MediaCreationActionAlreadyReceived:
      dumpStream << "Media creation failure - Action already received";
      break;
    case STATUS_N_MEDIA_Failed_MediaCreationRequestAlreadyCompleted:
      dumpStream << "Media creation failure - Request already completed";
      break;
    case STATUS_N_MEDIA_Failed_MediaCreationRequestAlreadyInProgress:
      dumpStream << "Media creation failure - Request already in progress";
      break;
    case STATUS_N_MEDIA_Failed_CancellationDenied:
      dumpStream << "Media creation failure - Cancellation denied";
      break;

    /* other codes (try to determine the Status Class) */
    default:
      DIMSE_printStatusClassString(dumpStream, status);
      break;
  }
}

static void DIMSE_printCStoreStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  /* General C-STORE Codes */
  if (status == STATUS_STORE_Success)
  {
    dumpStream << "Success";
  }
  else if (status == STATUS_STORE_Refused_SOPClassNotSupported)
  {
    dumpStream << "Refused: SOP Class not supported";
  }
  else if (status == STATUS_STORE_Refused_NotAuthorized)
  {
    dumpStream << "Refused: Not authorized";
  }
  else if (status == STATUS_STORE_InvalidSOPClass)
  {
    dumpStream << "Invalid SOP Class";
  }
  else if (status == STATUS_STORE_DuplicateInvocation)
  {
    dumpStream << "Duplicate invocation";
  }
  else if (status == STATUS_STORE_UnrecognizedOperation)
  {
    dumpStream << "Unrecognized operation";
  }
  else if (status == STATUS_STORE_MistypedArgument)
  {
    dumpStream << "Mistyped argument";
  }

  /* Service Class Specific C-STORE Codes */
  else if ((status & 0xff00) == STATUS_STORE_Refused_OutOfResources)
  {
    dumpStream << "Refused: Out of resources";
  }
  else if ((status & 0xff00) == STATUS_STORE_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << "Error: Data Set does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_STORE_Error_CannotUnderstand)
  {
    dumpStream << "Error: Cannot understand";
  }
  else if (status == STATUS_STORE_Warning_CoercionOfDataElements)
  {
    dumpStream << "Warning: Coercion of Data Elements";
  }
  else if (status == STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << "Warning: Data Set does not match SOP Class";
  }
  else if (status == STATUS_STORE_Warning_ElementsDiscarded)
  {
    dumpStream << "Warning: Elements discarded";
  }

  /* other codes (try to determine the Status Class) */
  else
  {
    DIMSE_printStatusClassString(dumpStream, status);
  }
}

static void DIMSE_printCFindStatusString(STD_NAMESPACE ostream& dumpStream, int status, const char *sopClass)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  /* General C-FIND Codes */
  if (status == STATUS_FIND_Refused_SOPClassNotSupported)
  {
    dumpStream << "Refused: SOP Class not supported";
  }

  /* Service Class Specific C-FIND Codes */
  else if (status == STATUS_FIND_Success_MatchingIsComplete)
  {
    dumpStream << "Success: Matching is complete";
  }
  else if (status == STATUS_FIND_Refused_OutOfResources)
  {
    dumpStream << "Refused: Out of resources";
  }
  else if ((status & 0xff00) == STATUS_FIND_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << "Error: Data Set does not match SOP Class";
  }
  else if ((status & 0xf000) == STATUS_FIND_Failed_UnableToProcess)
  {
    /* Relevant Patient Information Query uses some specific Codes in "Cxxx" */
    if (sopClass && (strncmp(sopClass, UID_RelevantPatientInformationQuery_Prefix, strlen(UID_RelevantPatientInformationQuery_Prefix)) == 0))
    {
      if (status == STATUS_FIND_Failed_MoreThanOneMatchFound)
      {
        dumpStream << "Failed: More than one match found";
      }
      else if (status == STATUS_FIND_Failed_UnableToSupportRequestedTemplate)
      {
        dumpStream << "Failed: Unable to support requested template";
      }
      else
      {
        dumpStream << "Failed: Unable to process";
      }
    }
    /* for all other Services Classes, the generic output is used */
    else
    {
      dumpStream << "Failed: Unable to process";
    }
  }
  else if (status == STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest)
  {
    dumpStream << "Cancel: Matching terminated due to Cancel Request";
  }
  else if (status == STATUS_FIND_Pending_MatchesAreContinuing)
  {
    dumpStream << "Pending: Matches are continuing";
  }
  else if (status == STATUS_FIND_Pending_WarningUnsupportedOptionalKeys)
  {
    dumpStream << "Pending: Matches are continuing - Warning: Unsupported optional keys";
  }

  /* other codes (try to determine the Status Class) */
  else
  {
    DIMSE_printStatusClassString(dumpStream, status);
  }
}

static void DIMSE_printCGetStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  /* General C-GET Codes */
  if (status == STATUS_GET_Refused_SOPClassNotSupported)
  {
    dumpStream << "Refused: SOP Class not supported";
  }
  else if (status == STATUS_GET_DuplicateInvocation)
  {
    dumpStream << "Duplicate invocation";
  }
  else if (status == STATUS_GET_UnrecognizedOperation)
  {
    dumpStream << "Unrecognized operation";
  }
  else if (status == STATUS_GET_MistypedArgument)
  {
    dumpStream << "Mistyped argument";
  }

  /* Service Class Specific C-GET Codes */
  else if (status == STATUS_GET_Success_SubOperationsCompleteNoFailures)
  {
    dumpStream << "Success: Sub-operations complete - No failures or warnings";
  }
  else if (status == STATUS_GET_Refused_OutOfResourcesNumberOfMatches)
  {
    dumpStream << "Refused: Out of resources - Unable to calculate number of matches";
  }
  else if (status == STATUS_GET_Refused_OutOfResourcesSubOperations)
  {
    dumpStream << "Refused: Out of resources - Unable to perform sub-operations";
  }
  else if (status == STATUS_GET_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << "Error: Data Set does not match SOP Class";
  }
  else if (status == STATUS_GET_Failed_NoneOfTheFramesWereFoundInSOPInstance)
  {
    dumpStream << "Failed: None of the frames requested were found in SOP Instance";
  }
  else if (status == STATUS_GET_Failed_UnableToCreateNewObjectForThisSOPClass)
  {
    dumpStream << "Failed: Unable to create new object for this SOP Class";
  }
  else if (status == STATUS_GET_Failed_UnableToExtractFrames)
  {
    dumpStream << "Failed: Unable to extract frames";
  }
  else if (status == STATUS_GET_Failed_TimeBasedRequestForNonTimeBasedSOPInstance)
  {
    dumpStream << "Failed: Time-based request for non-time-based SOP Instance";
  }
  else if (status == STATUS_GET_Failed_InvalidRequest)
  {
    dumpStream << "Failed: Invalid request";
  }
  else if ((status & 0xf000) == STATUS_GET_Failed_UnableToProcess)
  {
    dumpStream << "Failed: Unable to process";
  }
  else if (status == STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication)
  {
    dumpStream << "Cancel: Sub-operations terminated due to Cancel Indication";
  }
  else if (status == STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures)
  {
    dumpStream << "Warning: Sub-operations complete - One or more failures or warnings";
  }
  else if (status == STATUS_GET_Pending_SubOperationsAreContinuing)
  {
    dumpStream << "Pending: Sub-operations are continuing";
  }

  /* other codes (try to determine the Status Class) */
  else
  {
    DIMSE_printStatusClassString(dumpStream, status);
  }
}

static void DIMSE_printCMoveStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  /* General C-MOVE Codes */
  if (status == STATUS_MOVE_Refused_SOPClassNotSupported)
  {
    dumpStream << "Refused: SOP Class not supported";
  }
  else if (status == STATUS_MOVE_Refused_NotAuthorized)
  {
    dumpStream << "Refused: Not authorized";
  }
  else if (status == STATUS_MOVE_DuplicateInvocation)
  {
    dumpStream << "Duplicate invocation";
  }
  else if (status == STATUS_MOVE_UnrecognizedOperation)
  {
    dumpStream << "Unrecognized operation";
  }
  else if (status == STATUS_MOVE_MistypedArgument)
  {
    dumpStream << "Mistyped argument";
  }

  /* Service Class Specific C-MOVE Codes */
  else if (status == STATUS_MOVE_Success_SubOperationsCompleteNoFailures)
  {
    dumpStream << "Success: Sub-operations complete - No failures or warnings";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches)
  {
    dumpStream << "Refused: Out of resources - Unable to calculate number of matches";
  }
  else if (status == STATUS_MOVE_Refused_OutOfResourcesSubOperations)
  {
    dumpStream << "Refused: Out of resources - Unable to perform sub-operations";
  }
  else if (status == STATUS_MOVE_Refused_MoveDestinationUnknown)
  {
    dumpStream << "Refused: Move Destination unknown";
  }
  else if (status == STATUS_MOVE_Error_DataSetDoesNotMatchSOPClass)
  {
    dumpStream << "Error: Data Set does not match SOP Class";
  }
  else if (status == STATUS_MOVE_Failed_NoneOfTheFramesWereFoundInSOPInstance)
  {
    dumpStream << "Failed: None of the frames requested were found in SOP Instance";
  }
  else if (status == STATUS_MOVE_Failed_UnableToCreateNewObjectForThisSOPClass)
  {
    dumpStream << "Failed: Unable to create new object for this SOP Class";
  }
  else if (status == STATUS_MOVE_Failed_UnableToExtractFrames)
  {
    dumpStream << "Failed: Unable to extract frames";
  }
  else if (status == STATUS_MOVE_Failed_TimeBasedRequestForNonTimeBasedSOPInstance)
  {
    dumpStream << "Failed: Time-based request for non-time-based SOP Instance";
  }
  else if (status == STATUS_MOVE_Failed_InvalidRequest)
  {
    dumpStream << "Failed: Invalid request";
  }
  else if ((status & 0xf000) == STATUS_MOVE_Failed_UnableToProcess)
  {
    dumpStream << "Failed: Unable to process";
  }
  else if (status == STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication)
  {
    dumpStream << "Cancel: Sub-operations terminated due to Cancel Indication";
  }
  else if (status == STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures)
  {
    dumpStream << "Warning: Sub-operations complete - One or more failures or warnings";
  }
  else if (status == STATUS_MOVE_Pending_SubOperationsAreContinuing)
  {
    dumpStream << "Pending: Sub-operations are continuing";
  }

  /* other codes (try to determine the Status Class) */
  else
  {
    DIMSE_printStatusClassString(dumpStream, status);
  }
}

static void DIMSE_printCEchoStatusString(STD_NAMESPACE ostream& dumpStream, int status)
{
  /* first, output the DIMSE status code in numeric format */
  dumpStream << "0x" << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
      << STD_NAMESPACE setw(4) << status << ": ";

  /* General C-ECHO Codes */
  if (status == STATUS_ECHO_Success)
  {
    dumpStream << "Success";
  }
  else if (status == STATUS_ECHO_Refused_SOPClassNotSupported)
  {
    dumpStream << "Refused: SOP Class not supported";
  }
  else if (status == STATUS_ECHO_DuplicateInvocation)
  {
    dumpStream << "Duplicate invocation";
  }
  else if (status == STATUS_ECHO_UnrecognizedOperation)
  {
    dumpStream << "Unrecognized operation";
  }
  else if (status == STATUS_ECHO_MistypedArgument)
  {
    dumpStream << "Mistyped argument";
  }

  /* other codes (so try to determine the Status Class) */
  else
  {
    DIMSE_printStatusClassString(dumpStream, status);
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
    DIMSE_printCGetStatusString(stream, msg.DimseStatus);

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
    DIMSE_printCMoveStatusString(stream, msg.DimseStatus);

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
    const char *sopClassUID = (msg.opts & O_FIND_AFFECTEDSOPCLASSUID) ? msg.AffectedSOPClassUID : NULL;
    const char *sopClassName = dcmFindNameOfUID(sopClassUID);

    DIMSE_dumpMessage_start(str, dir);
    stream << "Message Type                  : C-FIND RSP" << OFendl;
    if (presID > 0)
    {
        stream << "Presentation Context ID       : " << OFstatic_cast(int, presID) << OFendl;
    }
    stream << "Message ID Being Responded To : " << msg.MessageIDBeingRespondedTo << OFendl
           << "Affected SOP Class UID        : ";
    if (sopClassName)
        stream << sopClassName << OFendl;
    else if (sopClassUID)
        stream << sopClassUID << OFendl;
    else
        stream << "none" << OFendl;
    stream << "Data Set                      : " << ((msg.DataSetType==DIMSE_DATASET_NULL) ? "none" : "present") << OFendl
           << "DIMSE Status                  : ";
    DIMSE_printCFindStatusString(stream, msg.DimseStatus, sopClassUID);

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
        str += "Message Type                  : UNKNOWN (DIMSE Protocol Error)";
        DIMSE_dumpMessage_end(str, dataset);
        break;
    }

    return str;
}
