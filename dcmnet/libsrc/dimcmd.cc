/*
 *
 *  Copyright (C) 1994-2022, OFFIS e.V.
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
** Author: Andrew Hewett                Created: 20-10-93
**
** Module: dimcmd
**
** Purpose:
**      Routines for building and parsing DIMSE Commands
**
** Module Prefix: DIMSE_
**
*/


/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dimcmd.h"

/*
 * Useful Macros
 */

#define RET(cond) if (cond.bad()) return cond
#define BRET(cond, t) if (cond.bad()) return buildError(t)
#define PRET(cond, t) if (cond.bad()) return parseError(t)

/*
 * Type definitions
 */

/*
 * Global variables (should be used very, very rarely).
 */

/*
** Private Functions Prototypes
*/

/*
** Private Functions Bodies
*/

static OFCondition
buildErrorWithMsg(const char* msg, DcmTagKey t)
{
    DcmTag tag(t);
    char buf[1024];
    sprintf(buf, "DIMSE: Command Build Failed: %s: Element: (%04x,%04x) %s",
        msg, t.getGroup(), t.getElement(), tag.getTagName());
    return makeDcmnetCondition(DIMSEC_BUILDFAILED, OF_error, buf);
}

static OFCondition
parseError(DcmTagKey t)
{
    DcmTag tag(t);
    char buf[1024];
    sprintf(buf, "DIMSE: Command Parse Failed: Element: (%04x,%04x) %s",
        t.getGroup(), t.getElement(), tag.getTagName());
    return makeDcmnetCondition(DIMSEC_PARSEFAILED, OF_error, buf);
}

static OFCondition
parseErrorWithMsg(const char* msg, DcmTagKey t)
{
    DcmTag tag(t);
    char buf[1024];
    sprintf(buf, "DIMSE: Command Parse Failed: %s: Element: (%04x,%04x) %s", msg,
        t.getGroup(), t.getElement(), tag.getTagName());
    return makeDcmnetCondition(DIMSEC_PARSEFAILED, OF_error, buf);
}

/*
 * Common dcmcode based utilities
 */

static OFCondition
deleteElem(DcmDataset *obj, DcmTagKey t)
{
    DcmTag tag(t);
    DcmElement *e = NULL;
    OFBool ok = OFTrue;

    e = obj->remove(tag);
    ok = (e != NULL);
    if (e) delete e;

    return (ok)?(EC_Normal):
        (parseErrorWithMsg("dimcmd:deleteElem: Cannot delete element", t));
}


static OFCondition
addString(DcmDataset *obj, DcmTagKey t, char *s, OFBool keepPadding)
{
    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    if (! keepPadding) DU_stripLeadingAndTrailingSpaces(s);

    ec = DcmItem::newDicomElement(e, tag);
    if (ec == EC_Normal && s != NULL) {
        ec = e->putString(s);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }

    return (ec == EC_Normal)?(EC_Normal):
        (buildErrorWithMsg("dimcmd:addString: Cannot add string", t));
}

static OFCondition
getString(DcmDataset *obj, DcmTagKey t, char *s, int maxlen, OFBool *spacePadded)
{
    DcmElement *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;
    char* aString;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        if (elem->getLength() == 0) {
            s[0] = '\0';
        } else if (elem->getLength() > (Uint32)maxlen) {
            return parseErrorWithMsg("dimcmd:getString: string too small", t);
        } else {
            ec =  elem->getString(aString);
            strncpy(s, aString, maxlen);
            if (spacePadded)
            {
                /* before we remove leading and tailing spaces we want to know
                 * whether the string is actually space padded. Required to communicate
                 * with dumb peers which send space padded UIDs and fail if they
                 * receive correct UIDs back.
                 *
                 * This test can only detect space padded strings if
                 * dcmEnableAutomaticInputDataCorrection is false; otherwise the padding
                 * has already been removed by dcmdata at this stage.
                 */
                size_t s_len = strlen(s);
                if ((s_len > 0)&&(s[s_len-1] == ' ')) *spacePadded = OFTrue; else *spacePadded = OFFalse;
            }
            DU_stripLeadingAndTrailingSpaces(s);
        }
    }
    return (ec.good())? ec : DIMSE_PARSEFAILED;
}

static OFCondition
getAndDeleteString(DcmDataset *obj, DcmTagKey t, char *s, int maxlen, OFBool *spacePadded)
{
    OFCondition cond = getString(obj, t, s, maxlen, spacePadded); PRET(cond, t);
    cond = deleteElem(obj, t);
    return cond;
}

static OFCondition
getAndDeleteStringOpt(DcmDataset *obj, DcmTagKey t, char *s, int maxlen, OFBool *spacePadded)
{
    OFCondition cond = getString(obj, t, s, maxlen, spacePadded); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}

