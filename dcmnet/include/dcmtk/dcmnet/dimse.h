/*
 *
 *  Copyright (C) 1994-2020, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**
**  This software and supporting documentation were
**  developed by
**
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**
**  for CEN/TC251/WG4 as a contribution to the Radiological
**  Society of North America (RSNA) 1993 Digital Imaging and
**  Communications in Medicine (DICOM) Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE
**  IS WITH THE USER.
**
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice.
**
*/

/*
**
** Author: Andrew Hewett                Created: 03-06-93
**
** Module: dimse
**
** Purpose:
**      This file contains the routines which provide dimse layer services
**      for DICOM applications.
**
** Module Prefix: DIMSE_
**
*/


#ifndef DIMSE_H
#define DIMSE_H

/*
 * Required Include Files.
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/ofstd/ofglobal.h"

class DcmOutputFileStream;

// include this file in doxygen documentation

/** @file dimse.h
 *  @brief type definitions, constants, global variables and functions for the dcmnet/dimse module
 */

/** Global flag to enable/disable workaround code for some buggy Store SCUs
 * in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
 * Affected SOP Instance UID field of the C-STORE-RQ message is retained
 * in the corresponding C-STORE-RSP message.
 * To enable the workaround, this flag must be set to OFTrue and
 * dcmEnableAutomaticInputDataCorrection must be set to OFFalse
 * (see declaration in dcmdata/include/dcobject.h).
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<OFBool> dcmPeerRequiresExactUIDCopy; /* default OFFalse */

/** global flag allowing to restrict the maximum size of outgoing
 *  P-DATA PDUs to a value less than the maximum supported by the
 *  remote application entity or this library.  May be useful
 *  if there is an interaction between PDU size and other network
 *  layers, e. g. TLS, IP or below.
 */
extern DCMTK_DCMNET_EXPORT OFGlobal<Uint32> dcmMaxOutgoingPDUSize; /* default 2^32-1 */


/*
 * General Status Codes.
 */
#define STATUS_Success  0x0000
#define STATUS_Pending  0xff00  // deprecated, will be removed in a future version

/*
 * Status Classes (see DICOM PS3.7 Annex C).
 */
#define DICOM_SUCCESS_STATUS(status)  ((status) == 0x0000)
#define DICOM_FAILURE_STATUS(status)  ((((status) & 0xf000) == 0xa000) || (((status) & 0xf000) == 0xc000) || ((((status) & 0xff00) == 0x0100) && ((status) != 0x0107) && ((status) != 0x0116)) || (((status) & 0xff00) == 0x0200))
#define DICOM_WARNING_STATUS(status)  ((((status) & 0xf000) == 0xb000) || ((status) == 0x0001) || ((status) == 0x0107) || ((status) == 0x0116))
#define DICOM_CANCEL_STATUS(status)   ((status) == 0xfe00)
#define DICOM_PENDING_STATUS(status)  (((status) == 0xff00) || ((status) == 0xff01))
#define DICOM_STANDARD_STATUS(status) (((status) == 0x0000) || ((status) == 0x0001) || (((status) & 0xff00) == 0x0100) || (((status) & 0xff00) == 0x0200) || (((status) & 0xf000) == 0xa000) || (((status) & 0xf000) == 0xb000) || (((status) & 0xf000) == 0xc000) || ((status) == 0xfe00) || ((status) == 0xff00) || ((status) == 0xff01))

/*
 * Service Class Specific Status Codes.
 * NOTE: some codes are only significant in the high byte or high nibble (4 bits).
 */

/* General C-ECHO Codes */
#define STATUS_ECHO_Success                                             0x0000
#define STATUS_ECHO_Refused_SOPClassNotSupported                        0x0122
#define STATUS_ECHO_DuplicateInvocation                                 0x0210
#define STATUS_ECHO_UnrecognizedOperation                               0x0211
#define STATUS_ECHO_MistypedArgument                                    0x0212

/* General C-STORE Codes */
#define STATUS_STORE_Success                                            0x0000
#define STATUS_STORE_Refused_SOPClassNotSupported                       0x0122
#define STATUS_STORE_Refused_NotAuthorized                              0x0124
#define STATUS_STORE_InvalidSOPClass                                    0x0117
#define STATUS_STORE_DuplicateInvocation                                0x0210
#define STATUS_STORE_UnrecognizedOperation                              0x0211
#define STATUS_STORE_MistypedArgument                                   0x0212
/* Service Class Specific C-STORE Codes (Storage) */
#define STATUS_STORE_Refused_OutOfResources             /* high byte */ 0xa700
#define STATUS_STORE_Error_DataSetDoesNotMatchSOPClass  /* high byte */ 0xa900
#define STATUS_STORE_Error_CannotUnderstand           /* high nibble */ 0xc000
#define STATUS_STORE_Warning_CoercionOfDataElements                     0xb000
#define STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass                0xb007
#define STATUS_STORE_Warning_ElementsDiscarded                          0xb006

/* General C-FIND Codes */
#define STATUS_FIND_Success                                             0x0000
#define STATUS_FIND_Refused_SOPClassNotSupported                        0x0122
#define STATUS_FIND_Cancel                                              0xfe00
/* Service Class Specific C-FIND Codes */
/* (Query/Retrieve, Modality Worklist Management, Relevant Patient Information Query) */
#define STATUS_FIND_Success_MatchingIsComplete                          0x0000
#define STATUS_FIND_Refused_OutOfResources                              0xa700
#define STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass               0xa900  // deprecated, will be removed in a future version
#define STATUS_FIND_Error_DataSetDoesNotMatchSOPClass                   0xa900
#define STATUS_FIND_Failed_UnableToProcess            /* high nibble */ 0xc000
#define STATUS_FIND_Failed_MoreThanOneMatchFound                        0xc100
#define STATUS_FIND_Failed_UnableToSupportRequestedTemplate             0xc200
#define STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest         0xfe00
#define STATUS_FIND_Pending_MatchesAreContinuing                        0xff00
#define STATUS_FIND_Pending_WarningUnsupportedOptionalKeys              0xff01

