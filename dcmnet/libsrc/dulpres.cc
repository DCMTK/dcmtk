/*
          Copyright (C) 1993, 1994, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993, 1994 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993, 1994 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/
/* Copyright marker.  Copyright will be inserted above.  Do not remove */
/*
** @$=@$=@$=
*/
/*
**				DICOM 93
**		     Electronic Radiology Laboratory
**		   Mallinckrodt Institute of Radiology
**		Washington University School of Medicine
**
** Module Name(s):	DUL_MakePresentationCtx
** Author, Date:	Stephen M. Moore, 15-Apr-93
** Intent:		This module contains routines for the user to
**			build and manipulate the public DUL structures.
** Last Update:		$Author: hewett $, $Date: 1996-04-25 16:11:21 $
** Source File:		$RCSfile: dulpres.cc,v $
** Revision:		$Revision: 1.2 $
** Status:		$State: Exp $
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <signal.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "dicom.h"
#include "cond.h"
#include "lst.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"


/* DUL_MakePresentationCtx
**
** Purpose:
**	Build a Presentation Context from the specified parameters
**
** Parameter Dictionary:
**	ctx		Pointer to the presentation context that is to
**			be built.
**	proposedSCRole	Proposed role played by the caller
**	acceptedSCRole	Accepted role (after negotiation)
**	ctxID		Unique ID for this presentation context
**	result
**	abstarctSyntax
**
** Return Values:
**	DUL_NORMAL
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**
**
** Notes:
**	The transfer syntax argument allows the caller to specify one
**	or more transfer syntaxes.  The function expects the caller to
**	terminate the set of transfer syntaxes with a NULL pointer.
**
**	Transfer syntaxes of 0 length are not considered an error and/but
**	are ignored.
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
DUL_MakePresentationCtx(DUL_PRESENTATIONCONTEXT ** ctx,
		     DUL_SC_ROLE proposedSCRole, DUL_SC_ROLE acceptedSCRole,
		      DUL_PRESENTATIONCONTEXTID ctxID, unsigned char result,
			char *abstractSyntax, char *transferSyntax,...)
{
    va_list
	args;
    LST_HEAD
	* lst;
    DUL_TRANSFERSYNTAX
	* transfer;
#ifdef lint
    char __builtin_va_alist;
#endif

    *ctx = (DUL_PRESENTATIONCONTEXT *) malloc(sizeof(**ctx));
    if (*ctx == NULL)
	return COND_PushCondition(DUL_MALLOCERROR, DUL_Message(DUL_MALLOCERROR),
				  "DUL_MakePresentationCtx", sizeof(**ctx));

    (void) memset(*ctx, 0, sizeof(**ctx));
    lst = LST_Create();
    if (lst == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
	      DUL_Message(DUL_LISTCREATEFAILED), "DUL_MakePresentationCtx");

    (*ctx)->presentationContextID = ctxID;
    (*ctx)->result = result;
    (*ctx)->proposedSCRole = proposedSCRole;
    (*ctx)->acceptedSCRole = acceptedSCRole;
    strcpy((*ctx)->abstractSyntax, abstractSyntax);

    va_start(args, transferSyntax);
    strcpy((*ctx)->acceptedTransferSyntax, transferSyntax);
    while ((transferSyntax = va_arg(args, char *)) != NULL) {
	if (strlen(transferSyntax) != 0) {
	    transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(*transfer));
	    if (transfer == NULL)
		return COND_PushCondition(DUL_MALLOCERROR, DUL_Message(DUL_MALLOCERROR),
			      "DUL_MakePresentationCtx", sizeof(*transfer));
	    strcpy(transfer->transferSyntax, transferSyntax);
	    if (LST_Enqueue(&lst, (LST_NODE*)transfer) != LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR, DUL_Message(DUL_LISTERROR),
					  "DUL_MakePresentationCtx");
	}
    }
    va_end(args);
    (*ctx)->proposedTransferSyntax = lst;
    return DUL_NORMAL;
}

/*
** CVS Log
** $Log: dulpres.cc,v $
** Revision 1.2  1996-04-25 16:11:21  hewett
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
