/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: generic list class
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-02-01 10:12:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dclist.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCLIST_H
#define DCLIST_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stddef.h>

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include "dcerror.h"
#include "dctypes.h"


const unsigned long DCM_EndOfListIndex = (unsigned long)(-1L);


class DcmObject;    // forward declaration


class DcmListNode {
    friend class DcmList;
    DcmListNode *nextNode;
    DcmListNode *prevNode;
    DcmObject *objNodeValue;

 // --- declarations to avoid compiler warnings
 
    DcmListNode(const DcmListNode &);
    DcmListNode &operator=(const DcmListNode &);

public:
    DcmListNode( DcmObject *obj );
    ~DcmListNode();
    DcmObject *value();
};


typedef enum
{
    ELP_atpos,
    ELP_first,
    ELP_last,
    ELP_prev,
    ELP_next
} E_ListPos;

/* this class only manages pointers to elements.
 * remove() does not delete the element pointed to.
 * Upon destruction of the list, all elements pointed to are also deleted.
 */

class DcmList {
    DcmListNode *firstNode;
    DcmListNode *lastNode;
    DcmListNode *actualNode;
    unsigned long cardinality;

 // --- declarations to avoid compiler warnings
 
    DcmList &operator=(const DcmList &);
    DcmList(const DcmList &newList);

public:
    DcmList();
    ~DcmList();

    DcmObject *append(  DcmObject *obj );
    DcmObject *prepend( DcmObject *obj );
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );
    DcmObject *remove();
    DcmObject *get(     E_ListPos pos = ELP_atpos );
    DcmObject *seek(    E_ListPos pos = ELP_next );
    DcmObject *seek_to(unsigned long absolute_position);
    inline unsigned long card() const { return cardinality; }
    inline OFBool empty(void) const { return firstNode == NULL; }
    inline OFBool valid(void) const { return actualNode != NULL; }
};

#endif  // DCLIST_H

/*
 * CVS/RCS Log:
 * $Log: dclist.h,v $
 * Revision 1.9  2000-02-01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.8  1999/03/31 09:24:41  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
