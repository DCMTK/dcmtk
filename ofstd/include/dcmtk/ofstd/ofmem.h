/*
 *
 *  Copyright (C) 2012-2014, OFFIS e.V.
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
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: Implement fallback support for modern memory management
 *           approaches (e.g. smart pointers) for older compilers.
 *
 */

#ifndef OFMEM_H
#define OFMEM_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first
#include "dcmtk/ofstd/ofutil.h"

// use native classes when c++11 is supported
#ifdef DCMTK_USE_CXX11_STL

#include <memory>
template<typename... ARGS>
using OFshared_ptr = std::shared_ptr<ARGS...>;
template<typename... ARGS>
using OFunique_ptr = std::unique_ptr<ARGS...>;

#else //fallback implementations

#if defined HAVE_SYNC_ADD_AND_FETCH && defined HAVE_SYNC_SUB_AND_FETCH
#define OF_SHARED_PTR_COUNTER_TYPE size_t
#elif defined HAVE_INTERLOCKED_INCREMENT && defined HAVE_INTERLOCKED_DECREMENT
#if _MSC_VER <= 1200
#define OF_SHARED_PTR_COUNTER_TYPE LONG
#else
#define OF_SHARED_PTR_COUNTER_TYPE volatile LONG
#endif
#else
#define OF_SHARED_PTR_COUNTER_TYPE size_t
#define OF_SHARED_PTR_NEED_MUTEX 1
#include "dcmtk/ofstd/ofthread.h"
#endif

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/** OFshared_ptr is a smart pointer that retains shared ownership of an object through a pointer.
 *  Several OFshared_ptr objects may own the same object; the object is destroyed when the last remaining
 *  OFshared_ptr referring to it is destroyed or reset.
 *
 *  An OFshared_ptr may also own no objects, in which case it is called empty.
 *
 *  OFshared_ptr meets the requirements of CopyConstructible and CopyAssignable.
 *  @tparam T the type of the managed object, e.g. int for an OFshared_ptr behaving like an int*.
 *  @note this implementation is meant to be a subset of the c++11's std::shared_ptr that lacks the
 *    following features: swap support, support for weak references, atomic access to the managed object,
 *                        custom deleters and some functions like comparing OFshared_ptrs or read access
 *                        to the reference counter.
 *    see http://en.cppreference.com/w/cpp/memory/shared_ptr to compare OFshared_ptr against std::shared_ptr.
 */
template<typename T>
class OFshared_ptr
{
public:
    /** Constructs a shared_ptr with pt as the managed object.
     *  @param pt the managed object that this shared_ptr takes
     *    the ownership to.
     */
    OFshared_ptr( T* const pt = OFnullptr )
    : m_pData( new Data( pt ) )
    {

    }

    /** Constructs a shared_ptr which shares ownership of the object managed by other.
     *  If other manages no object, *this manages no object too.
     *  @param other another shared_ptr that this one will be copy constructed from.
     */
    OFshared_ptr( const OFshared_ptr& other )
    : m_pData( other.m_pData )
    {
        m_pData->increment_count();
    }

    /** Replaces the managed object with the one managed by other.
     *  @param other another shared_ptr that this one will be copy assigned from.
     */
    OFshared_ptr& operator=( const OFshared_ptr& other )
    {
        if( this != &other )
        {
            destroy();
            m_pData = other.m_pData;
            m_pData->increment_count();
        }
        return *this;
    }

    /** If *this owns an object and it is the last shared_ptr owning it,
     *  the object is destroyed. Otherwise does nothing.
     */
    ~OFshared_ptr()
    {
        destroy();
    }

    /** Checks if *this manages an object, i.e. whether get() != 0.
     *  @return OFTrue if *this manages an object, OFFalse otherwise.
     */
    operator OFBool() const
    {
        return m_pData->pointer() != OFnullptr;
    }

    /** Checks if *this does not manage an object, i.e. whether get() == 0.
     *  @return OFFalse if *this manages an object, OFTrue otherwise.
     */
    OFBool operator!() const
    {
        return !m_pData->pointer();
    }

    /** Dereferences pointer to the managed object.
     *  @return a reference to the managed object.
     */
    T& operator*() const
    {
        return *m_pData->pointer();
    }

    /** Dereferences pointer to the managed object.
     *  @return a pointer to the managed object.
     */
    T* operator->() const
    {
        return m_pData->pointer();
    }

