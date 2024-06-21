/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
**      for DICOM V.3 applications.
**
** Module Prefix: DIMSE_
*/

/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dimse.h"        /* always include the module header */
#include "dcmtk/dcmnet/cond.h"
#include "dimcmd.h"
#include "dcmtk/dcmdata/dcdeftag.h"    /* for tag names */
#include "dcmtk/dcmdata/dcdict.h"      /* for dcmDataDict */
#include "dcmtk/dcmdata/dcfilefo.h"    /* for class DcmFileFormat */
#include "dcmtk/dcmdata/dcmetinf.h"    /* for class DcmMetaInfo */
#include "dcmtk/dcmdata/dcistrmb.h"    /* for class DcmInputBufferStream */
#include "dcmtk/dcmdata/dcostrmb.h"    /* for class DcmOutputBufferStream */
#include "dcmtk/dcmdata/dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcmtk/dcmdata/dcvrul.h"      /* for class DcmUnsignedLong */
#include "dcmtk/dcmdata/dcvrobow.h"    /* for class DcmOtherByteOtherWord */
#include "dcmtk/dcmdata/dcvrsh.h"      /* for class DcmShortString */
#include "dcmtk/dcmdata/dcvrae.h"      /* for class DcmApplicationEntity */
#include "dcmtk/dcmdata/dcdicent.h"    /* for class DcmDictEntry, needed for MSVC5 */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */
#include "dcmtk/dcmdata/dcvrui.h"      /* for class DcmUniqueIdentifier */
#include "dcmtk/ofstd/ofstd.h"

/*
 * Global variables, mutex protected
 */

/*  global flag allowing to restrict the maximum size of outgoing
 *  P-DATA PDUs to a value less than the maximum supported by the
 *  remote application entity or this library.  May be useful
 *  if there is an interaction between PDU size and other network
 *  layers, e. g. TLS, IP or below.
 */
OFGlobal<Uint32> dcmMaxOutgoingPDUSize((Uint32) -1);

/*
 * Other global variables (should be used very, very rarely).
 * Modification of this variables is THREAD UNSAFE.
 */

/*
 * Define global defaults for data encoding when sending out data-sets.
 * These can be adjusted to allow variants to be tested.
 */

E_GrpLenEncoding  g_dimse_send_groupLength_encoding = EGL_recalcGL;
E_EncodingType    g_dimse_send_sequenceType_encoding = EET_ExplicitLength;

/*
 * These globals control the options for saving all DIMSE messages
 */

OFBool g_dimse_save_dimse_data = OFFalse;
static unsigned long g_dimse_commandCounter = 0;
static unsigned long g_dimse_dataCounter = 0;


/*
** Private Functions Bodies
*/