/* General C-MOVE Codes */
#define STATUS_MOVE_Success                                             0x0000
#define STATUS_MOVE_Refused_SOPClassNotSupported                        0x0122
#define STATUS_MOVE_Refused_NotAuthorized                               0x0124
#define STATUS_MOVE_Cancel                                              0xfe00
#define STATUS_MOVE_DuplicateInvocation                                 0x0210
#define STATUS_MOVE_UnrecognizedOperation                               0x0211
#define STATUS_MOVE_MistypedArgument                                    0x0212
/* Service Class Specific C-MOVE Codes */
/* (Query/Retrieve, Composite Instance Root Retrieve) */
#define STATUS_MOVE_Success_SubOperationsCompleteNoFailures             0x0000
#define STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches               0xa701
#define STATUS_MOVE_Refused_OutOfResourcesSubOperations                 0xa702
#define STATUS_MOVE_Refused_MoveDestinationUnknown                      0xa801
#define STATUS_MOVE_Failed_SOPClassNotSupported                         0x0122  // deprecated, will be removed in a future version
#define STATUS_MOVE_Failed_MoveDestinationUnknown                       0xa801  // deprecated, will be removed in a future version
#define STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass               0xa900  // deprecated, will be removed in a future version
#define STATUS_MOVE_Error_DataSetDoesNotMatchSOPClass                   0xa900
#define STATUS_MOVE_Failed_NoneOfTheFramesWereFoundInSOPInstance        0xaa00
#define STATUS_MOVE_Failed_UnableToCreateNewObjectForThisSOPClass       0xaa01
#define STATUS_MOVE_Failed_UnableToExtractFrames                        0xaa02
#define STATUS_MOVE_Failed_TimeBasedRequestForNonTimeBasedSOPInstance   0xaa03
#define STATUS_MOVE_Failed_InvalidRequest                               0xaa04
#define STATUS_MOVE_Failed_UnableToProcess            /* high nibble */ 0xc000
#define STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication 0xfe00
#define STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures      0xb000
#define STATUS_MOVE_Pending_SubOperationsAreContinuing                  0xff00

/* General C-GET Codes */
#define STATUS_GET_Success                                              0x0000
#define STATUS_GET_Refused_SOPClassNotSupported                         0x0122
#define STATUS_GET_Cancel                                               0xfe00
#define STATUS_GET_DuplicateInvocation                                  0x0210
#define STATUS_GET_UnrecognizedOperation                                0x0211
#define STATUS_GET_MistypedArgument                                     0x0212
/* Service Class Specific C-GET Codes */
/* (Query/Retrieve, Composite Instance Root Retrieve and others) */
#define STATUS_GET_Success_SubOperationsCompleteNoFailures              0x0000
#define STATUS_GET_Refused_OutOfResourcesNumberOfMatches                0xa701
#define STATUS_GET_Refused_OutOfResourcesSubOperations                  0xa702
#define STATUS_GET_Failed_SOPClassNotSupported                          0x0122  // deprecated, will be removed in a future version
#define STATUS_GET_Failed_IdentifierDoesNotMatchSOPClass                0xa900  // deprecated, will be removed in a future version
#define STATUS_GET_Error_DataSetDoesNotMatchSOPClass                    0xa900
#define STATUS_GET_Failed_NoneOfTheFramesWereFoundInSOPInstance         0xaa00
#define STATUS_GET_Failed_UnableToCreateNewObjectForThisSOPClass        0xaa01
#define STATUS_GET_Failed_UnableToExtractFrames                         0xaa02
#define STATUS_GET_Failed_TimeBasedRequestForNonTimeBasedSOPInstance    0xaa03
#define STATUS_GET_Failed_InvalidRequest                                0xaa04
#define STATUS_GET_Failed_UnableToProcess             /* high nibble */ 0xc000
#define STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication  0xfe00
#define STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures       0xb000
#define STATUS_GET_Pending_SubOperationsAreContinuing                   0xff00

/* General DIMSE-N Codes */
#define STATUS_N_Success                                                0x0000
#define STATUS_N_Refused_NotAuthorized                                  0x0124
#define STATUS_N_Cancel                                                 0xfe00
#define STATUS_N_AttributeListError                                     0x0107
#define STATUS_N_SOPClassNotSupported                                   0x0122
#define STATUS_N_ClassInstanceConflict                                  0x0119
#define STATUS_N_DuplicateSOPInstance                                   0x0111
#define STATUS_N_DuplicateInvocation                                    0x0210
#define STATUS_N_InvalidArgumentValue                                   0x0115
#define STATUS_N_InvalidAttributeValue                                  0x0106
#define STATUS_N_AttributeValueOutOfRange                               0x0116
#define STATUS_N_InvalidObjectInstance                                  0x0117  // deprecated, will be removed in a future version
#define STATUS_N_InvalidSOPInstance                                     0x0117
#define STATUS_N_MissingAttribute                                       0x0120
#define STATUS_N_MissingAttributeValue                                  0x0121
#define STATUS_N_MistypedArgument                                       0x0212
#define STATUS_N_NoSuchAction                                           0x0123
#define STATUS_N_NoSuchArgument                                         0x0114
#define STATUS_N_NoSuchAttribute                                        0x0105
#define STATUS_N_NoSuchEventType                                        0x0113
#define STATUS_N_NoSuchObjectInstance                                   0x0112  // deprecated, will be removed in a future version
#define STATUS_N_NoSuchSOPInstance                                      0x0112
#define STATUS_N_NoSuchSOPClass                                         0x0118
#define STATUS_N_ProcessingFailure                                      0x0110
#define STATUS_N_ResourceLimitation                                     0x0213
#define STATUS_N_UnrecognizedOperation                                  0x0211
/* the following Codes are used by multiple Services */
#define STATUS_N_Warning_RequestedOptionalAttributesNotSupported        0x0001

