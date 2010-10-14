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
 *          Defines a template list class with interfaces similar to the C++ Standard
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFLIST_H
#define OFLIST_H

// Usage (only non standard):
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

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcast.h"

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

#ifdef HAVE_STD_NAMESPACE
#define OFList std::list
#define OFListIterator(x) std::list< x >::iterator
#define OFListConstIterator(x) std::list< x >::const_iterator
#else
#define OFList list
#define OFListIterator(x) list< x >::iterator
#define OFListConstIterator(x) list< x >::const_iterator
#endif

#define OFListInsert(InputIterator, T, c, pos, first, last) (c).insert((pos), (first), (last))
#define OFListRemoveIf(Predicate, T, c, pred) (c).remove_if((pred))

// workaround for "implicit typename" warning on gcc 3.x
#define OFLIST_TYPENAME OFTypename

#else

#define INCLUDE_CASSERT
#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"

#define OFLIST_TYPENAME

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
/* needed e.g. on Solaris for definition of size_t */
#include <sys/types.h>
#endif
END_EXTERN_C

// OFListLinkBase, OFListLink and OFListBase are classes for internal
// use only and shall not be used.

/* non-template double linked list. Base class fo OFListLink.
 * Implicitly used by OFList, should not be called by users.
 */
struct OFListLinkBase
{
    OFListLinkBase * next;
    OFListLinkBase * prev;
    OFBool dummy;
    OFListLinkBase(): next(NULL), prev(NULL), dummy(OFFalse) { }
    virtual ~OFListLinkBase() {}

  private:
    /* undefined */ OFListLinkBase(const OFListLinkBase&);
    /* undefined */ OFListLinkBase& operator=(const OFListLinkBase&);
};


/* non-template list. Base class fo OFList.
 * Implicitly used by OFList, should not be called by users.
 */
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

  private:
    /* undefined */ OFListBase(const OFListBase&);
    /* undefined */ OFListBase& operator=(const OFListBase&);
};



/* template class for double linked list entries.
 * Implicitly used by OFList, should not be called by users.
 */
template <class T>
struct OFListLink : public OFListLinkBase
{
    T info;
    OFListLink(const T& i) : OFListLinkBase(), info(i)  { }
    virtual ~OFListLink() {}
  private:
    /* undefined */ OFListLink(const OFListLink<T>&);
    /* undefined */ OFListLink<T>& operator=(const OFListLink<T>&);
};


// Definition of OFList and OFIterator

template <class T> class OFList;


/** iterator class for OFList. An iterator is a generalization of a pointer
 *  and allows a C++ program to work with different containers independently
 *  from their internal structure. Instances of this template class should
 *  be declared as OFListIterator(T) instead of OFListIterator<T>.  This
 *  allows to re-map OFList to the STL list class if available.
 */
template <class T>
class OFIterator
{
    friend class OFList<T>;
protected:

    /// list node referenced by the iterator
    OFListLinkBase * node;

    /** constructor.
     *  @param x list node referenced by the iterator
     */
    OFIterator(OFListLinkBase * x) : node(x) { }
public:

    /** default constructor. Creates an iterator referencing nothing.
     *  In general, iterators should always be copy-constructed
     *  in user code.
     */
    OFIterator() : node(NULL)  { }

    /** copy constructor
     */
    OFIterator(const OFIterator<T>& x) : node(x.node) { };

    /** copy assignment operator
     */
    OFIterator<T>& operator=(const OFIterator<T>& x)
    {
        node = x.node;
        return *this;
    }

    /** comparison of two iterators.  The iterators are equal if and only if
     *  they reference the same element, independent from the element values.
     *  @param x iterator to be compared
     *  @return OFTrue if equal, OFFalse otherwise.
     */
    OFBool operator==(const OFIterator<T>& x) const { return node == x.node; }

    /** comparison of two iterators.  The iterators are equal if and only if
     *  they reference the same element, independent from the element values.
     *  @param x iterator to be compared
     *  @return OFTrue if not equal, OFFalse otherwise.
     */
    OFBool operator!=(const OFIterator<T>& x) const { return node != x.node; }

    /** dereferences the iterator. May only be called if iterator references
     *  a valid element of a list.
     *  @return reference to the element "pointed to" by the iterator.
     */
    T& operator*() const
    {
        assert(!node->dummy);
        return (OFstatic_cast(OFListLink<T> *,node))->info;
    }

    /** dereferences the iterator. May only be called if iterator references
     *  a valid element of a list.
     *  @return reference to the element "pointed to" by the iterator.
     */
    T* operator->() const
    {
        return &(**this);
    }

    /** moves the iterator to the next element of the list.
     *  The list is circular: the first element follows after the end of the list.
     *  May only be called if iterator references a valid element or the end of a list.
     *  @return reference to the incremented iterator.
     */
    OFIterator<T>& operator++()
    {
        node = node->next;
        return *this;
    }