static void saveDimseFragment(
        DcmDataset *dset,
        OFBool isCommand,
        OFBool isReceive)
    /*
     * This function saves the information which is contained in dset to a file
     *
     * Parameters:
     *   dset      - [in] Contains the information which shall be written to a file.
     *   isCommand - [in] Specifies if dset's information refers to a DIMSE command (as for
     *                    example C-STORE) (OFTrue) or if it refers to instance data (OFFalse).
     *   isReceive - [in] Specifies if this application received (OFTrue) or sent (OFFalse)
     *                    the information in dset.
     */
{
  /* depending on if the information in dset refers to a DIMSE command or not, set some global variables */
  if (isCommand)
  {
    g_dimse_commandCounter++;
    g_dimse_dataCounter = 0;
  } else g_dimse_dataCounter++;

  /* if there is no information exit this function */
  if (dset==NULL) return;

  /* create a string which will contain the name of the file we are about to write to */
  /* filenames are supposed to look like "dimse-cmd-[rcv|snd]-[counter].dcm" */
  const char *transmission = (isReceive ? "rcv" : "snd");
  char filename[2048];
  if (isCommand)
  {
    OFStandard::snprintf(filename, sizeof(filename), "dimse-cmd-%s-%04lu.dcm", transmission, g_dimse_commandCounter);
  } else {
    if (g_dimse_dataCounter < 2)
    {
      OFStandard::snprintf(filename, sizeof(filename), "dimse-dat-%s-%04lu.dcm", transmission, g_dimse_commandCounter);
    } else {
      OFStandard::snprintf(filename, sizeof(filename), "dimse-dat-%s-%04lu-%02lu.dcm", transmission, g_dimse_commandCounter, g_dimse_dataCounter);
    }
  }

  /* write data to file */
  dset->saveFile(filename, EXS_LittleEndianImplicit, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
  return;
}

static OFBool
isDataDictPresent()
{
    /* is a data dictionary present */
    return dcmDataDict.isDictionaryLoaded();
}

/*
 * PDV Reading
 */

static OFCondition
DIMSE_readNextPDV(
        T_ASC_Association *assoc,
        T_DIMSE_BlockingMode blocking,
        int timeout,
        DUL_PDV *pdv)
    /*
     * This function returns the next PDV which was (earlier or just now) received on the incoming
     * socket stream. If there are no PDVs (which were already received earlier) waiting to be picked
     * up, this function will go ahead and read a new PDU (containing one or more new PDVs) from the
     * incoming socket stream.
     *
     * Parameters:
     *   assoc    - [in] The association (network connection to another DICOM application).
     *   blocking - [in] The blocking mode for reading data (either DIMSE_BLOCKING or DIMSE_NONBLOCKING)
     *   timeout  - [in] Timeout interval for receiving data. If the blocking mode is DIMSE_NONBLOCKING
     *                   and we are trying to read data from the incoming socket stream and no data has
     *                   been received after timeout seconds, an error will be reported.
     *   pdv      - [out] Contains in the end the next PDV which was (earlier or just now) received on
     *                    the incoming socket stream.
     *
     */
{
    DUL_BLOCKOPTIONS blk;

    /*
     * NOTE: DUL currently ignores blocking and timeout so do it here!
     */

    /* determine the DUL blocking option */
    blk = (blocking == DIMSE_BLOCKING) ? (DUL_BLOCK) : (DUL_NOBLOCK);

    /* get the next PDV from the association, in case there are still some PDVs waiting to be picked up */
    OFCondition cond = DUL_NextPDV(&assoc->DULassociation, pdv);

    if (cond.bad())
    {
        /* in case DUL_NextPDV(...) did not return DUL_NORMAL, the association */
        /* did not contain any more PDVs that are waiting to be picked up. Hence, */
        /* we need to read new PDVs from the incoming socket stream. */

        /* if the blocking mode is DIMSE_NONBLOCKING and there is no data waiting after timeout seconds, report an error */
        if (blocking == DIMSE_NONBLOCKING)
        {
            if (!ASC_dataWaiting(assoc, timeout)) return DIMSE_NODATAAVAILABLE;
        }

        /* try to receive new PDVs on the incoming socket stream (in detail, try to receive one PDU) */
        cond = DUL_ReadPDVs(&assoc->DULassociation, NULL, blk, timeout);

        /* check return value, if it is different from DUL_PDATAPDUARRIVED, an error occurred */
        if (cond != DUL_PDATAPDUARRIVED)
        {
            if (cond == DUL_NULLKEY || cond == DUL_ILLEGALKEY) return DIMSE_ILLEGALASSOCIATION;
            else if (cond == DUL_PEERREQUESTEDRELEASE ||
                     cond == DUL_PEERABORTEDASSOCIATION) return cond;
            else return makeDcmnetSubCondition(DIMSEC_READPDVFAILED, OF_error, "DIMSE Read PDV failed", cond);
        }

        /* get the next PDV, assign it to pdv */
        cond = DUL_NextPDV(&assoc->DULassociation, pdv);
        if (cond.bad())
        {
            return makeDcmnetSubCondition(DIMSEC_READPDVFAILED, OF_error, "DIMSE Read PDV failed", cond);
        }
    }

    /* return ok */
    return EC_Normal;
}

/*
 * Checking and Validation
 */


static OFCondition
getTransferSyntax(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID pid,
        E_TransferSyntax *xferSyntax)
    /*
     * This function checks if the presentation context id which was passed refers to a valid presentation
     * context. If this is the case, this function determines the transfer syntax the presentation context ID
     * refers to (will be returned to the user) and also checks if dcmtk supports this transfer syntax.
     *
     * Parameters:
     *   assoc      - [in] The association (network connection to another DICOM application).
     *   pid        - [in] The id of the presentation context which shall be checked regarding validity.
     *   xferSyntax - [out] If pid refers to a valid presentation context, this variable contains in the
     *                     end the transfer syntax which is specified in the presentation context.
     */
{
    T_ASC_PresentationContext pc;
    char *ts = NULL;

    /* figure out if is this a valid presentation context */
    OFCondition cond = ASC_findAcceptedPresentationContext(assoc->params, pid, &pc);
    if (cond.bad())
    {
        return makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", cond);
    }

    /* determine the transfer syntax which is specified in the presentation context */
    ts = pc.acceptedTransferSyntax;

    /* create a DcmXfer object on the basis of the transfer syntax which was determined above */
    DcmXfer xfer(ts);

    /* check if the transfer syntax is supported by dcmtk */
    *xferSyntax = xfer.getXfer();
    switch (*xferSyntax)
    {
        case EXS_LittleEndianImplicit:
        case EXS_LittleEndianExplicit:
        case EXS_BigEndianExplicit:
        case EXS_JPEGProcess1:
        case EXS_JPEGProcess2_4:
        case EXS_JPEGProcess3_5:
        case EXS_JPEGProcess6_8:
        case EXS_JPEGProcess7_9:
        case EXS_JPEGProcess10_12:
        case EXS_JPEGProcess11_13:
        case EXS_JPEGProcess14:
        case EXS_JPEGProcess15:
        case EXS_JPEGProcess16_18:
        case EXS_JPEGProcess17_19:
        case EXS_JPEGProcess20_22:
        case EXS_JPEGProcess21_23:
        case EXS_JPEGProcess24_26:
        case EXS_JPEGProcess25_27:
        case EXS_JPEGProcess28:
        case EXS_JPEGProcess29:
        case EXS_JPEGProcess14SV1:
        case EXS_RLELossless:
        case EXS_JPEGLSLossless:
        case EXS_JPEGLSLossy:
        case EXS_JPEG2000LosslessOnly:
        case EXS_JPEG2000:
        case EXS_JPEG2000MulticomponentLosslessOnly:
        case EXS_JPEG2000Multicomponent:
        case EXS_MPEG2MainProfileAtMainLevel:
        case EXS_MPEG2MainProfileAtHighLevel:
        case EXS_MPEG4HighProfileLevel4_1:
        case EXS_MPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_MPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_MPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_MPEG4StereoHighProfileLevel4_2:
        case EXS_HEVCMainProfileLevel5_1:
        case EXS_HEVCMain10ProfileLevel5_1:
        case EXS_FragmentableMPEG2MainProfileMainLevel:
        case EXS_FragmentableMPEG2MainProfileHighLevel:
        case EXS_FragmentableMPEG4HighProfileLevel4_1:
        case EXS_FragmentableMPEG4BDcompatibleHighProfileLevel4_1:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For2DVideo:
        case EXS_FragmentableMPEG4HighProfileLevel4_2_For3DVideo:
        case EXS_FragmentableMPEG4StereoHighProfileLevel4_2:
        case EXS_HighThroughputJPEG2000LosslessOnly:
        case EXS_HighThroughputJPEG2000withRPCLOptionsLosslessOnly:
        case EXS_HighThroughputJPEG2000:
#ifdef WITH_ZLIB
        case EXS_DeflatedLittleEndianExplicit:
#endif
        /* OK, these can be supported */
        break;
    default:
        /* all other transfer syntaxes are not supported; hence, set the error indicator variable */
        {
          char buf[256];
          OFStandard::snprintf(buf, sizeof(buf), "DIMSE Unsupported transfer syntax: %s", ts);
          OFCondition subCond = makeDcmnetCondition(DIMSEC_UNSUPPORTEDTRANSFERSYNTAX, OF_error, buf);
          cond = makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", subCond);
        }
        break;
    }

    /* return result value */
    return cond;
}


static OFCondition
checkPresentationContextForMessage(
        T_ASC_Association *assoc,
        T_DIMSE_Message *msg,
        T_ASC_PresentationContextID presID,
        E_TransferSyntax *xferSyntax)
    /*
     * This function checks if the presentation context id refers to a valid presentation context and
     * determines the transfer syntax which is specified for this presentation context. Additionally,
     * the function checks if the specified transfer syntax is supported at all. If any of the checks
     * returns an error, return a corresponding error..
     *
     * Parameters:
     *   assoc                - [in] The association (network connection to another DICOM application).
     *   msg                  - [in] Structure that represents a certain DIMSE message.
     *   presID               - [in] The ID of the presentation context which shall be used.
     *   xferSyntax           - [out] Contains in the end the transfer syntax which is specified
     *                                in the presentation context with the corresponding ID.
     */
{
    /* char *as; */     /* abstract syntax */
    /* char *ts; */     /* transfer syntax */
    T_ASC_PresentationContext pc;

    /* try to find the accepted presentation context on the basis of the ID which was passed */
    OFCondition cond = ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);

    /*
     * as = pc.abstractSyntax;
     * ts = pc.acceptedTransferSyntax;
     */

    /* if the accepted presentation context was found, check if the message type is supported */
    if (cond.good())
    {
      switch (msg->CommandField)
      {
        case DIMSE_C_ECHO_RQ:
        case DIMSE_C_ECHO_RSP:
        case DIMSE_C_STORE_RQ:
        case DIMSE_C_STORE_RSP:
        case DIMSE_C_GET_RQ:
        case DIMSE_C_GET_RSP:
        case DIMSE_C_FIND_RQ:
        case DIMSE_C_FIND_RSP:
        case DIMSE_C_MOVE_RQ:
        case DIMSE_C_MOVE_RSP:
        case DIMSE_C_CANCEL_RQ:
        case DIMSE_N_EVENT_REPORT_RQ:
        case DIMSE_N_EVENT_REPORT_RSP:
        case DIMSE_N_GET_RQ:
        case DIMSE_N_GET_RSP:
        case DIMSE_N_SET_RQ:
        case DIMSE_N_SET_RSP:
        case DIMSE_N_ACTION_RQ:
        case DIMSE_N_ACTION_RSP:
        case DIMSE_N_CREATE_RQ:
        case DIMSE_N_CREATE_RSP:
        case DIMSE_N_DELETE_RQ:
        case DIMSE_N_DELETE_RSP:
            break;

        default:
            cond = DIMSE_BADCOMMANDTYPE;
            break;
      }
    }

    /* if everything was ok so far determine the transfer syntax */
    /* which is specified in the accepted presentation context */
    if (cond.good()) cond = getTransferSyntax(assoc, presID, xferSyntax);

    /* otherwise return ok */
    return cond;
}

