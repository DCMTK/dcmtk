/*
 *
 *  Copyright (C) 2014-2017, OFFIS e.V.
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
 *  Purpose: Implement fallback support for modern techniques defined
 *           in the STL's <utility> header (e.g. move semantics)
 *           for older compilers.
 */

#ifndef OFUTIL_H
#define OFUTIL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftraits.h"
#include "dcmtk/ofstd/oftypes.h"

/** @file ofutil.h
 *  Implement fallback support for modern techniques defined
 *  in the STL's <utility> header (e.g.\ move semantics)
 *  for older compilers.
 */

// -------------------- misc C++11 / non C++11 utils --------------------

#ifdef HAVE_CXX11

#include <utility>
#include <tuple>
#define OFmove std::move
#define OFswap std::swap

// OFrvalue simply equals 'identity', as C++11 natively handles
// rvalues / prvalues and so on.
template<typename T>
using OFrvalue = T;

#define OFrvalue_ref(T) T&&
#define OFrvalue_access(RV) RV

#else // fallback implementations

#ifndef DOXYGEN

// Meta-template to select the base class for OFrvalue
template<typename T,OFBool>
struct OFrvalue_storage
{
    // Helper template to wrap types that we can't derive from,
    // e.g. primitive types.
    class type
    {
    public:
        // copy constructor should be fine for primitive types.
        inline type(const T& pt)
        : t( pt ) {}
        inline type(const OFrvalue_storage& rhs)
        : t( rhs.pt ) {}

        // automatic conversion to the underlying type
        inline operator T&() const { return OFconst_cast( T&, t ); }

    private:
        // the actual object
        T t;
    };
};

// specialization for compound types
template<typename T>
struct OFrvalue_storage<T,OFTrue>
{
    // simply use T itself as base
    typedef T type;
};

// SFINAE to detect if a type is derivable from
template<typename T>
class OFrvalue_base
{
    // magic SFINAE stuff stolen from en.cppreference.com
    struct no_type {};
    struct yes_type {double d;};
    template<typename X>
    static yes_type sfinae(int X::*);
    template<typename X>
    static no_type sfinae(...);

public:
    // employ SFINAE + template specialization to select
    // the base type
    typedef OFTypename OFrvalue_storage
    <
        T,
        sizeof(sfinae<T>(OFnullptr)) == sizeof(yes_type)
    >::type type;
};

#endif // NOT DOXYGEN

/** A helper class to 'tag' objects as <i>rvalues</i> to help
 *  DCMTK's move emulation employed on pre C++11 compilers.
 *  @tparam T the base type an rvalue should be create of.
 *  @details OFrvalue wrapps the type T inside a zero-overhead
 *    object employing T's move constructor when possible.
 *  @note When C++11 support is available, OFrvalue<T> will
 *    simply be a type alias for <i>T</i>, since a C++11 compiler
 *    handles rvalue reference conversions natively.
 *  @details
 *  <h2>Example</h2>
 *  This example describes how to move an object of type
 *  OFunique_ptr out of a function by using OFrvalue.
 *  @code
 *  OFrvalue<OFunique_ptr<DcmDataset> > getDataset()
 *  {
 *    return OFunique_ptr<DcmDataset>( new DcmDataset );
 *  }
 *  . . .
 *  OFunique_ptr<DcmDataset> pDataset = getDataset();
 *  @endcode
 *  @warning Some compilers might require you to use the following
 *    code instead, as older versions of the C++ standard allowed
 *    the compiler to use the copy constructor for binding an
 *    rvalue to an lvalue reference.
 *    Use this code template instead to achieve maximum portability:
 *  @code
 *  OFrvalue<OFunique_ptr<DcmDataset> > getDataset()
 *  {
 *    OFunique_ptr<DcmDataset> pDataset( new DcmDataset );
 *    return OFmove( pDataset );
 *  }
 *  @endcode
 */
