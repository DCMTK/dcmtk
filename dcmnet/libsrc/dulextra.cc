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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-06-20 07:35:50 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dulextra.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>

#include "dicom.h"
#include "lst.h"
#include "cond.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"

int
DUL_associationSocket(DUL_ASSOCIATIONKEY * callerAssociation)
{
    int s = -1;
    PRIVATE_ASSOCIATIONKEY *association;

    if (callerAssociation == NULL) return -1;

    association = (PRIVATE_ASSOCIATIONKEY *)callerAssociation;

    if (strcmp(association->networkType, DUL_NETWORK_TCP) == 0) {
	s = association->networkSpecific.TCP.socket;
    }

    return s;
}

BOOLEAN 
DUL_dataWaiting(DUL_ASSOCIATIONKEY * callerAssociation, int timeout)
{
    PRIVATE_ASSOCIATIONKEY *association;
    int                 s;
    BOOLEAN             dataWaiting = FALSE;
    struct timeval      t;
    fd_set              fdset;
    int                 nfound;

    if (callerAssociation == NULL)
	return FALSE;

    association = (PRIVATE_ASSOCIATIONKEY *)callerAssociation;

    if (strcmp(association->networkType, DUL_NETWORK_TCP) == 0) {
	s = association->networkSpecific.TCP.socket;

	FD_ZERO(&fdset);
	FD_SET(s, &fdset);
	t.tv_sec = timeout;
	t.tv_usec = 0;
	nfound = select(s + 1, &fdset, NULL, NULL, &t);
	if (nfound <= 0)
	    dataWaiting = FALSE;
	else {
	    if (FD_ISSET(s, &fdset))
		dataWaiting = TRUE;
	    else		/* This one should not really happen */
		dataWaiting = FALSE;
	}

    } else {
	dataWaiting = FALSE;
    }
    return dataWaiting;
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

BOOLEAN 
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout)
{
    PRIVATE_NETWORKKEY *net;
    int                 s;
    BOOLEAN             assocWaiting = FALSE;
    struct timeval      t;
    fd_set              fdset;
    int                 nfound;

    if (callerNet == NULL)
	return FALSE;

    net = (PRIVATE_NETWORKKEY*)callerNet;

    if (strcmp(net->networkType, DUL_NETWORK_TCP) == 0) {
	s = net->networkSpecific.TCP.listenSocket;

	FD_ZERO(&fdset);
	FD_SET(s, &fdset);
	t.tv_sec = timeout;
	t.tv_usec = 0;
	nfound = select(s + 1, &fdset, NULL, NULL, &t);
	if (nfound <= 0)
	    assocWaiting = FALSE;
	else {
	    if (FD_ISSET(s, &fdset))
		assocWaiting = TRUE;
	    else		/* This one should not really happen */
		assocWaiting = FALSE;
	}

    } else {
	assocWaiting = FALSE;
    }
    return assocWaiting;
}

/*
** CVS Log
** $Log: dulextra.cc,v $
** Revision 1.4  1996-06-20 07:35:50  hewett
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
