/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 */

#ifndef DCLIST_H
#define DCLIST_H

#include "dcmtk/dcmdata/dcobject.h"

#include <deque>

/// index indicating "end of list"
const unsigned long DCM_EndOfListIndex = OFstatic_cast(unsigned long, -1L);

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
class DCMTK_DCMDATA_EXPORT DcmList 
{
public:
    /// constructor
    DcmList();

    /// destructor
    ~DcmList();

    /** insert object at end of list and make it be the "current" one
     *  @param obj pointer to object
     *  @return pointer to object
     */
    DcmObject *append(  DcmObject *obj );

    /** insert object at start of list and make it be the "current" one
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
    inline unsigned long card() const { return static_cast<unsigned long>(objects.size()); }

    /// return true if list is empty, false otherwise
    inline OFBool empty(void) const { return objects.empty(); }

    /// return true if current node exists, false otherwise
    inline OFBool valid(void) const { return current != objects.end(); }

private:
    std::deque<DcmObject*> objects;

    std::deque<DcmObject*>::iterator current;

    /// private undefined copy constructor 
    DcmList &operator=(const DcmList &);

    /** private undefined copy assignment operator
     * @param newList documented to avoid doxygen warnings
     */
    DcmList(const DcmList &newList);
};

#endif  // DCLIST_H
