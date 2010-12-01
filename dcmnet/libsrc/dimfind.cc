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
** Module: dimfind
**
** Purpose:
**      This file contains the routines which help with
**      query services using the C-FIND operation.
**
** Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:36 $
** CVS/RCS Revision:    $Revision: 1.15 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
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
#include "dcmtk/dcmnet/dimse.h"              /* always include the module header */
#include "dcmtk/dcmnet/cond.h"

/*
**
*/


OFCondition
DIMSE_findUser(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_FindRQ *request, DcmDataset *requestIdentifiers,
        DIMSE_FindUserCallback callback, void *callbackData,
        T_DIMSE_BlockingMode blockMode, int timeout,
        T_DIMSE_C_FindRSP *response, DcmDataset **statusDetail)
    /*
     * This function sends a C-FIND-RQ message and data set information containing the given
     * search mask over the network connection to an SCP. Having sent this information, the
     * function tries to receive (one or more) C-FIND-RSP messages on the network connection
     * from the SCP. For C-FIND-RSP messages with a "pending" status (in element (0000,0900))
     * the function will also try to receive data set information over the network (this data
     * set information refers to one record that matches the search mask which was sent to the
     * SCP) and call the callback function which was passed. Having encountered a C-FIND-RSP
     * messages with a "success" status, this function terminates and returns to its caller.
     *
     * Parameters:
     *   assoc                - [in] The association (network connection to SCP).
     *   presId               - [in] The ID of the presentation context which shall be used
     *   request              - [in] Represents a DIMSE C-Find Request Message. Contains corresponding
     *                               information, e.g. message ID, affected SOP class UID, etc.
     *   requestIdentifiers   - [in] Data set object which contains the search mask,i.e. which specifies
     *                               the set of objects which will be requested by C-FIND-RQ.
     *   callback             - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackData         - [in] Pointer to data which shall be passed to the progress indicating function
     *   blockMode            - [in] The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
     *   timeout              - [in] Timeout interval for receiving data. If the blocking mode is DIMSE_NONBLOCKING
     *   response             - [out] Represents a DIMSE C-Find Response Message. Contains corresponding
     *                                information, e.g. message ID being responded to, affected SOP class UID, etc.
     *                                This variable contains in the end the last C-FIND-RSP message which was received
     *                                as a response to the C-FIND-RQ which was sent. Usually, this message will show a
     *                                status of "success".
     *   statusDetail         - [out] If a non-NULL value is passed this variable will in the end contain detailed
     *                                information with regard to the status information which is captured in the status
     *                                element (0000,0900) of the response message. Note that the value for element (0000,0900)
     *                                is not contained in this return value but in response.
     */
{
    T_DIMSE_Message req, rsp;
    DIC_US msgId;
    int responseCount = 0;
    DcmDataset *rspIds = NULL;
    DIC_US status = STATUS_Pending;

    /* if there is no search mask, nothing can be searched for */
    if (requestIdentifiers == NULL) return DIMSE_NULLKEY;

    /* initialize the variables which represent DIMSE C-FIND-RQ and DIMSE C-FIND-RSP messages */
    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    /* set corresponding values in the request message variable */
    req.CommandField = DIMSE_C_FIND_RQ;
    request->DataSetType = DIMSE_DATASET_PRESENT;
    req.msg.CFindRQ = *request;

    /* determine the message ID */
    msgId = request->MessageID;

    /* send C-FIND-RQ message and search mask data (requestIdentifiers) */
    OFCondition cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req,
                                          NULL, requestIdentifiers,
                                          NULL, NULL);
    if (cond.bad()) return cond;

    /* try to receive (one or more) C-STORE-RSP messages, continue loop as long */
    /* as no error occured and not all result information has been received. */
    while (cond == EC_Normal && DICOM_PENDING_STATUS(status))
    {
	/* initialize the response to collect */
        bzero((char*)&rsp, sizeof(rsp));
        if (rspIds != NULL) {
            delete rspIds;
            rspIds = NULL;
        }

        /* try to receive a C-FIND-RSP over the network. */
        cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID,
                &rsp, statusDetail);
        if (cond.bad()) return cond;

        /* if everything was successful so far, the rsp variable contains the command */
        /* which was received; if we did not encounter a C-FIND-RSP, something is wrong */
        if (rsp.CommandField != DIMSE_C_FIND_RSP)
        {
          char buf1[256];
          sprintf(buf1, "DIMSE: Unexpected Response Command Field: 0x%x", (unsigned)rsp.CommandField);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf1);
        }

        /* if we get to here, we received a C-FIND-RSP; store this message in the reference parameter */
        *response = rsp.msg.CFindRSP;

        /* check if the response relates to the request which was sent earlier; if not, return an error */
        if (response->MessageIDBeingRespondedTo != msgId)
        {
          char buf2[256];
          sprintf(buf2, "DIMSE: Unexpected Response MsgId: %d (expected: %d)", response->MessageIDBeingRespondedTo, msgId);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDRESPONSE, OF_error, buf2);
        }

        /* determine the status which was returned in the current C-FIND-RSP */
        status = response->DimseStatus;

        /* increase counter which counts the amount of received C-FIND-RSP messages */
        responseCount++;

        /* depending on the status which was returned in the current C-FIND-RSP, we need to do something */
        switch (status) {
        case STATUS_Pending:
        case STATUS_FIND_Pending_WarningUnsupportedOptionalKeys:
            /* in these cases we received a C-FIND-RSP which indicates that a result data set was */
            /* found and will be sent over the network. We need to receive this result data set. */

            /* forget about status detail information if there is some */
            if (*statusDetail != NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "findUser: Pending with statusDetail, ignoring detail");
                delete *statusDetail;
                *statusDetail = NULL;
            }

            /* if the response message's data set type field reveals that there is */
            /* no data set attached to the current C-FIND-RSP, something is fishy */
            if (response->DataSetType == DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "findUser: Status Pending, but DataSetType==NULL");
                DCMNET_WARN(DIMSE_warn_str(assoc) << "  Assuming response identifiers are present");
            }

            /* receive the result data set on the network connection */
            cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout,
                &presID, &rspIds, NULL, NULL);
            if (cond != EC_Normal) {
                return cond;
            }

            /* execute callback */
            if (callback) {
                callback(callbackData, request, responseCount,
                    response, rspIds);
            }
            break;
        case STATUS_Success:
            /* in this case the current C-FIND-RSP indicates that */
            /* there are no more records that match the search mask */

            /* in case the response message's data set type field reveals that there */
            /* is a data set attached to the current C-FIND-RSP, something is fishy */
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "findUser: Status Success, but DataSetType!=NULL");
                DCMNET_WARN(DIMSE_warn_str(assoc) << "Assuming no response identifiers are present");
            }
            break;
        default:
            /* in all other cases, simply dump warnings if there is no data set */
            /* following the C-FIND-RSP message, do nothing else (but go ahead  */
            /* and try to receive the next C-FIND-RSP) */
            if (response->DataSetType != DIMSE_DATASET_NULL) {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "findUser: Status " << DU_cfindStatusString(status)
                        << ", but DataSetType!=NULL");
                DCMNET_WARN(DIMSE_warn_str(assoc) << "Assuming no response identifiers are present");
            }
            break;
        } //switch
    } //while

    /* return result value */
    return cond;
}

