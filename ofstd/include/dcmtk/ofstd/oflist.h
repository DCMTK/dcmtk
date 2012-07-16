/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
#include "dcmtk/ofstd/ofdefine.h"

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
class DCMTK_OFSTD_EXPORT OFListBase
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

    /** returns a constant reference to the first element in the list.
     *  May only be called if list is non-empty.
     *  @return first element in list, by constant reference
     */
    const T& front() const { return *begin(); }

    /** returns a reference to the last element in the list.
     *  May only be called if list is non-empty.
     *  @return last element in list, by reference
     */
    T& back() { return *(--end()); }

    /** returns a constant reference to the last element in the list.
     *  May only be called if list is non-empty.
     *  @return last element in list, by constant reference
     */
    const T& back() const { return *(--end()); }

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

    /** copy assignment operator
     * @param arg the list to copy from
     * @return *this
     */
    OFList<T>& operator=(const OFList<T>& arg)
    {
        clear();
        copy(arg);
        return *this;
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