    /** moves the iterator to the next element of the list.
     *  The list is circular: the first element follows after the end of the list.
     *  May only be called if iterator references a valid element or the end of a list.
     *  This is the post-increment operator.
     *  @return previous iterator state, by value
     */
    OFIterator<T> operator++(int)
    {
        OFIterator<T> tmp(*this);
        node = node->next;
        return tmp;
    }

    /** moves the iterator to the previous element of the list.
     *  The list is circular: the end of the list follows before the first element.
     *  May only be called if iterator references a valid element or the end of a list.
     *  @return reference to the decremented iterator.
     */
    OFIterator<T>& operator--()
    {
        node = node->prev;
        return *this;
    }

    /** moves the iterator to the previous element of the list.
     *  The list is circular: the end of the list follows before the first element.
     *  May only be called if iterator references a valid element or the end of a list.
     *  This is the post-decremented operator.
     *  @return previous iterator state, by value
     */
    OFIterator<T> operator--(int)
    {
        OFIterator<T> tmp(*this);
        node = node->prev;
        return tmp;
    }
};


/** double linked list template class. The interface is a subset of the STL
 *  list class.
 */
template <class T>
class OFList : private OFListBase
{
public:
    /** inserts an element into the list before the given position.
     *  @param position iterator to position before which the element is inserted
     *  @param x value from which the new list entry is copy-constructed
     *  @return iterator pointing to the new element in the list
     */
    OFIterator<T> insert(OFIterator<T> position, const T& x)
    {
      return OFIterator<T>(OFListBase::base_insert(position.node, new OFListLink<T>(x)));
    }

private:

    /** inserts a copy of the given list into the current list.
     *  @param oldList list to be copied
     */
    void copy(const OFList<T>& oldList)
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

    /** counts the elements in the list and adjusts the listSize
     *  member variable.
     */
    void recalcListSize() { OFListBase::base_recalcListSize(); }

public:

    /** default constructor
     */
    OFList() : OFListBase() { }

    /** copy constructor
     */
    OFList(const OFList<T>& oldList):OFListBase()
    {
        copy(oldList);
    }

    /** returns an iterator referencing the first element in the list.
     *  If the list is empty, then begin() == end().
     *  @return iterator to first element of list, by value.
     */
    OFIterator<T> begin() const { return OFIterator<T>(OFListBase::base_begin()); }

    /** returns an iterator which points to the past-to-end element
     *  of the list.
     *  @return iterator to past-to-end, by value.
     */
    OFIterator<T> end() const { return OFIterator<T>(OFListBase::base_end()); }

    /** returns true if list is empty.
     *  @return OFTrue if list is empty, OFFalse otherwise.
     */
    OFBool empty() const { return OFListBase::base_empty(); }

    /** returns number of elements in the list.
     *  @return number of elements
     */
    size_t size() const { return OFListBase::base_size(); }

    /** returns a reference to the first element in the list.
     *  May only be called if list is non-empty.
     *  @return first element in list, by reference
     */
    T& front() { return *begin(); }

    /** returns a reference to the last element in the list.
     *  May only be called if list is non-empty.
     *  @return last element in list, by reference
     */
    T& back() { return *(--end()); }

    /** inserts before the first element of the list.
     *  @param x value from which the new list entry is copy constructed
     */
    void push_front(const T& x) { insert(begin(), OFconst_cast(T&, x)); }
    /* const cast away to keep some old compilers happy */

    /** removes the first element of the list.
     *  May only be called if list is non-empty.
     *  All iterators pointing to the removed element become invalid.
     */
    void pop_front() { erase(begin()); }

    /** inserts after the last element of the list.
     *  @param x value from which the new list entry is copy constructed
     */
    void push_back(const T& x) { insert(end(), OFconst_cast(T&, x)); }
    /* const cast away to keep some old compilers happy */

    /** removes the last element of the list.
     *  May only be called if list is non-empty.
     *  All iterators pointing to the removed element become invalid.
     */
    void pop_back() { erase(--end()); }

    /** inserts n elements with value x into the list, before the given position.
     *  @param position iterator to position before which the elements are inserted
     *  @param n number of entries to be created
     *  @param x value from which the new list entries are copy-constructed
     */
    void insert(OFIterator<T> position, size_t n, const T& x)
    {
      while(n--) OFListBase::base_insert(position.node, new OFListLink<T>(x));
    }

    /** removes the element at the given position from the list.
     *  All iterators pointing to the removed element become invalid.
     *  @return iterator pointing to the element after the removed one
     */
    OFIterator<T> erase(OFIterator<T> position)
    {
      return OFIterator<T>(OFListBase::base_erase(position.node));
    }