static OFCondition
addUS(DcmDataset *obj, DcmTagKey t, Uint16 us)
{
    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    ec = DcmItem::newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16(us);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(EC_Normal):
        (buildErrorWithMsg("dimcmd:addUS: Cannot add Uint16", t));
}

static OFCondition
getUS(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    DcmElement *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint16(*us, 0);
    }

    return (ec == EC_Normal)?(EC_Normal):(DIMSE_PARSEFAILED);
}

static OFCondition
getAndDeleteUS(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    OFCondition cond = getUS(obj, t, us); PRET(cond, t);
    cond = deleteElem(obj, t);
    return cond;
}

static OFCondition
getAndDeleteUSOpt(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    OFCondition cond = getUS(obj, t, us); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}


static OFCondition
addUL(DcmDataset *obj, DcmTagKey t, Uint32 ul)
{
    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    ec = DcmItem::newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint32(ul);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(EC_Normal):
        (buildErrorWithMsg("dimcmd:addUL: Cannot add Uint32", t));
}

static OFCondition
getUL(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    DcmElement *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint32(*ul, 0);
    }

    return (ec == EC_Normal)?(EC_Normal):(DIMSE_PARSEFAILED);
}

#if 0
/* Currently Unused */
static OFCondition
getAndDeleteUL(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    OFCondition cond = getUL(obj, t, ul); PRET(cond, t);
    cond = deleteElem(obj, t);
    return cond;
}
#endif

static OFCondition
getAndDeleteULOpt(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    OFCondition cond = getUL(obj, t, ul); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}

static OFCondition
addAttributeList(DcmDataset *obj, DcmTagKey t, Uint16 *lst, int listCount)
{
    /* ListCount should represent the number of US values in the list (not the
    ** number of (gggg,eeee) pairs.
    */

    OFCondition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    if ((listCount % 2) != 0) {
        return buildErrorWithMsg("dimcmd:addAttributeList: Error: Uneven listCount", t);
    }

    ec = DcmItem::newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16Array(lst, (listCount / 2));
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(EC_Normal):
        (buildErrorWithMsg("dimcmd:addAttributeList: Cannot add list", t));
}

static OFCondition
getAttributeList(DcmDataset *obj, DcmTagKey t, Uint16 **lst, int *listCount)
{
    DcmElement *elem;
    DcmStack stack;
    OFCondition ec = EC_Normal;
    Uint16 *aList = NULL;
    Uint32 nBytes = 0;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        nBytes = elem->getLength();
        *listCount = (int)(nBytes / sizeof(Uint16));
        if (*listCount > 0) {
            *lst = (Uint16*)malloc((size_t)(nBytes + 1));
            ec = elem->getUint16Array(aList);
            memcpy(*lst, aList, (size_t)nBytes);
        } else {
            *lst = NULL;
        }
    }

    return (ec == EC_Normal)?(EC_Normal):(DIMSE_PARSEFAILED);
}

/*
 * The attribute list in an N-GET-RQ is optional.  Do not require it.
 * The old code was causing problems with Print SCU software which
 * sent an N-GET-RQ on the Printer SOP Class to obtain the printer identity.
 */
static OFCondition
getAndDeleteAttributeListOpt(DcmDataset *obj, DcmTagKey t,
    Uint16 **lst, int *listCount)
{
    OFCondition cond = getAttributeList(obj, t, lst, listCount); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}

/*
 * Common Building/Parsing Routines
 */

static OFCondition
buildCommonRQ(DcmDataset *obj, Uint16 command, Uint16 messageID,
    Uint16 dataSetType)
{
    // insert group length but calculate later
    OFCondition cond = addUL(obj, DCM_CommandGroupLength, 0); RET(cond);
    cond = addUS(obj, DCM_CommandField, command); RET(cond);
    cond = addUS(obj, DCM_MessageID, messageID); RET(cond);
    cond = addUS(obj, DCM_CommandDataSetType, dataSetType); RET(cond);

    return EC_Normal;
}

static OFCondition
parseCommonRQ(DcmDataset *obj, Uint16 *command, Uint16 *messageID,
    Uint16 *dataSetType)
{
    OFCondition cond = getAndDeleteUS(obj, DCM_CommandField, command); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageID, messageID); RET(cond);
    cond = getAndDeleteUS(obj, DCM_CommandDataSetType, dataSetType); RET(cond);

    return EC_Normal;
}

static OFCondition
buildCommonRSP(DcmDataset *obj, Uint16 command,
    Uint16 messageIDBeingRespondedTo,
    Uint16 dataSetType, Uint16 status)
{
    // insert group length but calculate later
    OFCondition cond = addUL(obj, DCM_CommandGroupLength, 0); RET(cond);
    cond = addUS(obj, DCM_CommandField, command); RET(cond);
    cond = addUS(obj, DCM_MessageIDBeingRespondedTo, messageIDBeingRespondedTo); RET(cond);
    cond = addUS(obj, DCM_CommandDataSetType, dataSetType); RET(cond);
    cond = addUS(obj, DCM_Status, status); RET(cond);

    return EC_Normal;
}

