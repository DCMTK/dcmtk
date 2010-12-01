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
** Author, Date:  Stephen M. Moore, 11-May-92
** Intent:        This module defines structures and constants needed
**                to implement the DICOM Upper Layer state machine.
** Last Update:   $Author: joergr $, $Date: 2010-12-01 08:26:36 $
** Revision:      $Revision: 1.6 $
** Status:        $State: Exp $
*/

#ifndef DULFSM_H
#define DULFSM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define	A_ASSOCIATE_REQ_LOCAL_USER	0
#define	TRANS_CONN_CONFIRM_LOCAL_USER	1
#define	A_ASSOCIATE_AC_PDU_RCV		2
#define	A_ASSOCIATE_RJ_PDU_RCV		3
#define	TRANS_CONN_INDICATION		4
#define	A_ASSOCIATE_RQ_PDU_RCV		5
#define	A_ASSOCIATE_RESPONSE_ACCEPT	6
#define	A_ASSOCIATE_RESPONSE_REJECT	7
#define	P_DATA_REQ			8
#define	P_DATA_TF_PDU_RCV		9
#define	A_RELEASE_REQ			10
#define	A_RELEASE_RQ_PDU_RCV		11
#define	A_RELEASE_RP_PDU_RCV		12
#define	A_RELEASE_RESP			13
#define	A_ABORT_REQ			14
#define	A_ABORT_PDU_RCV			15
#define	TRANS_CONN_CLOSED		16
#define	ARTIM_TIMER_EXPIRED		17
#define	INVALID_PDU 			18
#define DUL_NUMBER_OF_EVENTS		19

#define	NOSTATE		-1
#define	STATE1		1
#define	STATE2		2
#define	STATE3		3
#define	STATE4		4
#define	STATE5		5
#define	STATE6		6
#define	STATE7		7
#define	STATE8		8
#define	STATE9		9
#define	STATE10		10
#define	STATE11		11
#define	STATE12		12
#define	STATE13		13
#define	DUL_NUMBER_OF_STATES		13

typedef enum {
    AE_1, AE_2, AE_3, AE_4, AE_5, AE_6, AE_7, AE_8,
    DT_1, DT_2,
    AA_1, AA_2, AA_2T, AA_3, AA_4, AA_5, AA_6, AA_7, AA_8,
    AR_1, AR_2, AR_3, AR_4, AR_5, AR_6, AR_7, AR_8, AR_9, AR_10,
    NOACTION
}   DUL_FSM_ACTION;

typedef struct {
    int event;
    const char *eventName;
}   FSM_Event_Description;

typedef struct {
    DUL_FSM_ACTION action;
    OFCondition (*actionFunction)(PRIVATE_NETWORKKEY **network,
				PRIVATE_ASSOCIATIONKEY **association,
				int nextState, void *params);
    char actionName[64];
}   FSM_FUNCTION;

typedef struct {
    int event;
    int state;
    DUL_FSM_ACTION action;
    int nextState;
    char eventName[64];
    char actionName[64];
    OFCondition (*actionFunction)(PRIVATE_NETWORKKEY **network,
				PRIVATE_ASSOCIATIONKEY **association,
				int nextState, void *params);

}   FSM_ENTRY;

#endif

/*
** CVS Log
** $Log: dulfsm.h,v $
** Revision 1.6  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.5  2010-10-14 13:19:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.4  2005/12/08 15:48:09  meichel
** Updated Makefiles to correctly install header files
**
** Revision 1.3  2001/10/12 10:18:39  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.2  1999/03/29 11:20:07  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/