/* Service Class Specific DIMSE-N Codes */
/* (Print Management) */
#define STATUS_N_PRINT_BFS_Warn_MemoryAllocation                        0xb600
#define STATUS_N_PRINT_BFS_Warn_NoSessionPrinting                       0xb601
#define STATUS_N_PRINT_BFS_Warn_EmptyPage                               0xb602
#define STATUS_N_PRINT_BFB_Warn_EmptyPage                               0xb603
#define STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageDemagnified                 0xb604
#define STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageCropped                     0xb609
#define STATUS_N_PRINT_BFS_BFB_IB_Warn_ImageDecimated                   0xb60a
#define STATUS_N_PRINT_BFS_Fail_NoFilmBox                               0xc600
#define STATUS_N_PRINT_BFS_Fail_PrintQueueFull                          0xc601
#define STATUS_N_PRINT_BFB_Fail_PrintQueueFull                          0xc602
#define STATUS_N_PRINT_BFS_BFB_Fail_ImageSize                           0xc603
#define STATUS_N_PRINT_BFS_BFB_Fail_PositionCollision                   0xc604  // retired, see DICOM PS3.4
#define STATUS_N_PRINT_BFS_BFB_Fail_CombinedImageSize                   0xc613
#define STATUS_N_PRINT_IB_Warn_MinMaxDensity                            0xb605
#define STATUS_N_PRINT_IB_Fail_InsufficientMemory                       0xc605
#define STATUS_N_PRINT_IB_Fail_MoreThanOneVOILUT                        0xc606
/* (Modality Performed Procedure Step Retrieve) */
#define STATUS_N_MPPS_Warning_RequestedOptionalAttributesNotSupported   0x0001
/* (Application Event Logging) */
#define STATUS_N_LOG_Failure_ProceduralLoggingNotAvailable              0xc101
#define STATUS_N_LOG_Failure_EventInformationDoesNotMatchTemplate       0xc102
#define STATUS_N_LOG_Failure_CannotMatchEventToCurrentStudy             0xc103
#define STATUS_N_LOG_Failure_IDsInconsistentInMatchingCurrentStudy      0xc104
#define STATUS_N_LOG_Warning_SynchronizationFrameOfReferenceDoesNotMatch 0xb101
#define STATUS_N_LOG_Warning_StudyInstanceUIDCoercion                   0xb102
#define STATUS_N_LOG_Warning_IDsInconsistentInMatchingCurrentStudy      0xb104
/* (Media Creation Management) */
#define STATUS_N_MEDIA_Failed_MediaCreationActionAlreadyReceived        0xa510
#define STATUS_N_MEDIA_Failed_MediaCreationRequestAlreadyCompleted      0xc201
#define STATUS_N_MEDIA_Failed_MediaCreationRequestAlreadyInProgress     0xc202
#define STATUS_N_MEDIA_Failed_CancellationDenied                        0xc203
#define STATUS_N_MEDIA_Warning_RequestedOptionalAttributesNotSupported  0x0001

// --- TODO (DIMSE-N) ---

// the following Services use different DIMSE Status Code meanings for
// each SOP Class, including standard Codes such as 0x0000 (Success):

/* (Unified Procedure Step) */
/* (RT Machine Verification) */


/*
 * Type Definitions.
 */


/*
 * DIMSE Command Codes (Request and Response).
 */
typedef enum {                  /* DIC_US */
    DIMSE_NOTHING = 0x0000,     /* none of the rest !! */
    DIMSE_C_STORE_RQ = 0x0001,  /* also known as C_SEND_RQ  */
    DIMSE_C_STORE_RSP = 0x8001, /* also known as C_SEND_RSP */
    DIMSE_C_GET_RQ = 0x0010,
    DIMSE_C_GET_RSP = 0x8010,
    DIMSE_C_FIND_RQ = 0x0020,
    DIMSE_C_FIND_RSP = 0x8020,
    DIMSE_C_MOVE_RQ = 0x0021,
    DIMSE_C_MOVE_RSP = 0x8021,
    DIMSE_C_ECHO_RQ = 0x0030,
    DIMSE_C_ECHO_RSP = 0x8030,
    DIMSE_C_CANCEL_RQ = 0x0fff,
    /* there is no DIMSE_C_CANCEL_RSP */

    DIMSE_N_EVENT_REPORT_RQ = 0x0100,
    DIMSE_N_EVENT_REPORT_RSP = 0x8100,
    DIMSE_N_GET_RQ = 0x0110,
    DIMSE_N_GET_RSP = 0x8110,
    DIMSE_N_SET_RQ = 0x0120,
    DIMSE_N_SET_RSP = 0x8120,
    DIMSE_N_ACTION_RQ = 0x0130,
    DIMSE_N_ACTION_RSP = 0x8130,
    DIMSE_N_CREATE_RQ = 0x0140,
    DIMSE_N_CREATE_RSP = 0x8140,
    DIMSE_N_DELETE_RQ = 0x0150,
    DIMSE_N_DELETE_RSP = 0x8150
} T_DIMSE_Command;

/*
 * DIMSE Data Set Type Codes.
 */

typedef enum {                  /* DIC_US */
    DIMSE_DATASET_PRESENT = 0x0001,     /* anything other than 0x0101 */
    DIMSE_DATASET_NULL = 0x0101
} T_DIMSE_DataSetType;

/*
 * DIMSE Priority Codes.
 */

typedef enum {                  /* DIC_US */
    DIMSE_PRIORITY_LOW = 0x0002,
    DIMSE_PRIORITY_MEDIUM = 0x0000,
    DIMSE_PRIORITY_HIGH = 0x0001
} T_DIMSE_Priority;

