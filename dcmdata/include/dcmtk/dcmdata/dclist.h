/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: generic list class
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCLIST_H
#define DCLIST_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/oftypes.h"

#define INCLUDE_CSTDDEF
#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dcobject.h"

/// index indicating "end of list"
const unsigned long DCM_EndOfListIndex = OFstatic_cast(unsigned long, -1L);

/** helper class maintaining an entry in a DcmList double-linked list
 */
class DcmListNode 
{

public:
    /** constructor
     *  @param obj object to be maintained by this list node
     */
    DcmListNode( DcmObject *obj );

    /// destructor
    ~DcmListNode();

    /// return pointer to object maintained by this list node
    inline DcmObject *value() { return objNodeValue; } 

private:
    friend class DcmList;

    /// pointer to next node in double-linked list
    DcmListNode *nextNode;

    /// pointer to previous node in double-linked list
    DcmListNode *prevNode;

    /// pointer to DcmObject instance maintained by this list entry
    DcmObject *objNodeValue;

    /// private undefined copy constructor 
    DcmListNode(const DcmListNode &);

    /// private undefined copy assignment operator 
    DcmListNode &operator=(const DcmListNode &);

};

/// list position indicator
typedef enum
{
    /// at current position in list
    ELP_atpos,

    /// at list start
    ELP_first,

    /// at list end
    ELP_last,

    /// before current list position
    ELP_prev,

    /// after current list position
    ELP_next
} E_ListPos;

/** double-linked list class that maintains pointers to DcmObject instances.
 *  The remove operation does not delete the object pointed to, however,
 *  the destructor will delete all elements pointed to
 */
class DcmList 
{
public:
    /// constructor
    DcmList();

    /// destructor
    ~DcmList();

    /** insert object at end of list
     *  @param obj pointer to object
     *  @return pointer to object
     */
    DcmObject *append(  DcmObject *obj );

    /** insert object at start of list
     *  @param obj pointer to object
     *  @return pointer to object
     */
    DcmObject *prepend( DcmObject *obj );

    /** insert object relative to current position and indicator
     *  @param obj pointer to object
     *  @param pos position indicator
     *  @return pointer to object
     */
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );

    /** remove current entry from list, return element
     *  @return pointer to removed element, which is not deleted
     */
    DcmObject *remove();

    /** get pointer to element in list at given position
     *  @param pos position indicator
     *  @return pointer to object
     */
    DcmObject *get(     E_ListPos pos = ELP_atpos );

    /** seek within element in list to given position
     *  (i.e. set current element to given position)
     *  @param pos position indicator
     *  @return pointer to new current object
     */
    DcmObject *seek(    E_ListPos pos = ELP_next );

    /** seek within element in list to given element index
     *  (i.e. set current element to given index)
     *  @param absolute_position position index < card()
     *  @return pointer to new current object
     */
    DcmObject *seek_to(unsigned long absolute_position);

    /** Remove and delete all elements from list. Thus, the 
     *  elements' memory is also freed by this operation. The list
     *  is empty after calling this function.
     */  
    void deleteAllElements();

    /// return cardinality of list
    inline unsigned long card() const { return cardinality; }

    /// return true if list is empty, false otherwise
    inline OFBool empty(void) const { return firstNode == NULL; }

    /// return true if current node exists, false otherwise
    inline OFBool valid(void) const { return currentNode != NULL; }

private:
    /// pointer to first node in list
    DcmListNode *firstNode;

    /// pointer to last node in list
    DcmListNode *lastNode;

    /// pointer to current node in list
    DcmListNode *currentNode;

    /// number of elements in list
    unsigned long cardinality;
 
    /// private undefined copy constructor 
    DcmList &operator=(const DcmList &);

    /// private undefined copy assignment operator 
    DcmList(const DcmList &newList);
};

#endif  // DCLIST_H


/*
 * CVS/RCS Log:
 * $Log: dclist.h,v $
 * Revision 1.22  2010-10-14 13:15:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.21  2010-03-24 11:52:55  onken
 * Introduced new function to delete all elements (including memory de-allocation)
 * from DcmList.
 *
 * Revision 1.20  2010-03-01 09:08:44  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.19  2010-02-22 11:39:53  uli
 * Remove some unneeded includes.
 *
 * Revision 1.18  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.17  2007-11-29 14:30:35  meichel
 * Updated doxygen API documentation
 *
 * Revision 1.16  2005/12/08 16:28:20  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.15  2003/08/08 13:32:45  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.14  2003/08/08 12:30:38  joergr
 * Made DcmListNode::value() inline.
 * Renamed member variable "actualNode" to "currentNode".
 *
 * Revision 1.13  2002/11/27 12:07:22  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.12  2001/06/01 15:48:41  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/03/08 16:26:15  meichel
 * Updated copyright header.
 *
 * Revision 1.10  2000/02/23 15:11:38  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.9  2000/02/01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.8  1999/03/31 09:24:41  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
