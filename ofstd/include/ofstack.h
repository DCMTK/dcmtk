/*
 *
 *  Copyright (C) 1997-2000, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *				
 *  Purpose:
 *	    Defines a template stack class with interfaces similar to the C++ Standard
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:36:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstack.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFSTACK_H
#define OFSTACK_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oftypes.h"

#if defined(HAVE_STL) || defined(HAVE_STL_STACK)
// It is possible to use the standard template library list class since the 
// interface is nearly identical.
// Important: If you want to use the standard template library, no variable
// in a namespace with using a list shall have the name stack 
#include <stack>
#define OFStack stack
#else

#include <assert.h>
#include <stddef.h>

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif


// OFStackLinkBase, OFStackLink and OFStackBase are classes for internal 
// use only and shall not be used.

struct OFStackLinkBase
{
    OFStackLinkBase * next;
    OFStackLinkBase()
      : next(NULL)
    {
    }

private:

 // --- declarations to avoid compiler warnings
 
    OFStackLinkBase(const OFStackLinkBase &);
    OFStackLinkBase &operator=(const OFStackLinkBase &);
};

class OFStackBase
{
private:

 // --- declarations to avoid compiler warnings
 
    OFStackBase(const OFStackBase &);
    OFStackBase &operator=(const OFStackBase &);

protected:
    OFStackLinkBase * head;
    size_t stackSize;
public:
    OFStackBase()
      :	head(NULL),
	    stackSize(0)
	{
    }

    virtual ~OFStackBase()
    {
	while(!base_empty())
	    base_pop();
    }

    OFBool base_empty() const { return head == NULL; }
    size_t base_size() const { return stackSize; }

    // returns element on top of stack
    // precondition: stack is not empty
    OFStackLinkBase * base_top() 
    {
	assert(head!=NULL);
	return head;
    }

    void base_push(OFStackLinkBase * element)
    {
	element->next = head;
	head = element;
	stackSize++;
    }
    void base_pop()
    {
	assert(head!=NULL);
	OFStackLinkBase * delObj = head;
	head = head->next;
	delete delObj;
	stackSize--;
    }
};

template <class T>
struct OFStackLink : public OFStackLinkBase
{
    T info;
    OFStackLink(const T & i) : OFStackLinkBase(), info(i)  { }
};


// Definition of OFStack

template <class T>
class OFStack : protected OFStackBase
{
friend class OFStackBase;
private:

    // copy is needed for the SUN CC 2.0.1. It returns a dummy value
    // to make this compiler happy.
    int copy(const OFStack<T> & x)
    {
	stackSize = x.size();
	if (stackSize)
	{
	    head = new OFStackLink<T>(((OFStackLink<T>*)(x.head))->info);
	    OFStackLinkBase * newPtr = head;
	    OFStackLinkBase * oldPtr = x.head->next;
	    while (oldPtr)
	    {
		newPtr->next = 
		    new OFStackLink<T>(((OFStackLink<T>*)oldPtr)->info);
		oldPtr = oldPtr->next;
		newPtr = newPtr->next;
	    }
	}
	return 0;
    }

public:
    // Constructors
    OFStack() {};

    OFStack(const OFStack<T> & x) : OFStackBase()
    {
	copy(x);
    }

    // returns TRUE if Stack is empty
    OFBool empty() const { return OFStackBase::base_empty(); }

    // returns number of Elements in the Stack
    size_t size() const { return OFStackBase::base_size(); }

    // returns top element in the stack
    // precondition: stack is not empty
    T & top() 
    { 
	return ((OFStackLink<T>*)(OFStackBase::base_top()))->info; 
    }

    // inserts new Element on top of stack
    void push(const T & x) 
    { 
	OFStackBase::base_push(new OFStackLink<T>(x));
    }

    // removes top element of the stack
    // precondition: stack is not empty
    void pop() { OFStackBase::base_pop(); }
};

#endif
#endif

/*
** CVS/RCS Log:
** $Log: ofstack.h,v $
** Revision 1.7  2000-03-08 16:36:02  meichel
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
