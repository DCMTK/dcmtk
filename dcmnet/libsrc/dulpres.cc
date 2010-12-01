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

/*
**          DICOM 93
**        Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**    Washington University School of Medicine
**
** Module Name(s):  DUL_MakePresentationCtx
** Author, Date:    Stephen M. Moore, 15-Apr-93
** Intent:          This module contains routines for the user to
**                  build and manipulate the public DUL structures.
** Last Update:     $Author: joergr $, $Date: 2010-12-01 08:26:37 $
** Revision:        $Revision: 1.14 $
** Status:          $State: Exp $
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_CSIGNAL
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/dul.h"
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
**	DUL_LISTERROR
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

OFCondition
DUL_MakePresentationCtx(DUL_PRESENTATIONCONTEXT ** ctx,
		     DUL_SC_ROLE proposedSCRole, DUL_SC_ROLE acceptedSCRole,
		      DUL_PRESENTATIONCONTEXTID ctxID, unsigned char result,
			const char *abstractSyntax, const char *transferSyntax,...)
{
    va_list
	args;
    LST_HEAD
	* lst;
    DUL_TRANSFERSYNTAX
	* transfer;

    *ctx = (DUL_PRESENTATIONCONTEXT *) malloc(sizeof(**ctx));
    if (*ctx == NULL) return EC_MemoryExhausted;

    (void) memset(*ctx, 0, sizeof(**ctx));
    lst = LST_Create();
    if (lst == NULL) return EC_MemoryExhausted;

    (*ctx)->presentationContextID = ctxID;
    (*ctx)->result = result;
    (*ctx)->proposedSCRole = proposedSCRole;
    (*ctx)->acceptedSCRole = acceptedSCRole;
    strcpy((*ctx)->abstractSyntax, abstractSyntax);

    va_start(args, transferSyntax);
    strcpy((*ctx)->acceptedTransferSyntax, transferSyntax);
    while ((transferSyntax = va_arg(args, char *)) != NULL)
    {
	if (strlen(transferSyntax) != 0)
	{
	    transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
	    if (transfer == NULL) return EC_MemoryExhausted;
	    strcpy(transfer->transferSyntax, transferSyntax);
	    OFCondition cond = LST_Enqueue(&lst, (LST_NODE*)transfer);
	    if (cond.bad()) return cond;
	}
    }
    va_end(args);
    (*ctx)->proposedTransferSyntax = lst;
    return EC_Normal;
}

/*
** CVS Log
** $Log: dulpres.cc,v $
** Revision 1.14  2010-12-01 08:26:37  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.13  2010-10-14 13:14:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.12  2005/12/08 15:44:53  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.11  2002/11/27 13:04:46  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.10  2002/11/26 15:37:02  meichel
** Changed DUL code to always send A-ASSOCIATE user information sub-items
**   in ascending order.
**
** Revision 1.9  2001/10/12 10:18:40  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.8  2000/02/23 15:12:51  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/01 10:24:14  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.6  1999/03/29 11:20:08  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.5  1997/07/04 09:24:57  meichel
** Simplified some sizeof() constructs to avoid compiler warnings
**   on the IBM xlC compiler (AIX 3.x).
**
** Revision 1.4  1996/06/20 07:35:52  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.3  1996/05/03 10:31:56  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.2  1996/04/25 16:11:21  hewett
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