static OFCondition
parseCommonRSP(DcmDataset *obj, Uint16 *command,
    Uint16 *messageIDBeingRespondedTo,
    Uint16 *dataSetType, Uint16 *status)
{
    OFCondition cond = getAndDeleteUS(obj, DCM_CommandField, command); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageIDBeingRespondedTo,
        messageIDBeingRespondedTo); RET(cond);
    cond = getAndDeleteUS(obj, DCM_CommandDataSetType, dataSetType); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Status, status); RET(cond);

    return EC_Normal;
}

/*
 * C-ECHO-RQ
 */

static OFCondition
buildCEchoRQ(T_DIMSE_C_EchoRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_C_ECHO_RQ, e->MessageID, OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_AffectedSOPClassUID, e->AffectedSOPClassUID, OFFalse); RET(cond);

    return cond;
}

static OFCondition
parseCEchoRQ(T_DIMSE_C_EchoRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_ECHO_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);

    return EC_Normal;
}

/*
 * C-ECHO-RSP
 */

static OFCondition
buildCEchoRSP(T_DIMSE_C_EchoRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_C_ECHO_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_ECHO_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseCEchoRSP(T_DIMSE_C_EchoRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_C_ECHO_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_ECHO_AFFECTEDSOPCLASSUID;

    return EC_Normal;
}

/*
 * C-STORE-RQ
 */

static OFCondition
buildCStoreRQ(T_DIMSE_C_StoreRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_C_STORE_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);

    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_Priority,
        OFstatic_cast(Uint16, e->Priority)); RET(cond);

    /* build optional items */
    if (e->opts & O_STORE_MOVEORIGINATORAETITLE) {
        cond = addString(obj, DCM_MoveOriginatorApplicationEntityTitle,
            e->MoveOriginatorApplicationEntityTitle, OFFalse); RET(cond);
    }
    if (e->opts & O_STORE_MOVEORIGINATORID) {
        cond = addUS(obj, DCM_MoveOriginatorMessageID,
            e->MoveOriginatorID); RET(cond);
    }
    return cond;
}

static OFCondition
parseCStoreRQ(T_DIMSE_C_StoreRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_STORE_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    OFBool uidSpacePadded = OFFalse;

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, &uidSpacePadded); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,
        &priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_MoveOriginatorApplicationEntityTitle,
        e->MoveOriginatorApplicationEntityTitle, DIC_AE_LEN, NULL);
    if (cond.good()) e->opts |= O_STORE_MOVEORIGINATORAETITLE;

    cond = getAndDeleteUSOpt(obj, DCM_MoveOriginatorMessageID,
        &e->MoveOriginatorID);
    if (cond.good()) e->opts |= O_STORE_MOVEORIGINATORID;
    if (uidSpacePadded) e->opts |= O_STORE_RQ_BLANK_PADDING;

    return EC_Normal;
}

/*
 * C-STORE-RSP
 */

static OFCondition
buildCStoreRSP(T_DIMSE_C_StoreRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_C_STORE_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_STORE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_STORE_AFFECTEDSOPINSTANCEUID)
    {
        char instanceuid[DIC_UI_LEN + 10];
        OFStandard::strlcpy(instanceuid, e->AffectedSOPInstanceUID, DIC_UI_LEN + 10);
        if ((e->opts & O_STORE_PEER_REQUIRES_EXACT_UID_COPY) &&
            (e->opts & O_STORE_RSP_BLANK_PADDING))
        {
            // restore illegal space padding.
            OFStandard::strlcat(instanceuid, " ", DIC_UI_LEN + 10);
        }
        cond = addString(obj, DCM_AffectedSOPInstanceUID, instanceuid, OFTrue); RET(cond);
    }
    return cond;
}

static OFCondition
parseCStoreRSP(T_DIMSE_C_StoreRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_C_STORE_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_STORE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_STORE_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}

/*
 * C-FIND-RQ
 */

static OFCondition
buildCFindRQ(T_DIMSE_C_FindRQ * e, DcmDataset * obj)
{

    OFCondition cond = buildCommonRQ(obj, DIMSE_C_FIND_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);

    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_Priority,
        OFstatic_cast(Uint16, e->Priority)); RET(cond);

    return cond;
}

static OFCondition
parseCFindRQ(T_DIMSE_C_FindRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_FIND_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,
        &priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    return EC_Normal;
}

/*
 * C-FIND-RSP
 */

