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
** Author: Andrew Hewett		Created: 20-10-93
** 
** Module: dimcmd
**
** Purpose: 
**	Routines for building and parsing DIMSE Commands
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-02-01 10:24:08 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimcmd.cc,v $
** CVS/RCS Revision:	$Revision: 1.10 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <errno.h>

#include "dcdatset.h"
#include "dcdeftag.h"

#include "diutil.h"
#include "dimse.h"		
#include "dimcmd.h"

/*
 * Useful Macros
 */

#define RET(cond) if (!SUCCESS(cond)) return cond
#define BRET(cond, t) if (!SUCCESS(cond)) return buildError(t)
#define PRET(cond, t) if (!SUCCESS(cond)) return parseError(t)

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

#if 0
/* Currently Unused */
static CONDITION
buildError(DcmTagKey t)
{
    DcmTag tag(t);
    return COND_PushCondition(DIMSE_BUILDFAILED,
        "DIMSE: Command Build Failed: Element: (%x,%x) %s", 
        t.getGroup(), t.getElement(), tag.getTagName());
}
#endif

static CONDITION
buildErrorWithMsg(const char* msg, DcmTagKey t)
{
    DcmTag tag(t);
    return COND_PushCondition(DIMSE_BUILDFAILED,
        "DIMSE: Command Build Failed: %s: Element: (%x,%x) %s", msg,
        t.getGroup(), t.getElement(), tag.getTagName());
}

static CONDITION
parseError(DcmTagKey t)
{
    DcmTag tag(t);
    return COND_PushCondition(DIMSE_PARSEFAILED,
        "DIMSE: Command Parse Failed: Element: (%x,%x) %s", 
        t.getGroup(), t.getElement(), tag.getTagName());
}

static CONDITION
parseErrorWithMsg(const char* msg, DcmTagKey t)
{
    DcmTag tag(t);
    return COND_PushCondition(DIMSE_PARSEFAILED,
        "DIMSE: Command Parse Failed: %s: Element: (%x,%x) %s", msg,
        t.getGroup(), t.getElement(), tag.getTagName());
}

/*
 * Common dcmcode based utilities
 */

static CONDITION
deleteElem(DcmDataset *obj, DcmTagKey t)
{
    DcmTag tag(t);
    DcmElement *e = NULL;
    OFBool ok = OFTrue;

    e = obj->remove(tag);
    ok = (e != NULL);
    if (e) delete e;
    
    return (ok)?(DIMSE_NORMAL):
        (parseErrorWithMsg("dimcmd:deleteElem: Cannot delete element", t));
}


static CONDITION 
addString(DcmDataset *obj, DcmTagKey t, char *s)
{
    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);
    
    DU_stripLeadingAndTrailingSpaces(s);

    ec = newDicomElement(e, tag);
    if (ec == EC_Normal && s != NULL) {
        ec = e->putString(s);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }

    return (ec == EC_Normal)?(DIMSE_NORMAL):
        (buildErrorWithMsg("dimcmd:addString: Cannot add string", t));
}

static CONDITION
getString(DcmDataset *obj, DcmTagKey t, char *s, int maxlen)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    char* aString;
    
    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
	if (elem->getLength() == 0) {
	    s[0] = '\0';
        } else if ((int)elem->getLength() > maxlen) {
	    return parseErrorWithMsg("dimcmd:getString: string too small", t);
        } else {
            ec =  elem->getString(aString);
            strncpy(s, aString, maxlen);
	    DU_stripLeadingAndTrailingSpaces(s);
        }
    }
    return (ec == EC_Normal)?(DIMSE_NORMAL):(DIMSE_PARSEFAILED);
}

static CONDITION
getAndDeleteString(DcmDataset *obj, DcmTagKey t, char *s, int maxlen)
{
    CONDITION cond = DIMSE_NORMAL;
    
    cond = getString(obj, t, s, maxlen); PRET(cond, t);
    cond = deleteElem(obj, t);
    return cond;
}

static CONDITION
getAndDeleteStringOpt(DcmDataset *obj, DcmTagKey t, char *s, int maxlen)
{
    CONDITION cond = DIMSE_NORMAL;
    
    cond = getString(obj, t, s, maxlen); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}