template<typename T>
struct OFrvalue : OFrvalue_base<T>::type
{
#ifndef DOXYGEN
    // allow to move construct from lvalue references
    inline OFrvalue(const T& t) : OFrvalue_base<T>::type( *OFreinterpret_cast( const OFrvalue*,  &t ) ) {}
    // copy-construct from an rvalue reference
    inline OFrvalue(const OFrvalue& rv) : OFrvalue_base<T>::type( rv ) {}
    // poor man's in-place construction
    template<typename X>
    inline explicit OFrvalue( X x ) : OFrvalue_base<T>::type( x ) {}
    template<typename X0,typename X1>
    inline explicit OFrvalue( X0 x0, X1 x1 ) : OFrvalue_base<T>::type( x0, x1 ) {}
    template<typename X0,typename X1,typename X2>
    inline explicit OFrvalue( X0 x0, X1 x1, X2 x2 ) : OFrvalue_base<T>::type( x0, x1, x2 ) {}
#endif // NOT DOXYGEN
};

#ifdef DOXYGEN
/** Determines <i>rvalue reference</i> type for the type <kbd>T</kbd>.
 *  @param T the base type to determine the rvalue reference type for.
 *  @note <i>OFrvalue_ref(T)</i> will expand to <kbd>T&&</kbd> when
 *    C++11 support is available. Otherwise DCMTK's move emulation will
 *    be used, employing an unspecified type to implement rvalue references.
 *  @details
 *  <h2>Example</h2>
 *  This example shows how to implement the <i>move constructor</i> and
 *  <i>move assignment</i> for a custom class in a portable fashion
 *  (employing C++11's native features when available and using DCMTK's
 *  move emulation otherwise).
 *  @code
 *  class MyMovable
 *  {
 *  public:
 *    MyMovable( OFrvalue_ref(MyMovable) rhs )
 *    : m_hDatabase( rhs.m_hDatabase )
 *    {
 *      // You need to use OFrvalue_access to get write access
 *      // to rvalue references when DCMTK's move emulation
 *      // is used.
 *      OFrvalue_access(rhs).m_hDatabase = OFnullptr;
 *    }
 *
 *    MyMovable& operator=( OFrvalue_ref(MyMovable) rvrhs )
 *    {
 *      // You may bind the rvalue reference to an lvalue
 *      // reference to ease access.
 *      MyMovable& rhs = OFrvalue_access(rvrhs);
 *      if( this != &rhs )
 *      {
 *        disconnectDatabase( m_hDatabase );
 *        m_hDatabase = rhs.m_hDatabase;
 *        rhs.m_hDatabase = OFnullptr;
 *      }
 *      return *this;
 *    }
 *  };
 *  @endcode
 */
#define OFrvalue_ref(T) unspecified
#else // NOT DOXYGEN
#define OFrvalue_ref(T) const OFrvalue<T >&
#endif

/** Obtain an lvalue reference from an rvalue reference.
 *  DCMTK's move emulations does restrict write access to rvalue references
 *  due to compiler limitations.
 *  This method enables you to workaround this restriction by converting
 *  DCMTK's emulated rvalue references to lvalue references.
 *  @note Native rvalue references from C++11 don't need this workaround,
 *   therefore <i>OFrvalue_access</i> has no effect when C++11 support is
 *   available.
 *  @param rv an rvalue reference, e.g. the parameter of a <i>move constructor</i>.
 */
template<typename T>
T& OFrvalue_access( OFrvalue_ref(T) rv )
{
#ifndef DOXYGEN
    return OFconst_cast( OFrvalue<T>&, rv );
#endif
}

/** Obtains an rvalue reference to its argument and converts it
 *  to an xvalue. OFmove is meant to 'mark' an object for a
 *  move operation, e.g. to move an OFVector object into another
 *  OFVector instance instead of copying it.
 *  @note OFmove will be an alias for std::move when native
 *    move semantics are supported (C++11 support is available).
 *    Otherwise DCMTK's move emulation will be used. This means
 *    you will have to specify rvalues (e.g. function return values)
 *    employing the OFrvalue class template.
 *  @param t The object to move.
 *  @see OFrvalue
 *  @see OFrvalue_ref
 */