static OFCondition
validateMessage(
        T_ASC_Association *assoc,
        T_DIMSE_Message *msg)
    /*
     * This function checks if the information which is contained in msg meets certain conditions.
     * For example, if msg represents a C-ECHO-RQ, then there is not supposed to be a corresponding
     * data set. If the specified conditions are not met, return an error.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   msg   - [in] Structure that represents a certain DIMSE message.
     */
{
    OFCondition cond = EC_Normal;

    /* check certain conditions depending on the message type */
    switch (msg->CommandField) {
    case DIMSE_C_ECHO_RQ:
        if (msg->msg.CEchoRQ.DataSetType != DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Echo RQ: DataSetType != NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_ECHO_RSP:
        if (msg->msg.CEchoRSP.DataSetType != DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Echo RSP: DataSetType != NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_STORE_RQ:
        if (msg->msg.CStoreRQ.DataSetType == DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Store RQ: DataSetType == NULL");
            cond = DIMSE_BADMESSAGE;
        }
        if (! IN_RANGE(strlen(msg->msg.CStoreRQ.AffectedSOPInstanceUID), 1, DIC_UI_LEN)) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Store RQ: AffectedSOPInstanceUID: bad size");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_STORE_RSP:
        if (msg->msg.CStoreRSP.DataSetType != DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Store RSP: DataSetType != NULL");
            cond = DIMSE_BADMESSAGE;
        }
        if ((msg->msg.CStoreRSP.opts & O_STORE_AFFECTEDSOPINSTANCEUID) &&
            (! IN_RANGE(strlen(msg->msg.CStoreRSP.AffectedSOPInstanceUID), 1, DIC_UI_LEN))) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Store RSP: AffectedSOPInstanceUID: bad size");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_GET_RQ:
        if (msg->msg.CGetRQ.DataSetType == DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Get RQ: DataSetType == NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_GET_RSP:
        /* data set can be empty or present */
        break;
    case DIMSE_C_FIND_RQ:
        if (msg->msg.CFindRQ.DataSetType == DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Find RQ: DataSetType == NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_FIND_RSP:
        /* data set can be empty or present */
        break;
    case DIMSE_C_MOVE_RQ:
        if (msg->msg.CMoveRQ.DataSetType == DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Move RQ: DataSetType == NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;
    case DIMSE_C_MOVE_RSP:
        /* data set can be empty or present */
        break;
    case DIMSE_C_CANCEL_RQ:
        if (msg->msg.CCancelRQ.DataSetType != DIMSE_DATASET_NULL) {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "C-Cancel RQ: DataSetType != NULL");
            cond = DIMSE_BADMESSAGE;
        }
        break;

    case DIMSE_N_EVENT_REPORT_RQ:
    case DIMSE_N_EVENT_REPORT_RSP:
    case DIMSE_N_GET_RQ:
    case DIMSE_N_GET_RSP:
    case DIMSE_N_SET_RQ:
    case DIMSE_N_SET_RSP:
    case DIMSE_N_ACTION_RQ:
    case DIMSE_N_ACTION_RSP:
    case DIMSE_N_CREATE_RQ:
    case DIMSE_N_CREATE_RSP:
    case DIMSE_N_DELETE_RQ:
    case DIMSE_N_DELETE_RSP:
        /* No checking on the normalized messages yet, assume ok */
        break;

    default:
        cond = DIMSE_BADCOMMANDTYPE;
        break;
    }

    return cond;
}


/*
 * Message sending support routines
 */

#if 0
/*** Currently Unused */
/*
** SendStraightFileData does not work for files with a Metaheader.
** It would need to be rewritten to read data after the Metaheader.
** However, finding out where the Metaheader ends means parsing the
** whole file in the current implementation.  It is thus not worth
** the effort!
*/
static OFCondition
sendStraightFileData(
        T_ASC_Association *assoc,
        const char *dataFileName,
        T_ASC_PresentationContextID presID,
        E_TransferSyntax /* xferSyntax */,
        DIMSE_ProgressCallback callback,
        void *callbackContext)

{
    /* we assume that the file contains transfer syntax compatible data */
    unsigned char *buf;
    unsigned long bufLen;
    FILE *f;
    long nbytes;
    OFBool last;
    unsigned long bytesTransmitted = 0;
    OFCondition dulCond = EC_Normal;
    DUL_PDVLIST pdvList;
    DUL_PDV pdv;
    /* the following variable is currently unused, leave it for future use */
    unsigned long pdvCount = 0;

    buf = assoc->sendPDVBuffer;
    bufLen = assoc->sendPDVLength;

    OFCondition cond = EC_Normal;

    f = fopen(dataFileName, "rb");
    if (f == NULL) {
        char buf[256];
        DCMNET_WARN(DIMSE_warn_str(assoc) << "sendStraightFileData: cannot open DICOM file ("
            << dataFileName << "): " << OFStandard::strerror(errno, buf, sizeof(buf)));
        cond = DIMSE_SENDFAILED;
    }

    while (cond.good() && ((nbytes = fread(buf, 1, bufLen, f)) > 0)) {
        last = ((unsigned long)nbytes != bufLen);
        pdv.fragmentLength = nbytes;
        pdv.presentationContextID = presID;
        pdv.pdvType = DUL_DATASETPDV;
        pdv.lastPDV = last;
        pdv.data = buf;

        pdvList.count = 1;
        pdvList.pdv = &pdv;

        DCMNET_TRACE("DIMSE sendStraightFileData: sending " << pdv.fragmentLength << " bytes (last: "
            << ((last)?("YES"):("NO")) << ")");

        dulCond = DUL_WritePDVs(&assoc->DULassociation, &pdvList);
        if (dulCond.bad())
        {
            cond = makeDcmnetSubCondition(DIMSEC_SENDFAILED, OF_error, "DIMSE Failed to send message", dulCond);
        }

        bytesTransmitted += nbytes;
        pdvCount += pdvList.count;

        if (callback) { /* execute callback function */
            callback(callbackContext, bytesTransmitted);
        }
    }

    fclose(f);

    return cond;
}
#endif

static OFCondition
sendDcmDataset(
        T_ASC_Association *assoc,
        DcmDataset *obj,
        T_ASC_PresentationContextID presID,
        E_TransferSyntax xferSyntax,
        DUL_DATAPDV pdvType,
        DIMSE_ProgressCallback callback,
        void *callbackContext)
    /*
     * This function sends all information which is included in a DcmDataset object over
     * the network which is provided in assoc.
     *
     * Parameters:
     *   assoc           - [in] The association (network connection to another DICOM application).
     *   obj             - [in] Contains the information which shall be sent over the network.
     *   presId          - [in] The ID of the presentation context which shall be used
     *   xferSyntax      - [in] The transfer syntax which shall be used.
     *   pdvType         - [in] Specifies if the information in this DcmDataset object belongs to
     *                          a DIMSE command (as for example C-STORE) (DUL_COMMANDPDV) or if
     *                          the information is actual instance information (DUL_DATASETPDV).
     *   callback        - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackContext - []
     */
{
    OFCondition dulCond = EC_Normal;
    OFCondition econd = EC_Normal;
    unsigned char *buf;
    unsigned long bufLen;
    OFBool last = OFFalse;
    OFBool written = OFFalse;
    offile_off_t rtnLength;
    Uint32 bytesTransmitted = 0;
    DUL_PDVLIST pdvList;
    DUL_PDV pdv;

#if 0
    /* the following variable is currently unused, leave it for future use */
    unsigned long pdvCount = 0;
#endif
    DcmWriteCache wcache;

    /* initialize some local variables (we want to use the association's send buffer */
    /* to store data) this buffer can only take a certain number of elements */
    buf = assoc->sendPDVBuffer;
    bufLen = assoc->sendPDVLength;

    /* we may wish to restrict output PDU size */
    Uint32 maxpdulen = dcmMaxOutgoingPDUSize.get();

    /* max PDV size is max PDU size minus 12 bytes PDU/PDV header */
    if (bufLen + 12 > maxpdulen)
    {
      bufLen = maxpdulen - 12;
    }

    /* on the basis of the association's buffer, create a buffer variable that we can write to */
    DcmOutputBufferStream outBuf(buf, bufLen);

    /* prepare all elements in the DcmDataset variable for transfer */
    obj->transferInit();

    /* initialize two more variables: groupLength_encoding specifies what will be done concerning */
    /* group length tags, sequenceType_encoding specifies how sequences will be handled */
    E_GrpLenEncoding groupLength_encoding = g_dimse_send_groupLength_encoding;
    E_EncodingType sequenceType_encoding = g_dimse_send_sequenceType_encoding;

    /* Mind that commands must always include group length (0000,0000) and */
    /* that commands do not contain sequences, yet */
    if (pdvType == DUL_COMMANDPDV)
        groupLength_encoding = EGL_withGL;

    /* start a loop: in each iteration information from the DcmDataset object (i.e. infor- */
    /* mation which shall be sent) will be set in the buffer (we need more than one itera- */
    /* tion if there is more information than the buffer can take at a time), a PDV object */
    /* with the buffer's data will be created and assigned to a list, and finally the */
    /* list's information will be sent over the network to the other DICOM application. */
    while (!last)
    {
        /* write data values which are contained in the DcmDataSet variable to the above created */
        /* buffer. Mind the transfer syntax, the sequence type encoding, the group length encoding */
        /* and remove all padding data elements. Depending on whether all information has been written */
        /* to the buffer, update the variable that determines the end of the while loop. (Note that */
        /* DcmDataset stores information about what of its content has already been sent to the buffer.) */
        if (! written)
        {
          econd = obj->write(outBuf, xferSyntax, sequenceType_encoding, &wcache,
                             groupLength_encoding, EPD_withoutPadding);
          if (econd == EC_Normal)                   /* all contents have been written to the buffer */
          {
              written = OFTrue;
          }
          else if (econd == EC_StreamNotifyClient)  /* no more space in buffer, _not_ all elements have been written to it */
          {
              // nothing to do
          }
          else                                      /* some error has occurred */
          {
              DCMNET_WARN(DIMSE_warn_str(assoc) << "writeBlock Failed (" << econd.text() << ")");
              return DIMSE_SENDFAILED;
          }
        }

        if (written) outBuf.flush(); // flush stream including embedded compression codec.

        /* get buffer and its length, assign to local variable */
        void *fullBuf = NULL;
        outBuf.flushBuffer(fullBuf, rtnLength);

        last = written && outBuf.isFlushed();

        /* if the buffer is not empty, do something with its contents */
        if (rtnLength > 0)
        {
            /* rtnLength could be odd */
            if (rtnLength & 1)
            {
              /* this should only happen if we use a stream compressed transfer
               * syntax and then only at the very end of the stream. Everything
               * else is a failure.
               */
              if (!last)
              {
                return makeDcmnetCondition(DIMSEC_SENDFAILED, OF_error,
                  "DIMSE Failed to send message: odd block length encountered");
              }

              /* since the block size is always even, block size must be larger
               * than rtnLength, so we can safely add a pad byte (and hope that
               * the pad byte will not confuse the receiver's decompressor).
               */
              unsigned char *cbuf = (unsigned char *)fullBuf;
              cbuf[rtnLength++] = 0; // add zero pad byte
            }

            /* initialize a DUL_PDV variable with the buffer's data */
            pdv.fragmentLength = OFstatic_cast(unsigned long, rtnLength);
            pdv.presentationContextID = presID;
            pdv.pdvType = pdvType;
            pdv.lastPDV = last;
            pdv.data = fullBuf;

            /* append this PDV to a PDV list structure, set the counter variable */
            /* to 1 since this structure contains only 1 element */
            pdvList.count = 1;
            pdvList.pdv = &pdv;

            /* dump some information if required */
            DCMNET_TRACE("DIMSE sendDcmDataset: sending " << pdv.fragmentLength << " bytes");

            /* send information over the network to the other DICOM application */
            dulCond = DUL_WritePDVs(&assoc->DULassociation, &pdvList);
            if (dulCond.bad())
                return makeDcmnetSubCondition(DIMSEC_SENDFAILED, OF_error, "DIMSE Failed to send message", dulCond);

            /* count the bytes and the amount of PDVs which were transmitted */
            bytesTransmitted += OFstatic_cast(Uint32, rtnLength);
#if 0
            pdvCount += pdvList.count;
#endif

            /* execute callback function to indicate progress */
            if (callback) {
                callback(callbackContext, bytesTransmitted);
            }
        }
    }

    /* indicate the end of the transfer */
    obj->transferEnd();

    return EC_Normal;
}

/*
** Public Functions Bodies
*/

/*
 * DIMSE Messaging
 */

/*
 * Message Send
 */

static OFCondition
DIMSE_sendMessage(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_Message *msg,
        DcmDataset *statusDetail,
        DcmDataset *dataObject,
        const char *dataFileName,
        DIMSE_ProgressCallback callback,
        void *callbackContext,
        DcmDataset **commandSet)
    /*
     * This function sends a DIMSE command and possibly also instance data from a file or from a given
     * data object via network to another DICOM application.
     *
     * Parameters:
     *   assoc           - [in] The association (network connection to another DICOM application).
     *   presId          - [in] The ID of the presentation context which shall be used
     *   msg             - [in] Structure that represents a certain DIMSE command which shall be sent.
     *   statusDetail    - [in] Detailed information with regard to the status information which is captured
     *                          in the status element (0000,0900). Note that the value for element (0000,0900)
     *                          is contained in this variable.
     *   dataObject      - [in] The instance data which shall be sent to the other DICOM application,
     *                          NULL, if there is none
     *   dataFileName    - [in] The name of the file that contains the instance data which shall be sent to
     *                          the other DICOM application, NULL; if there is none.
     *   callback        - [in] Pointer to a function which shall be called to indicate progress.
     *   callbackContext - []
     *   commandSet      - [out] [optional parameter, default = NULL] If this parameter is not NULL
     *                           it will return a copy of the DIMSE command which is sent to the other
     *                           DICOM application.
     */
{
    E_TransferSyntax xferSyntax;
    DcmDataset *cmdObj = NULL;
    DcmFileFormat dcmff;
    int fromFile = 0;
    OFCondition cond = EC_Normal;

    if (commandSet) *commandSet = NULL;

    /* check if the data dictionary is available. If not return an error */
    if (!isDataDictPresent()) return DIMSE_NODATADICT;

    /* validate DIMSE command information, i.e. check if the information which is */
    /* contained in msg meets certain conditions. For example, if msg represents a */
    /* C-ECHO-RQ, then there is not supposed to be a corresponding data set. If the */
    /* specified conditions are not met, return an error. */
    if (EC_Normal != (cond = validateMessage(assoc, msg))) return cond;

    /* check if the presentation context id refers to a valid presentation context and determine the */
    /* transfer syntax which is specified for this presentation context. Additionally, check if the specified */
    /* transfer syntax is supported at all. If any of the checks returns an error, return this error. */
    if (EC_Normal != (cond = checkPresentationContextForMessage(assoc, msg, presID, &xferSyntax))) return cond;

    /* create a DcmDataset object ("command object") based on the information in the DIMSE command */
    /* variable (remember that all DICOM commands are - in the end - particular data sets). The */
    /* information which will shortly be set in this object will be sent over the network later. */
    cond = DIMSE_buildCmdObject(msg, &cmdObj);

    /* if the command object has been created successfully and there is status detail */
    /* information move the status detail information to the command object. */
    if (cond.good() && statusDetail != NULL)
    {
      /* move the status detail to the command */
      DcmElement* e;
      while ((e = statusDetail->remove((unsigned long)0)) != NULL) cmdObj->insert(e, OFTrue);
    }

    /* if the command object has been created successfully and the data set is present */
    /* according to the information in the DIMSE command information variable, do something */
    if (cond.good() && DIMSE_isDataSetPresent(msg))
    {
      /* if a data object and a file name were passed, something is fishy */
      if ((dataObject != NULL)&&(dataFileName != NULL))
      {
        DCMNET_WARN(DIMSE_warn_str(assoc) << "sendData: both object and file specified (sending object only)");
      }
      /* if there is no data object but a file name, we need to read data from the specified file */
      /* to create a data object with the actual instance data that shall be sent */
      else if ((dataObject == NULL)&&(dataFileName != NULL))
      {
        if (! dcmff.loadFile(dataFileName, EXS_Unknown).good())
        {
          DCMNET_WARN(DIMSE_warn_str(assoc) << "sendMessage: cannot open DICOM file ("
            << dataFileName << "): " << OFStandard::getLastSystemErrorCode().message());
          cond = DIMSE_SENDFAILED;
        } else {
          dataObject = dcmff.getDataset();
          fromFile = 1;
        }
      }

      /* if we have a data object now, check if we can write the data object's elements in the required  */
      /* transfer syntax. In detail, every single item of the data object will be checked. */
      if (dataObject)
      {
        if (dataObject->isEmpty())
        {
          /* if dataset is empty, i.e. it contains no data elements, create a warning. */
          DCMNET_WARN(DIMSE_warn_str(assoc) << "sendMessage: dataset is empty");
          cond = DIMSE_SENDFAILED;
        }
        else if (! dataObject->canWriteXfer(xferSyntax))
        {
          /* if we cannot write all elements in the required transfer syntax, create a warning. */
          DcmXfer writeXferSyntax(xferSyntax);
          DcmXfer originalXferSyntax(dataObject->getOriginalXfer());
          if (fromFile && dataFileName)
          {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "sendMessage: unable to convert DICOM file '"
              << dataFileName << "' from '" << originalXferSyntax.getXferName()
              << "' transfer syntax to '" << writeXferSyntax.getXferName() << "'");
          } else {
            DCMNET_WARN(DIMSE_warn_str(assoc) << "sendMessage: unable to convert dataset from '"
              << originalXferSyntax.getXferName() << "' transfer syntax to '"
              << writeXferSyntax.getXferName() << "'");
          }
          cond = DIMSE_SENDFAILED;
        }
      } else {
        /* if there is neither a data object nor a file name, create a warning, since */
        /* the information in msg specified that instance data should be present. */
        DCMNET_WARN(DIMSE_warn_str(assoc) << "sendMessage: no dataset to send");
        cond = DIMSE_SENDFAILED;
      }
    }

    /* if all previous calls were successful, go ahead and send the */
    /* specified DIMSE command to the other DICOM application */
    if (cond.good())
    {
      /* if the global variable says so, we want to save the */
      /* DIMSE command's information to a file */
      if (g_dimse_save_dimse_data) saveDimseFragment(cmdObj, OFTrue, OFFalse);

      /* return a copy of the DIMSE command if required */
      if (commandSet) *commandSet = new DcmDataset(*cmdObj);

      /* dump information if required */
      DCMNET_TRACE("DIMSE Command to be sent on Presentation Context ID: " << OFstatic_cast(Uint16, presID));
      DCMNET_TRACE("DIMSE Command to send:" << OFendl << DcmObject::PrintHelper(*cmdObj));

      /* Send the DIMSE command. DIMSE commands are always little endian implicit. */
      cond = sendDcmDataset(assoc, cmdObj, presID, EXS_LittleEndianImplicit, DUL_COMMANDPDV, NULL, NULL);
    }

    /* Then we still have to send the actual instance data if the DIMSE command information variable */
    /* says that instance data is present and there actually is a corresponding data object */
    if (cond.good() && DIMSE_isDataSetPresent(msg) && (dataObject))
    {
      /* again, if the global variable says so, we want to save the instance data to a file */
      if (g_dimse_save_dimse_data) saveDimseFragment(dataObject, OFFalse, OFFalse);

      /* Send the instance data set using the corresponding transfer syntax */
      cond = sendDcmDataset(assoc, dataObject, presID, xferSyntax,
          DUL_DATASETPDV, callback, callbackContext);
    }

    /* clean up some memory */
    delete cmdObj;

    /* return result value */
    return cond;
}

OFCondition
DIMSE_sendMessageUsingFileData(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_Message *msg,
        DcmDataset *statusDetail,
        const char *dataFileName,
        DIMSE_ProgressCallback callback,
        void *callbackContext,
        DcmDataset **commandSet)
{
    /* simply call DIMSE_sendMessage to accomplish this task */
    return DIMSE_sendMessage(assoc, presID, msg, statusDetail, NULL, dataFileName, callback, callbackContext, commandSet);
}

OFCondition
DIMSE_sendMessageUsingMemoryData(
        T_ASC_Association *assoc,
        T_ASC_PresentationContextID presID,
        T_DIMSE_Message *msg,
        DcmDataset *statusDetail,
        DcmDataset *dataObject,
        DIMSE_ProgressCallback callback,
        void *callbackContext,
        DcmDataset **commandSet)
{
    /* simply call DIMSE_sendMessage to accomplish this task */
    return DIMSE_sendMessage(assoc, presID, msg, statusDetail, dataObject, NULL, callback, callbackContext, commandSet);
}

/*
 * Message Receive
 */

OFCondition DIMSE_ignoreDataSet(
        T_ASC_Association *assoc,
        T_DIMSE_BlockingMode blocking,
        int timeout,
        DIC_UL *bytesRead,
        DIC_UL *pdvCount)
{
    OFCondition cond = EC_Normal;
    DUL_PDV pdv;
    OFBool last = OFFalse;

    while (!last) {
        cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
        if (cond.bad()) {
            break;
        }
        if (pdv.pdvType != DUL_DATASETPDV) {
            cond = DIMSE_UNEXPECTEDPDVTYPE;
            break;
        }
        *bytesRead += pdv.fragmentLength;
        (*pdvCount)++;
        last = pdv.lastPDV;
    }
    return cond;
}


OFCondition
DIMSE_receiveCommand(
        T_ASC_Association *assoc,
        T_DIMSE_BlockingMode blocking,
        int timeout,
        T_ASC_PresentationContextID*presID,
        T_DIMSE_Message *msg,
        DcmDataset **statusDetail,
        DcmDataset **commandSet)
{
    OFCondition cond = EC_Normal;
    unsigned long bytesRead;
    unsigned long pdvCount;

    DUL_DATAPDV type;
    OFBool last;
    DUL_PDV pdv;

    DIC_UL elemsLeft;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax;
    DcmDataset *cmdSet;
    OFCondition econd = EC_Normal;

    if (statusDetail) *statusDetail = NULL;
    if (commandSet) *commandSet = NULL;

    /* dump some information if required */
    DCMNET_TRACE("DIMSE receiveCommand");

    /* check if the data dictionary is available. If not return an error */
    if (!isDataDictPresent()) return DIMSE_NODATADICT;

    /* set PDV counter to 0 */
    pdvCount = 0;

    /* create a new DcmDataset variable to capture the DIMSE command which we are about to receive */
    cmdSet = new DcmDataset();
    if (cmdSet == NULL) return EC_MemoryExhausted;

    /* prepare the DcmDataset variable for transfer of data */
    cmdSet->transferInit();

    /* create a buffer variable which can be used to store the received information */
    DcmInputBufferStream cmdBuf;
    if (! cmdBuf.good())
    {
        delete cmdSet;
        return makeDcmnetCondition(DIMSEC_PARSEFAILED, OF_error, "DIMSE: receiveCommand: Failed to initialize cmdBuf");
    }

    /* start a loop in which we want to read a DIMSE command from the incoming socket stream. */
    /* Since the command could stretch over more than one PDU, the use of a loop is mandatory. */
    for (last = OFFalse, bytesRead = 0, type = DUL_COMMANDPDV;
         type == DUL_COMMANDPDV && !last;)
    {
        /* make the stream remember any unread bytes */
        cmdBuf.releaseBuffer();

        /* get next PDV (in detail, in order to get this PDV, a */
        /* PDU has to be read from the incoming socket stream) */
        cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
        if (cond.bad() || (cond == DUL_PEERREQUESTEDRELEASE))
        {
            delete cmdSet;
            if (cond == DIMSE_READPDVFAILED)
                return makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", cond);
            else return cond; /* it was an abort or release request */
        }

        /* if this is the first loop iteration, get the presentation context ID which is captured in the */
        /* current PDV. If this is not the first loop iteration, check if the presentation context IDs in */
        /* the current PDV and in the last PDV are identical. If they are not, return an error. */
        if (pdvCount == 0)
        {
            pid = pdv.presentationContextID;
        }
        else if (pdv.presentationContextID != pid)
        {
            delete cmdSet;
            char buf1[256];
            OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Different PresIDs inside Command Set: %d != %d", pid, pdv.presentationContextID);
            OFCondition subCond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, buf1);
            return makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", subCond);
        }

        /* check if the fragment length of the current PDV is odd. This should */
        /* never happen (see DICOM standard (year 2000) part 7, annex F) (or */
        /* the corresponding section in a later version of the standard.) */
        if ((pdv.fragmentLength % 2) != 0)
        {
            /* This should NEVER happen.  See Part 7, Annex F. */
            char buf2[256];
            OFStandard::snprintf(buf2, sizeof(buf2), "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
            cond = makeDcmnetCondition(DIMSEC_RECEIVEFAILED, OF_error, buf2);
            break;
        }

        /* if information is contained the PDVs fragment, we want to insert this information into the buffer */
        if (pdv.fragmentLength > 0) {
            cmdBuf.setBuffer(pdv.data, pdv.fragmentLength);
        }

        /* if this fragment contains the last fragment of the DIMSE command, set the end of the stream */
        if (pdv.lastPDV) {
            cmdBuf.setEos();
        }

        /* insert the information which is contained in the buffer into the DcmDataset */
        /* variable. Mind that DIMSE commands are always specified in the little endian */
        /* implicit transfer syntax. Additionally, we want to remove group length tags. */
        econd = cmdSet->read(cmdBuf, EXS_LittleEndianImplicit, EGL_withoutGL);
        if (econd != EC_Normal && econd != EC_StreamNotifyClient)
        {
          delete cmdSet;
          return makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE: receiveCommand: cmdSet->read() Failed", econd);
        }

        /* update the counter that counts how many bytes were read from the incoming socket */
        /* stream. This variable will only be used for dumping general information. */
        bytesRead += pdv.fragmentLength;

        /* update the following variables which will be evaluated at the beginning of each loop iteration. */
        last = pdv.lastPDV;
        type = pdv.pdvType;

        /* update the counter that counts how many PDVs were received on the incoming */
        /* socket stream. This variable will be used for determining the first */
        /* loop iteration and dumping general information. */
        pdvCount++;
    }

    /* indicate the end of the transfer */
    cmdSet->transferEnd();

    /* dump information if required */
    DCMNET_TRACE("DIMSE receiveCommand: " << pdvCount << " PDVs (" << bytesRead << " bytes), PresID=" << (int) pid);

    /* check if this is a valid presentation context */
    cond = getTransferSyntax(assoc, pid, &xferSyntax);
    if (cond.bad())
    {
        delete cmdSet;
        return cond;
    }

    /* check if the PDVs which were read actually do belong */
    /* to a DIMSE command. If not, return an error. */
    if (type != DUL_COMMANDPDV)
    {
        delete cmdSet;
        OFCondition subCond = makeDcmnetCondition(DIMSEC_UNEXPECTEDPDVTYPE, OF_error, "DIMSE: Command PDV Expected");
        return makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", subCond);
    }

    /* if the global variable says so, we want to save the */
    /* DIMSE command's information to a file */
    if (g_dimse_save_dimse_data) saveDimseFragment(cmdSet, OFTrue, OFTrue);

    /* return a copy of the DIMSE command if required */
    if (commandSet) *commandSet = new DcmDataset(*cmdSet);

    /* dump some more information if required */
    DCMNET_TRACE("DIMSE Command Received:" << OFendl << DcmObject::PrintHelper(*cmdSet));

    /* parse the information in cmdSet and create a corresponding T_DIMSE_Message */
    /* structure which represents the the DIMSE message which was received */
    cond = DIMSE_parseCmdObject(msg, cmdSet);

    /* if the T_DIMSE_Message structure was created successfully, validate the message, i.e. */
    /* check if the information which is contained in msg meets certain conditions */
    if (cond == EC_Normal) {
        cond = validateMessage(assoc, msg);
    }

    /* Whatever is left in the cmdSet object should be status detail */
    /* information. Return this information to the caller if required. */
    if (cond == EC_Normal) {
        /* Count the elements that are still existent in cmdSet */
        elemsLeft = DIMSE_countElements(cmdSet);

        /* if the caller requires status Detail information and there is some */
        if ((statusDetail != NULL) && (elemsLeft > 0)) {
            /* return it to the caller in the reference parameter */
            *statusDetail = cmdSet;
        } else {
            /* either nothing is left or the caller does not want status */
            /* detail information; hence, delete the cmdSet object */
            delete cmdSet;
        }
    }
    /* if some error occurred before, delete cmdSet */
    else
        delete cmdSet;

    /* set the Presentation Context ID we received (out parameter) */
    *presID = pid;

    /* return result value */
    return cond;
}


OFCondition DIMSE_createFilestream(
        const OFFilename &filename,
        const T_DIMSE_C_StoreRQ *request,
        const T_ASC_Association *assoc,
        T_ASC_PresentationContextID presIdCmd,
        int writeMetaheader,
        DcmOutputFileStream **filestream)
{
  OFCondition cond = EC_Normal;
  DcmElement *elem=NULL;
  DcmMetaInfo *metainfo=NULL;
  DcmTag metaElementGroupLength(DCM_FileMetaInformationGroupLength);
  DcmTag fileMetaInformationVersion(DCM_FileMetaInformationVersion);
  DcmTag mediaStorageSOPClassUID(DCM_MediaStorageSOPClassUID);
  DcmTag mediaStorageSOPInstanceUID(DCM_MediaStorageSOPInstanceUID);
  DcmTag transferSyntaxUID(DCM_TransferSyntaxUID);
  DcmTag implementationClassUID(DCM_ImplementationClassUID);
  DcmTag implementationVersionName(DCM_ImplementationVersionName);
  DcmTag sourceApplicationEntityTitle(DCM_SourceApplicationEntityTitle);
  T_ASC_PresentationContext presentationContext;

  if (filename.isEmpty() || (request==NULL) || (assoc==NULL) ||
      (assoc->params==NULL) || (filestream==NULL))
  {
    return DIMSE_NULLKEY;
  }

  cond = ASC_findAcceptedPresentationContext(assoc->params, presIdCmd, &presentationContext);
  if (cond.bad()) return cond;

  if (writeMetaheader)
  {
    if (NULL == (metainfo = new DcmMetaInfo())) return EC_MemoryExhausted;
    if (NULL != (elem = new DcmUnsignedLong(metaElementGroupLength)))
    {
      metainfo->insert(elem, OFTrue);
      Uint32 temp = 0;
      ((DcmUnsignedLong*)elem)->putUint32Array(&temp, 1);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmOtherByteOtherWord(fileMetaInformationVersion)))
    {
      metainfo->insert(elem, OFTrue);
      Uint8 version[2] = {0,1};
      ((DcmOtherByteOtherWord*)elem)->putUint8Array(version, 2);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmUniqueIdentifier(mediaStorageSOPClassUID)))
    {
      metainfo->insert(elem, OFTrue);
      ((DcmUniqueIdentifier*)elem)->putString(request->AffectedSOPClassUID);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmUniqueIdentifier(mediaStorageSOPInstanceUID)))
    {
      metainfo->insert(elem, OFTrue);
      ((DcmUniqueIdentifier*)elem)->putString(request->AffectedSOPInstanceUID);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmUniqueIdentifier(transferSyntaxUID)))
    {
      metainfo->insert(elem, OFTrue);
      elem->putString(presentationContext.acceptedTransferSyntax);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmUniqueIdentifier(implementationClassUID)))
    {
      metainfo->insert(elem, OFTrue);
      const char *uid = OFFIS_IMPLEMENTATION_CLASS_UID;
      ((DcmUniqueIdentifier*)elem)->putString(uid);
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmShortString(implementationVersionName)))
    {
      metainfo->insert(elem, OFTrue);
      const char *version = OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2;
      ((DcmShortString*)elem)->putString(version);

      if (strlen(OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2) > 16)
      {
        DCMNET_WARN("DICOM implementation version name too long: " << OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2);
      }
    } else cond = EC_MemoryExhausted;
    if (NULL != (elem = new DcmApplicationEntity(sourceApplicationEntityTitle)))
    {
      metainfo->insert(elem, OFTrue);
      const char *aet = assoc->params->DULparams.callingAPTitle;
      if (aet) ((DcmApplicationEntity*)elem)->putString(aet);
    } else cond = EC_MemoryExhausted;

    if (cond == EC_MemoryExhausted)
    {
      delete metainfo;
      return cond;
    }

    cond = metainfo->computeGroupLengthAndPadding(EGL_withGL, EPD_noChange,
      META_HEADER_DEFAULT_TRANSFERSYNTAX, EET_UndefinedLength);
    if (cond.bad())
    {
      delete metainfo;
      return cond;
    }
  }

  *filestream = new DcmOutputFileStream(filename);
  if ((*filestream == NULL)||(! (*filestream)->good()))
  {
     if (metainfo) delete metainfo;
     if (*filestream)
     {
       delete *filestream;
       *filestream = NULL;
     }
     OFOStringStream stream;
     stream << "DIMSE createFilestream: cannot create file '" << filename << "'" << OFStringStream_ends;
     OFSTRINGSTREAM_GETOFSTRING(stream, msg)
     return makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, msg.c_str());
  }

  if (metainfo)
  {
    metainfo->transferInit();
    if (EC_Normal != metainfo->write(**filestream, META_HEADER_DEFAULT_TRANSFERSYNTAX, EET_ExplicitLength, NULL))
    {
      OFOStringStream stream;
      stream << "DIMSE createFilestream: cannot write metaheader to file '" << filename << "'" << OFStringStream_ends;
      OFSTRINGSTREAM_GETOFSTRING(stream, msg)
      cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, msg.c_str());
    }
    metainfo->transferEnd();
    delete metainfo;
  }

  return cond;
}