static CONDITION
addUS(DcmDataset *obj, DcmTagKey t, Uint16 us)
{
    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);
    
    ec = newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16(us);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(DIMSE_NORMAL):
        (buildErrorWithMsg("dimcmd:addUS: Cannot add Uint16", t));
}

static CONDITION
getUS(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint16(*us, 0);
    }

    return (ec == EC_Normal)?(DIMSE_NORMAL):(DIMSE_PARSEFAILED);
}

static CONDITION
getAndDeleteUS(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    CONDITION cond;
    
    cond = getUS(obj, t, us); PRET(cond, t);
    cond = deleteElem(obj, t);
    return cond;
}

static CONDITION
getAndDeleteUSOpt(DcmDataset *obj, DcmTagKey t, Uint16 *us)
{
    CONDITION cond;
    
    cond = getUS(obj, t, us); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}


static CONDITION
addUL(DcmDataset *obj, DcmTagKey t, Uint32 ul)
{
    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);
    
    ec = newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint32(ul);
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(DIMSE_NORMAL):
        (buildErrorWithMsg("dimcmd:addUL: Cannot add Uint32", t));
}

static CONDITION
getUL(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        ec = elem->getUint32(*ul, 0);
    }

    return (ec == EC_Normal)?(DIMSE_NORMAL):(DIMSE_PARSEFAILED);
}

#if 0
/* Currently Unused */
static CONDITION
getAndDeleteUL(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    CONDITION cond;
    
    cond = getUL(obj, t, ul); PRET(cond, t);
    cond = deleteElem(obj, t); 
    return cond;
}
#endif

static CONDITION
getAndDeleteULOpt(DcmDataset *obj, DcmTagKey t, Uint32 *ul)
{
    CONDITION cond;
    
    cond = getUL(obj, t, ul); RET(cond);
    cond = deleteElem(obj, t); 
    return cond;
}

static CONDITION
addAttributeList(DcmDataset *obj, DcmTagKey t, Uint16 *list, int listCount)
{
    /* ListCount should represent the number of US values in the list (not the
    ** number of (gggg,eeee) pairs.
    */

    E_Condition ec = EC_Normal;
    DcmElement *e = NULL;
    DcmTag tag(t);

    if ((listCount % 2) != 0) {
        return buildErrorWithMsg("dimcmd:addAttributeList: Error: Uneven listCount", t);
    }
    
    ec = newDicomElement(e, tag);
    if (ec == EC_Normal) {
        ec = e->putUint16Array(list, (listCount / 2));
    }
    if (ec == EC_Normal) {
        ec = obj->insert(e, OFTrue);
    }
    return (ec == EC_Normal)?(DIMSE_NORMAL):
        (buildErrorWithMsg("dimcmd:addAttributeList: Cannot add list", t));
}

static CONDITION
getAttributeList(DcmDataset *obj, DcmTagKey t, Uint16 **list, int *listCount)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    Uint16 *aList = NULL;
    Uint32 nBytes = 0;

    ec = obj->search(t, stack);
    elem = (DcmElement*)stack.top();
    if (ec == EC_Normal && elem != NULL) {
        nBytes = elem->getLength();
	*listCount = (int)(nBytes / sizeof(Uint16));
	if (*listCount > 0) {
	    *list = (Uint16*)malloc((size_t)(nBytes + 1));
            ec = elem->getUint16Array(aList);
	    memcpy(*list, aList, (size_t)nBytes);
	} else {
	    *list = NULL;
	}
    }

    return (ec == EC_Normal)?(DIMSE_NORMAL):(DIMSE_PARSEFAILED);
}

/*
 * The attribute list in an N-GET-RQ is optional.  Do not require it.
 * The old code was causing problems with Print SCU software which 
 * sent an N-GET-RQ on the Printer SOP Class to obtain the printer identity.
 */
static CONDITION
getAndDeleteAttributeListOpt(DcmDataset *obj, DcmTagKey t, 
    Uint16 **list, int *listCount)
{
    CONDITION cond;
    cond = getAttributeList(obj, t, list, listCount); RET(cond);
    cond = deleteElem(obj, t);
    return cond;
}

/*
 * Common Building/Parsing Routines
 */