template<typename T>
#ifndef DOXYGEN
OFrvalue<T>& OFmove( T& t )
{
    return *OFreinterpret_cast( OFrvalue<T>*, &t );
}

template<typename T>
OFrvalue<T>& OFmove( OFrvalue<T>& rv )
{
    return rv;
}

template<typename T>
OFrvalue<T>& OFmove( const OFrvalue<T>& rv )
{
    return OFconst_cast( OFrvalue<T>&, rv );
}
#else // NOT DOXYGEN
OFconstexpr xvalue OFmove( T< unspecified > t );
#endif // DOXYGEN

/** Exchanges the given values.
 *  OFswap is an alias for std::swap if C++11 is supported.
 *  Otherwise OFswap simply creates a temporary copy of one
 *  argument to exchange both values.
 *  @note As intended for std::swap, there are some
 *    specializations for OFswap available, e.g. for OFoptional,
 *    which specializes OFswap to exchange optional objects
 *    more efficiently. When creating your own specializations
 *    for OFswap, make sure to specialize std::swap instead
 *    when C++11 support is available.
 *  @param t0 An object to be exchanged.
 *  @param t1 The object to be exchanged with t0.
 */
template<typename T>
void OFswap( T& t0, T& t1 )
#ifndef DOXYGEN
{
    T temp( OFmove( t0 ) );
    t0 = OFmove( t1 );
    t1 = OFmove( temp );
}
#else // NOT DOXYGEN
;
#endif // DOXYGEN

#endif // NOT C++11

// -------------------- STL pair --------------------

#ifdef HAVE_STL_MAP

// Use native pair class, to be compatible to std::map
#define OFPair std::pair
#define OFMake_pair std::make_pair

#else // fallback implementation of std::pair

/** a pair - this implements parts of std::pair's interface.
 */
template<typename K, typename V> class OFPair
{
public:

    /** this is the first value of the pair */
    K first;

    /** this is the second value of the pair */
    V second;

    /** default constructor */
    OFPair() : first(), second() { }

    /** construct a OFPair for the two given values
     *  @param f the value for first.
     *  @param s the value for second.
     */
    OFPair(const K& f, const V& s) : first(f), second(s) { }

    /** copy constructor
     *  @param p Other OFPair to copy from.
     */
    template<class OK, class OV>
    OFPair(const OFPair<OK, OV>& p) : first(p.first), second(p.second) { }

    /** copy constructor
     *  @param p Other OFPair to copy from.
     */
    OFPair(const OFPair& p) : first(p.first), second(p.second) { }

    /** assignment operator */
    OFPair<K, V>& operator=(const OFPair<K, V>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }
};

/** helper function to create a pair. This is similar to std::make_pair()
 *  @param first the first part of the pair
 *  @param second the second art of the pair
 *  @relates OFPair
 *  @return the pair (first, second)
 */
template<typename K, typename V>
OFPair<K, V> OFMake_pair(const K& first, const V& second)
{
    return OFPair<K, V>(first, second);
}

#endif // HAVE_STL_MAP - fallback implementation of OFPair

// -------------------- STL tuple --------------------

#ifdef HAVE_STL_TUPLE

#ifdef HAVE_CXX11

template<std::size_t Index,typename T>
constexpr auto OFget( T&& t ) -> decltype( std::get<Index>( std::forward<T>( t ) ) )
{
    return std::get<Index>( std::forward<T>( t ) );
}

template<typename X,typename T>
constexpr auto OFget( T&& t ) -> decltype( std::get<X>( std::forward<T>( t ) ) )
{
    return std::get<X>( std::forward<T>( t ) );
}

template<typename Tuple>
using OFtuple_size = std::tuple_size<Tuple>;

template<std::size_t Index,typename Tuple>
using OFtuple_element = std::tuple_element<Index,Tuple>;

#else // HAVE_CXX11