OFCondition
DIMSE_receiveDataSetInFile(
        T_ASC_Association *assoc,
        T_DIMSE_BlockingMode blocking,
        int timeout,
        T_ASC_PresentationContextID *presID,
        DcmOutputStream *filestream,
        DIMSE_ProgressCallback callback,
        void *callbackData)
{
    OFCondition cond = EC_Normal;
    DUL_PDV pdv;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax;
    OFBool last = OFFalse;
    DIC_UL pdvCount = 0;
    DIC_UL bytesRead = 0;

    if ((assoc == NULL) || (presID==NULL) || (filestream==NULL)) return DIMSE_NULLKEY;

    *presID = 0;        /* invalid value */
    offile_off_t written = 0;
    while (!last)
    {
        cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);
        if (cond != EC_Normal) last = OFTrue; // terminate loop

        if (!last)
        {
          if (pdv.pdvType != DUL_DATASETPDV)
          {
            cond = DIMSE_UNEXPECTEDPDVTYPE;
            last = OFTrue;
          }
        }

        if (!last)
        {
          if (pdvCount == 0)
          {
            pid = pdv.presentationContextID;
            /* is this a valid presentation context ? */
            cond = getTransferSyntax(assoc, pid, &xferSyntax);
            if (cond.bad()) last = OFTrue; // terminate loop
          }
          else if (pdv.presentationContextID != pid)
          {
            char buf1[256];
            OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Different PresIDs inside data set: %d != %d", pid, pdv.presentationContextID);
            OFCondition subCond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, buf1);
            cond = makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", subCond);
            last = OFTrue; // terminate loop
          }
        }

        if (!last)
        {
          if ((pdv.fragmentLength % 2) != 0)
          {
            /* This should NEVER happen.  See Part 7, Annex F. */
            char buf2[256];
            OFStandard::snprintf(buf2, sizeof(buf2), "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
            cond = makeDcmnetCondition(DIMSEC_RECEIVEFAILED, OF_error, buf2);
            last = OFTrue; // terminate loop
          }
        }

        if (!last)
        {
          written = filestream->write((void *)(pdv.data), (Uint32)(pdv.fragmentLength));
          if ((! filestream->good()) || (written != (Uint32)(pdv.fragmentLength)))
          {
              cond = DIMSE_ignoreDataSet(assoc, blocking, timeout, &bytesRead, &pdvCount);
              if (cond == EC_Normal)
              {
                cond = makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, "DIMSE receiveDataSetInFile: Cannot write to file");
              }
              last = OFTrue; // terminate loop
          }
        }

        if (!last)
        {
          bytesRead += pdv.fragmentLength;
          pdvCount++;
          last = pdv.lastPDV;

          DCMNET_TRACE("DIMSE receiveDataSetInFile: " << pdv.fragmentLength
              << " bytes read (last: " << ((last)?("YES"):("NO")) << ")");

          if (callback)
          { /* execute callback function */
            callback(callbackData, bytesRead);
          }
        }
    }

    /* set the Presentation Context ID we received */
    *presID = pid;
    return cond;
}


