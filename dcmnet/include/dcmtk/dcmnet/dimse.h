/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:10 $
** CVS/RCS Revision:    $Revision: 1.23 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DIMSE_H
#define DIMSE_H

/*
 * Required Include Files
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

/** Global flag to enable/disable workaround code for some buggy Store SCUs
 * in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
 * Affected SOP Instance UID field of the C-STORE-RQ message is retained
 * in the corresponding C-STORE-RSP message.
 * To enable the workaround, this flag must be set to OFTrue and
 * dcmEnableAutomaticInputDataCorrection must be set to OFFalse.
 * (see declaration in dcmdata/include/dcobject.h)
 */
extern OFGlobal<OFBool> dcmPeerRequiresExactUIDCopy; /* default OFFalse */

/** global flag allowing to restrict the maximum size of outgoing
 *  P-DATA PDUs to a value less than the maximum supported by the
 *  remote application entity or this library.  May be useful
 *  if there is an interaction between PDU size and other network
 *  layers, e. g. TLS, IP or below.
 */
extern OFGlobal<Uint32> dcmMaxOutgoingPDUSize; /* default 2^32-1 */


/*
 * General Status Codes
 */
#define STATUS_Success  0x0000
#define STATUS_Pending  0xff00

#define DICOM_PENDING_STATUS(status) (((status)&0xff00) == 0xff00)
#define DICOM_WARNING_STATUS(status) (((status)&0xf000) == 0xb000)

/*
 * Service Class Specific Status Codes
 * NOTE: some codes are only significant in the high byte
 * or high nibble (4 bits).
 */
/* Storage Specific Codes*/
#define STATUS_STORE_Refused_OutOfResources             /* high byte */ 0xa700
#define STATUS_STORE_Refused_SOPClassNotSupported       /* high byte */ 0xa800
#define STATUS_STORE_Error_DataSetDoesNotMatchSOPClass  /* high byte */ 0xa900
#define STATUS_STORE_Error_CannotUnderstand           /* high nibble */ 0xc000
#define STATUS_STORE_Warning_CoersionOfDataElements                     0xb000
#define STATUS_STORE_Warning_DataSetDoesNotMatchSOPClass                0xb007
#define STATUS_STORE_Warning_ElementsDiscarded                          0xb006

/* Find Specific Codes */
#define STATUS_FIND_Refused_OutOfResources                              0xa700
#define STATUS_FIND_Refused_SOPClassNotSupported                        0xa800
#define STATUS_FIND_Failed_IdentifierDoesNotMatchSOPClass               0xa900
#define STATUS_FIND_Failed_UnableToProcess            /* high nibble */ 0xc000
#define STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest         0xfe00
#define STATUS_FIND_Pending_WarningUnsupportedOptionalKeys              0xff01

/* Move Specific Codes */
#define STATUS_MOVE_Refused_OutOfResourcesNumberOfMatches               0xa701
#define STATUS_MOVE_Refused_OutOfResourcesSubOperations                 0xa702
#define STATUS_MOVE_Failed_SOPClassNotSupported                         0xa800
#define STATUS_MOVE_Failed_MoveDestinationUnknown                       0xa801
#define STATUS_MOVE_Failed_IdentifierDoesNotMatchSOPClass               0xa900
#define STATUS_MOVE_Failed_UnableToProcess            /* high nibble */ 0xc000
#define STATUS_MOVE_Cancel_SubOperationsTerminatedDueToCancelIndication 0xfe00
#define STATUS_MOVE_Warning_SubOperationsCompleteOneOrMoreFailures      0xb000

/* Get Specific Codes */
#define STATUS_GET_Refused_OutOfResourcesNumberOfMatches                0xa701
#define STATUS_GET_Refused_OutOfResourcesSubOperations                  0xa702
#define STATUS_GET_Failed_SOPClassNotSupported                          0xa800
#define STATUS_GET_Failed_IdentifierDoesNotMatchSOPClass                0xa900
#define STATUS_GET_Failed_UnableToProcess             /* high nibble */ 0xc000
#define STATUS_GET_Cancel_SubOperationsTerminatedDueToCancelIndication  0xfe00
#define STATUS_GET_Warning_SubOperationsCompleteOneOrMoreFailures       0xb000