template<typename Tuple>
struct OFtuple_size : STD_NAMESPACE tuple_size<Tuple> {};

template<size_t Index,typename Tuple>
struct OFtuple_element : STD_NAMESPACE tuple_element<Index,Tuple> {};

template<size_t Index,typename T>
OFTypename OFtuple_element<Index,T>::type OFget( T& t ) { return STD_NAMESPACE get<Index>( t ); }

template<size_t Index,typename T>
OFTypename OFtuple_element<Index,T>::type OFget( const T& t ) { return STD_NAMESPACE get<Index>( t ); }

#endif // NOT HAVE_CXX11

#else // HAVE_STL_TUPLE

template<typename Tuple>
struct OFtuple_size;
template<size_t Index,typename Tuple>
struct OFtuple_element;

// specialization of OFtuple_size for OFPair -> 2
template<typename K,typename V>
struct OFtuple_size<OFPair<K,V> >
: OFintegral_constant<size_t,2> {};

// specialization of OFtuple_element for OFPair
// 0 -> K
template<typename K,typename V>
struct OFtuple_element<0,OFPair<K,V> >
{
    typedef K type;
};

// specialization of OFtuple_element for OFPair
// 1 -> V
template<typename K,typename V>
struct OFtuple_element<1,OFPair<K,V> >
{
    typedef V type;
};

// metafunction to apply OFget to OFPair
template<size_t Element>
struct OFpair_element;

// specialization for 0 -> first
template<>
struct OFpair_element<0>
{
    template<typename K,typename V>
    static K& from( OFPair<K,V>& p ) { return p.first; }
    template<typename K,typename V>
    static const K& from( const OFPair<K,V>& p ) { return p.first; }
};

// specialization for 1 -> second
template<>
struct OFpair_element<1>
{
    template<typename K,typename V>
    static V& from( OFPair<K,V>& p ) { return p.second; }
    template<typename K,typename V>
    static const V& from( const OFPair<K,V>& p ) { return p.second; }
};

// overload of OFget for OFPair, see above metafunction 'OFpair_element'
template<size_t Element,typename K,typename V>
typename OFtuple_element<Element,OFPair<K,V> >::type& OFget( OFPair<K,V>& p )
{
    return OFpair_element<Element>::from( p );
}

// overload of OFget for const OFPair, see above metafunction 'OFpair_element'
template<size_t Element,typename K,typename V>
const typename OFtuple_element<Element,OFPair<K,V> >::type& OFget( const OFPair<K,V>& p )
{
    return OFpair_element<Element>::from( p );
}

// tag to identify invalid OFtuple elements, needed for emulating
// variadic templates.
struct OFtuple_nil;

// include generated forward declaration for OFtuple.
#include "dcmtk/ofstd/variadic/tuplefwd.h"

#endif // HAVE_STL_TUPLE

#ifdef DOXYGEN // doxygen documentation of OFtuple utils

/** A metafunction to determine the size of a tuple.
 *  @tparam Tuple a tuple type, e.g. an instance of OFtuple.
 *  @pre Tuple is a tuple type, see @ref tuple_types "Tuple Types"
 *    for definition.
 *  @return OFtuple_size is derived from an appropriate instance of
 *    OFintegral_constant if the preconditions are met. This means
 *    OFtuple_size declares a static member constant <i>value</i>
 *    set to the tuple's size.
 *  @relates OFPair
 *  @relates OFtuple
 *  @details
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  typedef OFtuple<OFString,size_t,OFVector<int> > MyTuple;
 *  typedef OFPair<OFString,MyTuple> MyPair;
 *  COUT << "OFtuple_size<MyTuple>::value: " << OFtuple_size<MyTuple>::value << OFendl;
 *  COUT << "OFtuple_size<MyPair>::value: " << OFtuple_size<MyPair>::value << OFendl;
 *  @endcode
 *  <b>Output:</b>
 *  @verbatim
    OFtuple_size<MyTuple>::value: 3
    OFtuple_size<MyPair>::value: 2
    @endverbatim
 *
 */
