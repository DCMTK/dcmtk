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
** Author: Andrew Hewett        Created: 03-06-93
**
** Module: asccond
**
** Purpose:
**  Condition messages for Association module
**
**  Module Prefix: ASC_
**
** Last Update:     $Author: joergr $
** Update Date:     $Date: 2001-09-28 13:36:33 $
** Source File:     $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/Attic/asccond.cc,v $
** CVS/RCS Revision:    $Revision: 1.4 $
** Status:      $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "assoc.h"
#include "asccond.h"


typedef struct vector {
    CONDITION cond;
    const char *message;
} VECTOR;

static VECTOR messageVector[] = {
                 {ASC_NORMAL, "Normal return from ASC routine"},
                 {ASC_NETWORKERROR, "ASC Network layer error"},
                 {ASC_NULLKEY, "ASC Caller passed in a NULL key"},
                 {ASC_ILLEGALKEY, "ASC Caller passed in an illegal key"},
                 {ASC_MALLOCERROR, "ASC Failed to malloc a buffer"},
                 {ASC_CODINGERROR, "ASC Coding error in: %s"},
                 {ASC_BADPRESENTATIONCONTEXTID, "ASC Bad presentation context ID: %d"},
                 {ASC_DUPLICATEPRESENTATIONCONTEXTID, "ASC Duplicate presentation context ID: %d"},
                 {ASC_BADPRESENTATIONCONTEXTPOSITION, "ASC Bad presentation context position: %d"},
                 {ASC_MISSINGTRANSFERSYNTAX, "ASC Missing transfer syntax: %s"},
                 {ASC_ASSOCIATIONREJECTED, "ASC Association rejected"},
                 {ASC_NORMAL, NULL}
};

const char *
ASC_Message(CONDITION condition)
{
    int l_index;

    for (l_index = 0; messageVector[l_index].message != NULL; l_index++)
        if (condition == messageVector[l_index].cond)
            return messageVector[l_index].message;

    return NULL;
}

/*
** CVS Log
** $Log: asccond.cc,v $
** Revision 1.4  2001-09-28 13:36:33  joergr
** Removed "#include <stdio.h>" to keep gcc 3.0 quiet.
**
** Revision 1.3  1999/03/29 11:20:00  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.2  1998/06/29 12:14:30  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