/* DIMSE-N Specific Codes */
#define STATUS_N_Cancel                                                 0xFE00
#define STATUS_N_AttributeListError                                     0x0107
#define STATUS_N_SOPClassNotSupported                                   0x0122
#define STATUS_N_ClassInstanceConflict                                  0x0119
#define STATUS_N_DuplicateSOPInstance                                   0x0111
#define STATUS_N_DuplicateInvocation                                    0x0210
#define STATUS_N_InvalidArgumentValue                                   0x0115
#define STATUS_N_InvalidAttributeValue                                  0x0106
#define STATUS_N_InvalidObjectInstance                                  0x0117
#define STATUS_N_MissingAttribute                                       0x0120
#define STATUS_N_MissingAttributeValue                                  0x0121
#define STATUS_N_MistypedArgument                                       0x0212
#define STATUS_N_NoSuchArgument                                         0x0114
#define STATUS_N_NoSuchAttribute                                        0x0105
#define STATUS_N_NoSuchEventType                                        0x0113
#define STATUS_N_NoSuchObjectInstance                                   0x0112
#define STATUS_N_NoSuchSOPClass                                         0x0118
#define STATUS_N_ProcessingFailure                                      0x0110
#define STATUS_N_ResourceLimitation                                     0x0213
#define STATUS_N_UnrecognizedOperation                                  0x0211
#define STATUS_N_NoSuchAction                                           0x0123

/* Print Management Service Class Specific Codes */
#define STATUS_N_PRINT_BFS_Warn_MemoryAllocation                        0xB600
#define STATUS_N_PRINT_BFS_Warn_NoSessionPrinting                       0xB601
#define STATUS_N_PRINT_BFS_Warn_EmptyPage                               0xB602
#define STATUS_N_PRINT_BFB_Warn_EmptyPage                               0xB603
#define STATUS_N_PRINT_BFS_Fail_NoFilmBox                               0xC600
#define STATUS_N_PRINT_BFS_Fail_PrintQueueFull                          0xC601
#define STATUS_N_PRINT_BSB_Fail_PrintQueueFull                          0xC602
#define STATUS_N_PRINT_BFS_BFB_Fail_ImageSize                           0xC603
#define STATUS_N_PRINT_BFS_BFB_Fail_PositionCollision                   0xC604
#define STATUS_N_PRINT_IB_Fail_InsufficientMemory                       0xC605
#define STATUS_N_PRINT_IB_Fail_MoreThanOneVOILUT                        0xC606


/*
 * Type Definitions
 */


/*
 * DIMSE Command Codes (Request and Response)
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
 * DIMSE Data Set Type Codes
 */

typedef enum {                  /* DIC_US */
    DIMSE_DATASET_PRESENT = 0x0001,     /* anything other than 0x0101) */
    DIMSE_DATASET_NULL = 0x0101
} T_DIMSE_DataSetType;

/*
 * DIMSE Priority Codes
 */

typedef enum {                  /* DIC_US */
    DIMSE_PRIORITY_LOW = 0x0002,
    DIMSE_PRIORITY_MEDIUM = 0x0000,
    DIMSE_PRIORITY_HIGH = 0x0001
} T_DIMSE_Priority;

/*
 * DIMSE Blocking Modes (reading)
 */

typedef enum {
    DIMSE_BLOCKING,
    DIMSE_NONBLOCKING
} T_DIMSE_BlockingMode;


/*
 * DIMSE Messages
 *
 */

/* C-STORE */