OFCondition
DIMSE_receiveDataSetInMemory(
        T_ASC_Association *assoc,
        T_DIMSE_BlockingMode blocking,
        int timeout,
        T_ASC_PresentationContextID *presID,
        DcmDataset **dataObject,
        DIMSE_ProgressCallback callback,
        void *callbackData)
{
    OFCondition cond = EC_Normal;
    OFCondition econd = EC_Normal;
    DcmDataset *dset = NULL;
    DUL_PDV pdv;
    T_ASC_PresentationContextID pid = 0;
    E_TransferSyntax xferSyntax = EXS_Unknown;
    OFBool last = OFFalse;
    DIC_UL pdvCount = 0;
    DIC_UL bytesRead = 0;

    /* check if the caller provided an address where the data set can be stored. If not return an error */
    if (dataObject == NULL) return DIMSE_NULLKEY;

    /* check if the data dictionary is available. If not return an error */
    if (!isDataDictPresent()) return DIMSE_NODATADICT;

    /* if we need to create a DcmDataset object at the given address, do so */
    if (*dataObject == NULL) {
        dset = new DcmDataset();
    } else {
        /* if not, use the one which was passed */
        dset = *dataObject;
    }

    /* check if there is still no DcmDataset object which can be used to store the data set. */
    if (dset == NULL)
    {
        /* if this is the case, just go ahead an receive data, but do not store it anywhere */
        cond = DIMSE_ignoreDataSet(assoc, blocking, timeout, &bytesRead, &pdvCount);

        /* if receiving was successful, let the caller know though that no DcmDataset variable could be created */
        if (cond == EC_Normal)
        {
            return makeDcmnetCondition(DIMSEC_OUTOFRESOURCES, OF_error, "DIMSE receiveDataSetInMemory: Cannot create DcmDataset");
        }

        /* if we get to here, receiving was not successful; there must have */
        /* been a network problem. Just pass the result on to the caller. */
        return cond;
    }

    /* create a buffer variable which can be used to store the received information */
    DcmInputBufferStream dataBuf;

    /* prepare the DcmDataset variable for transfer of data */
    dset->transferInit();

    /* start a loop in which we want to read the data set from the incoming socket stream. */
    /* Since the data set could stretch over more than one PDU, the use of a loop is mandatory. */
    while (!last && cond == EC_Normal)
    {

        /* make the stream remember any unread bytes */
        dataBuf.releaseBuffer();

        /* get next PDV (in detail, in order to get this PDV, a */
        /* PDU has to be read from the incoming socket stream) */
        cond = DIMSE_readNextPDV(assoc, blocking, timeout, &pdv);

        /* if some error occurred, end the loop */
        if (cond != EC_Normal) last = OFTrue;

        /* we are expecting to see a data set PDV; if the PDV which was received does not refer */
        /* to a data set, set the error indicating variable correspondingly and end the loop. */
        if (!last)
        {
          if (pdv.pdvType != DUL_DATASETPDV)
          {
            cond = DIMSE_UNEXPECTEDPDVTYPE;
            last = OFTrue;
          }
        }

        /* if this is the first loop iteration, get the presentation context ID which is captured */
        /* in the current PDV and determine what transfer syntax the presentation context ID refers */
        /* to (this is also a check concerning the question if the presentation context ID is valid). */
        /* If this is not the first loop iteration, check if the presentation context IDs in the */
        /* current PDV and in the last PDV are identical. If they are not, return an error. */
        if (!last)
        {
            if (pdvCount == 0)
            {
                pid = pdv.presentationContextID;

                cond = getTransferSyntax(assoc, pid, &xferSyntax);
                if (cond.bad()) last = OFTrue;
            }
            else if (pdv.presentationContextID != pid)
            {
                char buf1[256];
                OFStandard::snprintf(buf1, sizeof(buf1), "DIMSE: Different PresIDs inside data set: %d != %d", pid, pdv.presentationContextID);
                OFCondition subCond = makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, buf1);
                cond = makeDcmnetSubCondition(DIMSEC_RECEIVEFAILED, OF_error, "DIMSE Failed to receive message", subCond);
                last = OFTrue;
            }
        }

        if (!last)
        {
            /* check if the fragment length of the current PDV is odd. This should */
            /* never happen (see DICOM standard (year 2000) part 7, annex F) (or */
            /* the corresponding section in a later version of the standard.) */
            if ((pdv.fragmentLength % 2) != 0)
            {
                char buf2[256];
                OFStandard::snprintf(buf2, sizeof(buf2), "DIMSE: Odd Fragment Length: %lu", pdv.fragmentLength);
                cond = makeDcmnetCondition(DIMSEC_RECEIVEFAILED, OF_error, buf2);
                last = OFTrue;
            }
        }

        if (!last)
        {
            /* if information is contained the PDVs fragment, we want to insert this information into the buffer */
            if (pdv.fragmentLength > 0)
            {
                dataBuf.setBuffer(pdv.data, pdv.fragmentLength);
            }

            /* if this fragment contains the last fragment of the data set, set the end of the stream */
            if (pdv.lastPDV)
            {
                dataBuf.setEos();
            }

            /* insert the information which is contained in the buffer into the DcmDataset variable. Mind the */
            /* transfer syntax which was specified through the presentation context ID of the first PDV. */
            econd = dset->read(dataBuf, xferSyntax);
            if (econd != EC_Normal && econd != EC_StreamNotifyClient)
            {
                DCMNET_WARN(DIMSE_warn_str(assoc) << "DIMSE receiveDataSetInMemory: dset->read() Failed ("
                    << econd.text() << ")");
                cond = DIMSE_RECEIVEFAILED;
                last = OFTrue;
            }
        }

        if (!last)
        {
            /* update the counter that counts how many bytes were read from the incoming socket */
            /* stream. This variable will only be used for dumping general information. */
            bytesRead += pdv.fragmentLength;

            /* update the counter that counts how many PDVs were received on the incoming */
            /* socket stream. This variable will be used for determining the first */
            /* loop iteration and dumping general information. */
            pdvCount++;

            /* update the variable which will be evaluated at the beginning of each loop iteration. */
            last = pdv.lastPDV;

            /* dump information if required */
            DCMNET_TRACE("DIMSE receiveDataSetInMemory: " << pdv.fragmentLength
                << " bytes read (last: " << ((last)?("YES"):("NO")) << ")");

            /* execute callback function after each received PDV */
            if (callback)
            {
                callback(callbackData, bytesRead);
            }
        }
    }

    /* indicate the end of the transfer */
    dset->transferEnd();

    /* in case an error occurred, return this error */
    if (cond.bad())
    {
        if (*dataObject == NULL)
        {
            delete dset;
        }
        return cond;
    }

    /* if the global variable says so, we want to save the */
    /* DIMSE command's information to a file */
    if (g_dimse_save_dimse_data) saveDimseFragment(dset, OFFalse, OFTrue);

    /* set the Presentation Context ID we received */
    *presID = pid;

    /* set the object we received */
    *dataObject = dset;

    /* return result value */
    return cond;
}


/*
 * Misc functions
 */

OFString DIMSE_warn_str(T_ASC_Association *assoc)
{
    return OFString("DIMSE Warning: (") + assoc->params->DULparams.callingAPTitle
        + "," + assoc->params->DULparams.calledAPTitle + "): ";
}
