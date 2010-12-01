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
** Author: Andrew Hewett                Created: 03-08-93
**
** Module: dulextra
**
** Purpose:
**      Supplementary DUL functions.
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:36 $
** CVS/RCS Revision:    $Revision: 1.17 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

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

int
DUL_networkSocket(DUL_NETWORKKEY * callerNet)
{
    if (callerNet == NULL) return -1;
    PRIVATE_NETWORKKEY *net = (PRIVATE_NETWORKKEY*)callerNet;
    return net->networkSpecific.TCP.listenSocket;
}

OFBool
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout)
{
    PRIVATE_NETWORKKEY *net;
    int                 s;
    OFBool             assocWaiting = OFFalse;
    struct timeval      t;
    fd_set              fdset;
    int                 nfound;

    if (callerNet == NULL)
        return OFFalse;

    net = (PRIVATE_NETWORKKEY*)callerNet;

    s = net->networkSpecific.TCP.listenSocket;

    FD_ZERO(&fdset);
#ifdef __MINGW32__
    // on MinGW, FD_SET expects an unsigned first argument
    FD_SET((unsigned int) s, &fdset);
#else
    FD_SET(s, &fdset);
#endif

    t.tv_sec = timeout;
    t.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
    nfound = select(s + 1, (int *)(&fdset), NULL, NULL, &t);
#else
    nfound = select(s + 1, &fdset, NULL, NULL, &t);
#endif
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

/*
** CVS Log
** $Log: dulextra.cc,v $
** Revision 1.17  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.16  2010-10-14 13:14:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.15  2005/12/08 15:44:50  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.14  2003/07/03 14:21:10  meichel
** Added special handling for FD_SET() on MinGW, which expects an
**   unsigned first argument.
**
** Revision 1.13  2002/11/27 13:04:44  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.12  2001/10/12 10:18:38  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.11  2000/08/10 14:50:57  meichel
** Added initial OpenSSL support.
**
** Revision 1.10  2000/02/23 15:12:46  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.9  2000/02/01 10:24:13  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.8  1999/01/07 14:25:03  meichel
** Changed sequence of include files in some dcmnet modules
**   to keep the Unixware compiler happy.
**
** Revision 1.7  1997/07/21 08:47:22  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/07/04 11:44:34  meichel
** Configure now also tests <sys/select.h> if available
**   when searching for a select() prototype.
**   Updated files using select() to include <sys/select.h> and
**   <sys/types.h> if available (needed for AIX).
**
** Revision 1.5  1996/12/03 15:29:48  meichel
** Added support for HP-UX 9.05 systems using GCC 2.7.2.1
**
** Revision 1.4  1996/06/20 07:35:50  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.3  1996/05/03 10:31:54  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.2  1996/04/25 16:11:19  hewett
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
