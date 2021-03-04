/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
*/

/*
** Include Files
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

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
    memset((char*)&req, 0, sizeof(req));

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