/*
 * DIMSE Blocking Modes (reading).
 */

typedef enum {
    DIMSE_BLOCKING,
    DIMSE_NONBLOCKING
} T_DIMSE_BlockingMode;


/*
 * DIMSE Messages.
 *
 */

/* C-STORE */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_StoreRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* M */
        DIC_AE          MoveOriginatorApplicationEntityTitle;   /* U */
        DIC_US          MoveOriginatorID;                       /* U */
        /* DataSet provided as argument to DIMSE functions */   /* M */
        unsigned int    opts; /* which optional items are set */
#define O_STORE_MOVEORIGINATORAETITLE                   0x0001
#define O_STORE_MOVEORIGINATORID                        0x0002
        /* the following flag is set on incoming C-STORE requests if
         * the SOP instance UID is (incorrectly) padded with a space
         * character. Will only be detected if the dcmdata flag
         * dcmEnableAutomaticInputDataCorrection is false.
         */
#define O_STORE_RQ_BLANK_PADDING                        0x0008
};

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_StoreRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U(=) */
        unsigned int    opts; /* which optional items are set */
#define O_STORE_AFFECTEDSOPCLASSUID             0x0001
#define O_STORE_AFFECTEDSOPINSTANCEUID          0x0002
        /* peer requires an exact copy of the SOP instance UID
         * as it was sent in the C-STORE-RQ,
         * including any illegal trailing space padding.
         */
#define O_STORE_PEER_REQUIRES_EXACT_UID_COPY    0x0004
        /* SOP instance UID in C-STORE-RQ was space padded. */
#define O_STORE_RSP_BLANK_PADDING               0x0008
} ;

/* C-ECHO */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_EchoRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_EchoRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        unsigned int    opts; /* which optional items are set */
#define O_ECHO_AFFECTEDSOPCLASSUID              0x0001
} ;

/* C-FIND */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_FindRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_FindRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        /* Identifier provided as argument to DIMSE functions *//* C */
        unsigned int    opts; /* which optional items are set */
#define O_FIND_AFFECTEDSOPCLASSUID              0x0001
} ;

/* C-GET */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_GetRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_GetRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        DIC_US          NumberOfRemainingSubOperations;         /* C */
        DIC_US          NumberOfCompletedSubOperations;         /* C */
        DIC_US          NumberOfFailedSubOperations;            /* C */
        DIC_US          NumberOfWarningSubOperations;           /* C */
        unsigned int    opts; /* which optional items are set */
#define O_GET_AFFECTEDSOPCLASSUID               0x0001
#define O_GET_NUMBEROFREMAININGSUBOPERATIONS    0x0002
#define O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS    0x0004
#define O_GET_NUMBEROFFAILEDSUBOPERATIONS       0x0008
#define O_GET_NUMBEROFWARNINGSUBOPERATIONS      0x0010
} ;

/* C-MOVE */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_MoveRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_AE          MoveDestination;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_MoveRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        DIC_US          NumberOfRemainingSubOperations;         /* C */
        DIC_US          NumberOfCompletedSubOperations;         /* C */
        DIC_US          NumberOfFailedSubOperations;            /* C */
        DIC_US          NumberOfWarningSubOperations;           /* C */
        unsigned int    opts; /* which optional items are set */
#define O_MOVE_AFFECTEDSOPCLASSUID              0x0001
#define O_MOVE_NUMBEROFREMAININGSUBOPERATIONS   0x0002
#define O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS   0x0004
#define O_MOVE_NUMBEROFFAILEDSUBOPERATIONS      0x0008
#define O_MOVE_NUMBEROFWARNINGSUBOPERATIONS     0x0010
} ;


/* C-CANCEL */

struct DCMTK_DCMNET_EXPORT T_DIMSE_C_CancelRQ {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;


/* N-EVENT-REPORT */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_EventReportRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          EventTypeID;                            /* M */
        /* EventInformation provided as argument to DIMSE functions *//* U */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_EventReportRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          EventTypeID;                            /* C(=) */
        /* EventReply provided as argument to DIMSE functions *//* C */
        unsigned int    opts; /* which optional items are set */
#define O_NEVENTREPORT_AFFECTEDSOPCLASSUID              0x0001
#define O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID           0x0002
#define O_NEVENTREPORT_EVENTTYPEID                      0x0004
} ;

/* N-GET */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_GetRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /*
         * In the following array, the attributes to get should be coded
         * in pairs along the array (e.g. {g,e,g,e,g,e,...}).  The ListCount
         * variable should contain the number of 'DIC_US' values in the
         * array (not the number of pairs).
         */
        int             ListCount;
        DIC_US          *AttributeIdentifierList;               /* U */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_GetRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* AttributeList provided as argument to DIMSE functions *//* C */
        unsigned int    opts; /* which optional items are set */
#define O_NGET_AFFECTEDSOPCLASSUID              0x0001
#define O_NGET_AFFECTEDSOPINSTANCEUID           0x0002
} ;

/* N-SET */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_SetRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* ModificationList provided as argument to DIMSE functions *//* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_SetRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* AttributeList provided as argument to DIMSE functions *//* U */
        unsigned int    opts; /* which optional items are set */
#define O_NSET_AFFECTEDSOPCLASSUID              0x0001
#define O_NSET_AFFECTEDSOPINSTANCEUID           0x0002
} ;

/* N-ACTION */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_ActionRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        DIC_US          ActionTypeID;                           /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* ActionInformation provided as argument to DIMSE functions *//* U */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_ActionRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        DIC_US          ActionTypeID;                           /* C(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* ActionReply provided as argument to DIMSE functions *//* C */
        unsigned int    opts; /* which optional items are set */
#define O_NACTION_AFFECTEDSOPCLASSUID           0x0001
#define O_NACTION_AFFECTEDSOPINSTANCEUID        0x0002
#define O_NACTION_ACTIONTYPEID                  0x0004
} ;