static CONDITION
buildCommonRQ(DcmDataset *obj, Uint16 command, Uint16 messageID,  
	Uint16 dataSetType)
{
    CONDITION cond;

    // insert group length but calculate later
    cond = addUL(obj, DCM_CommandGroupLength, 0); RET(cond);
    cond = addUS(obj, DCM_CommandField, command); RET(cond);
    cond = addUS(obj, DCM_MessageID, messageID); RET(cond);
    cond = addUS(obj, DCM_DataSetType, dataSetType); RET(cond);

    return DIMSE_NORMAL;
}

static CONDITION
parseCommonRQ(DcmDataset *obj, Uint16 *command, Uint16 *messageID,
	Uint16 *dataSetType)
{
    CONDITION cond;

    cond = getAndDeleteUS(obj, DCM_CommandField, command); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageID, messageID); RET(cond);
    cond = getAndDeleteUS(obj, DCM_DataSetType, dataSetType); RET(cond);

    return DIMSE_NORMAL;
}

static CONDITION
buildCommonRSP(DcmDataset *obj, Uint16 command, 
	Uint16 messageIDBeingRespondedTo,  
	Uint16 dataSetType, Uint16 status)
{
    CONDITION cond;

    // insert group length but calculate later
    cond = addUL(obj, DCM_CommandGroupLength, 0); RET(cond);
    cond = addUS(obj, DCM_CommandField, command); RET(cond);
    cond = addUS(obj, DCM_MessageIDBeingRespondedTo, 
    	messageIDBeingRespondedTo); RET(cond);
    cond = addUS(obj, DCM_DataSetType, dataSetType); RET(cond);
    cond = addUS(obj, DCM_Status, status); RET(cond);

    return DIMSE_NORMAL;
}

static CONDITION
parseCommonRSP(DcmDataset *obj, Uint16 *command, 
	Uint16 *messageIDBeingRespondedTo,
	Uint16 *dataSetType, Uint16 *status)
{
    CONDITION cond;

    cond = getAndDeleteUS(obj, DCM_CommandField, command); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageIDBeingRespondedTo,  
    	messageIDBeingRespondedTo);	 RET(cond);
    cond = getAndDeleteUS(obj, DCM_DataSetType, dataSetType); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Status, status); RET(cond);

    return DIMSE_NORMAL;
}

/*
 * C-ECHO-RQ
 */

static CONDITION
buildCEchoRQ(T_DIMSE_C_EchoRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_C_ECHO_RQ, e->MessageID,
	e->DataSetType); RET(cond);
	
    cond = addString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID); RET(cond);

    return cond;
}

static CONDITION
parseCEchoRQ(T_DIMSE_C_EchoRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); RET(cond);

    return DIMSE_NORMAL;
}

/*
 * C-ECHO-RSP
 */
 
static CONDITION
buildCEchoRSP(T_DIMSE_C_EchoRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_C_ECHO_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_ECHO_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseCEchoRSP(T_DIMSE_C_EchoRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_ECHO_AFFECTEDSOPCLASSUID;

    return DIMSE_NORMAL;
}

/*
 * C-STORE-RQ
 */

static CONDITION
buildCStoreRQ(T_DIMSE_C_StoreRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_C_STORE_RQ, e->MessageID,
	e->DataSetType); RET(cond);
	
    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_AffectedSOPInstanceUID,  
        e->AffectedSOPInstanceUID); RET(cond);
    cond = addUS(obj, DCM_Priority,  
	e->Priority); RET(cond);
	
    /* build optional items */
    if (e->opts & O_STORE_MOVEORIGINATORAETITLE) {
        cond = addString(obj, DCM_MoveOriginatorApplicationEntityTitle,  
	    e->MoveOriginatorApplicationEntityTitle); RET(cond);
    }
    if (e->opts & O_STORE_MOVEORIGINATORID) {
        cond = addUS(obj, DCM_MoveOriginatorMessageID,  
	    e->MoveOriginatorID); RET(cond);
    }
    return cond;
}

