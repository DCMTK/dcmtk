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
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/dulstruc.h"
#include "dulpriv.h"
#include "dulfsm.h"


/* DUL_MakePresentationCtx
**
** Purpose:
**  Build a Presentation Context from the specified parameters
**
** Parameter Dictionary:
**  ctx     Pointer to the presentation context that is to
**          be built.
**  proposedSCRole  Proposed role played by the caller
**  acceptedSCRole  Accepted role (after negotiation)
**  ctxID       Unique ID for this presentation context
**  result
**  abstarctSyntax
**
** Return Values:
**  DUL_LISTERROR
**
**
** Notes:
**  The transfer syntax argument allows the caller to specify one
**  or more transfer syntaxes.  The function expects the caller to
**  terminate the set of transfer syntaxes with a NULL pointer.
**
**  Transfer syntaxes of 0 length are not considered an error and/but
**  are ignored.
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
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
    OFStandard::strlcpy((*ctx)->abstractSyntax, abstractSyntax, sizeof((*ctx)->abstractSyntax));

    va_start(args, transferSyntax);
    OFStandard::strlcpy((*ctx)->acceptedTransferSyntax, transferSyntax, sizeof((*ctx)->acceptedTransferSyntax));
    while ((transferSyntax = va_arg(args, char *)) != NULL)
    {
    if (strlen(transferSyntax) != 0)
    {
        transfer = (DUL_TRANSFERSYNTAX*)malloc(sizeof(DUL_TRANSFERSYNTAX));
        if (transfer == NULL) return EC_MemoryExhausted;
        OFStandard::strlcpy(transfer->transferSyntax, transferSyntax, sizeof(transfer->transferSyntax));
        LST_Enqueue(&lst, (LST_NODE*)transfer);
    }
    }
    va_end(args);
    (*ctx)->proposedTransferSyntax = lst;
    return EC_Normal;
}
