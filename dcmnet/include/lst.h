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
/*
 *	lst.h		-- function prototypes and constants for LST facility
 */

#ifndef LST_IS_IN
#define LST_IS_IN

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define LST_K_BEFORE		0x00000000
#define LST_K_AFTER		0xFFFFFFFF

#ifndef LST_KEYS
#ifdef __cplusplus
typedef struct lst_head LST_HEAD;
typedef struct lst_node LST_NODE;
#else
typedef void LST_HEAD;
typedef void LST_NODE;
#endif
#endif

typedef unsigned long LST_END;

LST_HEAD *LST_Create(void);
CONDITION LST_Destroy(LST_HEAD **list);
CONDITION LST_Enqueue(LST_HEAD **list, LST_NODE *node);
CONDITION LST_Push(LST_HEAD **list, LST_NODE *node);
LST_NODE *LST_Dequeue(LST_HEAD **list);
LST_NODE *LST_Pop(LST_HEAD **list);
unsigned long LST_Count(LST_HEAD **list);
LST_NODE *LST_Head(LST_HEAD **list);
LST_NODE *LST_Current(LST_HEAD **list);
LST_NODE *LST_Tail(LST_HEAD **list);
CONDITION LST_Insert(LST_HEAD **list, LST_NODE *node, LST_END where);
LST_NODE *LST_Remove(LST_HEAD **list, LST_END dir);
LST_NODE *LST_Next(LST_HEAD **list);
LST_NODE *LST_Previous(LST_HEAD **list);
LST_NODE *LST_Position(LST_HEAD **list, LST_NODE *node);
const char *LST_Message(CONDITION cond);

#define LST_Top(x) LST_Head((x))
#define LST_Front(x) LST_Head((x))

#define LST_NORMAL		/* Normal return from LST package */ \
	FORM_COND(FAC_LST, SEV_SUCC, 1)
#define LST_LISTNOTEMPTY	/* Attempt to destroy list with elements */ \
	FORM_COND(FAC_LST, SEV_ERROR, 3)
#define LST_BADEND		/* */ \
	FORM_COND(FAC_LST, SEV_ERROR, 5)
#define LST_NOCURRENT	/* */ \
	FORM_COND(FAC_LST, SEV_ERROR, 7)

#endif

/*
** CVS Log
** $Log: lst.h,v $
** Revision 1.2  1999-03-29 11:20:00  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