    /** Returns a pointer to the managed object.
     *  @return a pointer to the managed object.
     */
    T* get() const
    {
        return m_pData->pointer();
    }

    /** Replaces the managed object with an object pointed to by pt.
     *  @param pt pointer to an object to acquire ownership of.
     */
    void reset( T* const pt = OFnullptr )
    {
        destroy();
        m_pData = new Data( pt );
    }

private:
    /** Helper class containing the reference to the
     *  managed object and the reference counter.
     */
    class Data
    {
    public:
        /** Construct reference and counter from a raw pointer.
         *  @param pt the raw pointer that becomes the managed object.
         */
        Data( T* const pt )
        : m_Count( 1 )
        , m_pT( pt )
#ifdef OF_SHARED_PTR_NEED_MUTEX
        , m_Mutex()
#endif
        {

        }

        /** Delete the managed object.
         *  We are deleted only if the reference counter reached zero,
         *  so we now have to delete the managed object.
         */
        ~Data()
        {
            delete m_pT;
        }

        /** Increment the reference counter in a thread-safe and (if possible)
         *  lock-free fashion.
         */
        void increment_count()
        {
#ifdef HAVE_SYNC_ADD_AND_FETCH
            __sync_add_and_fetch( &m_Count, 1 );
#elif defined HAVE_INTERLOCKED_INCREMENT
            InterlockedIncrement( &m_Count );
#else
            m_Mutex.lock();
            ++m_Count;
            m_Mutex.unlock();
#endif
        }

        /** Decrement the reference counter in a thread-safe and (if possible)
         *  lock-free fashion.
         *  @return OFTrue if the reference counter reached zero, OFFalse otherwise.
         */
        OFBool decrement_count()
        {
#ifdef HAVE_SYNC_SUB_AND_FETCH
            return !__sync_sub_and_fetch( &m_Count, 1 );
#elif defined HAVE_INTERLOCKED_DECREMENT
            return !InterlockedDecrement( &m_Count );
#else
            m_Mutex.lock();
            const OFBool result = !--m_Count;
            m_Mutex.unlock();
            return result;
#endif
        }

        /** Get the managed object.
         *  @return a pointer to the managed object.
         */
        T* pointer() const
        {
            return m_pT;
        }

    private:
        // Just to silence some warnings.
        Data(const Data&);
        Data& operator=(const Data&);

        /// The counter.
        OF_SHARED_PTR_COUNTER_TYPE m_Count;
        /// The pointer to the managed object.
        T* const m_pT;
#ifdef OF_SHARED_PTR_NEED_MUTEX
        /// A mutex for platforms that don't support lock-free counters.
        OFMutex m_Mutex;
#endif
    };

    /** decrement the reference counter, destroy the managed
     *  object and the counter if the counter reached zero.
     */
    void destroy()
    {
        if( m_pData->decrement_count() )
            delete m_pData;
    }

    /** A pointer to the management data structure containing the actual
     *  reference to the managed object and the reference counter.
     */
    Data* m_pData;
};

/** OFunique_ptr is a smart pointer that retains sole ownership of an object through a pointer and destroys
 *  that object when the unique_ptr goes out of scope. No two unique_ptr instances can manage the same object.
 *
 *  The object is destroyed and its memory deallocated when either of the following happens:
 *  - unique_ptr managing the object is destroyed
 *  - unique_ptr managing the object is assigned another pointer via reset().
 *
 *  A unique_ptr may also own no objects, in which case it is called empty.
 *
 *  OFunique_ptr is NOT CopyConstructible or CopyAssignable.
 *  @tparam T the type of the managed object, e.g. int for an OFunique_ptr behaving like an int*.
 *  @note this implementation is meant to be a subset of the c++11's std::unique_ptr that lacks the
 *    following features: swap support, custom deleters, specialized handling of pointers to
 *                        arrays and some functions like comparing OFunique_ptrs or creating a hash key.
 *    see http://en.cppreference.com/w/cpp/memory/unique_ptr to compare OFunique_ptr against std::unique_ptr.
 */
template<typename T>
class OFunique_ptr
{
public:
    /** typedef of pointer type T* for template programmers.
     */
    typedef T* pointer;
    /** T, the type of the object managed by this unique_ptr.
     */
    typedef T element_type;

    /** Constructs an empty OFunique_ptr
     */
    OFunique_ptr() : m_pData( OFnullptr ) {}

