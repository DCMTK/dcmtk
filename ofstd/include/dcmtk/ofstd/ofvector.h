/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Defines a template vector class based on the STL vector class
 *
 */

#ifndef OFVECTOR_H
#define OFVECTOR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif

#if defined(HAVE_STL) || defined(HAVE_STL_VECTOR)

// Use the standard template library (STL) vector class.
#include <vector>

#ifdef HAVE_STD_NAMESPACE
#define OFVector std::vector
#else
#define OFVector vector
#endif

#else

#define INCLUDE_CASSERT          /* for assert() */
#define INCLUDE_CSTDLIB          /* for NULL */
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/oftypes.h" /* for OFBool */

/** this is a resizable array. You can add and remove elements after it was
 *  created and this class will handle all the memory management needed. This
 *  implements parts of std::vector's features.
 */
template<typename T>
class OFVector
{
public:
    /** the type of elements that this OFVector stores */
    typedef T        value_type;
    /** the type used for sizes and indexes */
    typedef size_t   size_type;
    /** the type of mutable iterators on this object */
    typedef T*       iterator;
    /** the type of constant iterators on this object */
    typedef const T* const_iterator;
    /** the type of mutable references on this object */
    typedef T&       reference;
    /** the type of constant references on this object */
    typedef const T& const_reference;

protected:

    /** array that is used for storing the entries in this OFVector */
    T* values_;

    /** the size for which values_ was allocated */
    size_type allocated_;

    /** the number of valid entries in values_. elements past this
     *  index have undefined content.
     */
    size_type size_;

public:

    /** default constructor. This creates an empty OFVector. */
    OFVector() : values_(NULL), allocated_(0), size_(0)
    {
        reserve(0);
    }

    /** copy constructor.
     *  @param other OFVector from which all elements are copied
     */
    OFVector(const OFVector& other) : values_(NULL), allocated_(0), size_(0)
    {
        reserve(other.size());
        for (const_iterator it = other.begin(); it != other.end(); ++it)
            push_back(*it);
    }

    /** construct an OFVector with predefined content.
     *  @param n number of elements that this OFVector should have.
     *  @param v The value used for all elements.
     */
    explicit OFVector(size_type n, const T& v = T()) : values_(NULL), allocated_(0), size_(0)
    {
        if (n > 0)
            resize(n, v);
        else
            // Make sure that values_ never is a NULL pointer
            reserve(0);
    }

    /** construct an OFVector from a range of iterators.
     *  @param from first iterator to include
     *  @param to first iterator that should not be included anymore
     */
    OFVector(const_iterator from, const_iterator to) : values_(NULL), allocated_(0), size_(0)
    {
        reserve(to - from);
        while (from != to)
            push_back(*(from++));
    }

    /** destructor. Frees all memory used by this object.
     */
    ~OFVector()
    {
        delete[] values_;
    }

    /** assignment operator. All elements from this object are removed and then
     *  a copy of other is made. All iterators to this object will become
     *  invalid.
     *  @param other OFVector instance to copy elements from.
     */
    OFVector& operator=(const OFVector& other)
    {
        if (this == &other)
            return *this;

        clear();
        reserve(other.size());
        for (const_iterator it = other.begin(); it != other.end(); ++it)
            push_back(*it);
        return *this;
    }

    /** swap this vector's content with some other vector.
     *  All iterators will stay valid.
     *  @param other object to swap with
     */
    void swap(OFVector& other)
    {
        T* tmp_val = values_;
        size_type tmp_all = allocated_;
        size_type tmp_size = size_;

        values_ = other.values_;
        allocated_ = other.allocated_;
        size_ = other.size_;

        other.values_ = tmp_val;
        other.allocated_ = tmp_all;
        other.size_ = tmp_size;
    }

    /** get an iterator for the first element in this object.
     *  @return iterator that points to the first element.
     */
    iterator begin() { return &values_[0]; }

    /** get an iterator for the first element in this object.
     *  @return iterator that points to the first element.
     */
    const_iterator begin() const { return &values_[0]; }

    /** get an iterator that points past the last valid object.
     *  @return iterator that points to the end of this OFVector.
     */
    iterator end() { return &values_[size_]; }

    /** get an iterator that points past the last valid object.
     *  @return iterator that points to the end of this OFVector.
     */
    const_iterator end() const { return &values_[size_]; }

    /** get the size of this OFVector.
     *  @return number of elements in this instance.
     */
    size_type size() const { return size_; }

    /** check wether this OFVector is empty.
     *  @return true if this OFVector is empty.
     */
    OFBool empty() const { return size_ == 0; }