static OFCondition
buildCFindRSP(T_DIMSE_C_FindRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_C_FIND_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_FIND_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseCFindRSP(T_DIMSE_C_FindRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_C_FIND_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_FIND_AFFECTEDSOPCLASSUID;

    return EC_Normal;
}

/*
 * C-GET-RQ
 */

static OFCondition
buildCGetRQ(T_DIMSE_C_GetRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_C_GET_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);

    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_Priority,
        OFstatic_cast(Uint16, e->Priority)); RET(cond);

    return cond;
}

static OFCondition
parseCGetRQ(T_DIMSE_C_GetRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_GET_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,
        &priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    return EC_Normal;
}

/*
 * C-GET-RSP
 */

static OFCondition
buildCGetRSP(T_DIMSE_C_GetRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_C_GET_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_GET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_GET_NUMBEROFREMAININGSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfRemainingSuboperations,
            e->NumberOfRemainingSubOperations); RET(cond);
    }
    if (e->opts & O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfCompletedSuboperations,
            e->NumberOfCompletedSubOperations); RET(cond);
    }
    if (e->opts & O_GET_NUMBEROFFAILEDSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfFailedSuboperations,
            e->NumberOfFailedSubOperations); RET(cond);
    }
    if (e->opts & O_GET_NUMBEROFWARNINGSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfWarningSuboperations,
            e->NumberOfWarningSubOperations); RET(cond);
    }
    return cond;
}

static OFCondition
parseCGetRSP(T_DIMSE_C_GetRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_C_GET_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_GET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfRemainingSuboperations,
        &e->NumberOfRemainingSubOperations);
    if (cond.good()) e->opts |= O_GET_NUMBEROFREMAININGSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfCompletedSuboperations,
        &e->NumberOfCompletedSubOperations);
    if (cond.good()) e->opts |= O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfFailedSuboperations,
        &e->NumberOfFailedSubOperations);
    if (cond.good()) e->opts |= O_GET_NUMBEROFFAILEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfWarningSuboperations,
        &e->NumberOfWarningSubOperations);
    if (cond.good()) e->opts |= O_GET_NUMBEROFWARNINGSUBOPERATIONS;

    return EC_Normal;
}

/*
 * C-MOVE-RQ
 */

static OFCondition
buildCMoveRQ(T_DIMSE_C_MoveRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_C_MOVE_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);

    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_Priority, OFstatic_cast(Uint16, e->Priority)); RET(cond);
    cond = addString(obj, DCM_MoveDestination, e->MoveDestination, OFFalse); RET(cond);

    return cond;
}

static OFCondition
parseCMoveRQ(T_DIMSE_C_MoveRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_MOVE_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority, &priority);
        e->Priority = (T_DIMSE_Priority)priority; RET(cond);
    cond = getAndDeleteString(obj, DCM_MoveDestination,
        e->MoveDestination, DIC_AE_LEN, NULL); RET(cond);

    return EC_Normal;
}

/*
 * C-MOVE-RSP
 */

static OFCondition
buildCMoveRSP(T_DIMSE_C_MoveRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_C_MOVE_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_MOVE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_MOVE_NUMBEROFREMAININGSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfRemainingSuboperations,
            e->NumberOfRemainingSubOperations); RET(cond);
    }
    if (e->opts & O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfCompletedSuboperations,
            e->NumberOfCompletedSubOperations); RET(cond);
    }
    if (e->opts & O_MOVE_NUMBEROFFAILEDSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfFailedSuboperations,
            e->NumberOfFailedSubOperations); RET(cond);
    }
    if (e->opts & O_MOVE_NUMBEROFWARNINGSUBOPERATIONS) {
        cond = addUS(obj, DCM_NumberOfWarningSuboperations,
            e->NumberOfWarningSubOperations); RET(cond);
    }
    return cond;
}

static OFCondition
parseCMoveRSP(T_DIMSE_C_MoveRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_C_MOVE_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_MOVE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfRemainingSuboperations,
        &e->NumberOfRemainingSubOperations);
    if (cond.good()) e->opts |= O_MOVE_NUMBEROFREMAININGSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfCompletedSuboperations,
        &e->NumberOfCompletedSubOperations);
    if (cond.good()) e->opts |= O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfFailedSuboperations,
        &e->NumberOfFailedSubOperations);
    if (cond.good()) e->opts |= O_MOVE_NUMBEROFFAILEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfWarningSuboperations,
        &e->NumberOfWarningSubOperations);
    if (cond.good()) e->opts |= O_MOVE_NUMBEROFWARNINGSUBOPERATIONS;

    return EC_Normal;
}

/*
 * C-CANCEL-RQ
 */

static OFCondition
buildCCancelRQ(T_DIMSE_C_CancelRQ * e, DcmDataset * obj)
{
    OFCondition cond = addUS(obj, DCM_CommandField, DIMSE_C_CANCEL_RQ); RET(cond);
    cond = addUS(obj, DCM_MessageIDBeingRespondedTo,
        e->MessageIDBeingRespondedTo); RET(cond);
    cond = addUS(obj, DCM_CommandDataSetType, OFstatic_cast(Uint16, e->DataSetType)); RET(cond);

    return EC_Normal;
}