template<typename Tuple>
<metafunction> OFtuple_size;

/** A metafunction to determine the type of one element of a tuple.
 *  @tparam Index the index of the element its type should be determined.
 *  @tparam Tuple a tuple type, e.g. an instance of OFtuple.
 *  @pre Tuple is a tuple type, see @ref tuple_types "Tuple Types"
 *    for definition.
 *  @pre Index is a valid index , essentially: <kbd>Index < OFtuple_size<Tuple>::value</kbd>.
 *  @return if the preconditions are met, OFtuple_element declares a member
 *    type alias <i>type</i> that yields the type of the element at the given index.
 *  @relates OFtuple
 *  @details
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  typedef OFPair<OFString,size_t> MyPair;
 *  typedef OFtuple<OFtuple_element<0,MyPair>::type,OFtuple_element<1,MyPair>::type> MyTuple;
 *  MyPair pair( "Hello World", 42 );
 *  MyTuple tuple( pair ); // Works, since both elements' types are the same as within MyPair.
 *  @endcode
 *
 */
template<size_t Index,typename Tuple>
<metafunction> OFtuple_element;

/** A function template to access an element of a tuple.
 *  @tparam Index the index of the element that should be accessed.
 *  @tparam Tuple a tuple type, e.g. an instance of OFtuple. This parameter
 *    is deduced automatically.
 *  @param tuple a reference to the tuple to access an element of.
 *  @pre Tuple is a tuple type, see @ref tuple_types "Tuple Types"
 *    for definition.
 *  @pre Index is a valid index , essentially: <kbd>Index < OFtuple_size<Tuple>::value</kbd>.
 *  @return a reference to the tuple's element at the given index.
 *  @relates OFtuple
 *  @details
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  OFtuple<OFString,size_t,OFVector<int> > myTuple;
 *  OFget<0>( myTuple ) = "Hamish Alexander";
 *  OFget<1>( myTuple ) = 23;
 *  OFget<2>( myTuple ).push_back( 42 );
 *  @endcode
 */
template<size_t Index,typename Tuple>
typename OFtuple_element<Index,Tuple>::type& OFget( Tuple& tuple );

/** A function template to access an element of a tuple.
 *  @tparam Index the index of the element that should be accessed.
 *  @tparam Tuple a tuple type, e.g. an instance of OFtuple. This parameter
 *    is deduced automatically.
 *  @param tuple a const reference to the tuple to access an element of.
 *  @pre Tuple is a tuple type, see @ref tuple_types "Tuple Types"
 *    for definition.
 *  @pre Index is a valid index , essentially: <kbd>Index < OFtuple_size<Tuple>::value</kbd>.
 *  @return a const reference to the tuple's element at the given index.
 *  @relates OFtuple
 *  @details
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  const OFtuple<OFString,size_t,OFBool> myConstTuple( "Homer Simpson", 38, OFTrue );
 *  if( OFget<0>( myConstTuple ) == "Homer Simpson" )
 *  {
 *    // OFget<1>( myConstTuple ) = 23; INVALID, myConstTuple is const!
 *    OFBool isMale = OFget<2>( myConstTuple );
 *    if( isMale )
 *      COUT << OFget<0>( myConstTuple ) << ", age "
 *           << OFget<1>( myConstTuple ) << " is male." << OFendl;
 *  }
 *  @endcode
 *  <b>Output:</b>
 *  @verbatim
    Homer Simpson, age 38 is male.
    @endverbatim
 */
template<size_t Index,typename Tuple>
const typename OFtuple_element<Index,Tuple>::type& OFget( const Tuple& tuple );
#endif // DOXYGEN

// -------------------- misc utils (OFinplace etc.) --------------------

#ifndef DOXYGEN

