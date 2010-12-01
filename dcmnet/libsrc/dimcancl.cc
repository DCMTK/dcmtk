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
** Module: dimcancel
**
** Purpose:
**      This file contains the routines which help with
**      sending C-CANCEL-RQ.
**
** Module Prefix: DIMSE_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:35 $
** CVS/RCS Revision:    $Revision: 1.9 $
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
#include "dcmtk/dcmnet/dimse.h"		/* always include the module header */
#include "dcmtk/dcmnet/cond.h"

OFCondition
DIMSE_sendCancelRequest(T_ASC_Association * assoc,
	T_ASC_PresentationContextID presId, DIC_US msgId)
{
    T_DIMSE_Message req;
    bzero((char*)&req, sizeof(req));

    req.CommandField = DIMSE_C_CANCEL_RQ;
    req.msg.CCancelRQ.MessageIDBeingRespondedTo = msgId;
    req.msg.CCancelRQ.DataSetType = DIMSE_DATASET_NULL;

    return DIMSE_sendMessageUsingMemoryData(assoc, presId, &req, NULL, NULL, NULL, NULL);
}

OFCondition
DIMSE_checkForCancelRQ(T_ASC_Association * assoc,
    T_ASC_PresentationContextID presId, DIC_US msgId)
{
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presIdCmd;

    OFCondition cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, 0, &presIdCmd, &msg, NULL);

    if (cond.good()) /* could be DIMSE_NODATAAVAILABLE or some error condition */
    {
        if (presIdCmd != presId)
        {
          return makeDcmnetCondition(DIMSEC_INVALIDPRESENTATIONCONTEXTID, OF_error, "DIMSE: Checking for C-CANCEL-RQ, bad presId");
	}
        if (msg.CommandField != DIMSE_C_CANCEL_RQ)
        {
          char buf1[256];
          sprintf(buf1, "DIMSE: Checking for C-CANCEL-RQ, Protocol Error: Cmd=0x%x", msg.CommandField);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDREQUEST, OF_error, buf1);
	}
	if (msg.msg.CCancelRQ.MessageIDBeingRespondedTo != msgId)
	{
          char buf2[256];
          sprintf(buf2, "DIMSE: Checking for C-CANCEL-RQ, Protocol Error: msgId=%d", msg.msg.CCancelRQ.MessageIDBeingRespondedTo);
          return makeDcmnetCondition(DIMSEC_UNEXPECTEDREQUEST, OF_error, buf2);
	}
    }
    return cond;
}

/*
** CVS Log
** $Log: dimcancl.cc,v $
** Revision 1.9  2010-12-01 08:26:35  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.8  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.7  2005/12/08 15:44:37  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.6  2002/11/27 13:04:38  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.5  2001/10/12 10:18:31  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.4  2000/02/23 15:12:30  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.3  2000/02/01 10:24:07  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
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