/* N-CREATE */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_CreateRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* AttributeList provided as argument to DIMSE functions *//* M */
        unsigned int    opts; /* which optional items are set */
#define O_NCREATE_AFFECTEDSOPINSTANCEUID        0x0002
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_CreateRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* C */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* AttributeList provided as argument to DIMSE functions *//* U */
        unsigned int    opts; /* which optional items are set */
#define O_NCREATE_AFFECTEDSOPCLASSUID           0x0001
#define O_NCREATE_AFFECTEDSOPINSTANCEUID        0x0002
} ;

/* N-DELETE */

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_DeleteRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;

struct DCMTK_DCMNET_EXPORT T_DIMSE_N_DeleteRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U */
        DIC_US          DimseStatus;                            /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        unsigned int    opts; /* which optional items are set */
#define O_NDELETE_AFFECTEDSOPCLASSUID           0x0001
#define O_NDELETE_AFFECTEDSOPINSTANCEUID        0x0002
} ;



/*
 * Composite  DIMSE Message
 */

struct DCMTK_DCMNET_EXPORT T_DIMSE_Message {
        T_DIMSE_Command CommandField;   /* M */

        union {
                /* requests */
                T_DIMSE_C_StoreRQ CStoreRQ;
                T_DIMSE_C_EchoRQ  CEchoRQ;
                T_DIMSE_C_FindRQ  CFindRQ;
                T_DIMSE_C_GetRQ   CGetRQ;
                T_DIMSE_C_MoveRQ  CMoveRQ;
                T_DIMSE_C_CancelRQ  CCancelRQ;
                T_DIMSE_N_EventReportRQ NEventReportRQ;
                T_DIMSE_N_GetRQ NGetRQ;
                T_DIMSE_N_SetRQ NSetRQ;
                T_DIMSE_N_ActionRQ NActionRQ;
                T_DIMSE_N_CreateRQ NCreateRQ;
                T_DIMSE_N_DeleteRQ NDeleteRQ;

                /* responses */
                T_DIMSE_C_StoreRSP CStoreRSP;
                T_DIMSE_C_EchoRSP CEchoRSP;
                T_DIMSE_C_FindRSP CFindRSP;
                T_DIMSE_C_GetRSP  CGetRSP;
                T_DIMSE_C_MoveRSP CMoveRSP;
                T_DIMSE_N_EventReportRSP NEventReportRSP;
                T_DIMSE_N_GetRSP NGetRSP;
                T_DIMSE_N_SetRSP NSetRSP;
                T_DIMSE_N_ActionRSP NActionRSP;
                T_DIMSE_N_CreateRSP NCreateRSP;
                T_DIMSE_N_DeleteRSP NDeleteRSP;
        } msg;

};


/*
 * Global Variables
 */

/*
 * Define global defaults for data encoding when sending out data-sets.
 * These can be adjusted to allow variants to be tested.
 */

extern DCMTK_DCMNET_EXPORT E_GrpLenEncoding  g_dimse_send_groupLength_encoding;    /* default: EGL_recalcGL */
extern DCMTK_DCMNET_EXPORT E_EncodingType    g_dimse_send_sequenceType_encoding;   /* default: EET_ExplicitLength */

/*
 * If this global flag is set to OFTrue, all DIMSE messages sent or received
 * are stored in files with the name
 * "dimse-TTT-DDD-XXXX.dcm" or "dimse-TTT-DDD-XXXX-YY.dcm", where
 *   TTT is "cmd" for a command set, or "dat" for a data set
 *   DDD is "snd" for data sent, or "rcv" for data received
 *   XXXX is a counter, starting at 1 (global for all associations)
 *   YY is an additional counter used if multiple datasets follow a single command set
 * The files are in implicit VR little endian encoding, without meta-header.
 */

extern DCMTK_DCMNET_EXPORT OFBool            g_dimse_save_dimse_data;              /* default: OFFalse */

/*
 * Public Function Prototypes
 */


/*
 * High Level DIMSE Messaging.
 */

/*
 * General Information.
 *
 * Many of the DIMSE routines take parameters for a blocking mode
 * and a timeout.  In all cases, these parameters are only used
 * when receiving commands or data sets (reading).  There are no blocking
 * and timeout options for send operations.
 * When receiving commands or data sets:
 * If the parameter blockMode is DIMSE_BLOCKING, the DIMSE routine
 * will wait until a response arrives before returning.
 * If the parameter blockMode is DIMSE_NONBLOCKING then the DIMSE routine
 * will wait at most timeout seconds for a response and if no response arrives
 * will return DIMSE_NODATAAVAILABLE.  In both cases waiting will be
 * interrupted by association release, and abort messages, or network
 * disruption.
 */


/*
 * Verification Service Class
 */

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_echoUser(
        /* in */
        T_ASC_Association *assoc, DIC_US msgId,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* out */
        DIC_US *status,
        /* if a pointer to a DcmDataset instance is passed in this variable,
         * then any status detail information from the C-ECHO-RSP message will
         * be returned in this dataset.
         */
        DcmDataset **statusDetail);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendEchoResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        const T_DIMSE_C_EchoRQ *request, DIC_US status, DcmDataset *statusDetail);


/*
 * Storage Service Class
 */

typedef enum {
    DIMSE_StoreBegin,       /* before data set */
    DIMSE_StoreProgressing, /* during data set */
    DIMSE_StoreEnd          /* after data set */
} T_DIMSE_StoreProgressState;

struct DCMTK_DCMNET_EXPORT T_DIMSE_StoreProgress { /* progress structure for store callback routines */
    T_DIMSE_StoreProgressState state;   /* current state */
    long callbackCount; /* callback execution count */
    long progressBytes; /* sent/received so far */
    long totalBytes;    /* total/estimated total to send/receive */
} ;