static CONDITION
parseCStoreRQ(T_DIMSE_C_StoreRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
    e->MessageID = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_STORE_RQ) {
	cond = DIMSE_PARSEFAILED; RET(cond);
    }

    /* parse other mandatory items */
    cond = getAndDeleteString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID, DIC_UI_LEN); 	RET(cond);
    cond = getAndDeleteString(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,  
	&priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_MoveOriginatorApplicationEntityTitle,  
        e->MoveOriginatorApplicationEntityTitle, DIC_AE_LEN);
    if (SUCCESS(cond)) e->opts |= O_STORE_MOVEORIGINATORAETITLE;

    cond = getAndDeleteUSOpt(obj, DCM_MoveOriginatorMessageID,  
	&e->MoveOriginatorID); 
    if (SUCCESS(cond)) e->opts |= O_STORE_MOVEORIGINATORID;

    return DIMSE_NORMAL;
}

/*
 * C-STORE-RSP
 */

static CONDITION
buildCStoreRSP(T_DIMSE_C_StoreRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_C_STORE_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_STORE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_STORE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseCStoreRSP(T_DIMSE_C_StoreRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_STORE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_STORE_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}

/*
 * C-FIND-RQ
 */
 
static CONDITION
buildCFindRQ(T_DIMSE_C_FindRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_C_FIND_RQ, e->MessageID,
	e->DataSetType); RET(cond);
	
    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID); RET(cond);
    cond = addUS(obj, DCM_Priority,  
	e->Priority); RET(cond);
	
    return cond;
}

static CONDITION
parseCFindRQ(T_DIMSE_C_FindRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); 	RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,  
	&priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    return DIMSE_NORMAL;
}
 
/*
 * C-FIND-RSP
 */
 
static CONDITION
buildCFindRSP(T_DIMSE_C_FindRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_C_FIND_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_FIND_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseCFindRSP(T_DIMSE_C_FindRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_FIND_AFFECTEDSOPCLASSUID;

    return DIMSE_NORMAL;
}

/*
 * C-GET-RQ
 */

static CONDITION
buildCGetRQ(T_DIMSE_C_GetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_C_GET_RQ, e->MessageID,
	e->DataSetType); RET(cond);
	
    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID); RET(cond);
    cond = addUS(obj, DCM_Priority,  
	e->Priority); RET(cond);
	
    return cond;
}

static CONDITION
parseCGetRQ(T_DIMSE_C_GetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); 	RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority,  
	&priority); e->Priority = (T_DIMSE_Priority)priority; RET(cond);

    return DIMSE_NORMAL;
}
 
/*
 * C-GET-RSP
 */
 
static CONDITION
buildCGetRSP(T_DIMSE_C_GetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_C_GET_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_GET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
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

static CONDITION
parseCGetRSP(T_DIMSE_C_GetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_GET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfRemainingSuboperations,  
        &e->NumberOfRemainingSubOperations);
    if (SUCCESS(cond)) e->opts |= O_GET_NUMBEROFREMAININGSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfCompletedSuboperations,  
        &e->NumberOfCompletedSubOperations);
    if (SUCCESS(cond)) e->opts |= O_GET_NUMBEROFCOMPLETEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfFailedSuboperations,  
        &e->NumberOfFailedSubOperations);
    if (SUCCESS(cond)) e->opts |= O_GET_NUMBEROFFAILEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfWarningSuboperations,  
        &e->NumberOfWarningSubOperations);
    if (SUCCESS(cond)) e->opts |= O_GET_NUMBEROFWARNINGSUBOPERATIONS;

    return DIMSE_NORMAL;
}
 
/*
 * C-MOVE-RQ
 */

static CONDITION
buildCMoveRQ(T_DIMSE_C_MoveRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_C_MOVE_RQ, e->MessageID,
	e->DataSetType); RET(cond);
	
    /* build other mandatory items */
    cond = addString(obj, DCM_AffectedSOPClassUID, 
        e->AffectedSOPClassUID); RET(cond);
    cond = addUS(obj, DCM_Priority, e->Priority); RET(cond);
    cond = addString(obj, DCM_MoveDestination, e->MoveDestination); RET(cond);
	
    return cond;
}

static CONDITION
parseCMoveRQ(T_DIMSE_C_MoveRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 priority;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); 	RET(cond);
    cond = getAndDeleteUS(obj, DCM_Priority, &priority); 
        e->Priority = (T_DIMSE_Priority)priority; RET(cond);
    cond = getAndDeleteString(obj, DCM_MoveDestination,  
	e->MoveDestination, DIC_AE_LEN); RET(cond);

    return DIMSE_NORMAL;
}
 