    /** clear this OFVector. The existing content will be freed and all
     *  iterators become invalid.
     */
    void clear()
    {
        delete[] values_;
        values_ = NULL;
        size_ = 0;
        allocated_ = 0;
        // We must never have values_ == NULL
        reserve(0);
    }

    /** removes an entry from this OFVector.
     *  All iterators pointing to the element removed or elements that come
     *  behind it in this OFVector will become invalid.
     *  @param it iterator for the entry that should be removed.
     */
    void erase(iterator it)
    {
        size_type idx = it - begin();
        for (size_type i = idx + 1; i < size_; i++) {
            values_[i - 1] = values_[i];
        }
        size_--;
    }

    /** insert an entry in this OFVector.
     *  All iterators for this OFVector become invalid.
     *  @param it the new element will be inserted in front of the element to
     *            which this iterator points.
     *  @param v the element to insert
     *  @return iterator for the newly inserted element.
     */
    iterator insert(iterator it, const T& v)
    {
        size_type idx = it - begin();

        if (size_ == allocated_) {
            reserve(size_ * 2);
        }

        if (idx < size_)
            for (size_type i = size_; i > idx; i--) {
                values_[i] = values_[i - 1];
            }
        values_[idx] = v;
        size_++;
        return &values_[idx];
    }

    /** insert a range of elements in this OFVector.
     *  All iterators for this OFVector become invalid.
     *  @param it the new elements will be inserted in front of the element to
     *            which this iterator points.
     *  @param from iterator to the beginning of the range to insert.
     *  @param to iterator past the end of the range to insert.
     */
    template<class InputIterator>
    void insert(iterator it, InputIterator from, InputIterator to)
    {
        while (from != to)
        {
            it = insert(it, *from);
            it++;
            from++;
        }
    }

    /** get a reference to the first element of this vector.
     *  @return reference to the first element.
     */
    T& front()
    {
        return values_[0];
    }

    /** get a reference to the first element of this vector.
     *  @return reference to the first element.
     */
    const T& front() const
    {
        return values_[0];
    }

    /** get a reference to the last element of this vector.
     *  @return reference to the last element.
     */
    T& back()
    {
        return values_[size_ - 1];
    }

    /** get a reference to the last element of this vector.
     *  @return reference to the last element.
     */
    const T& back() const
    {
        return values_[size_ - 1];
    }

    /** insert an entry at the end of this object
     *  @param v the value to insert
     */
    void push_back(const T& v)
    {
        insert(end(), v);
    }

    /** remove the last entry in this object
     */
    void pop_back()
    {
        erase(end() - 1);
    }

    /** access an entry by index. undefined behavior occurs when the index is
     *  out of bounds (bigger than the maximum allowed index).
     *  @param i index of the element to return
     *  @return reference to the element.
     */
    T& operator[](size_type i)
    {
        return values_[i];
    }

    /** access an entry by index. undefined behavior occurs when the index is
     *  out of bounds (bigger than the maximum allowed index).
     *  @param i index of the element to return
     *  @return reference to the element.
     */
    const T& operator[](size_type i) const
    {
        return values_[i];
    }

    /** access an entry by index.
     *  @param i index of the element to return
     *  @return reference to the element.
     */
    T& at(size_type i)
    {
        assert(i < size_);
        return (*this)[i];
    }

    /** access an entry by index.
     *  @param i index of the element to return
     *  @return reference to the element.
     */
    const T& at(size_type i) const
    {
        assert(i < size_);
        return (*this)[i];
    }

    /** resize this OFVector. after this call, size() will be n.
     *  @param n the new size that this object should use
     *  @param v if any new elements need to be inserted, they get this value.
     */
    void resize(size_type n, T v = T())
    {
        if (n > size_)
        {
            reserve(n);
            // Set up the new elements
            for (size_t i = size_; i < n; i++)
                values_[i] = v;
        }
        size_ = n;
    }

    /** reserves enough space for the given number of elements. from now on, no
     *  memory allocations will occur as long as this OFVector's size stays
     *  below n and clear() is not called.
     *  @param n the number of elements for which space should be reserved.
     */
    void reserve(size_type n)
    {
        T* old_values = values_;
        T* new_values;

        if (n == 0)
            n = 1;
        if (n <= allocated_)
            return;

        // While we are at it, let's reserve some extra space
        n += 10;

        new_values = new T[n];
        if (old_values)
        {
            for (size_type i = 0; i < size_; i++)
                new_values[i] = old_values[i];
            delete[] old_values;
        }

        values_ = new_values;
        allocated_ = n;
    }
};

#endif

#endif
