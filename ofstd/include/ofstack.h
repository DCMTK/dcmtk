/*
** Author: Andreas Barth	Created: 19.06.97
**				
** Kuratorium OFFIS e.V.
**
** Module: ofstack.h
**
** Purpose:
**	Defines a template stack class with interfaces similar to the
**      C++ Standard
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-02 11:51:15 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/ofstack.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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
    OFStackLinkBase() { next = NULL; }
};

class OFStackBase
{
private:
    OFStackLinkBase * head;
    size_t stackSize;
public:
    OFStackBase()
    {
	head = NULL;
	stackSize = 0;
    }

    ~OFStackBase()
    {
	while(!empty())
	    pop();
    }

    OFBool empty() const { return head == NULL; }
    size_t size() const { return stackSize; }

    // returns element on top of stack
    // precondition: stack is not empty
    OFStackLinkBase * top() 
    {
	assert(head!=NULL);
	return head;
    }

    void push(OFStackLinkBase * element)
    {
	element->next = head;
	head = element;
	stackSize++;
    }
    void pop()
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
    OFStackLink(const T & i) : info(i), OFStackLinkBase() { }
};


// Definition of OFStack

template <class T>
class OFStack : protected OFStackBase
{
public:
    // Constructors
    OFStack() {};

    // returns TRUE if Stack is empty
    OFBool empty() const { return OFStackBase::empty(); }

    // returns number of Elements in the Stack
    size_t size() const { return OFStackBase::size(); }

    // returns top element in the stack
    // precondition: stack is not empty
    T & top() 
    { 
	return ((OFStackLink<T>*)(OFStackBase::top()))->info; 
    }

    // inserts new Element on top of stack
    void push(const T & x) 
    { 
	OFStackBase::push(new OFStackLink<T>(x));
    }

    // removes top element of the stack
    // precondition: stack is not empty
    void pop() { OFStackBase::pop(); }
};

#endif
#endif

/*
** CVS/RCS Log:
** $Log: ofstack.h,v $
** Revision 1.1  1997-07-02 11:51:15  andreas
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