static OFCondition
parseCCancelRQ(T_DIMSE_C_CancelRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = getAndDeleteUS(obj, DCM_CommandField, &cmd); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageIDBeingRespondedTo,
        &msgid); RET(cond);
    cond = getAndDeleteUS(obj, DCM_CommandDataSetType, &dtype); RET(cond);

    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_CANCEL_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    return EC_Normal;
}

/*
 * N-EVENT-REPORT-RQ
 */

static OFCondition
buildNEventReportRQ(T_DIMSE_N_EventReportRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_EVENT_REPORT_RQ, e->MessageID,
       OFstatic_cast(Uint16,  e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_EventTypeID,
        e->EventTypeID); RET(cond);

    return cond;
}

static OFCondition
parseNEventReportRQ(T_DIMSE_N_EventReportRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_EVENT_REPORT_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteUS(obj, DCM_EventTypeID,
        &e->EventTypeID); RET(cond);
    return cond;
}

/*
 * N-EVENT-REPORT-RSP
 */

static OFCondition
buildNEventReportRSP(T_DIMSE_N_EventReportRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_EVENT_REPORT_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NEVENTREPORT_EVENTTYPEID) {
        cond = addUS(obj, DCM_EventTypeID,
            e->EventTypeID); RET(cond);
    }
    return cond;
}

static OFCondition
parseNEventReportRSP(T_DIMSE_N_EventReportRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_EVENT_REPORT_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NEVENTREPORT_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID;

    cond = getAndDeleteUSOpt(obj, DCM_EventTypeID, &e->EventTypeID);
    if (cond.good()) e->opts |= O_NEVENTREPORT_EVENTTYPEID;

    return EC_Normal;
}

/*
 * N-GET-RQ
 */

static OFCondition
buildNGetRQ(T_DIMSE_N_GetRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_GET_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, OFFalse); RET(cond);
    if (e->AttributeIdentifierList != NULL) {
        cond = addAttributeList(obj, DCM_AttributeIdentifierList,
            e->AttributeIdentifierList, e->ListCount); RET(cond);
    }

    return cond;
}

static OFCondition
parseNGetRQ(T_DIMSE_N_GetRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_GET_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, DIC_UI_LEN, NULL); RET(cond);
    /* ignore any condition */
    e->ListCount = 0;
    e->AttributeIdentifierList = NULL;
    getAndDeleteAttributeListOpt(obj, DCM_AttributeIdentifierList,
        &e->AttributeIdentifierList, &e->ListCount);
    return cond;
}

/*
 * N-GET-RSP
 */

static OFCondition
buildNGetRSP(T_DIMSE_N_GetRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_GET_RSP, e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NGET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NGET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseNGetRSP(T_DIMSE_N_GetRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_GET_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NGET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NGET_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}

/*
 * N-SET-RQ
 */

static OFCondition
buildNSetRQ(T_DIMSE_N_SetRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_SET_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, OFFalse); RET(cond);
    return cond;
}

static OFCondition
parseNSetRQ(T_DIMSE_N_SetRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_SET_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, DIC_UI_LEN, NULL); RET(cond);
    return cond;
}

/*
 * N-SET-RSP
 */

static OFCondition
buildNSetRSP(T_DIMSE_N_SetRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_SET_RSP, e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NSET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NSET_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseNSetRSP(T_DIMSE_N_SetRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_SET_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NSET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NSET_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}

/*
 * N-ACTION-RQ
 */

static OFCondition
buildNActionRQ(T_DIMSE_N_ActionRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_ACTION_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, OFFalse); RET(cond);
    cond = addUS(obj, DCM_ActionTypeID,
        e->ActionTypeID); RET(cond);

    return cond;
}

static OFCondition
parseNActionRQ(T_DIMSE_N_ActionRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_ACTION_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteUS(obj, DCM_ActionTypeID,
        &e->ActionTypeID); RET(cond);
    return cond;
}

/*
 * N-ACTION-RSP
 */

static OFCondition
buildNActionRSP(T_DIMSE_N_ActionRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_ACTION_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NACTION_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NACTION_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NACTION_ACTIONTYPEID) {
        cond = addUS(obj, DCM_ActionTypeID,
            e->ActionTypeID); RET(cond);
    }
    return cond;
}

static OFCondition
parseNActionRSP(T_DIMSE_N_ActionRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_ACTION_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NACTION_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NACTION_AFFECTEDSOPINSTANCEUID;

    cond = getAndDeleteUSOpt(obj, DCM_ActionTypeID, &e->ActionTypeID);
    if (cond.good()) e->opts |= O_NACTION_ACTIONTYPEID;

    return EC_Normal;
}

