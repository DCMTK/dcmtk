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
** Module: dimcancel
**
** Purpose: 
**	This file contains the routines which help with
**	sending C-CANCEL-RQ.
**
**	Module Prefix: DIMSE_
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-26 18:38:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dimcancl.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

/* 
** Include Files
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "diutil.h"
#include "dimse.h"		/* always include the module header */
#include "dimcond.h"

CONDITION
DIMSE_sendCancelRequest(T_ASC_Association * assoc, 
	T_ASC_PresentationContextID presId, DIC_US msgId)
{
    CONDITION           cond;
    T_DIMSE_Message	req;

    bzero(&req, sizeof(req));
	
    req.CommandField = DIMSE_C_CANCEL_RQ;
    req.msg.CCancelRQ.MessageIDBeingRespondedTo = msgId;
    req.msg.CCancelRQ.DataSetType = DIMSE_DATASET_NULL;

    cond = DIMSE_sendMessageUsingMemoryData(assoc, presId, &req,
        NULL, NULL, NULL, NULL);

    return cond;
}

CONDITION
DIMSE_checkForCancelRQ(T_ASC_Association * assoc, 
    T_ASC_PresentationContextID presId, DIC_US msgId)
{
    CONDITION cond = DIMSE_NORMAL;
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presIdCmd;

    cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 0, &presIdCmd,
				    &msg, NULL);

    if (cond != DIMSE_NORMAL) {
        /* DIMSE_NODATAAVAILABLE or some error condition */
        return cond;
    } else {
        if (presIdCmd != presId) {
	    return COND_PushCondition(DIMSE_INVALIDPRESENTATIONCONTEXTID,
	        "DIMSE: Checking for C-CANCEL-RQ, bad presId");
	}
        if (msg.CommandField != DIMSE_C_CANCEL_RQ) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDREQUEST,
	        "DIMSE: Checking for C-CANCEL-RQ, Protocol Error: Cmd=0x%x",
		msg.CommandField);
	}
	if (msg.msg.CCancelRQ.MessageIDBeingRespondedTo != msgId) {
	    return COND_PushCondition(DIMSE_UNEXPECTEDREQUEST,
	        "DIMSE: Checking for C-CANCEL-RQ, Protocol Error: msgId=%d",
		msg.msg.CCancelRQ.MessageIDBeingRespondedTo);
	}
    }
    /* return DIMSE_NORMAL if appropriate cancel req received */
    return DIMSE_NORMAL;
}

/*
** CVS Log
** $Log: dimcancl.cc,v $
** Revision 1.1  1996-03-26 18:38:45  hewett
** Initial revision
**
**
*/