struct T_DIMSE_C_StoreRQ {
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

struct T_DIMSE_C_StoreRSP {
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

struct T_DIMSE_C_EchoRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;

struct T_DIMSE_C_EchoRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        unsigned int    opts; /* which optional items are set */
#define O_ECHO_AFFECTEDSOPCLASSUID              0x0001
} ;

/* C-FIND */

struct T_DIMSE_C_FindRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct T_DIMSE_C_FindRSP {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* U(=) */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          DimseStatus;                            /* M */
        /* Identifier provided as argument to DIMSE functions *//* C */
        unsigned int    opts; /* which optional items are set */
#define O_FIND_AFFECTEDSOPCLASSUID              0x0001
} ;

/* C-GET */

struct T_DIMSE_C_GetRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct T_DIMSE_C_GetRSP {
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

struct T_DIMSE_C_MoveRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        T_DIMSE_Priority Priority;                              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_AE          MoveDestination;                        /* M */
        /* Identifier provided as argument to DIMSE functions *//* M */
} ;

struct T_DIMSE_C_MoveRSP {
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

struct T_DIMSE_C_CancelRQ {
        DIC_US          MessageIDBeingRespondedTo;              /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;


/* N-EVENT-REPORT */

struct T_DIMSE_N_EventReportRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        DIC_US          EventTypeID;                            /* M */
        /* EventInformation provided as argument to DIMSE functions *//* U */
} ;

struct T_DIMSE_N_EventReportRSP {
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

struct T_DIMSE_N_GetRQ {
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

struct T_DIMSE_N_GetRSP {
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

struct T_DIMSE_N_SetRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* ModificationList provided as argument to DIMSE functions *//* M */
} ;

struct T_DIMSE_N_SetRSP {
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

struct T_DIMSE_N_ActionRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        DIC_US          ActionTypeID;                           /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* ActionInformation provided as argument to DIMSE functions *//* U */
} ;

struct T_DIMSE_N_ActionRSP {
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

struct T_DIMSE_N_CreateRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          AffectedSOPClassUID;                    /* M */
        DIC_UI          AffectedSOPInstanceUID;                 /* U */
        T_DIMSE_DataSetType DataSetType;                        /* M */
        /* AttributeList provided as argument to DIMSE functions *//* M */
        unsigned int    opts; /* which optional items are set */
#define O_NCREATE_AFFECTEDSOPINSTANCEUID        0x0002
} ;

struct T_DIMSE_N_CreateRSP {
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

struct T_DIMSE_N_DeleteRQ {
        DIC_US          MessageID;                              /* M */
        DIC_UI          RequestedSOPClassUID;                   /* M */
        DIC_UI          RequestedSOPInstanceUID;                /* M */
        T_DIMSE_DataSetType DataSetType;                        /* M */
} ;

struct T_DIMSE_N_DeleteRSP {
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

struct T_DIMSE_Message {
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
 * Globale Variables
 */

/*
 * Define global defaults for data encoding when sending out data-sets.
 * These can be adjusted to allow variants to be tested.
 */

extern E_GrpLenEncoding  g_dimse_send_groupLength_encoding;    /* default: EGL_recalcGL */
extern E_EncodingType    g_dimse_send_sequenceType_encoding;   /* default: EET_ExplicitLength */

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

extern OFBool            g_dimse_save_dimse_data;              /* default: OFFalse */

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
 * interupted by association release, and abort messages, or network
 * disruption.
 */


/*
 * Verification Service Class
 */

OFCondition
DIMSE_echoUser(
        /* in */
        T_ASC_Association *assoc, DIC_US msgId,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* out */
        DIC_US *status, DcmDataset **statusDetail);

OFCondition
DIMSE_sendEchoResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_EchoRQ *request, DIC_US status, DcmDataset *statusDetail);


/*
 * Storage Service Class
 */

typedef enum {
    DIMSE_StoreBegin,   /* before data set */
    DIMSE_StoreProgressing, /* during data set */
    DIMSE_StoreEnd              /* after data set */
} T_DIMSE_StoreProgressState;

struct T_DIMSE_StoreProgress { /* progress structure for store callback routines */
    T_DIMSE_StoreProgressState state;   /* current state */
    long callbackCount; /* callback execution count */
    long progressBytes; /* sent/received so far */
    long totalBytes;            /* total/estimated total to send/receive */
} ;


typedef void (*DIMSE_StoreUserCallback)(
    void *callbackData,
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *request  /* original store request */
   );

struct T_DIMSE_DetectedCancelParameters {
    OFBool cancelEncountered;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_CancelRQ req;
} ;

OFCondition
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
    /* out */
    DcmDataset **statusDetail);

OFCondition
DIMSE_storeProvider(/* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_StoreRQ *request,
    const char* imageFileName, int writeMetaheader,
    DcmDataset **imageDataSet,
        DIMSE_StoreProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

OFCondition
DIMSE_sendStoreResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_StoreRQ *request, /* send response to this request */
        T_DIMSE_C_StoreRSP *response, /* response structure */
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (FIND)
 */


typedef void (*DIMSE_FindUserCallback)(
        /* in */
        void *callbackData,
        T_DIMSE_C_FindRQ *request,      /* original find request */
        int responseCount,
        T_DIMSE_C_FindRSP *response,    /* pending response received */
        DcmDataset *responseIdentifiers /* pending response identifiers */
        );

OFCondition
DIMSE_findUser(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_FindRQ *request, DcmDataset *requestIdentifiers,
        DIMSE_FindUserCallback callback, void *callbackData,
        /* blocking info for response */
        T_DIMSE_BlockingMode blockMode, int timeout,
        /* out */
        T_DIMSE_C_FindRSP *response, DcmDataset **statusDetail);

typedef void (*DIMSE_FindProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_FindRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_FindRSP *response,
        DcmDataset **responseIdentifiers,
        DcmDataset **statusDetail);

OFCondition
DIMSE_findProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_FindRQ *request,
        DIMSE_FindProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

OFCondition
DIMSE_sendFindResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_FindRQ *request,
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

OFCondition
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
        T_DIMSE_C_MoveRSP *response, DcmDataset **statusDetail,
        DcmDataset **responseIdentifers,
        OFBool ignorePendingDatasets = OFFalse);

typedef void (*DIMSE_MoveProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_MoveRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_MoveRSP *response, DcmDataset **statusDetail,
        DcmDataset **responseIdentifiers);

OFCondition
DIMSE_moveProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_MoveRQ *request,
        DIMSE_MoveProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

OFCondition
DIMSE_sendMoveResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID, T_DIMSE_C_MoveRQ *request,
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

OFCondition
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
        T_DIMSE_C_GetRSP *response, DcmDataset **statusDetail,
        DcmDataset **responseIdentifers);

typedef void (*DIMSE_GetProviderCallback)(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_GetRSP *response, DcmDataset **statusDetail,
        DcmDataset **responseIdentifiers);

OFCondition
DIMSE_getProvider(
        /* in */
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_GetRQ *request,
        DIMSE_GetProviderCallback callback, void *callbackData,
        /* blocking info for data set */
        T_DIMSE_BlockingMode blockMode, int timeout);

OFCondition
DIMSE_sendGetResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID, T_DIMSE_C_GetRQ *request,
        T_DIMSE_C_GetRSP *response, DcmDataset *rspIds,
        DcmDataset *statusDetail);

/*
 * Query/Retrieve Service Class (CANCEL)
 */

OFCondition
DIMSE_sendCancelRequest(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presId, DIC_US msgId);

OFCondition
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


OFCondition
DIMSE_sendMessageUsingFileData(T_ASC_Association *association,
                  T_ASC_PresentationContextID presID,
                  T_DIMSE_Message *msg, DcmDataset *statusDetail,
                  const char* dataFileName,
                  DIMSE_ProgressCallback callback,
                  void *callbackContext,
                  DcmDataset **commandSet=NULL);

OFCondition
DIMSE_sendMessageUsingMemoryData(T_ASC_Association *association,
                  T_ASC_PresentationContextID presID,
                  T_DIMSE_Message *msg, DcmDataset *statusDetail,
                  DcmDataset *dataObject,
                  DIMSE_ProgressCallback callback,
                  void *callbackContext,
                  DcmDataset **commandSet=NULL);

OFCondition
DIMSE_receiveCommand(T_ASC_Association *association,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     T_ASC_PresentationContextID *presID,
                     T_DIMSE_Message *msg,
                     DcmDataset **statusDetail,
                     DcmDataset **commandSet=NULL);

OFCondition
DIMSE_receiveDataSetInMemory(T_ASC_Association *association,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     T_ASC_PresentationContextID *presID,
                     DcmDataset **dataObject,
                     DIMSE_ProgressCallback callback,
                     void *callbackContext);

OFCondition
DIMSE_createFilestream(
                     /* in */
                     const char *filename,
                     const T_DIMSE_C_StoreRQ *request,
                     const T_ASC_Association *assoc,
                     T_ASC_PresentationContextID presIdCmd,
                     int writeMetaheader,
                     /* out */
                     DcmOutputFileStream **filestream);

OFCondition
DIMSE_receiveDataSetInFile(T_ASC_Association *assoc,
                     T_DIMSE_BlockingMode blocking, int timeout,
                     T_ASC_PresentationContextID *presID,
                     DcmOutputStream *filestream,
                     DIMSE_ProgressCallback callback, void *callbackData);

OFCondition
DIMSE_ignoreDataSet( T_ASC_Association * assoc,
                     T_DIMSE_BlockingMode blocking,
                     int timeout,
                     DIC_UL * bytesRead,
                     DIC_UL * pdvCount);

/*
 * Misc functions
 */

OFString DIMSE_warn_str(T_ASC_Association *assoc);

enum DIMSE_direction {
    DIMSE_INCOMING,
    DIMSE_OUTGOING
};

/* Debugging functions for printing contents of a command structure */
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_Message &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_EchoRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_GetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_MoveRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_FindRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_StoreRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_C_CancelRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_EventReportRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_GetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_SetRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_ActionRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_CreateRSP &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRQ &msg,
                            enum DIMSE_direction dir, DcmItem *dataset = NULL, T_ASC_PresentationContextID presID = 0);
OFString& DIMSE_dumpMessage(OFString &str, T_DIMSE_N_DeleteRSP &msg,
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

/*
** CVS Log
** $Log: dimse.h,v $
** Revision 1.23  2010-12-01 08:26:10  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.22  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.20  2009-08-04 10:07:48  joergr
** Added optional parameter to printCStore() functions that allows for printing
** the Presentation Context ID.
**
** Revision 1.19  2007/07/12 12:18:00  onken
** Added status codes and corresponding printing routines for DIMSE-N.
**
** Revision 1.18  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.17  2005/12/08 16:02:21  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.16  2005/11/22 16:44:44  meichel
** Added option to movescu that allows graceful handling of Move SCPs
**   that send illegal datasets following pending C-MOVE-RSP messages.
**
** Revision 1.15  2005/02/22 09:40:55  meichel
** Fixed two bugs in "bit-preserving" Store SCP code. Errors while creating or
**   writing the DICOM file (e.g. file system full) now result in a DIMSE error
**   response (out of resources) being sent back to the SCU.
**
** Revision 1.14  2003/08/27 15:03:33  meichel
** Changed anonymous struct typedefs into struct declarations
**
** Revision 1.13  2002/09/10 16:00:47  meichel
** Added global flag dcmMaxOutgoingPDUSize that allows to restrict the maximum
**   size of outgoiung P-DATA PDUs to a value less than the maximum supported by
**   the remote application entity or this library.  May be useful if there is an
**   interaction between PDU size and other network layers, e. g. TLS, IP or
**   below.
**
** Revision 1.12  2002/08/27 17:00:59  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.11  2001/10/12 10:18:26  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.10  2000/12/15 13:28:16  meichel
** Global flag to enable/disable workaround code for some buggy Store SCUs
**   in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
**   Affected SOP Instance UID field of the C-STORE-RQ message is retained
**   in the corresponding C-STORE-RSP message.
**
** Revision 1.9  2000/06/07 08:57:52  meichel
** dcmnet DIMSE routines now allow to retrieve raw command sets as DcmDataset
**   objects, e.g. for logging purposes. Added enhanced message dump functions.
**
** Revision 1.8  2000/01/31 17:14:17  meichel
** ntroduced new flag g_dimse_save_dimse_data. If enabled, all DIMSE messages
** and data sets sent or received are stored in files.
** This facilitates debugging of DIMSE problems.
**
** Revision 1.7  1999/04/19 08:36:48  meichel
** Added support for C-FIND-CANCEL/C-MOVE-CANCEL in DIMSE_storeUser().
**
** Revision 1.6  1998/08/10 08:53:40  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.5  1998/06/29 12:14:26  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.4  1997/07/21 08:40:10  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1997/05/23 10:47:06  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes required to interfaces of some DIMSE functions.
**
** Revision 1.2  1996/04/25 16:06:28  hewett
** Replaced declarations of DIC_UL with unsigned long.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