typedef void (*DIMSE_StoreUserCallback)(
    void *callbackData,
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *request  /* original store request */
   );

struct DCMTK_DCMNET_EXPORT T_DIMSE_DetectedCancelParameters {
    OFBool cancelEncountered;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_CancelRQ req;
} ;

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_storeUser(
        /* in */
        T_ASC_Association *assoc, T_ASC_PresentationContextID presId,
        T_DIMSE_C_StoreRQ *request,
        const char *imageFileName, DcmDataset *imageDataSet,
        DIMSE_StoreUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* out */
        T_DIMSE_C_StoreRSP *response,
        /* if a pointer to a DcmDataset instance is passed in this variable,
         * then any status detail information from the C-STORE-RSP message will
         * be returned in this dataset.
         */
        DcmDataset **statusDetail,
        T_DIMSE_DetectedCancelParameters *checkForCancelParams = NULL,
        /* in */
        long imageFileTotalBytes=0);

typedef void (*DIMSE_StoreProviderCallback)(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *request,         /* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* in/out */
    T_DIMSE_C_StoreRSP *response,       /* final store response */
    /* out. The callback can assign a pointer to a dataset here and insert
     * status detail attributes which will be added to the C-STORE-RSP message.
     * The ownership of the DcmDataset instance is not transferred, and it is
     * the responsibility of the library user to delete the instance after
     * the call to DIMSE_storeProvider() has completed.
     */
    DcmDataset **statusDetail);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_storeProvider(/* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_StoreRQ *request,
    const char* imageFileName, int writeMetaheader,
    DcmDataset **imageDataSet,
        DIMSE_StoreProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendStoreResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        const T_DIMSE_C_StoreRQ *request, /* send response to this request */
        T_DIMSE_C_StoreRSP *response,     /* response structure */
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (FIND)
 */


typedef void (*DIMSE_FindUserCallback)(
        /* in */
        void *callbackData,
        T_DIMSE_C_FindRQ *request,      /* original find request */
        int responseCount,              /* number of responses so far*/
        T_DIMSE_C_FindRSP *response,    /* pending response received */
        DcmDataset *responseIdentifiers /* pending response identifiers */
        );

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_findUser(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_FindRQ *request, DcmDataset *requestIdentifiers,
        int &responseCount,
        DIMSE_FindUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* out */
        T_DIMSE_C_FindRSP *response,
        /* if a pointer to a DcmDataset instance is passed in this variable,
         * then any status detail information from the C-FIND-RSP message will
         * be returned in this dataset.
         */
        DcmDataset **statusDetail);

typedef void (*DIMSE_FindProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_FindRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_FindRSP *response,
        DcmDataset **responseIdentifiers,
        /* out. The callback can assign a pointer to a dataset here and insert
         * status detail attributes which will be added to the C-FIND-RSP message.
         * The ownership of the DcmDataset instance is not transferred, and it is
         * the responsibility of the library user to delete the instance after
         * the call to DIMSE_storeProvider() has completed.
         */
        DcmDataset **statusDetail);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_findProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_FindRQ *request,
        DIMSE_FindProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendFindResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        const T_DIMSE_C_FindRQ *request,
        T_DIMSE_C_FindRSP *response, DcmDataset *responseIdentifiers,
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (MOVE)
 */

typedef void (*DIMSE_MoveUserCallback)(
        /* in */
        void *callbackData,
        T_DIMSE_C_MoveRQ *request,
        int responseCount, T_DIMSE_C_MoveRSP *response);

typedef void (*DIMSE_SubOpProviderCallback)(void *subOpCallbackData,
        T_ASC_Network *net, T_ASC_Association **subOpAssoc);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_moveUser(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_MoveRQ *request,
        DcmDataset *requestIdentifiers,
        DIMSE_MoveUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* sub-operation provider callback */
        T_ASC_Network *net,
        DIMSE_SubOpProviderCallback subOpCallback, void *subOpCallbackData,
        /* out */
        T_DIMSE_C_MoveRSP *response,
        /* if a pointer to a DcmDataset instance is passed in this variable,
         * then any status detail information from the C-MOVE-RSP message will
         * be returned in this dataset.
         */
        DcmDataset **statusDetail,
        DcmDataset **responseIdentifers,
        OFBool ignorePendingDatasets = OFFalse);

typedef void (*DIMSE_MoveProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_MoveRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_MoveRSP *response,
        /* out. The callback can assign a pointer to a dataset here and insert
         * status detail attributes which will be added to the C-MOVE-RSP message.
         * The ownership of the DcmDataset instance is not transferred, and it is
         * the responsibility of the library user to delete the instance after
         * the call to DIMSE_storeProvider() has completed.
         */
        DcmDataset **statusDetail,
        DcmDataset **responseIdentifiers);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_moveProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_MoveRQ *request,
        DIMSE_MoveProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendMoveResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID, const T_DIMSE_C_MoveRQ *request,
        T_DIMSE_C_MoveRSP *response, DcmDataset *rspIds,
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (GET)
 */

typedef void (*DIMSE_GetUserCallback)(
        /* in */
        void *callbackData,
        T_DIMSE_C_GetRQ *request,
        int responseCount, T_DIMSE_C_GetRSP *response);

typedef void (*DIMSE_SubOpProviderCallback)(void *subOpCallbackData,
        T_ASC_Network *net, T_ASC_Association **subOpAssoc);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_getUser(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers,
        DIMSE_GetUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* sub-operation provider callback */
        T_ASC_Network *net,
        DIMSE_SubOpProviderCallback subOpCallback, void *subOpCallbackData,
        /* out */
        T_DIMSE_C_GetRSP *response,
        /* if a pointer to a DcmDataset instance is passed in this variable,
         * then any status detail information from the C-GET-RSP message will
         * be returned in this dataset.
         */
        DcmDataset **statusDetail,
        DcmDataset **responseIdentifers);

typedef void (*DIMSE_GetProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_GetRSP *response,
        /* out. The callback can assign a pointer to a dataset here and insert
         * status detail attributes which will be added to the C-GET-RSP message.
         * The ownership of the DcmDataset instance is not transferred, and it is
         * the responsibility of the library user to delete the instance after
         * the call to DIMSE_storeProvider() has completed.
         */
        DcmDataset **statusDetail,
        DcmDataset **responseIdentifiers);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_getProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_GetRQ *request,
        DIMSE_GetProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendGetResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID, const T_DIMSE_C_GetRQ *request,
        T_DIMSE_C_GetRSP *response, DcmDataset *rspIds,
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (CANCEL)
 */

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendCancelRequest(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presId, DIC_US msgId);

DCMTK_DCMNET_EXPORT OFCondition
DIMSE_checkForCancelRQ(T_ASC_Association * assoc,
    T_ASC_PresentationContextID presId, DIC_US msgId);


/****
 *
 * Low Level DIMSE Messaging
 * With the exception of DIMSE_receiveCommand,
 * the following function are only intended for use if you
 * wish to implement an alternative interface to the
 * higher level routines provided above.
 * Service call providers will have to use DIMSE_receiveCommand
 * before calling the appropriate provider routine declared above.
 *
 */


typedef void (*DIMSE_ProgressCallback)(void *callbackContext,
    unsigned long byteCount);

/** send a DIMSE command and possibly also instance data from a file via network to another
 *  DICOM application.
 *  @param assoc           The association (network connection to another DICOM application).
 *  @param presId          The ID of the presentation context which shall be used
 *  @param msg             Structure that represents a certain DIMSE command which shall be sent.
 *  @param statusDetail    Detailed information with regard to the status information which is captured
 *                         in the status element (0000,0900). Note that the value for element (0000,0900)
 *                         is contained in this variable.
 *  @param dataFileName    The name of the file that contains the instance data which shall be sent to
 *                          the other DICOM application, NULL; if there is none.
 *  @param callback        Pointer to a function which shall be called to indicate progress.
 *  @param callbackContext pointer to opaque object passed to the callback
 *  @param commandSet      [out] If this parameter is not NULL
 *                         it will return a copy of the DIMSE command which is sent to the other
 *                         DICOM application.
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendMessageUsingFileData(T_ASC_Association *association,
                  T_ASC_PresentationContextID presID,
                  T_DIMSE_Message *msg, DcmDataset *statusDetail,
                  const char* dataFileName,
                  DIMSE_ProgressCallback callback,
                  void *callbackContext,
                  DcmDataset **commandSet=NULL);


/** send a DIMSE command and possibly also instance data from a data object via network
 *  to another DICOM application.
 *  @param assoc            The association (network connection to another DICOM application).
 *  @param presId           The ID of the presentation context which shall be used
 *  @param msg              Structure that represents a certain DIMSE command which shall be sent.
 *  @param statusDetail     Detailed information with regard to the status information which is captured
 *                          in the status element (0000,0900). Note that the value for element (0000,0900)
 *                          is contained in this variable.
 *  @param dataObject       The instance data which shall be sent to the other DICOM application,
 *                          NULL, if there is none
 *  @param callback         Pointer to a function which shall be called to indicate progress.
 *  @param callbackContext  pointer to opaque object passed to the callback
 *  @param commandSet       [out] If this parameter is not NULL
 *                          it will return a copy of the DIMSE command which is sent to the other
 *                          DICOM application.
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_sendMessageUsingMemoryData(T_ASC_Association *association,
                  T_ASC_PresentationContextID presID,
                  T_DIMSE_Message *msg, DcmDataset *statusDetail,
                  DcmDataset *dataObject,
                  DIMSE_ProgressCallback callback,
                  void *callbackContext,
                  DcmDataset **commandSet=NULL);

/** receive a DIMSE command via network from another DICOM application.
 *  @param assoc        The association (network connection to another DICOM application).
 *  @param blocking     The blocking mode for reading data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
 *  @param timeout      Timeout interval for receiving data. If the blocking mode is DIMSE_NONBLOCKING
 *                      and we are trying to read data from the incoming socket stream and no data has
 *                      been received after timeout seconds, an error will be reported.
 *  @param presId       [out] Contains in the end the ID of the presentation context which was specified in the DIMSE command.
 *  @param msg          [out] Contains in the end information which represents a certain DIMSE command which was received.
 *  @param statusDetail [out] If a non-NULL value is passed this variable will in the end contain detailed
 *                      information with regard to the status information which is captured in the status
 *                      element (0000,0900). Note that the value for element (0000,0900) is not contained
 *                      in this return value but in msg. For details on the structure of this object, see
 *                      DICOM standard (year 2000) part 7, annex C) (or the corresponding section in a later
 *                      version of the standard.)
 *  @param commandSet   [out] If this parameter is not NULL
 *                      it will return a copy of the DIMSE command which was received from the other
 *                      DICOM application.
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_receiveCommand(T_ASC_Association *association,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     T_ASC_PresentationContextID *presID,
                     T_DIMSE_Message *msg,
                     DcmDataset **statusDetail,
                     DcmDataset **commandSet=NULL);

/** receive one data set (of instance data) via network from another DICOM application and store in memory
 *  @param assoc           The association (network connection to another DICOM application).
 *  @param blocking        The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
 *  @param timeout         Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
 *  @param presID          [out] Contains in the end the ID of the presentation context which was used in the PDVs
 *                         that were received on the network. If the PDVs show different presentation context
 *                         IDs, this function will return an error.
 *  @param dataObject      [out] Contains in the end the information which was received over the network.
 *                         Note that this function assumes that either imageFileName or imageDataSet does not equal NULL.
 *  @param callback        Pointer to a function which shall be called to indicate progress.
 *  @param callbackData    Pointer to data which shall be passed to the progress indicating function
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_receiveDataSetInMemory(T_ASC_Association *association,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     T_ASC_PresentationContextID *presID,
                     DcmDataset **dataObject,
                     DIMSE_ProgressCallback callback,
                     void *callbackContext);

/** create a DICOM file, populate the meta-header from the content of the given
 *  C-STORE request, and return an ouput stream that can be used to store the
 *  dataset associated with the C-STORE request message. This function is used
 *  in conjunction with DIMSE_receiveDataSetInFile() to directly "stream" incoming
 *  DICOM datasets to file without storing them in memory and without passing
 *  them through the dcmdata parser.
 *  @param filename filename of file to be created
 *  @param request C-STORE request message from which the meta-header is populated
 *  @param assoc association network association over which the C-STORE request
 *    was received. Used to populate the aetitles in the metaheader.
 *  @param presIdCmd presentation context ID of the C-STORE message, determines
 *    the transfer syntax
 *  @param writeMetaheader write file with/without metaheader
 *  @param filestream pointer to output stream returned in this variable upon success.
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_createFilestream(
                     /* in */
                     const OFFilename &filename,
                     const T_DIMSE_C_StoreRQ *request,
                     const T_ASC_Association *assoc,
                     T_ASC_PresentationContextID presIdCmd,
                     int writeMetaheader,
                     /* out */
                     DcmOutputFileStream **filestream);

