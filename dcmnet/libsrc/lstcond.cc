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
/*			lstcond
**
**  Define the ASCIZ messages that go with condition codes and provide
**  a function that returns a pointer to the message given a code.
**
**  Author:
**	Stephen Moore (smm@wuerl.wustl.edu)
**	Electronic Radiology Laboratory
**	Mallinckrodt Institute of Radiology
**	Washington University School of Medicine
**	26-May-1992
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>

#include "dicom.h"
#include "lstpriv.h"
#include "lst.h"

typedef struct vector {
  CONDITION	cond;
  const char	*message;
  } VECTOR;

static VECTOR messageVector[] = {
    { LST_NORMAL,		"Normal return from LST routine" },
    { LST_LISTNOTEMPTY,		"Destroy requested on nonempty list" },
    { LST_BADEND,		"Bad END requested in LST routine" },
    { LST_NOCURRENT,		"No current node in LST routine" }
  };

const char *LST_Message(CONDITION condition)
{
  int
    l_index;

    for (l_index = 0; messageVector[l_index].message != NULL; l_index++)
	if (condition == messageVector[l_index].cond)
	    return messageVector[l_index].message;

    return NULL;
}