/*
 * C-MOVE-RSP
 */
 
static CONDITION
buildCMoveRSP(T_DIMSE_C_MoveRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_C_MOVE_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_MOVE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
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

static CONDITION
parseCMoveRSP(T_DIMSE_C_MoveRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_MOVE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfRemainingSuboperations,  
        &e->NumberOfRemainingSubOperations);
    if (SUCCESS(cond)) e->opts |= O_MOVE_NUMBEROFREMAININGSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfCompletedSuboperations,  
        &e->NumberOfCompletedSubOperations);
    if (SUCCESS(cond)) e->opts |= O_MOVE_NUMBEROFCOMPLETEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfFailedSuboperations,  
        &e->NumberOfFailedSubOperations);
    if (SUCCESS(cond)) e->opts |= O_MOVE_NUMBEROFFAILEDSUBOPERATIONS;

    cond = getAndDeleteUSOpt(obj, DCM_NumberOfWarningSuboperations,  
        &e->NumberOfWarningSubOperations);
    if (SUCCESS(cond)) e->opts |= O_MOVE_NUMBEROFWARNINGSUBOPERATIONS;

    return DIMSE_NORMAL;
}

/* 
 * C-CANCEL-RQ
 */
 
static CONDITION
buildCCancelRQ(T_DIMSE_C_CancelRQ * e, DcmDataset * obj)
{
    CONDITION cond;

    cond = addUS(obj, DCM_CommandField, DIMSE_C_CANCEL_RQ); RET(cond);
    cond = addUS(obj, DCM_MessageIDBeingRespondedTo, 
    	e->MessageIDBeingRespondedTo); RET(cond);
    cond = addUS(obj, DCM_DataSetType, e->DataSetType); RET(cond);

    return DIMSE_NORMAL;
}

static CONDITION
parseCCancelRQ(T_DIMSE_C_CancelRQ * e, DcmDataset * obj)
{
    CONDITION cond;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = getAndDeleteUS(obj, DCM_CommandField, &cmd); RET(cond);
    cond = getAndDeleteUS(obj, DCM_MessageIDBeingRespondedTo, 
        &msgid); RET(cond);
    cond = getAndDeleteUS(obj, DCM_DataSetType, &dtype); RET(cond);

    e->MessageIDBeingRespondedTo = msgid;
    if (dtype == DIMSE_DATASET_NULL) {
        e->DataSetType = DIMSE_DATASET_NULL;
    } else {
        e->DataSetType = DIMSE_DATASET_PRESENT;
    }

    if (cmd != DIMSE_C_CANCEL_RQ) {
	cond = DIMSE_PARSEFAILED; RET(cond);
    }

    return DIMSE_NORMAL;
}

/*
 * N-EVENT-REPORT-RQ
 */

static CONDITION
buildNEventReportRQ(T_DIMSE_N_EventReportRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_EVENT_REPORT_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_AffectedSOPClassUID,  
        e->AffectedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID); RET(cond);
    cond = addUS(obj, DCM_EventTypeID,  
	e->EventTypeID); RET(cond);
	
    return cond;
}