    /** Constructs a OFunique_ptr which owns p.
     *  @param p the pointer that's going to be owned by this unique_ptr.
     */
    explicit OFunique_ptr( pointer p ) : m_pData( p ) {}

    /** Move constructor.
     *  The move constructor <i>moves</i> the ownership of the managed
     *  object from its parameter to the newly created OFunique_ptr
     *  object, effectively emptying the former OFunique_ptr
     *  object.
     *  @param rhs an <i>rvalue reference</i> to another OFunique_ptr
     *    object, e.g. obtained via OFmove.
     *  @details
     *  <h3>Example</h3>
     *  This example shows how to move the ownership of a managed
     *  object from one OFunique_ptr to another. Note that the
     *  ownership always remains unique in this example, even
     *  if an error occurred while reading the Dataset.
     *  Therefore no memory leaks are possible!
     *  @code
     *  void consumeDataset( OFunique_ptr<DcmDataset> pDataset )
     *  . . .
     *  OFunique_ptr<DcmDataset> pDataset( new DcmDataset );
     *  if( pDataset && pDataset->read( ... ).good() )
     *  {
     *    // Error: copy constructor not available, as it would
     *    // compromise the unique ownership principle
     *    // consumeDataset( pDataset );
     *
     *    // OFmove allows us to 'tell' OFunique_ptr we want
     *    // to give away the ownership.
     *    consumeDataset( OFmove( pDataset ) );
     *  }
     *  @endcode
     */
    OFunique_ptr( OFrvalue_ref(OFunique_ptr) rhs )
#ifndef DOXYGEN
    : m_pData( rhs.m_pData )
    {
        OFrvalue_access(rhs).m_pData = OFnullptr;
    }
#else
    ;
#endif

    /** Move assignment operator.
     *  The move assignment operator <i>moves</i> the ownership of the
     *  managed object from its parameter to itself, effectively emptying
     *  the other OFunique_ptr object. See OFunique_ptr's <i>move constructor</i>
     *  for more information.
     *  @param rhs an <i>rvalue reference</i> to another OFunique_ptr
     *    object, e.g. obtained via OFmove.
     */
    OFunique_ptr& operator=( OFrvalue_ref(OFunique_ptr) rhs )
#ifndef DOXYGEN
    {
        OFunique_ptr& other = OFrvalue_access(rhs);
        if( &other != this )
        {
            reset( other.m_pData );
            other.m_pData = OFnullptr;
        }
        return *this;
    }
#else
    ;
#endif

    /** If get() == OFnullptr there are no effects -- otherwise, the owned object is destroyed.
     */
    ~OFunique_ptr() { delete m_pData; }

    /** Replaces the managed object. The previously owned object is deleted if the unique_ptr
     *  was not empty.
     *  @param p the new pointer to be owned, defaults to OFnullptr.
     */
    void reset( pointer p = OFnullptr ) { delete m_pData; m_pData = p; }

    /** Releases the ownership of the managed object if any.
     *  Retrieves the owned object (if any) and lets the unique_ptr become empty.
     *  @return same as get().
     */
    pointer release() { pointer const p = m_pData; m_pData = OFnullptr; return p; }

    /** Returns a pointer to the managed object or OFnullptr if no object is owned.
     *  @return Pointer to the managed object or OFnullptr if no object is owned.
     */
    pointer get() const { return m_pData; }

    /** Checks whether *this owns an object, i.e. whether get() != OFnullptr.
     *  @return get() != OFnullptr.
     */
    operator bool() const { return m_pData != OFnullptr; }

    /** Checks whether *this does NOT own an object, i.e. whether get() == OFnullptr.
     *  @return get() == OFnullptr.
     */
    bool operator!() const { return m_pData == OFnullptr; }

    /** Access the object owned by *this.
     *  @return the object owned by *this, i.e. *get().
     */
    T& operator*() const { return *m_pData; }

    /** Access the object owned by *this.
     *  @return same as get().
     */
    pointer operator->() const { return m_pData; }

private:
    /// Disable copy construction.
    OFunique_ptr(const OFunique_ptr&);
    /// Disable copy assignment.
    OFunique_ptr& operator=(const OFunique_ptr&);

    /// The underlying (raw) pointer.
    pointer m_pData;
};

#endif //c++11

#endif //OFMEM_H