OFCondition
DIMSE_sendFindResponse(T_ASC_Association * assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_C_FindRQ *request,
        T_DIMSE_C_FindRSP *response, DcmDataset *rspIds,
        DcmDataset *statusDetail)
    /*
     * This function takes care of sending a C-FIND-RSP message over the network to the DICOM
     * application this application is connected with.
     *
     * Parameters:
     *   assoc        - [in] The association (network connection to another DICOM application).
     *   presID       - [in] The ID of the presentation context which was specified in the PDV
     *                       which contained the DIMSE C-FIND-RQ command.
     *   request      - [in] The DIMSE C-FIND-RQ command which was received earlier.
     *   response     - [inout] The C-FIND-RSP command which shall be sent. Might be modified.
     *   statusDetail - [in] Detailed information with regard to the status information which is captured
     *                       in the status element (0000,0900). Note that the value for element (0000,0900)
     *                       is contained in this variable.
     */
{
    OFCondition         cond = EC_Normal;
    DIC_US      dtype;
    T_DIMSE_Message rsp;

    /* create response message */
    bzero((char*)&rsp, sizeof(rsp));
    rsp.CommandField = DIMSE_C_FIND_RSP;
    rsp.msg.CFindRSP = *response;
    rsp.msg.CFindRSP.MessageIDBeingRespondedTo = request->MessageID;
    strcpy(rsp.msg.CFindRSP.AffectedSOPClassUID,
            request->AffectedSOPClassUID);
    rsp.msg.CFindRSP.opts = O_FIND_AFFECTEDSOPCLASSUID;

    /* specify if the response message will contain a search result or if it will not contain one, */
    /* thus indicating that there are no more results that match the search mask */
    if (rspIds != NULL) {
        dtype = DIMSE_DATASET_PRESENT;
    } else {
        dtype = DIMSE_DATASET_NULL;
    }
    rsp.msg.CFindRSP.DataSetType = (T_DIMSE_DataSetType)dtype;

    /* send C-FIND-RSP response message over the network */
    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp,
        statusDetail, rspIds, NULL, NULL);

    /* return result value */
    return cond;
}