/** receive one data set (of instance data) via network from another DICOM application and store in file.
 *  @param assoc           The association (network connection to another DICOM application).
 *  @param blocking        The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
 *  @param timeout         Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
 *  @param presID          [out] Contains in the end the ID of the presentation context which was used in the PDVs
 *                         that were received on the network. If the PDVs show different presentation context
 *                         IDs, this function will return an error.
 *  @param filestream      output stream to which the incoming dataset is written
 *  @param dataObject      [out] Contains in the end the information which was received over the network.
 *                         Note that this function assumes that either imageFileName or imageDataSet does not equal NULL.
 *  @param callback        Pointer to a function which shall be called to indicate progress.
 *  @param callbackData    Pointer to data which shall be passed to the progress indicating function
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_receiveDataSetInFile(T_ASC_Association *assoc,
                     T_DIMSE_BlockingMode blocking, int timeout,
                     T_ASC_PresentationContextID *presID,
                     DcmOutputStream *filestream,
                     DIMSE_ProgressCallback callback, void *callbackData);

/** receive and discard one data set (of instance data) via network from another DICOM application.
 *  @param assoc           The association (network connection to another DICOM application).
 *  @param blocking        The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
 *  @param timeout         Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
 *  @param bytesRead       [out] number of bytes read
 *  @param pdvCount        [out] number of PDVs read
 *  @return EC_Normal if successful, an error code otherwise.
 */
