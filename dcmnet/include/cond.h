/*
          Copyright (C) 1993, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993 DICOM Central Test Node project for, and
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
/*		condition.h
**
**  This file contains functions implementing a simple error facility.
**
**  MODULE DESCRIPTION:
**
**      COND - facility to handle condition generation, reporting
**	The routines included in this module are as follows:
**	    CONDITION COND_PushCondition(code, controlstring, var args)
**	    CONDITION COND_ExtractConditions(int *callback(), char *buff,
**		int bufflength)
**	    CONDITION COND_TopCondition(int *code, int *text, int maxlength)
**	    CONDITION COND_PopCondition(int clearstack)
**		void COND_DumpConditions(void)
**
**
**
**
**  AUTHOR:
**
**      Stephen M. Moore (smm@wuerl.wustl.edu)
**	Electronic Radiology Laboratory
**	Mallinckrodt Institute of Radiology
**	Washington University School of Medicine
**
**  CREATION DATE:  26-May-1992
**
**
**       
*/

#ifndef COND_IS_IN
#define COND_IS_IN 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dicom.h"

typedef BOOLEAN (*CALLBACK_TYPE)();

CONDITION COND_PushCondition(CONDITION cond, char *controlString, ...);
CONDITION COND_ExtractConditions(CALLBACK_TYPE callback) ;
CONDITION COND_TopCondition(CONDITION *condition, char *text,
	unsigned long maxlength);
CONDITION COND_PopCondition(BOOLEAN clearstack);
CONDITION COND_EstablishError(CALLBACK_TYPE callback);
void COND_DumpConditions(void);
CONDITION COND_ExtractConditions(BOOLEAN (*callback)(CONDITION cond, char *str));
CONDITION COND_EstablishCallback(void (* callback)(CONDITION cond, char *str));

/*  Now define the fixed values for conditions returned by this
**  package.  Note that FAC_COND is used to generate these
**  conditions.  This should be defined in some global include
**  file so that we can keep all of the facilities straight.
*/

#define	COND_NORMAL	/* Successful return */ \
	FORM_COND(FAC_COND, SEV_SUCC, 1)

#endif