static CONDITION
parseNEventReportRQ(T_DIMSE_N_EventReportRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteString(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteUS(obj, DCM_EventTypeID,  
	&e->EventTypeID); RET(cond);
    return cond;
}

/*
 * N-EVENT-REPORT-RSP
 */

static CONDITION
buildNEventReportRSP(T_DIMSE_N_EventReportRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_EVENT_REPORT_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NEVENTREPORT_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    if (e->opts & O_NEVENTREPORT_EVENTTYPEID) {
        cond = addUS(obj, DCM_EventTypeID,  
	    e->EventTypeID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNEventReportRSP(T_DIMSE_N_EventReportRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NEVENTREPORT_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NEVENTREPORT_AFFECTEDSOPINSTANCEUID;

    cond = getAndDeleteUSOpt(obj, DCM_EventTypeID,  
	&e->EventTypeID); 
    if (SUCCESS(cond)) e->opts |= O_NEVENTREPORT_EVENTTYPEID;

    return DIMSE_NORMAL;
}

/*
 * N-GET-RQ
 */

static CONDITION
buildNGetRQ(T_DIMSE_N_GetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_GET_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,  
        e->RequestedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID); RET(cond);
    if (e->AttributeIdentifierList != NULL) {
        cond = addAttributeList(obj, DCM_AttributeIdentifierList, 
    	    e->AttributeIdentifierList, e->ListCount); RET(cond);
    }

    return cond;
}

static CONDITION
parseNGetRQ(T_DIMSE_N_GetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->RequestedSOPClassUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID, DIC_UI_LEN); RET(cond);
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

static CONDITION
buildNGetRSP(T_DIMSE_N_GetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_GET_RSP, e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NGET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NGET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNGetRSP(T_DIMSE_N_GetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NGET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NGET_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}

/*
 * N-SET-RQ
 */

static CONDITION
buildNSetRQ(T_DIMSE_N_SetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_SET_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,  
        e->RequestedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID); RET(cond);
    return cond;
}

static CONDITION
parseNSetRQ(T_DIMSE_N_SetRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->RequestedSOPClassUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID, DIC_UI_LEN); RET(cond);
    return cond;
}

/*
 * N-SET-RSP
 */

static CONDITION
buildNSetRSP(T_DIMSE_N_SetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_SET_RSP, e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NSET_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NSET_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNSetRSP(T_DIMSE_N_SetRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NSET_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NSET_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}

/*
 * N-ACTION-RQ
 */
 
static CONDITION
buildNActionRQ(T_DIMSE_N_ActionRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_ACTION_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,  
        e->RequestedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID); RET(cond);
    cond = addUS(obj, DCM_ActionTypeID,  
	e->ActionTypeID); RET(cond);
	
    return cond;
}

static CONDITION
parseNActionRQ(T_DIMSE_N_ActionRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->RequestedSOPClassUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteUS(obj, DCM_ActionTypeID,  
	&e->ActionTypeID); RET(cond);
    return cond;
}

/*
 * N-ACTION-RSP
 */

static CONDITION
buildNActionRSP(T_DIMSE_N_ActionRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_ACTION_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NACTION_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NACTION_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    if (e->opts & O_NACTION_ACTIONTYPEID) {
        cond = addUS(obj, DCM_ActionTypeID,  
	    e->ActionTypeID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNActionRSP(T_DIMSE_N_ActionRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NACTION_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NACTION_AFFECTEDSOPINSTANCEUID;

    cond = getAndDeleteUSOpt(obj, DCM_ActionTypeID,  
	&e->ActionTypeID); 
    if (SUCCESS(cond)) e->opts |= O_NACTION_ACTIONTYPEID;

    return DIMSE_NORMAL;
}

/*
 * N-CREATE-RQ
 */
 
static CONDITION
buildNCreateRQ(T_DIMSE_N_CreateRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_CREATE_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_AffectedSOPClassUID,  
        e->AffectedSOPClassUID); RET(cond);
    /* build optional items */
    if (e->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNCreateRQ(T_DIMSE_N_CreateRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN); RET(cond);

    /* parse optional items */
    e->opts = 0;
    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NCREATE_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}

/*
 * N-CREATE-RSP
 */

static CONDITION
buildNCreateRSP(T_DIMSE_N_CreateRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_CREATE_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NCREATE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NCREATE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNCreateRSP(T_DIMSE_N_CreateRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NCREATE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NCREATE_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}

/*
 * N-DELETE-RQ
 */

static CONDITION
buildNDeleteRQ(T_DIMSE_N_DeleteRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRQ(obj, DIMSE_N_DELETE_RQ, e->MessageID,
	e->DataSetType); RET(cond);
    cond = addString(obj, DCM_RequestedSOPClassUID,  
        e->RequestedSOPClassUID); RET(cond);
    cond = addString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID); RET(cond);
    return cond;
}

static CONDITION
parseNDeleteRQ(T_DIMSE_N_DeleteRQ * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;

    cond = parseCommonRQ(obj, &cmd, &msgid, &dtype); RET(cond);
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
        e->RequestedSOPClassUID, DIC_UI_LEN); RET(cond);
    cond = getAndDeleteString(obj, DCM_RequestedSOPInstanceUID,  
	e->RequestedSOPInstanceUID, DIC_UI_LEN); RET(cond);
    return cond;
}

/*
 * N-DELETE-RSP
 */

static CONDITION
buildNDeleteRSP(T_DIMSE_N_DeleteRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;

    cond = buildCommonRSP(obj, DIMSE_N_DELETE_RSP, 
        e->MessageIDBeingRespondedTo,
	e->DataSetType, e->DimseStatus); RET(cond);

    /* build optional items */
    if (e->opts & O_NDELETE_AFFECTEDSOPCLASSUID) {
        cond = addString(obj, DCM_AffectedSOPClassUID,  
            e->AffectedSOPClassUID); RET(cond);
    }
    if (e->opts & O_NDELETE_AFFECTEDSOPINSTANCEUID) {
        cond = addString(obj, DCM_AffectedSOPInstanceUID,  
	    e->AffectedSOPInstanceUID); RET(cond);
    }
    return cond;
}

static CONDITION
parseNDeleteRSP(T_DIMSE_N_DeleteRSP * e, DcmDataset * obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd;
    Uint16 msgid;
    Uint16 dtype;
    Uint16 status;

    cond = parseCommonRSP(obj, &cmd, &msgid, &dtype, &status); RET(cond);
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
        e->AffectedSOPClassUID, DIC_UI_LEN);
    if (SUCCESS(cond)) e->opts |= O_NDELETE_AFFECTEDSOPCLASSUID;

    cond = getAndDeleteStringOpt(obj, DCM_AffectedSOPInstanceUID,  
	e->AffectedSOPInstanceUID, DIC_UI_LEN); 
    if (SUCCESS(cond)) e->opts |= O_NDELETE_AFFECTEDSOPINSTANCEUID;

    return DIMSE_NORMAL;
}


CONDITION
DIMSE_buildCmdObject(T_DIMSE_Message *msg, DcmDataset **obj)
{
    CONDITION cond = DIMSE_NORMAL;

    if (*obj == NULL) {
        *obj = new DcmDataset();
    }

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
	cond = COND_PushCondition(DIMSE_BADCOMMANDTYPE, 
	    "DIMSE_buildCmdObject: Invalid Command Message: 0x%x", 
	    msg->CommandField);
	break;
    }

    if (!SUCCESS(cond)) {
	delete *obj;
	*obj = NULL;
    }

    return cond;
}

CONDITION
DIMSE_parseCmdObject(T_DIMSE_Message *msg, DcmDataset *obj)
{
    CONDITION cond = DIMSE_NORMAL;
    Uint16 cmd = DIMSE_NOTHING;
    Uint32 glen = 0;

    /* throw away the GroupLength attribute if present */
    getAndDeleteULOpt(obj, DCM_CommandGroupLength, &glen);

    /* get the command field */
    cond = getUS(obj, DCM_CommandField, &cmd);
    if (!SUCCESS(cond)) {
	return COND_PushCondition(DIMSE_PARSEFAILED,
	    "DIMSE_parseCmdObject: Missing CommandField");
    }

    bzero((char*)msg, sizeof(*msg));	/* make message empty */
    msg->CommandField = (T_DIMSE_Command)cmd;

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
	cond = COND_PushCondition(DIMSE_BADCOMMANDTYPE, 
	    "DIMSE_parseCmdObject: Invalid Command Message: 0x%x", 
	    msg->CommandField);
	break;
    }

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

/*
** CVS Log
** $Log: dimcmd.cc,v $
** Revision 1.10  2000-02-01 10:24:08  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.9  1999/04/19 08:34:40  meichel
** Fixed bug in getAndDeleteAttributeList() that caused
**   problems when an N-GET-RQ with an empty attribute list was sent.
**
** Revision 1.8  1998/08/10 08:53:41  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.7  1997/09/18 08:10:57  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.6  1997/07/21 08:47:16  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.5  1997/06/30 14:07:00  meichel
** Fixed bug in DIMSE module - the mandatory tag (0000,0000)
** (command group length) was created only for DIMSE-RQ messages
** but not for DIMSE-RSP messages.
**
** Revision 1.4  1997/05/16 08:18:34  andreas
** - Reactivate addUL in dimcmd.cc to add group length attribute explicit
**   to command dataset.
**
** Revision 1.3  1997/04/18 08:40:30  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.2  1996/04/25 16:11:13  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