DCMTK_DCMNET_EXPORT OFCondition
DIMSE_ignoreDataSet( T_ASC_Association * assoc,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     DIC_UL * bytesRead,
                     DIC_UL * pdvCount);

/*
 * Misc functions
 */

DCMTK_DCMNET_EXPORT OFString DIMSE_warn_str(T_ASC_Association *assoc);

enum DIMSE_direction {
    DIMSE_INCOMING,
    DIMSE_OUTGOING
};

/* Debugging functions for printing contents of a command structure */
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_Message &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_CancelRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
DCMTK_DCMNET_EXPORT OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);


// Compatibility wrapper
#define DIMSE_COMPAT_WRAP(func, type)                 \
    static inline void func (FILE * f, type * msg)    \
    {                                                 \
        OFString str;                                 \
        DIMSE_dumpMessage(str, *msg, DIMSE_INCOMING); \
        fprintf(f, "%s\n", str.c_str());              \
    }
#define DIMSE_COMPAT_WRAP_R(func, type)         \
    DIMSE_COMPAT_WRAP(func ## RQ, type ## RQ)   \
    DIMSE_COMPAT_WRAP(func ## RSP, type ## RSP)

static inline void DIMSE_printMessage(STD_NAMESPACE ostream& ostream, T_DIMSE_Message &msg, DcmItem *dataset = NULL)
{
    OFString str;
    ostream << DIMSE_dumpMessage(str, msg, DIMSE_INCOMING, dataset) << OFendl;
}

DIMSE_COMPAT_WRAP(DIMSE_printCommand, T_DIMSE_Message)
DIMSE_COMPAT_WRAP_R(DIMSE_printCEcho, T_DIMSE_C_Echo)
DIMSE_COMPAT_WRAP_R(DIMSE_printCStore, T_DIMSE_C_Store)
DIMSE_COMPAT_WRAP_R(DIMSE_printCGet, T_DIMSE_C_Get)
DIMSE_COMPAT_WRAP_R(DIMSE_printCMove, T_DIMSE_C_Move)
DIMSE_COMPAT_WRAP_R(DIMSE_printCFind, T_DIMSE_C_Find)
DIMSE_COMPAT_WRAP(DIMSE_printCCancelRQ, T_DIMSE_C_EchoRQ)
DIMSE_COMPAT_WRAP_R(DIMSE_printNEventReport, T_DIMSE_N_EventReport)
DIMSE_COMPAT_WRAP_R(DIMSE_printNGet, T_DIMSE_N_Get)
DIMSE_COMPAT_WRAP_R(DIMSE_printNSet, T_DIMSE_N_Set)
DIMSE_COMPAT_WRAP_R(DIMSE_printNAction, T_DIMSE_N_Action)
DIMSE_COMPAT_WRAP_R(DIMSE_printNCreate, T_DIMSE_N_Create)
DIMSE_COMPAT_WRAP_R(DIMSE_printNDelete, T_DIMSE_N_Delete)

#undef DIMSE_COMPAT_WRAP
#undef DIMSE_COMPAT_WRAP_R

#endif
