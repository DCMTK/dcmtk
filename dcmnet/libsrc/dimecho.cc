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
** Author: Andrew Hewett		Created: 03-06-93
** 
** Module: dimecho
**
** Purpose: 
**	This file contains the routines which help with
**	verification services.
**
**	Module Prefix: DIMSE_
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-02-01 10:24:09 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimecho.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
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

#include "diutil.h"
#include "dimse.h"		/* always include the module header */
#include "dimcond.h"


CONDITION
DIMSE_echoUser(
	/* in */ 
	T_ASC_Association *assoc, DIC_US msgId, 
	/* blocking info for response */
	T_DIMSE_BlockingMode blockMode, int timeout,
	/* out */
	DIC_US *status, DcmDataset **statusDetail)
{
    CONDITION cond;
    T_DIMSE_Message req, rsp;
    T_ASC_PresentationContextID presID;
    const char *sopClass;

    /* which SOP class  */
    sopClass = UID_VerificationSOPClass;

    /* which presentation context should be used */
    presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presID == 0) {
	return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, 
	    "DIMSE: No Presentation Context for: %s", sopClass);
    }

    bzero((char*)&req, sizeof(req));
    bzero((char*)&rsp, sizeof(rsp));

    req.CommandField = DIMSE_C_ECHO_RQ;
    req.msg.CEchoRQ.MessageID = msgId;
    strcpy(req.msg.CEchoRQ.AffectedSOPClassUID,
	   sopClass);
    req.msg.CEchoRQ.DataSetType = DIMSE_DATASET_NULL;
    
    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &req, 
        NULL, NULL, NULL, NULL);
    if (cond != DIMSE_NORMAL) {
	return cond;
    }

    /* receive response */
    cond = DIMSE_receiveCommand(assoc, blockMode, timeout, &presID, 
        &rsp, statusDetail);
    if (cond != DIMSE_NORMAL) {
	return cond;
    }

    if (rsp.CommandField != DIMSE_C_ECHO_RSP) {
	return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response Command Field: 0x%x",
		(unsigned)rsp.CommandField);
    }
 
    if (rsp.msg.CEchoRSP.MessageIDBeingRespondedTo != msgId) {
	return COND_PushCondition(DIMSE_UNEXPECTEDRESPONSE, 
		"DIMSE: Unexpected Response MsgId: %d (expected: %d)",
		rsp.msg.CEchoRSP.MessageIDBeingRespondedTo, msgId);
    }
    
    *status = rsp.msg.CEchoRSP.DimseStatus;

    return DIMSE_NORMAL;
   
}
 
CONDITION
DIMSE_sendEchoResponse(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presID,
	T_DIMSE_C_EchoRQ *req, DIC_US status, DcmDataset *statusDetail)
{
    CONDITION cond;
    T_DIMSE_Message rsp;

    bzero((char*)&rsp, sizeof(rsp));

    rsp.CommandField = DIMSE_C_ECHO_RSP;
    rsp.msg.CEchoRSP.MessageIDBeingRespondedTo = req->MessageID;
    strcpy(rsp.msg.CEchoRSP.AffectedSOPClassUID,
	req->AffectedSOPClassUID);
    rsp.msg.CEchoRSP.opts = O_ECHO_AFFECTEDSOPCLASSUID;
    rsp.msg.CEchoRSP.DataSetType = DIMSE_DATASET_NULL;
    rsp.msg.CEchoRSP.DimseStatus = status;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presID, &rsp, statusDetail,
        NULL, NULL, NULL);

    return cond;
}



/*
** CVS Log
** $Log: dimecho.cc,v $
** Revision 1.5  2000-02-01 10:24:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.4  1999/03/29 11:20:03  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.3  1998/08/10 08:53:43  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.2  1996/04/25 16:11:14  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