/*
 * N-CREATE-RQ
 */

static OFCondition
buildNCreateRQ(T_DIMSE_N_CreateRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_CREATE_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, OFFalse); RET(cond);
    /* build optional items */
    if (e->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseNCreateRQ(T_DIMSE_N_CreateRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_CREATE_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NCREATE_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}

/*
 * N-CREATE-RSP
 */

static OFCondition
buildNCreateRSP(T_DIMSE_N_CreateRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_CREATE_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NCREATE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseNCreateRSP(T_DIMSE_N_CreateRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_CREATE_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NCREATE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NCREATE_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}

/*
 * N-DELETE-RQ
 */

static OFCondition
buildNDeleteRQ(T_DIMSE_N_DeleteRQ * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRQ(obj, DIMSE_N_DELETE_RQ, e->MessageID,
        OFstatic_cast(Uint16, e->DataSetType)); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, OFFalse); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, OFFalse); RET(cond);
    return cond;
}

static OFCondition
parseNDeleteRQ(T_DIMSE_N_DeleteRQ * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    OFCondition cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_N_DELETE_RQ) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_RequestedSOPClassUID,
        e->RequestedSOPClassUID, DIC_UI_LEN, NULL); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,
        e->RequestedSOPInstanceUID, DIC_UI_LEN, NULL); RET(cond);
    return cond;
}

/*
 * N-DELETE-RSP
 */

static OFCondition
buildNDeleteRSP(T_DIMSE_N_DeleteRSP * e, DcmDataset * obj)
{
    OFCondition cond = buildCommonRSP(obj, DIMSE_N_DELETE_RSP,
        e->MessageIDBeingRespondedTo,
        OFstatic_cast(Uint16, e->DataSetType), e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NDELETE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,
            e->AffectedSOPClassUID, OFFalse); RET(cond);
    }
    if (e->opts & O_NDELETE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,
            e->AffectedSOPInstanceUID, OFFalse); RET(cond);
    }
    return cond;
}

static OFCondition
parseNDeleteRSP(T_DIMSE_N_DeleteRSP * e, DcmDataset * obj)
{
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    OFCondition cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }
    e->DimseStatus = status;

    if (cmd != DIMSE_N_DELETE_RSP) {
        cond = DIMSE_PARSEFAILED; RET(cond);
    }

    e->opts = 0;
    /* parse optional items */
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPClassUID,
        e->AffectedSOPClassUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NDELETE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,
        e->AffectedSOPInstanceUID, DIC_UI_LEN, NULL);
    if (cond.good()) e->opts |= O_NDELETE_AFFECTEDSOPINSTANCEUID;

    return EC_Normal;
}


