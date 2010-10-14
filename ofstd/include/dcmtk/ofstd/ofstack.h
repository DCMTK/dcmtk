/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *              
 *  Purpose:
 *      Defines a template stack class with interfaces similar to the C++ Standard
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFSTACK_H
#define OFSTACK_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

#if defined(HAVE_STL) || defined(HAVE_STL_STACK)
// It is possible to use the standard template library list class since the 
// interface is nearly identical.
// Important: If you want to use the standard template library, no variable
// in a namespace with using a list shall have the name stack 
#include <stack>
#define OFStack std::stack
#else

#define INCLUDE_CASSERT
#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif


/**  non-template single linked list class, used to store elements of a stack.
 *  Implicitly used by OFStack, should not be called by users.
 */
struct OFStackLinkBase
{
    /// pointer to next entry in list
    OFStackLinkBase * next;

    /// default constructor
    OFStackLinkBase()
      : next(NULL)
    {
    }

    /// destructor
    virtual ~OFStackLinkBase()
    {
    }
private:

    /// private undefined copy constructor 
    OFStackLinkBase(const OFStackLinkBase &);

    /// private undefined copy assignment operator 
    OFStackLinkBase &operator=(const OFStackLinkBase &);
};

/** non-template base class for OFStack.
 *  Implicitly used by OFStack, should not be called by users.
 */
class OFStackBase
{
public:
    /// default constructor
    OFStackBase()
      : head(NULL),
        stackSize(0)
    {
    }

    /// destructor
    virtual ~OFStackBase()
    {
      while(!base_empty())
        base_pop();
    }

    /** checks if the stack is empty
     *  @return true if stack is emtpy, false otherwise
     */
    OFBool base_empty() const { return head == NULL; }

    /** returns size of stack
     *  @return size of stack
     */
    size_t base_size() const { return stackSize; }

    /** returns element on top of stack.
     *  precondition: stack is not empty
     */
    OFStackLinkBase * base_top() 
    {
    assert(head!=NULL);
    return head;
    }

    /** pushes element onto stack.
     *  @param element pointer to element
     */
    void base_push(OFStackLinkBase * element)
    {
      element->next = head;
      head = element;
      stackSize++;
    }
    
    /** removes top element from stack.
     *  precondition: stack not empty.
     */
    void base_pop()
    {
      assert(head!=NULL);
      OFStackLinkBase * delObj = head;
      head = head->next;
      delete delObj;
      stackSize--;
    }

protected:

    /// pointer to top element of stack
    OFStackLinkBase * head;
    
    /// size of stack
    size_t stackSize;

private:
 
    /// private undefined copy constructor 
    OFStackBase(const OFStackBase &);

    /// private undefined copy assignment operator 
    OFStackBase &operator=(const OFStackBase &);

};

/** template class used to store stack entries.
 *  Implicitly used by OFStack, should not be called by users.
 */
template <class T>
struct OFStackLink : public OFStackLinkBase
{
    /// element on stack
    T info;

    /** constructor
     *  @param i element maintain by this object
     */
    OFStackLink(const T & i) : OFStackLinkBase(), info(i)  { }

    /// destructor
    virtual ~OFStackLink()
    {
    }

private:
 
    /// private undefined copy constructor 
    OFStackLink(const OFStackLink<T> &);

    /// private undefined copy assignment operator 
    OFStackLink<T> &operator=(const OFStackLink<T> &);
};



/** template stack class. The interface is a subset of
 *  the STL stack class.
 */
template <class T>
class OFStack : private OFStackBase
{

public:

    /// Default constructor
    OFStack() {};

    /// copy constructor
    OFStack(const OFStack<T> & x) : OFStackBase()
    {
        copy(x);
    }

    /// Assignment operator
    OFStack<T> &operator=(const OFStack<T> &x)
    {
        copy(x);
        return *this;
    }

    /** checks if the stack is empty.
     *  @return OFTrue if stack is empty, OFFalse otherwise.
     */
    OFBool empty() const { return OFStackBase::base_empty(); }

