/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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

/** @file ofutil.h
 *  Implement fallback support for modern techniques defined
 *  in the STL's <utility> header (e.g.\ move semantics)
 *  for older compilers.
 */

#ifdef DCMTK_USE_CXX11_STL
#include <utility>
#define OFmove std::move
#define OFswap std::swap
#define OFget std::get
#define OFMake_pair std::make_pair

template<typename K,typename V>
using OFPair = std::pair<K,V>;

template<typename Tuple>
using OFtuple_size = std::tuple_size<Tuple>;

template<std::size_t Index,typename Tuple>
using OFtuple_element = std::tuple_element<Index,Tuple>;
#else // fallback implementations
/** Obtains an rvalue reference to its argument and converts it
 *  to an xvalue. OFmove is meant to 'mark' an object for a
 *  move operation, e.g. to move an OFVector object into another
 *  OFVector instance instead of copying it.
 *  @note Currently move semantics are only supported when C++11
 *    support is available. If that's the case, OFmove is simply
 *    an alias for std::move. Otherwise OFmove has no effect
 *    and the object will still be copied.
 *  @param t The object to move.
 */
template<typename T>
#ifndef DOXYGEN
T& OFmove( T& t )
{
    return t;
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

#if defined(HAVE_STL) || defined(HAVE_STL_MAP)
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
#endif // fallback implementation of OFPair

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
#ifndef DOXYGEN
struct OFtuple_size;
#else // NOT DOXYGEN
<metafunction> OFtuple_size;
#endif // DOXYGEN

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
#ifndef DOXYGEN
struct OFtuple_element;
#else // NOT DOXYGEN
<metafunction> OFtuple_element;
#endif // DOXYGEN

#ifndef DOXYGEN
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
#else // DOXYGEN
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

#endif // NOT C++11

#endif // OFUTIL_H
