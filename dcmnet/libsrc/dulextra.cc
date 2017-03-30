/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
** Author: Andrew Hewett                Created: 03-08-93
**
** Module: dulextra
**
** Purpose:
**      Supplementary DUL functions.
**
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
// on Windows, we need Winsock2 for network functions
#include <winsock2.h>
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dcmtk/dcmnet/dcmtrans.h"
#include "dcmtk/dcmnet/diutil.h"

OFBool
DUL_dataWaiting(DUL_ASSOCIATIONKEY * callerAssociation, int timeout)
{
    PRIVATE_ASSOCIATIONKEY * association = (PRIVATE_ASSOCIATIONKEY *)callerAssociation;
    if ((association==NULL)||(association->connection == NULL)) return OFFalse;
    return association->connection->networkDataAvailable(timeout);
}

DcmTransportConnection *DUL_getTransportConnection(DUL_ASSOCIATIONKEY * callerAssociation)
{
  if (callerAssociation == NULL) return NULL;
  else return ((PRIVATE_ASSOCIATIONKEY *)callerAssociation)->connection;
}

DcmNativeSocketType DUL_networkSocket(DUL_NETWORKKEY * callerNet)
{
    if (callerNet == NULL) return DCMNET_INVALID_SOCKET;
    PRIVATE_NETWORKKEY *net = (PRIVATE_NETWORKKEY*)callerNet;
    return net->networkSpecific.TCP.listenSocket;
}

OFBool
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout)
{
    PRIVATE_NETWORKKEY *net;
    DcmNativeSocketType s;
    OFBool              assocWaiting = OFFalse;
    struct timeval      t;
    fd_set              fdset;
    int                 nfound;

    if (callerNet == NULL)
        return OFFalse;

    net = (PRIVATE_NETWORKKEY*)callerNet;

    s = net->networkSpecific.TCP.listenSocket;

    FD_ZERO(&fdset);
    FD_SET(s, &fdset);

    t.tv_sec = timeout;
    t.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
    nfound = select(OFstatic_cast(int, s + 1), (int *)(&fdset), NULL, NULL, &t);
#else
    // This is safe because on Windows the first select() parameter is ignored anyway
    nfound = select(OFstatic_cast(int, s + 1), &fdset, NULL, NULL, &t);
#endif
    if (DCM_dcmnetLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        DU_logSelectResult(nfound);
    }
    if (nfound <= 0) assocWaiting = OFFalse;
    else
    {
        if (FD_ISSET(s, &fdset))
            assocWaiting = OFTrue;
        else                /* This one should not really happen */
            assocWaiting = OFFalse;
    }

    return assocWaiting;
}