OFCondition
DIMSE_findProvider(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        T_DIMSE_C_FindRQ *request,
        DIMSE_FindProviderCallback callback, void *callbackData,
        T_DIMSE_BlockingMode blockMode, int timeout)
    /*
     * This function receives a data set which represents the search mask over the network and
     * stores this data in memory. Then, it tries to select corresponding records which match the
     * search mask from some database (done whithin the callback function) and sends corresponding
     * C-FIND-RSP messages to the other DICOM application this application is connected with.
     * The selection of each matching record and the sending of a corresponding C-FIND-RSP message
     * is conducted in a loop since there can be more than one search result. In the end, also the
     * C-FIND-RSP message which indicates that there are no more search results will be sent.
     *
     * Parameters:
     *   assoc           - [in] The association (network connection to another DICOM application).
     *   presIDCmd       - [in] The ID of the presentation context which was specified in the PDV which contained
     *                          the DIMSE command.
     *   request         - [in] The DIMSE C-FIND-RQ message that was received.
     *   callback        - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackData    - [in] Pointer to data which shall be passed to the progress indicating function
     *   blockMode       - [in] The blocking mode for receiving data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
     *   timeout         - [in] Timeout interval for receiving data (if the blocking mode is DIMSE_NONBLOCKING).
     */
{
    T_ASC_PresentationContextID presIdData;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    DcmDataset *reqIds = NULL;
    DcmDataset *rspIds = NULL;
    OFBool cancelled = OFFalse;
    OFBool normal = OFTrue;
    int responseCount = 0;

    /* receive data (i.e. the search mask) and store it in memory */
    OFCondition cond = DIMSE_receiveDataSetInMemory(assoc, blockMode, timeout, &presIdData, &reqIds, NULL, NULL);

    /* if no error occured while receiving data */
    if (cond.good())
    {
        /* check if the presentation context IDs of the C-FIND-RQ and */
        /* the search mask data are the same; if not, return an error */
        if (presIdData != presIdCmd)
        {
          cond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Presentation Contexts of Command and Data Differ");
        }
        else
        {
            /* if the IDs are the same go ahead */
            /* initialize the C-FIND-RSP message variable */
            bzero((char*)&rsp, sizeof(rsp));
            rsp.DimseStatus = STATUS_Pending;

            /* as long as no error occured and the status of the C-FIND-RSP message which will */
            /* be/was sent is pending, perform this loop in which records that match the search */
            /* mask are selected (whithin the execution of the callback function) and sent over */
            /* the network to the other DICOM application using C-FIND-RSP messages. */
            while (cond.good() && DICOM_PENDING_STATUS(rsp.DimseStatus) && normal)
            {
                /* increase the counter that counts the number of response messages */
                responseCount++;

                /* check if a C-CANCEL-RQ is received */
                cond = DIMSE_checkForCancelRQ(assoc, presIdCmd, request->MessageID);
                if (cond.good())
                {
                    /* if a C-CANCEL-RQ was received, we need to set status and an indicator variable */
                    rsp.DimseStatus = STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
                    cancelled = OFTrue;
                } else if (cond == DIMSE_NODATAAVAILABLE)
                {
                    /* timeout */
                }
                else
                {
                    /* some execption condition occured, bail out */
                    normal = OFFalse;
                }

                /* if everything is still ok */
                if (normal)
                {
                    /* execute callback function (note that this function always determines the next record */
                    /* which matches the search mask. This record will be available here through rspIds) */
                    if (callback) {
                        callback(callbackData, cancelled, request, reqIds,
                            responseCount, &rsp, &rspIds, &statusDetail);
                    } else {
                        return makeDcmnetCondition(DIMSEC_NULLKEY, OF_error, "DIMSE_findProvider: no callback function");
                    }

                    /* if we encountered a C-CANCEL-RQ earlier, set a variable and possibly delete the search mask */
                    if (cancelled) {
                        /* make sure */
                        rsp.DimseStatus =
                            STATUS_FIND_Cancel_MatchingTerminatedDueToCancelRequest;
                        if (rspIds != NULL) {
                            delete reqIds;
                            reqIds = NULL;
                        }
                    }

                    /* send a C-FIND-RSP message over the network to the other DICOM application */
                    cond = DIMSE_sendFindResponse(assoc, presIdCmd, request,
                        &rsp, rspIds, statusDetail);

                    /* if there are search results, delete them */
                    if (rspIds != NULL) {
                        delete rspIds;
                        rspIds = NULL;
                    }

                    /* if there is status detail information, delete it */
                    if (statusDetail != NULL) {
                        delete statusDetail;
                        statusDetail = NULL;
                    }
                }
            }
        }
    }

    /* delete search mask */
    delete reqIds;

    /* delete latest search result */
    delete rspIds;

    /* return result value */
    return cond;
}

/*
** CVS Log
** $Log: dimfind.cc,v $
** Revision 1.15  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.14  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.13  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.12  2005-12-08 15:44:42  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.11  2002/11/27 13:04:40  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.10  2001/11/01 13:49:05  wilkens
** Added lots of comments.
**
** Revision 1.9  2001/10/12 10:18:34  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.8  2000/02/23 15:12:34  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/01 10:24:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.6  1998/08/10 08:53:44  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.5  1998/01/27 10:51:44  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.4  1997/07/21 08:47:18  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1996/04/25 16:11:15  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.2  1996/04/22 10:02:59  hewett
** Corrected memory leak whereby response ids where not being deleted.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