OFCondition
DIMSE_buildCmdObject(T_DIMSE_Message *msg, DcmDataset **obj)
    /*
     * This function creates a DcmDataSet object ("command object") based on the information in the DIMSE
     * message variable (remember that all DICOM command messages are - in the end - particular data sets). This
     * newly created object will contain all the information that is needed for a particular DIMSE command
     * that will be sent over the network, i.e. also command group length, command field, data set type etc.
     * Note that a group length element will already be added to the created object but that the value in
     * this element will not yet be set. (It has to be set before the command is sent over the network though.)
     *
     * Parameters:
     *   msg - [in] Structure that represents a certain DIMSE command which shall be sent.
     *   obj - [out] The DcmDataSet object which was created from msg.
     */
{
    OFCondition cond = EC_Normal;

    /* if there is no such result object yet, create one */
    if (*obj == NULL) {
        *obj = new DcmDataset();
    }

    /* depending on the type of DIMSE command, create a corresponding DcmDataSet object */
    switch (msg->CommandField) {
    case DIMSE_C_ECHO_RQ:
        cond = buildCEchoRQ(&msg->msg.CEchoRQ, *obj);
        break;
    case DIMSE_C_ECHO_RSP:
        cond = buildCEchoRSP(&msg->msg.CEchoRSP, *obj);
        break;
    case DIMSE_C_STORE_RQ:
        cond = buildCStoreRQ(&msg->msg.CStoreRQ, *obj);
        break;
    case DIMSE_C_STORE_RSP:
        cond = buildCStoreRSP(&msg->msg.CStoreRSP, *obj);
        break;
    case DIMSE_C_GET_RQ:
        cond = buildCGetRQ(&msg->msg.CGetRQ, *obj);
        break;
    case DIMSE_C_GET_RSP:
        cond = buildCGetRSP(&msg->msg.CGetRSP, *obj);
        break;
    case DIMSE_C_FIND_RQ:
        cond = buildCFindRQ(&msg->msg.CFindRQ, *obj);
        break;
    case DIMSE_C_FIND_RSP:
        cond = buildCFindRSP(&msg->msg.CFindRSP, *obj);
        break;
    case DIMSE_C_MOVE_RQ:
        cond = buildCMoveRQ(&msg->msg.CMoveRQ, *obj);
        break;
    case DIMSE_C_MOVE_RSP:
        cond = buildCMoveRSP(&msg->msg.CMoveRSP, *obj);
        break;
    case DIMSE_C_CANCEL_RQ:
        cond = buildCCancelRQ(&msg->msg.CCancelRQ, *obj);
        break;
    case DIMSE_N_EVENT_REPORT_RQ:
        cond = buildNEventReportRQ(&msg->msg.NEventReportRQ, *obj);
        break;
    case DIMSE_N_EVENT_REPORT_RSP:
        cond = buildNEventReportRSP(&msg->msg.NEventReportRSP, *obj);
        break;
    case DIMSE_N_GET_RQ:
        cond = buildNGetRQ(&msg->msg.NGetRQ, *obj);
        break;
    case DIMSE_N_GET_RSP:
        cond = buildNGetRSP(&msg->msg.NGetRSP, *obj);
        break;
    case DIMSE_N_SET_RQ:
        cond = buildNSetRQ(&msg->msg.NSetRQ, *obj);
        break;
    case DIMSE_N_SET_RSP:
        cond = buildNSetRSP(&msg->msg.NSetRSP, *obj);
        break;
    case DIMSE_N_ACTION_RQ:
        cond = buildNActionRQ(&msg->msg.NActionRQ, *obj);
        break;
    case DIMSE_N_ACTION_RSP:
        cond = buildNActionRSP(&msg->msg.NActionRSP, *obj);
        break;
    case DIMSE_N_CREATE_RQ:
        cond = buildNCreateRQ(&msg->msg.NCreateRQ, *obj);
        break;
    case DIMSE_N_CREATE_RSP:
        cond = buildNCreateRSP(&msg->msg.NCreateRSP, *obj);
        break;
    case DIMSE_N_DELETE_RQ:
        cond = buildNDeleteRQ(&msg->msg.NDeleteRQ, *obj);
        break;
    case DIMSE_N_DELETE_RSP:
        cond = buildNDeleteRSP(&msg->msg.NDeleteRSP, *obj);
        break;
    default:
        {
          char buf[256];
          sprintf(buf, "DIMSE_buildCmdObject: Invalid Command Message: 0x%x", msg->CommandField);
          cond = makeDcmnetCondition(DIMSEC_BADCOMMANDTYPE, OF_error, buf);
        }
        break;
    }

    /* if the creation was not successful delete the DcmDataSet object */
    if (cond.bad())
    {
        delete *obj;
        *obj = NULL;
    }

    /* return result value */
    return cond;
}

