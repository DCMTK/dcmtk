/*
** Author: Andreas Barth	Created: 19.06.97
**				
** Kuratorium OFFIS e.V.
**
** Module: oflist.h
**
** Purpose:
**	Defines a template list class with interfaces similar to the
**      C++ Standard
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-06-29 12:09:23 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/Attic/oflist.h,v $
** CVS/RCS Revision:	$Revision: 1.7 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef OFLIST_H
#define OFLIST_H

// Usage (only non standard): 
//   OFList<T>          defines a list containing elements of type T
//   OFListIterator(T)  defines an iterator to an element of type T in an List
//                      No default constructor is defined. You can only use
//                      a copy constructor for the definition of an iterator.
//                      For compatibility reasons with the C++ Standard 
//                      iterator classes, do not use OFIterator<T>!
//   OFListInsert(InputIterator_type, T_type, c, pos, first, last)    
//                      Inserts the elements of type T in 
//                      range [first, last) into list c, 
//   OFListRemoveIf(Predicate_Type, T_type, c, pred)
//                      Erases all elements of type T in the list referred by 
//                      an iterator i where pred(*i) == true.
//                      Predicate_Type is the function type of pred
//
// Additional Remarks:
//   In some template functions one template parameter is another function.
//   Some compilers  cannot determine automatically the type of template 
//   function parameters, so you must give  them a hint casting 
//   the parameter function to the correct type (e.g. NeXT gcc 2.5.8)

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oftypes.h"

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif

#if defined(HAVE_STL) || defined(HAVE_STL_LIST)
// It is possible to use the standard template library list class since the 
// interface is nearly identical.
// Important: If you want to use the standard template library (STL), no 
// variable within a namespace using a class of the STL shall have a name
// of one class of the STL
#include <list>
#define OFList list
#define OFListIterator(x) list<x>::iterator
#define OFListInsert(InputIterator, T, c, pos, first, last) (c).insert((pos), (first), (last))
#define OFListRemoveIf(Predicate, T, c, pred) (c).remove_if((pred))
#else

#include <assert.h>
#include <stddef.h>


// OFListLinkBase, OFListLink and OFListBase are classes for internal 
// use only and shall not be used.

struct OFListLinkBase
{
    OFListLinkBase * next;
    OFListLinkBase * prev;
    OFBool dummy;
    OFListLinkBase(): next(NULL), prev(NULL), dummy(OFFalse) { }
    virtual ~OFListLinkBase() {}
};


class OFListBase
{
protected:
    OFListLinkBase * afterLast;
    size_t listSize;
    void base_recalcListSize();
public:
    OFListBase();
    virtual ~OFListBase();
    OFListLinkBase * base_begin() const { return afterLast->next; }
    OFListLinkBase * base_end() const { return afterLast; }
    OFBool base_empty() const { return afterLast == afterLast->next; }
    size_t base_size() const { return listSize; }
    OFListLinkBase * base_insert(OFListLinkBase * pos, OFListLinkBase * newElem);
    OFListLinkBase * base_erase(OFListLinkBase * pos);
    void base_splice(OFListLinkBase * pos, 
		OFListLinkBase * begin, OFListLinkBase * end);
    void base_clear();
};  



template <class T>
struct OFListLink : public OFListLinkBase
{
    T info;
    OFListLink(const T & i) : OFListLinkBase(), info(i)  { }
    virtual ~OFListLink() {}
};


// Definition of OFList and OFIterator

template <class T> class OFList;

// class OFIterator
// An iterator is  a generalization of a pointer and allows a C++ program to 
// work with different containers (e.g. a list).

template <class T>
class OFIterator
{
    friend class OFList<T>;
protected:
    OFListLinkBase * node;
    OFIterator(OFListLinkBase * x) : node(x) { }
public:
    // constructors
    OFIterator() : node(NULL)  { }
    
    // Creates a copy of another iterator
    OFIterator(const OFIterator<T> & x) : node(x.node) { };

    // Assign an Iterator
    OFIterator<T> & operator=(const OFIterator<T> & x) 
    { 
	node = x.node; 
	return *this; 
    }

    // The following operations are only defined if the iterator is
    // valid. An iterator is invalid if the element in the container
    // the iterator is pointed to was deleted.

    // Compares two iterators. Two iterators are equal if
    // they point to the same list element
    OFBool operator==(const OFIterator<T> & x) const { return node == x.node; }
    OFBool operator!=(const OFIterator<T> & x) const { return node != x.node; }
    
    // Returns the value of the list element the iterator is pointed to.
    T & operator*() const 
    { 
	assert(!node->dummy);
	return ((OFListLink<T> *)node)->info;
    }

    // These operators point to the next resp. previous element in the list.
    // After the end of the list the first element follows.
    // Caution: The pre-operators return references while the
    // post-operators return the value!
    OFIterator<T> & operator++()
    {
	node = node->next;
	return *this;
    }

    OFIterator<T> operator++(int)
    {
	OFIterator<T> tmp(*this);
	node = node->next;
	return tmp;
    }

    OFIterator<T> & operator--()
    {
	node = node->prev;
	return *this;
    }

    OFIterator<T> operator--(int)
    {
	OFIterator<T> tmp(*this);
	node = node->prev;
	return tmp;
    }
};


template <class T>
class OFList : private OFListBase
{
public:
    // Insert a element x  before position.
    // Returns an iterator pointing to the new element in the list.
    OFIterator<T> insert(OFIterator<T> position, const T & x)
	{
	    return OFIterator<T>(OFListBase::base_insert(position.node, 
						    new OFListLink<T>(x)));
	}
private:
    void copy(const OFList<T> & oldList) 
    {
	OFIterator<T> vfirst(oldList.begin());
	OFIterator<T> vend(oldList.end());
	OFIterator<T> vpos(this->end());
	while (vfirst != vend)
	{
	    insert(vpos, *vfirst);
	    ++vfirst;
	}
    }

    void recalcListSize() { OFListBase::base_recalcListSize(); }
public:
    OFList() : OFListBase() { }

    // Copy Constructor
    OFList(const OFList<T> & oldList):OFListBase()
    {
	copy(oldList);
    }

    // Returns an iterator referring to the first element in the list
    // If the list is empty then begin() == end().
    OFIterator<T> begin() const { return OFIterator<T>(OFListBase::base_begin()); }

    // Returns an iterator which points to the past-to-end value for the
    // list.
    OFIterator<T> end() const { return OFIterator<T>(OFListBase::base_end()); }
    
    // Returns OFTrue if the list is empty.
    OFBool empty() const { return OFListBase::base_empty(); }
    
    // Returns the number of elements in the list.
    size_t size() const { return OFListBase::base_size(); }

    // Returns the first element of the list. If the list is empty
    // an assertion is raised.
    T & front() { return *begin(); }

    // Returns the last element of the list. If the list is empty
    // an assertion is raised
    T & back() { return *(--end()); }

    // Inserts before the first element
    void push_front(const T & x) { insert(begin(), (T&)x); } 
         /* const cast away to keep some old compilers happy */

    // Deletes the first element of the list.
    // All iterators pointing to the deleted element are invalid.
    void pop_front() { erase(begin()); }

    // Inserts after the last element in the list
    void push_back(const T & x) { insert(end(), (T&)x); }
         /* const cast away to keep some old compilers happy */

    // Deletes the last element of the list.
    // All iterators pointing to the deleted element are invalid.
    void pop_back() { erase(--end()); }

    // Insert n times element x before position.
    void insert(OFIterator<T> position, size_t n, const T & x)
	{
	    while(n--)
		OFListBase::base_insert(position.node, new OFListLink<T>(x));
	}

    // Erase the element at position in the list.
    // All iterators pointing to the deleted element are invalid.
    // Returns Iterator pointing to the element after the deleted.
    OFIterator<T> erase(OFIterator<T> position)
	{
	    return OFIterator<T>(OFListBase::base_erase(position.node));
	}

    // Erase elements in range [position, last) from the list
    // All iterators pointing to the deleted elements are invalis
    OFIterator<T> erase(OFIterator<T> position, OFIterator<T> last)
	{
	    while (position != last)
		position = erase(position);
	    return last;
	}

    // Erases the complete list.	    
    void clear() { OFListBase::base_clear(); }


    // Inserts the contents of x before position and x becomes empty
    void splice(OFIterator<T> position, OFList<T> & x)
    {
	splice(position, x, x.begin(), x.end());
    }

    // Inserts an element pointed by iterator i from list x before
    // position and removes the element from x
    void splice(OFIterator<T> position, OFList<T> & x, OFIterator<T> i)
    {
	OFIterator<T> change(i);
	++i;
	splice(position, x, change, i);
    }

    // Inserts elements in the range [first, last) before position and
    // removes the elements from x
    void splice(OFIterator<T> position, OFList<T> & x,
		OFIterator<T> first, OFIterator<T> last)
    {
	OFListBase::base_splice(position.node, first.node, last.node);
	x.recalcListSize();
    }

    // Erases all elements in the list referred by an iterator i where
    // *i == value
    void remove(const T & value)
    {
	OFIterator<T> first = begin();
	OFIterator<T> last = end();
	while(first != last)
	{
	    if (*first == value)
		first = erase(first);
	    else
		++first;
	}
    }
};