    /** removes all elements in the range [position,last) from the list.
     *  All iterators pointing to the removed elements become invalid.
     *  @param position iterator to the first element to be deleted
     *  @param last iterator pointing to the element after the last element to be removed
     *  @return iterator pointing to the element after the last removed element
     */
    OFIterator<T> erase(OFIterator<T> position, OFIterator<T> last)
    {
      while (position != last) position = erase(position);
      return last;
    }

    /** removes all elements from the list.
     *  All iterators pointing to elements in the list become invalid.
     */
    void clear() { OFListBase::base_clear(); }

    /** moves the contents of list x into the current list before the
     *  given position.
     *  @param position iterator to position before which the elements are inserted
     *  @param x list from which the elements are taken, becomes empty
     */
    void splice(OFIterator<T> position, OFList<T>& x)
    {
      splice(position, x, x.begin(), x.end());
    }

    /** inserts one element from list x into the current list and removes it from x
     *  @param position iterator to position before which the element is inserted
     *  @param x list from which the element is taken
     *  @param i iterator to element in list x which is to be moved
     */
    void splice(OFIterator<T> position, OFList<T>& x, OFIterator<T> i)
    {
      OFIterator<T> change(i);
      ++i;
      splice(position, x, change, i);
    }

    /** inserts elements in the range [first, last) before position and
     *  removes the elements from x
     *  @param position iterator to position before which the elements are inserted
     *  @param x list from which the elements are taken
     *  @param first iterator to first element in list x to be moved
     *  @param last iterator to element after last element in list x to be moved
     */
    void splice(OFIterator<T> position, OFList<T>& x,
                OFIterator<T> first, OFIterator<T> last)
    {
      OFListBase::base_splice(position.node, first.node, last.node);
      x.recalcListSize();
    }

    /** removes all elements from the list referred by an iterator i where
     *  *i == value
     *  @param value value to be compared with list contents
     */
    void remove(const T& value)
    {
      OFIterator<T> first = begin();
      OFIterator<T> last = end();
      while(first != last)
      {
        if (*first == value) first = erase(first);
        else ++first;
      }
    }

private:

	/** private undefined copy assignment operator
	 */
	OFList<T>& operator=(const OFList<T>& arg);
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
void OF_ListInsert(OFList<T>& c, OFIterator<T> position,
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
void OF_ListRemoveIf(OFList<T>& c, Predicate pred)
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

#define OFListIterator(x) OFIterator< x >
#define OFListConstIterator(x) OFIterator< x >

#endif

#endif

/*
** CVS/RCS Log:
** $Log: oflist.h,v $
** Revision 1.26  2010-10-14 13:15:50  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.25  2010-05-07 11:13:42  uli
** Use OFTypename for OFLIST_TYPENAME.
**
** Revision 1.24  2009-08-19 11:55:45  meichel
** Added additional includes needed for Sun Studio 11 on Solaris.
**
** Revision 1.23  2009-08-19 10:44:36  joergr
** Added missing dereference operator.
**
** Revision 1.22  2005/12/08 16:05:58  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.21  2004/04/14 11:44:48  joergr
** Replaced non-Unix newline characters.
**
** Revision 1.20  2003/08/07 11:44:55  joergr
** Slightly modified header comments to conform to doxygen syntax.
**
** Revision 1.19  2003/07/11 13:46:14  joergr
** Added workaround to get rid of "implicit typename" warnings on gcc 3.x
** (introduced macro OFLIST_TYPENAME).
**
** Revision 1.18  2003/07/09 13:57:43  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.17  2003/06/12 15:20:30  joergr
** Slightly modified macro definitions to avoid potential parser errors (added
** space character after '<' and before '>').
**
** Revision 1.16  2003/06/12 13:21:54  joergr
** Introduced macro OFListConstIterator() to support STL const_iterators.
**
** Revision 1.15  2003/06/03 10:20:00  meichel
** OFList now explicitly defined as std::list if std namespace present
**
** Revision 1.14  2002/11/27 11:23:05  meichel
** Adapted module ofstd to use of new header file ofstdinc.h
**
** Revision 1.13  2001/08/23 16:05:52  meichel
** Added private undefined copy assignment operators to avoid gcc warnings
**
** Revision 1.12  2001/06/01 15:51:34  meichel
** Updated copyright header
**
** Revision 1.11  2000/10/10 12:01:21  meichel
** Created/updated doc++ comments
**
** Revision 1.10  2000/03/08 16:36:02  meichel
** Updated copyright header.
**
** Revision 1.9  1998/11/27 12:42:51  joergr
** Added copyright message to source files and changed CVS header.
**
** Revision 1.8  1998/07/02 07:47:02  meichel
** Some code purifications to avoid gcc 2.8.1 -Weffc++ warnings.
**
** Revision 1.7  1998/06/29 12:09:23  meichel
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