OFCondition
DIMSE_parseCmdObject(T_DIMSE_Message *msg, DcmDataset *obj)
    /*
     * This function parses the information in the DcmDataset object which was passed
     * and creates a corresponding T_DIMSE_Message structure which represents the
     * DIMSE message which is contained in obj.
     *
     * Parameters:
     *   msg - [out] Contains in the end the DIMSE message which is contained in obj.
     *   obj - [in] The DcmDataset object which shall be parsed.
     */
{
    Uint16 cmd = DIMSE_NOTHING;
    Uint32 glen = 0;

    /* remove group length attribute if there is one in obj */
    getAndDeleteULOpt(obj, DCM_CommandGroupLength, &glen);

    /* get the command field */
    OFCondition cond = getUS(obj, DCM_CommandField, &cmd);
    if (cond.bad())
    {
      return makeDcmnetCondition(DIMSEC_PARSEFAILED, OF_error, "DIMSE_parseCmdObject: Missing CommandField");
    }

    /* initialize msg structure */
    memset((char*)msg, 0, sizeof(*msg));
    msg->CommandField = (T_DIMSE_Command)cmd;

    /* depending on the command, parse the rest of obj */
    /* and insert corresponding information into msg */
    switch (cmd) {
    case DIMSE_C_ECHO_RQ:
        cond = parseCEchoRQ(&msg->msg.CEchoRQ, obj);
        break;
    case DIMSE_C_ECHO_RSP:
        cond = parseCEchoRSP(&msg->msg.CEchoRSP, obj);
        break;
    case DIMSE_C_STORE_RQ:
        cond = parseCStoreRQ(&msg->msg.CStoreRQ, obj);
        break;
    case DIMSE_C_STORE_RSP:
        cond = parseCStoreRSP(&msg->msg.CStoreRSP, obj);
        break;
    case DIMSE_C_GET_RQ:
        cond = parseCGetRQ(&msg->msg.CGetRQ, obj);
        break;
    case DIMSE_C_GET_RSP:
        cond = parseCGetRSP(&msg->msg.CGetRSP, obj);
        break;
    case DIMSE_C_FIND_RQ:
        cond = parseCFindRQ(&msg->msg.CFindRQ, obj);
        break;
    case DIMSE_C_FIND_RSP:
        cond = parseCFindRSP(&msg->msg.CFindRSP, obj);
        break;
    case DIMSE_C_MOVE_RQ:
        cond = parseCMoveRQ(&msg->msg.CMoveRQ, obj);
        break;
    case DIMSE_C_MOVE_RSP:
        cond = parseCMoveRSP(&msg->msg.CMoveRSP, obj);
        break;
    case DIMSE_C_CANCEL_RQ:
        cond = parseCCancelRQ(&msg->msg.CCancelRQ, obj);
        break;
    case DIMSE_N_EVENT_REPORT_RQ:
        cond = parseNEventReportRQ(&msg->msg.NEventReportRQ, obj);
        break;
    case DIMSE_N_EVENT_REPORT_RSP:
        cond = parseNEventReportRSP(&msg->msg.NEventReportRSP, obj);
        break;
    case DIMSE_N_GET_RQ:
        cond = parseNGetRQ(&msg->msg.NGetRQ, obj);
        break;
    case DIMSE_N_GET_RSP:
        cond = parseNGetRSP(&msg->msg.NGetRSP, obj);
        break;
    case DIMSE_N_SET_RQ:
        cond = parseNSetRQ(&msg->msg.NSetRQ, obj);
        break;
    case DIMSE_N_SET_RSP:
        cond = parseNSetRSP(&msg->msg.NSetRSP, obj);
        break;
    case DIMSE_N_ACTION_RQ:
        cond = parseNActionRQ(&msg->msg.NActionRQ, obj);
        break;
    case DIMSE_N_ACTION_RSP:
        cond = parseNActionRSP(&msg->msg.NActionRSP, obj);
        break;
    case DIMSE_N_CREATE_RQ:
        cond = parseNCreateRQ(&msg->msg.NCreateRQ, obj);
        break;
    case DIMSE_N_CREATE_RSP:
        cond = parseNCreateRSP(&msg->msg.NCreateRSP, obj);
        break;
    case DIMSE_N_DELETE_RQ:
        cond = parseNDeleteRQ(&msg->msg.NDeleteRQ, obj);
        break;
    case DIMSE_N_DELETE_RSP:
        cond = parseNDeleteRSP(&msg->msg.NDeleteRSP, obj);
        break;
    default:
        {
          char buf[256];
          sprintf(buf, "DIMSE_parseCmdObject: Invalid Command Message: 0x%x", msg->CommandField);
          cond = makeDcmnetCondition(DIMSEC_BADCOMMANDTYPE, OF_error, buf);
        }
        break;
    }

    /* return result value */
    return cond;
}

OFBool
DIMSE_isDataSetPresent(T_DIMSE_Message *msg)
{
    OFBool present = OFFalse;

    switch (msg->CommandField) {
    case DIMSE_C_ECHO_RQ:
        present = (msg->msg.CEchoRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_ECHO_RSP:
        present = (msg->msg.CEchoRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_STORE_RQ:
        present = (msg->msg.CStoreRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_STORE_RSP:
        present = (msg->msg.CStoreRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_GET_RQ:
        present = (msg->msg.CGetRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_GET_RSP:
        present = (msg->msg.CGetRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_FIND_RQ:
        present = (msg->msg.CFindRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_FIND_RSP:
        present = (msg->msg.CFindRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_MOVE_RQ:
        present = (msg->msg.CMoveRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_MOVE_RSP:
        present = (msg->msg.CMoveRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_C_CANCEL_RQ:
        present = (msg->msg.CCancelRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_EVENT_REPORT_RQ:
        present = (msg->msg.NEventReportRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_EVENT_REPORT_RSP:
        present = (msg->msg.NEventReportRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_GET_RQ:
        present = (msg->msg.NGetRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_GET_RSP:
        present = (msg->msg.NGetRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_SET_RQ:
        present = (msg->msg.NSetRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_SET_RSP:
        present = (msg->msg.NSetRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_ACTION_RQ:
        present = (msg->msg.NActionRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_ACTION_RSP:
        present = (msg->msg.NActionRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_CREATE_RQ:
        present = (msg->msg.NCreateRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_CREATE_RSP:
        present = (msg->msg.NCreateRSP.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_DELETE_RQ:
        present = (msg->msg.NDeleteRQ.DataSetType != DIMSE_DATASET_NULL);
        break;
    case DIMSE_N_DELETE_RSP:
        present = (msg->msg.NDeleteRSP.DataSetType != DIMSE_DATASET_NULL);
        break;

    default:
        present = OFFalse;
        break;
    }

    return present;
}

unsigned long
DIMSE_countElements(DcmDataset *obj)
{
    unsigned long n = 0;

    n = obj->card();

    return n;
}