#ifdef HAVE_FUNCTION_TEMPLATE

#define OFListInsert(InputIterator, T, c, pos, first, last) OF_ListInsert((c), (pos), (first), (last))

#define OFListRemoveIf(Predicate, T, c, pred) OF_ListRemoveIf((c), (pred))

#elif defined(HAVE_STATIC_TEMPLATE_METHOD)

#define OFListInsert(InputIterator, T, c, pos, first, last) OF_ListInsertClass<InputIterator, T>::OF_ListInsert((c), (pos), (first), (last))

#define OFListRemoveIf(Predicate, T, c, pred) OF_ListRemoveIfClass<Predicate, T>::OF_ListRemoveIf((c), (pred))

#else
#error Your C++ Compiler is not capable of compiling this code
#endif

// Insert the elements in range [first, last) into list
template <class InputIterator, class T>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_ListInsertClass
{ 
public:
static
#endif
void OF_ListInsert(OFList<T> & c, OFIterator<T> position, 
		  InputIterator first, InputIterator last)
{
    while(first != last)
    {
	c.insert(position, *first);
	++first;
    }
}
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

// Erases all elements in the list referred by an iterator i where
// pred(*i) == true
template <class Predicate, class T>
#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
class OF_ListRemoveIfClass
{
public:
static
#endif
void OF_ListRemoveIf(OFList<T> & c, Predicate pred)
{
    OFIterator<T> first = c.begin();
    OFIterator<T> last = c.end();
    while (first != last)
    {
	if (pred(*first))
	    first = c.erase(first);
	else
	    ++first;
    }
}

#if defined(HAVE_STATIC_TEMPLATE_METHOD) && !defined(HAVE_FUNCTION_TEMPLATE)
};
#endif

#define OFListIterator(x) OFIterator<x>

#endif

#endif

/*
** CVS/RCS Log:
** $Log: oflist.h,v $
** Revision 1.7  1998-06-29 12:09:23  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.6  1998/02/06 15:07:38  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.5  1997/11/10 16:31:19  meichel
** Corrected bug possibly causing a memory leak in OFList.
**   Added virtual destructors to classes OFListLinkBase and OFListLink.
**
** Revision 1.4  1997/09/11 15:43:15  hewett
** Minor changes to eliminate warnings when compiled under the
** Signus GnuWin32 envionment.  Changed order of initialisers
** for OFListLink and OFStackLink.  Make ~OFLisBase and ~OFStackBase
** virtual destructors.
**
** Revision 1.3  1997/07/24 13:11:00  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.2  1997/07/07 07:34:18  andreas
** - Corrected destructor for OFListBase, now the dummy element is
**   deleted.
**
** Revision 1.1  1997/07/02 11:51:14  andreas
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