// OFin_place hacks, look at the doxygen documentation instead if
// you know what's good for you!
class DCMTK_OFSTD_EXPORT OFin_place_tag { OFin_place_tag(); };
typedef OFin_place_tag(&OFin_place_t)();
#define OFin_place_type_t(T) OFin_place_tag(&)(T&)
#define OFin_place_index_t(I) OFin_place_tag(&)(OFintegral_constant<size_t,I>&)
DCMTK_OFSTD_EXPORT OFin_place_tag OFin_place();
template<typename T>
OFin_place_tag OFin_place(T&) { return OFin_place(); }
template<size_t I>
OFin_place_tag OFin_place(OFintegral_constant<size_t,I>&) { return OFin_place(); }

#else // NOT DOXYGEN

/** @defgroup OFin_place_helpers_brief
 *  @details Tools for in-place construction of objects, e.g. certain OFvariant alternatives.
 *  @defgroup OFin_place_helpers Tools for in-place construction
 *  @details
 *  <b><em style="color:#7f0000">#include</em> <span class="keyword">"dcmtk/ofstd/ofutil.h"</span></b><br><br>
 *  @copydoc OFin_place_helpers_brief
 *  <table class="memberdecls">
 *    <tr class="heading">
 *      <td colspan="2"><div><h2 class="groupheader">Type Definitions</h2></div></td>
 *    </tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_t">OFin_place_t</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">A type for tagging an in-place constructor as such. <a href="#OFin_place_t">More...</a></td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *    <tr><td class="memTemplParams" colspan="2">template&lt;typename T&gt;</td></tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_type_t">OFin_place_type_t(T)</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">A type for tagging an in-place constructor for a certain type as such. <a href="#OFin_place_type_t">More...</a></td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *    <tr><td class="memTemplParams" colspan="2">template&lt;size_t I&gt;</td></tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_index_t">OFin_place_index_t(I)</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">A type for tagging an in-place constructor based on a certain index  as such. <a href="#OFin_place_index_t">More...</a></td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *  </table>
 *  <table class="memberdecls">
 *    <tr class="heading">
 *      <td colspan="2"><div><h2 class="groupheader">Global Constants</h2></div></td>
 *    </tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><a href="#OFin_place_t">OFin_place_t</a></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_generic">OFin_place</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">
 *        A constant of type <a href="#OFin_place_t">OFin_place_t</a> that may be used for in-place construction.
 *        <a href="#OFin_place_generic">More...</a>
 *      </td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *    <tr><td class="memTemplParams" colspan="2">template&lt;typename T&gt;</td></tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><a href="#OFin_place_type_t">OFin_place_type_t(T)</a></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_type">OFin_place&lt;T&gt;</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">
 *        A constant of type <a href="#OFin_place_type_t">OFin_place_type_t(T)</a> that may be used for in-place construction.
 *        <a href="#OFin_place_type">More...</a>
 *      </td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *    <tr><td class="memTemplParams" colspan="2">template&lt;size_t I&gt;</td></tr>
 *    <tr>
 *      <td class="memItemLeft" align="right" valign="top"><a href="#OFin_place_index_t">OFin_place_index_t(I)</a></td>
 *      <td class="memItemRight" valign="bottom"><a href="#OFin_place_index">OFin_place&lt;I&gt;</a></td>
 *    </tr>
 *    <tr>
 *      <td class="mdescLeft"></td>
 *      <td class="mdescRight">
 *        A constant of type <a href="#OFin_place_index_t">OFin_place_index_t(I)</a> that may be used for in-place construction.
 *        <a href="#OFin_place_index">More...</a>
 *      </td>
 *    </tr>
 *    <tr><td class="memSeparator" colspan="2"></td></tr>
 *  </table>
 *  <h2 class="groupheader">Type Definition Documentation</h2>
 *  @anchor OFin_place_t
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memname">
 *        <span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em> OFin_place_t
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A type for tagging an in-place constructor as such.<br>
 *      <dl></dl>
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename T>
 *      class Wrapper
 *      {
 *      public:
 *        // Will copy construct the wrapped value from a T.
 *        Wrapper( const T& t );
 *
 *        // Will in-place construct the value from the given arguments,
 *        // calling T( arguments... ) internally, without unnecessary
 *        // copies.
 *        template<typename... Arguments>
 *        Wrapper( OFin_place_t, Arguments... arguments );
 *
 *      private:
 *        // ... wrapper implementation ...
 *      };
 *      @endcode
 *    </div>
 *  </div>
 *  @anchor OFin_place_type_t
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memtemplate">template<typename T></div>
 *      <div class="memname">
 *        <span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em> OFin_place_type_t(T)
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A type for tagging an in-place constructor for a certain type as such.
 *      <br>
 *      <dl class="tparams">
 *        <dt>Template Parameters<dt>
 *        <dd><span class="paramname">T</span> the type this in-pace constructor handles, i.e. the type that will be constructed.</dd>
 *      </dl>
 *      @note Pre C++11 compilers do not support alias templates, therefore, OFin_place_type_t is implemented
 *        using preprocessor macros internally. This is why you need to use curved brackets instead of angled ones.
 *
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename A,typename B>
 *      class Union
 *      {
 *      public:
 *        // Will copy construct the wrapped value as an A from a.
 *        Union( const A& a );
 *
 *        // Will copy construct the wrapped value as a B from b.
 *        Union( const B& b );
 *
 *        // Will in-place construct the value as an A from the given
 *        // arguments, calling A( arguments... ) internally, without
 *        // unnecessary copies.
 *        template<typename... Arguments>
 *        Union( OFin_place_type_t(A), Arguments... arguments );
 *
 *        // Will in-place construct the value as a B from the given
 *        // arguments, calling B( arguments... ) internally, without
 *        // unnecessary copies.
 *        template<typename... Arguments>
 *        Union( OFin_place_type_t(B), Arguments... arguments );
 *
 *      private:
 *        // ... union implementation ...
 *      };
 *      @endcode
 *    </div>
 *  </div>
 *  @anchor OFin_place_index_t
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memtemplate">template<size_t I></div>
 *      <div class="memname">
 *        <span class="keyword">typedef</span> <em style="color:#7f0000;opacity:.7">unspecified</em> OFin_place_index_t(I)
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A type for tagging an in-place constructor for a certain index as such.<br>
 *      <dl class="tparams">
 *        <dt>Template Parameters<dt>
 *        <dd>
 *          <span class="paramname">I</span> the index this in-pace constructor handles, i.e. the zero
 *          based index of the type that will be constructed.
 *        </dd>
 *      </dl>
 *      @note Pre C++11 compilers do not support alias templates, therefore, OFin_place_index_t is implemented
 *        using preprocessor macros internally. This is why you need to use curved brackets instead of angled ones.
 *
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename A,typename B>
 *      class Union
 *      {
 *      public:
 *        // Will copy construct the wrapped value as an A from a.
 *        Union( const A& a );
 *
 *        // Will copy construct the wrapped value as a B from b.
 *        Union( const B& b );
 *
 *        // Will in-place construct the value as an A from the given
 *        // arguments, calling A( arguments... ) internally, without
 *        // unnecessary copies.
 *        // This will even work if A and B refer to the same type.
 *        template<typename... Arguments>
 *        Union( OFin_place_index_t(0), Arguments... arguments );
 *
 *        // Will in-place construct the value as a B from the given
 *        // arguments, calling B( arguments... ) internally, without
 *        // unnecessary copies.
 *        // This will even work if A and B refer to the same type.
 *        template<typename... Arguments>
 *        Union( OFin_place_index_t(1), Arguments... arguments );
 *
 *      private:
 *        // ... union implementation ...
 *      };
 *      @endcode
 *    </div>
 *  </div>
 *  <h2 class="groupheader">Global Constant Documentation</h2>
 *  @anchor OFin_place_generic
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memname">
 *        <a href="#OFin_place_t">OFin_place_t</a> OFin_place
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A constant of type <a href="#OFin_place_t">OFin_place_t</a> that may be used for in-place construction.<br>
 *      @remarks OFin_place is actually an overloaded function, but instead of calling it
 *        (which one should never do), its address is used as a tag, since the type of
 *        its address differs depending on which overload and template parameters are used.
 *        See http://en.cppreference.com/w/cpp/utility/in_place for more information.
 *
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename T>
 *      class Wrapper; // see OFin_place_t example
 *      // ...
 *      // will construct an OFString and then copy construct the value in the wrapper
 *      Wrapper<OFString>( "Hello World" );
 *      // will in-place construct the value in the wrapper
 *      Wrapper<OFString>( OFin_place, "Hello World" );
 *      // this also works with multiple arguments:
 *      // will take only the fist five characters of the const char*
 *      Wrapper<OFString>( OFin_place, "Hello World", 5 );
 *      @endcode
 *    </div>
 *  </div>
 *  @anchor OFin_place_type
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memtemplate">template<typename T></div>
 *      <div class="memname">
 *        <a href="#OFin_place_type_t">OFin_place_type_t(T)</a> OFin_place<T>
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A constant of type <a href="#OFin_place_type_t">OFin_place_type_t(T)</a> that may be used for in-place construction.<br>
 *      <dl class="tparams">
 *        <dt>Template Parameters<dt>
 *        <dd><span class="paramname">T</span> the type for selecting an in-pace constructor, i.e. the type that will be constructed.</dd>
 *      </dl>
 *      @remarks OFin_place is actually an overloaded function, but instead of calling it
 *        (which one should never do), its address is used as a tag, since the type of
 *        its address differs depending on which overload and template parameters are used.
 *        See http://en.cppreference.com/w/cpp/utility/in_place for more information.
 *
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename A,typename B>
 *      class Union; // see OFin_place_type_t example
 *      // ...
 *      // will construct an OFString and then copy construct the value inside the union
 *      Union<int,OFString>( OFString( "Hello World" ) );
 *      // will in-place construct an OFString value inside the union
 *      // with only the fist five characters
 *      Union<int,OFString>( OFin_place<OFString>, "Hello World", 5 );
 *      // will construct an integer value inside the union by casting
 *      // the address of the character array constant to int
 *      Union<int,OFString>( OFin_place<int>, "Hello World" );
 *      @endcode
 *    </div>
 *  </div>
 *  @anchor OFin_place_index
 *  <div class="memitem">
 *    <div class="memproto">
 *      <div class="memtemplate">template<size_t I></div>
 *      <div class="memname">
 *        <a href="#OFin_place_index_t">OFin_place_index_t(I)</a> OFin_place&lt;I&gt;
 *      </div>
 *    </div>
 *    <div class="memdoc">
 *      <br>A constant of type <a href="#OFin_place_index_t">OFin_place_index_t(I)</a> that may be used for in-place construction.<br>
 *      <dl class="tparams">
 *        <dt>Template Parameters<dt>
 *        <dd>
 *          <span class="paramname">I</span> the index for selecting an in-pace constructor, i.e. the
 *          zero based index of the type that will be constructed.
 *        </dd>
 *      </dl>
 *      @remarks OFin_place is actually an overloaded function, but instead of calling it
 *        (which one should never do), its address is used as a tag, since the type of
 *        its address differs depending on which overload and template parameters are used.
 *        See http://en.cppreference.com/w/cpp/utility/in_place for more information.
 *
 *      <b>Usage Example:</b><br>
 *      @code{.cpp}
 *      template<typename A,typename B>
 *      class Union; // see OFin_place_index_t example
 *      // ...
 *      // error, cannot determine which constructor shall be used,
 *      // since both take an int
 *      Union<int,int>( 3 );
 *      // will in-place construct an int value inside the union
 *      // tagging it as an A
 *      Union<int,int>( OFin_place<0>, 3 );
 *      // will in-place construct an int value inside the union
 *      // tagging it as a B
 *      Union<int,int>( OFin_place<1>, 3 );
 *      @endcode
 *    </div>
 *  </div>
 */

#endif // DOXYGEN

#endif // OFUTIL_H