    /** returns the number of elements on the stack
     *  @return number of elements on stack
     */
    size_t size() const { return OFStackBase::base_size(); }

    /** returns a reference to the top element on the stack.
     *  This method may not be called if the stack is empty.
     *  @return reference to top element
     */
    T & top() 
    { 
        return (OFstatic_cast(OFStackLink<T> *, OFStackBase::base_top()))->info; 
    }

    /** inserts a new element on top of the stack. The value of
     *  the new element is copy constructed from the given argument.
     *  @param x value to be pushed (copied) onto the stack
     */
    void push(const T & x) 
    { 
        OFStackBase::base_push(new OFStackLink<T>(x));
    }

    /** removes the top element from the stack.
     *  This method may not be called if the stack is empty.
     */    
    void pop() { OFStackBase::base_pop(); }

private:

    /** copy assignment of a stack. 
     *  @param x stack to be copied
     *  @return dummy value, required to keep Sun CC 2.0.1 happy
     */
    int copy(const OFStack<T> & x)
    {
        stackSize = x.size();
        if (stackSize)
        {
            head = new OFStackLink<T>((OFstatic_cast(OFStackLink<T>*, x.head))->info);
            OFStackLinkBase * newPtr = head;
            OFStackLinkBase * oldPtr = x.head->next;
            while (oldPtr)
            {
            newPtr->next = 
                new OFStackLink<T>((OFstatic_cast(OFStackLink<T>*, oldPtr))->info);
            oldPtr = oldPtr->next;
            newPtr = newPtr->next;
            }
        }
        return 0;
    }

};

#endif
#endif

/*
** CVS/RCS Log:
** $Log: ofstack.h,v $
** Revision 1.18  2010-10-14 13:15:50  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.17  2005/12/08 16:06:03  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.16  2003/08/14 14:41:39  meichel
** OFStack now explicitly defined as std::stack if compiling with HAVE_STL
**
** Revision 1.15  2003/07/09 13:57:43  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.14  2002/11/27 11:23:06  meichel
** Adapted module ofstd to use of new header file ofstdinc.h
**
** Revision 1.13  2002/07/10 11:50:40  meichel
** Added vitual destructor to class OFStackLink
**
** Revision 1.12  2001/12/03 15:09:09  meichel
** Completed doc++ documentation
**
** Revision 1.11  2001/07/03 14:35:01  meichel
** Fixed memory leak in ofstack.h
**
** Revision 1.10  2001/06/01 15:51:35  meichel
** Updated copyright header
**
** Revision 1.9  2000/10/12 08:11:35  joergr
** Added assignment operator to class OFStack.
** Declared (unimplemented) copy constructor and assignment operator in class
** OFStackLink to avoid compiler warnings (e.g. on Sun CC 2.0.1).
**
** Revision 1.8  2000/10/10 12:01:21  meichel
** Created/updated doc++ comments
**
** Revision 1.7  2000/03/08 16:36:02  meichel
** Updated copyright header.
**
** Revision 1.6  1998/11/27 12:42:52  joergr
** Added copyright message to source files and changed CVS header.
**
** Revision 1.5  1998/07/02 10:11:31  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/02/06 15:07:40  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.3  1997/09/11 15:43:16  hewett
** Minor changes to eliminate warnings when compiled under the
** Signus GnuWin32 envionment.  Changed order of initialisers
** for OFListLink and OFStackLink.  Make ~OFLisBase and ~OFStackBase
** virtual destructors.
**
** Revision 1.2  1997/07/21 09:02:24  andreas
** - New copy constructor for class OFStack
**
** Revision 1.1  1997/07/02 11:51:15  andreas
** - Preliminary release of the OFFIS Standard Library.
**   In the future this library shall contain a subset of the
**   ANSI C++ Library (Version 3) that works on a lot of different
**   compilers. Additionally this library shall include classes and
**   functions that are often used. All classes and functions begin
**   with OF... This library is independent of the DICOM development and
**   shall contain no DICOM specific stuff.
**
**
*/
