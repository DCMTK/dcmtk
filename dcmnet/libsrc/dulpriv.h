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
** Module Name(s):
** Author, Date:  Stephen M. Moore, xx-May-92
** Intent:
**  This header contains private typedefs for the DICOM Upper Layer
**  (DUL) protocol package.  This is to be used to compile the DUL
**  package and is not intended to be seen by the applications programmer.
**  Files at the package level should include this file to get the
**  proper typedefs and include the public file "dulprotocol.h" to
**  get the public definitions and function prototypes.  I have omitted
**  the public definitions and prototypes on purpose so that they
**  exist in only one location.
**
** Last Update:   $Author: joergr $, $Date: 2010-12-01 08:26:37 $
** Revision:      $Revision: 1.11 $
** Status:        $State: Exp $
*/

#ifndef DULPRIVATE_H
#define DULPRIVATE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */


#define	PRV_DEFAULTTIMEOUT	-1
#define	PRV_LISTENBACKLOG	50

OFCondition DUL_InitializeFSM(void);
OFCondition
PRV_StateMachine(PRIVATE_NETWORKKEY ** network,
		 PRIVATE_ASSOCIATIONKEY ** association, int event, int state,
		 void *params);

OFCondition
constructAssociatePDU(DUL_ASSOCIATESERVICEPARAMETERS * items,
		      unsigned char type, PRV_ASSOCIATEPDU * pdu);
OFCondition
constructAssociateRejectPDU(unsigned char result,
			    unsigned char source, unsigned char reason,
			    DUL_REJECTRELEASEABORTPDU * pdu);
OFCondition constructReleaseRQPDU(DUL_REJECTRELEASEABORTPDU * pdu, unsigned long mode);
OFCondition constructReleaseRPPDU(DUL_REJECTRELEASEABORTPDU * pdu);
OFCondition
constructAbortPDU(unsigned char src, unsigned char reason,
		  DUL_REJECTRELEASEABORTPDU * pdu, unsigned long mode);
OFCondition
constructDataPDU(void *buf, unsigned long length, DUL_DATAPDV type,
       DUL_PRESENTATIONCONTEXTID ctxID, OFBool last, DUL_DATAPDU * pdu);
OFCondition
streamAssociatePDU(PRV_ASSOCIATEPDU * assoc, unsigned char *b,
		   unsigned long maxLength, unsigned long *rtnLen);
OFCondition
streamRejectReleaseAbortPDU(DUL_REJECTRELEASEABORTPDU * pdu,
	  unsigned char *b, unsigned long maxLength, unsigned long *rtnLen);
OFCondition
streamDataPDUHead(DUL_DATAPDU * pdu, unsigned char *buf,
		  unsigned long maxLength, unsigned long *rtnLen);
OFCondition
parseAssociate(unsigned char *buf, unsigned long len,
	       PRV_ASSOCIATEPDU * pdu);
OFCondition
PRV_NextPDUType(PRIVATE_ASSOCIATIONKEY ** association,
		DUL_BLOCKOPTIONS block, int timeout, unsigned char *type);

#endif

/*
** CVS Log
** $Log: dulpriv.h,v $
** Revision 1.11  2010-12-01 08:26:37  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.10  2010-10-14 13:19:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.9  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.8  2005-12-08 15:48:09  meichel
** Updated Makefiles to correctly install header files
**
** Revision 1.7  2004/02/25 12:31:17  meichel
** Added global option flag for compatibility with very old DCMTK releases in the
**   DICOM upper layer and ACSE code. Default is automatic handling, which should
**   work in most cases.
**
** Revision 1.6  2001/10/12 10:18:41  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.5  2001/09/26 12:29:04  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.4  2000/03/03 14:11:26  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.3  1997/07/21 08:47:25  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.2  1997/01/13 15:53:13  hewett
** Added missing function prototypes (required for CodeWarrior 10).
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
