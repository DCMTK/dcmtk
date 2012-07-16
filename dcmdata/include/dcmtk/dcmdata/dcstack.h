/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Purpose: stack class
 *
 */

#ifndef DCSTACK_H
#define DCSTACK_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dcdefine.h"

class DcmObject;    // forward declaration


/** helper class used by DcmStack.  Maintains a single stack entry.
 */
class DCMTK_DCMDATA_EXPORT DcmStackNode
{
public:
    /** constructor.
     *  @param obj object pointer for this stack entry
     */
    DcmStackNode(DcmObject *obj);

    /// destructor, non virtual.
    ~DcmStackNode();

    /** return pointer maintained by this entry
     *  @return object pointer
     */
    DcmObject *value() const;

private:
    /// private undefined copy constructor
    DcmStackNode(const DcmStackNode &);

    /// private undefined copy assignment operator
    DcmStackNode &operator=(const DcmStackNode &);

    /// pointer to next stack entry, NULL if last one
    DcmStackNode *link;

    /// pointer to object referenced by this stack entry
    DcmObject *objNodeValue;

    /// class DcmStack directly manipulates the pointer chain
    friend class DcmStack;
};


/** this class manages a stack of pointers to DcmObject instances.
 *  The objects pointed to are never touched, e.g. deleted.
 */
class DCMTK_DCMDATA_EXPORT DcmStack
{
public:
    /// default constructor, creates empty stack
    DcmStack();

    /** copy constructor. Only pointers to objects are copied,
     *  the DcmObject instances are not duplicated.
     *  @param arg stack to copy from
     */
    DcmStack(const DcmStack& arg);

    /// destructor, not virtual. Do not derive from this class.
    ~DcmStack();

    /** copy assignment operator.  Only pointers to objects are copied,
     *  the DcmObject instances are not duplicated.
     *  @param arg object to assign from
     *  @return reference to this object
     */
    DcmStack& operator=(const DcmStack &arg);

    /** comparison operator, needed for MSVC5.
     *  @param arg stack to compare to
     *  @return true if stacks are equal, false otherwise
     */
    OFBool operator==(const DcmStack& arg) const;

    /** dummy comparison operator, needed for MSVC5.
     *  @param arg stack to compare to
     *  @return true if the cardinality of this stack is smaller than the
     *     cardinality of arg, or if the cardinality is equal and pointer
     *     comparison, from the top to the bottom of the stack results
     *     in a smaller pointer for this stack.
     */     
    OFBool operator<(const DcmStack& arg) const;
    
    /** push new pointer do DcmObject instance on stack
     *  @param obj object pointer to push on stack
     *  @return pointer passed as obj
     */
    DcmObject* push( DcmObject *obj );

    /** removes uppermost entry from stack and returns it.
     *  @return uppermost stack entry
     */
    DcmObject* pop();

    /** returns uppermost entry of stack without removing it.
     *  @returns uppermost stack entry
     */
    DcmObject* top() const;

    /** returns n-th element from stack without removing it.
     *  @return n-th element from stack
     */
    DcmObject* elem(const unsigned long number) const;

    /** checks if the stack is empty
     *  @return true if stack is empty, false otherwise
     */
    OFBool empty() const;

    /** returns cardinality (number of entries) of the stack
     *  @return cardinality of stack
     */
    unsigned long card() const;

    /** returns the stack to default-constructed state, i.e. empty state.
     */
    void clear();

private:
    /// pointer to the upmost stack entry, NULL if empty
    DcmStackNode *topNode_;

    /// current cardinality of the stack
    unsigned long cardinality_;
};


#endif  // DCSTACK_H
