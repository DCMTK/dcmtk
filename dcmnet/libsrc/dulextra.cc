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
** Author: Andrew Hewett		Created: 03-08-93
** 
** Module: dulextra
**
** Purpose: 
**	Supplementary DUL functions.
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2000-08-10 14:50:57 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dulextra.cc,v $
** CVS/RCS Revision:	$Revision: 1.11 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <errno.h>

#include "dicom.h"
#include "lst.h"
#include "cond.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dcmtrans.h"

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
    PRIVATE_NETWORKKEY *net;
    int                 s = -1;

    if (callerNet == NULL)
	return -1;

    net = (PRIVATE_NETWORKKEY*)callerNet;

    if (strcmp(net->networkType, DUL_NETWORK_TCP) == 0) {
	s = net->networkSpecific.TCP.listenSocket;
    }
    return s;
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

    if (strcmp(net->networkType, DUL_NETWORK_TCP) == 0) {
	s = net->networkSpecific.TCP.listenSocket;

	FD_ZERO(&fdset);
	FD_SET(s, &fdset);
	t.tv_sec = timeout;
	t.tv_usec = 0;
#ifdef HAVE_INTP_SELECT
	nfound = select(s + 1, (int *)(&fdset), NULL, NULL, &t);
#else
	nfound = select(s + 1, &fdset, NULL, NULL, &t);
#endif
	if (nfound <= 0)
	    assocWaiting = OFFalse;
	else {
	    if (FD_ISSET(s, &fdset))
		assocWaiting = OFTrue;
	    else		/* This one should not really happen */
		assocWaiting = OFFalse;
	}

    } else {
	assocWaiting = OFFalse;
    }
    return assocWaiting;
}

/*
** CVS Log
** $Log: dulextra.cc,v $
** Revision 1.11  2000-08-10 14:50:57  meichel
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
