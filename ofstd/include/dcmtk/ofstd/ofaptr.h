/*
 *
 *  Copyright (C) 2009-2010, OFFIS e.V.
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
 *  Purpose: Template class for automatically deleting pointers when they go out
 *           of scope.
 *
 */

#ifndef OFAPTR_H
#define OFAPTR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if defined(HAVE_STL) || defined(HAVE_STL_AUTO_PTR)
// std::auto_ptr has an identicaly interface so it can be used as an alternative
#include <memory>
#define OFauto_ptr std::auto_ptr

#else

#define INCLUDE_CSTDDEF               /* For NULL */
#include "dcmtk/ofstd/ofstdinc.h"

/** internal class, don't ever use this directly!
 *  This is needed to make the following code work:
 *  <pre>
 *  OFauto_ptr<X> func() { return new X; }
 *  OFauto_ptr<X> local_var = func();
 *  </pre>
 *
 *  The compiler will add implicit casts to OFauto_ptr_ref to make this work:
 *  <pre>
 *  OFauto_ptr<X> func() { return OFauto_ptr_ref<X>(new X); }
 *  OFauto_ptr<X> local_var = OFstatic_cast(OFauto_ptr_ref<X>, func());
 *  </pre>
 */
template <class T> class OFauto_ptr_ref
{
public:
    /// the pointer that it's all about
    T *ptr;

    /** constructs a OFauto_ptr_ref from a pointer.
     *  @param p the pointer to use.
     */
    explicit OFauto_ptr_ref(T* p) : ptr(p)
    {
    }
};

/** a simple class which makes sure a pointer is deleted once it goes out of
 *  scope. This class does NOT do reference counting!
 *  This is a reimplementation of std::auto_ptr.
 */
template <class T> class OFauto_ptr
{
  protected:
    /// the pointer that it's all about
    T *ptr;

  public:
    /** constructs a OFauto_ptr from the given pointer.
     *  You must not call delete on a pointer that is managed by OFauto_ptr!
     *  @param p the pointer
     */
    explicit OFauto_ptr(T* p = NULL) : ptr(p)
    {
    }

    /** constructs a OFauto_ptr from another OFauto_ptr. The other OFauto_ptr
     *  will be invalidated. This means he will point to NULL from now on!
     *  @param other the OFauto_ptr to get the pointer from.
     */
    OFauto_ptr(OFauto_ptr<T>& other) : ptr(other.release())
    {
    }

    /** constructs a OFauto_ptr from an OFauto_ptr_ref.
     *  @param other the OFauto_ptr_ref to get the pointer from.
     */
    OFauto_ptr(OFauto_ptr_ref<T> other) : ptr(other.ptr)
    {
    }

    /** destructor. Destroys the pointer that is managed by this instance,
     *  if there is any.
     */
    ~OFauto_ptr()
    {
        reset();
    }

    /** assign another pointer to this OFauto_ptr, possibly deleting our
     *  current pointer
     *  @param p the new pointer
     */
    void reset(T* p = NULL)
    {
        if (this->ptr)
            delete this->ptr;
        this->ptr = p;
    }

    /** get the pointer that is managed by this class
     *  @return the pointer
     */
    T* get() const { return this->ptr; }

    /** get the pointer that is managed by this class
     *  @return the pointer
     */
    T* operator->() const { return get(); }

    /** dereference the pointer that is managed by this class
     *  @return the reference
     */
    T& operator*() const { return *get(); }

    /** this is used automatically by the compiler, don't call directly
     *  @return an OFauto_ptr_ref that stores this object's pointer
     */
    operator OFauto_ptr_ref<T>()
    {
        return OFauto_ptr_ref<T>(release());
    }

    /** assigns the other pointer to this OFauto_ptr
     *  @param other other OFauto_ptr which will be invalidated
     *  @return reference to this object
     */
    OFauto_ptr& operator=(OFauto_ptr<T>& other)
    {
        reset(other.release());
        return *this;
    }

    /** assigns the other pointer to this OFauto_ptr
     *  @param other other OFauto_ptr_ref
     *  @return reference to this object
     */
    OFauto_ptr& operator=(OFauto_ptr_ref<T> other)
    {
        reset(other.ptr);
        return *this;
    }

    /** release the pointer that is managed by this class. You are now
     *  responsible for destroying that pointer yourself!
     *  @return the pointer
     */
    T* release()
    {
        T* tmp = this->ptr;
        this->ptr = NULL;
        return tmp;
    }
};

#endif

#endif
