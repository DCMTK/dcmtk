/*
 *
 *  Copyright (C) 2014-2015, OFFIS e.V.
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
 *  Purpose: Implementing generic tuples similar to C++11's std::tuple.
 *
 */

#ifndef OFTUPLE_H
#define OFTUPLE_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofutil.h"
#include "dcmtk/ofstd/ofdefine.h"

// use native classes if C++11 is supported
#ifdef DCMTK_USE_CXX11_STL
#include <tuple>

#define OFignore std::ignore
#define OFmake_tuple std::make_tuple
#define OFtie std::tie

template<typename... Args>
using OFtuple = std::tuple<Args...>;

#elif !defined(DOXYGEN) // fallback implementations

#include <cstdarg>

// Implementation of OFignore: struct OFignore_t
// that can be constructed and assigned to anything
// which it then simply ignores.
struct DCMTK_OFSTD_EXPORT OFignore_t
{
    inline OFignore_t() {}
    template<typename X>
    inline OFignore_t(const X&) {}
    template<typename X>
    inline OFignore_t& operator=(const X&) { return *this; }
    template<typename X>
    inline const OFignore_t& operator=(const X&) const { return *this; }
};

// OFignore is a reference to an object of type OFignore_t, to
// fix some external linkage problems in MinGW
extern DCMTK_OFSTD_EXPORT const OFignore_t& OFignore;

// Helper metatemplate for OFget for OFtuple. The first
// parameter is the return type. This way OFtuple_element
// has to be instantiated only once instead of at every
// (template-recursive) instantiation of OFget_tuple_element.
// This should increase compiletime performance.
// Unspecialized version implements recursion step:
// N -> OFget_tuple_element<N-1>( tail )
template<typename T,size_t N>
struct OFget_tuple_element
{
    template<typename Seq>
    static T& from( Seq& seq ) { return OFget_tuple_element<T,N-1>::from( seq.tail() ); }
    template<typename Seq>
    static const T& from( const Seq& seq ) { return OFget_tuple_element<T,N-1>::from( seq.tail() ); }
};

// Specialization to implement recursion base:
// 0 -> head
template<typename T>
struct OFget_tuple_element<T,0>
{
    template<typename Seq>
    static T& from( Seq& seq ) { return seq.head; }
    template<typename Seq>
    static const T& from( const Seq& seq ) { return seq.head; }
};

// Helper template for resolving recursive definition
// of content tail.
template<typename Tail>
struct OFtuple_content_tail
{
    typedef Tail type;
};

// Metafunction to determine appropriate parameter types
// for constructing a tuple from a sequence of arguments.
template<typename T>
struct OFtuple_param
{
    typedef const T& type;
};

// Specialization for pointers that allows passing
// pointers by value instead of by reference.
template<typename T>
struct OFtuple_param<T*>
{
    typedef const T* type;
};

// Specialization for compilers that can't ignore
// "double references" themselves.
template<typename T>
struct OFtuple_param<T&>
{
    typedef const T& type;
};

// The actual implementation of OFtuple's content
// OFtuple_content is used as a sequence of nested instances
// of OFtuple_content. The argument 'Head' declares the element
// type of the current instance, where 'Tail' is used to
// pass the next instance. The last sequence element is always
// OFtuple_content<> ~ OFtuple_content<OFtuple_nil,void>,
// so the end of the sequence can be 'detected' when using
// OFtuple_content in template-recursion.
template<typename Head = OFtuple_nil,typename Tail = void>
class OFtuple_content : OFtuple_content_tail<Tail>::type
{
public:
    // Helper typedef to resolve the matching OFtuple_content
    // type on compilers that don't support accessing this
    // directly (older versions of GCC).
    // Needs to be accessible in global scope, therefore public.
    typedef OFtuple_content content_type;

private:
    // Friend declarations. OFtuple_content only has private
    // members, since these are accessed via free standing
    // functions (OFget, ...) only. Therefore this API
    // has to be declared OFtuple_contents friend.
#include "dcmtk/ofstd/variadic/tuplefrd.h"
    template<typename,typename>
    friend class OFtuple_content;
    template<typename T,size_t N>
    friend class OFget_tuple_element;

    // Helper class that wraps 'va_list' in a portable way.
    // This is for example necessary on platforms where
    // va_list is implemented as an array-type and therefore
    // can't be used as a function argument directly.
    // This is also used for ensuring the correct overload
    // of the constructor gets picked.
    struct va_args{ va_args() {} mutable va_list args; };

    // Determine real Tailtype, see specialization of
    // OFtuple_content_tail for details.
    typedef typename OFtuple_content_tail<Tail>::type tail_type;

    // Default constructor, default constructs all the elements.
    OFtuple_content()
    : tail_type()
    , head() {}

    // Copy constructor, copy elements from any other tuple,
    // as long ans the elements themselves are fine with that.
    template<typename UH,typename UT>
    OFtuple_content( const OFtuple_content<UH,UT>& rhs )
    : tail_type( rhs.tail() )
    , head( rhs.head ) {}

    // Construct from OFPair.
    // Note: Ensuring only sequences with two elements are
    //  constructed from OFPair is done in OFtuple itself.
    //  It's not verified in this constructor (again) to
    //  improve compiletime performance.
    template<typename U0,typename U1>
    OFtuple_content( const OFPair<U0,U1>& p )
    : tail_type( p )
    , head( OFget<2-OFtuple_size<OFtuple_content>::value>( p ) ) {}

    // Construct a tuple element sequence via C-style variadic
    // arguments. This overload initializes the va_list and
    // passes a pointer to it to the tail's constructor.
    // Note: Since 'tail_type' is a base class, it has to
    //   be initialized before 'head'. Therefore the arguments
    //   need to be given to this method in the reversed order!
    OFtuple_content( va_args args, ... )
    : tail_type( ( va_start( args.args, args ), &args.args ) )
    , head( *va_arg( args.args, typename OFadd_pointer<Head>::type ) ) { va_end( args.args ); }

    // Overload that takes a 'va_list' pointer and passes it to
    // its tail. Then takes its 'head' from the va_list.
    OFtuple_content( va_list* args )
    : tail_type( args )
    , head( *va_arg( *args, typename OFadd_pointer<Head>::type ) ) {}

    // Implementation of tuple assignment.
    // Effectively: head -> rhs.head
    //              tail().head -> rhs.tail().head
    //              tail().tail() ...
    template<typename UH,typename UT>
    void assign( const OFtuple_content<UH,UT>& rhs )
    {
        head = rhs.head;
        tail().assign( rhs.tail() );
    }

    // Assign an OFPair. No recursion required since
    // a pair always has just two arguments.
    // Note: Ensuring only sequences with two elements are
    //  assigned an OFPair to is done in OFtuple itself.
    //  It's not verified in this method (again) to
    //  improve compiletime performance.
    template<typename K,typename V>
    void assign( const OFPair<K,V>& p )
    {
        head = p.first;
        tail().head = p.second;
    }

    // Check if two tuple element sequences are equal.
    // Effectively: OFget<0>( a ) == OFget<0>( b ) &&
    //              OFget<1>( a ) == OFget<1>( b ) &&
    //                            ...
    //              OFget<N>( a ) == OFget<N>( b )
    template<typename UH,typename UT>
    OFBool equal( const OFtuple_content<UH,UT>& rhs ) const
    {
        return head == rhs.head && tail().equal( rhs.tail() );
    }

    // Check if two tuple element sequences are not equal.
    // Effectively: OFget<0>( a ) != OFget<0>( b ) ||
    //              OFget<1>( a ) != OFget<1>( b ) ||
    //                            ...
    //              OFget<N>( a ) != OFget<N>( b )
    template<typename UH,typename UT>
    OFBool not_equal( const OFtuple_content<UH,UT>& rhs ) const
    {
        return head != rhs.head || tail().not_equal( rhs.tail() );
    }

    // Check if one tuple element sequences is less then another.
    template<typename UH,typename UT>
    OFBool less( const OFtuple_content<UH,UT>& rhs ) const
    {
        return head < rhs.head || ( !( rhs.head < head ) && tail().less( rhs.tail() ) );
    }

    // Check if one tuple element sequences is less then or equal to another.
    template<typename UH,typename UT>
    OFBool less_equal( const OFtuple_content<UH,UT>& rhs ) const
    {
        return head < rhs.head || ( !( rhs.head < head ) && tail().less_equal( rhs.tail() ) );
    }

    // Implementation of swap.
    // Swaps 'head' and recurses to 'tail()'
    void swap_content( OFtuple_content& rhs )
    {
        OFswap( head, rhs.head );
        tail().swap_content( rhs.tail() );
    }

    // Helper methods to access the base class (tail_type)
    // without the need to specify its type.
    tail_type& tail() { return *this; }
    const tail_type& tail() const { return *this; }

    // The actual content of the current sequence element.
    Head head;
};

// Specialization that resolves passing
// 'void' for OFtuple_content's 'Tail' argument
// to OFtuple_content<>.
template<>
struct OFtuple_content_tail<void>
{
    typedef OFtuple_content<> type;
};

// Specialization of OFtuple_content's sentinel
// element 'OFtuple_content<>'. This class
// supplies the recursion base for all the required
// methods, ignoring any given arguments and returning
// the appropriate values.
// This is especially important for the comparison
// methods, where two instances of OFtuple_content<>
// are considered 'equal' and 'less_equal' but not
// 'less' or 'not_equal'.
template<>
struct OFtuple_content<>
{
    OFtuple_content() {}
    template<typename X>
    OFtuple_content(const X&) {}
    template<typename X>
    void assign(const X&) {}
    template<typename X>
    OFBool equal(const X&) const { return OFTrue; }
    template<typename X>
    OFBool not_equal(const X&) const { return OFFalse; }
    template<typename X>
    OFBool less(const X&) const { return OFFalse; }
    template<typename X>
    OFBool less_equal(const X&) const { return OFTrue; }
    template<typename X>
    void swap_content(const X&) {}
};

// Specialization of OFtuple_element for OFtuple.
// This implements the recursion base: 0 -> Head.
template<typename Head,typename Tail>
struct OFtuple_element<0,OFtuple_content<Head,Tail> >
{
    typedef Head type;
};

// Specialization of OFtuple_element for OFtuple.
// This implements the recursion step: Index -> OFtuple_element<Index-1,Tail>.
template<size_t Index,typename Head,typename Tail>
struct OFtuple_element<Index,OFtuple_content<Head,Tail> >
{
    typedef typename OFtuple_element<Index-1,Tail>::type type;
};

// Specialization of OFtuple_size for OFtuple.
// This implements the recursion base: OFtuple_content<> -> 0.
template<>
struct OFtuple_size<OFtuple_content<> >
: OFintegral_constant<size_t,0> {};

// Specialization of OFtuple_size for OFtuple.
// This implements the recursion step: X -> OFtuple_size<X::tail_type>.
template<typename Head,typename Tail>
struct OFtuple_size<OFtuple_content<Head,Tail> >
: OFintegral_constant<size_t,OFtuple_size<typename OFtuple_content_tail<Tail>::type>::value + 1> {};

// Specialization of OFtuple to implement empty tuples.
// This is not generated, since it's so different to the
// other specializations.
template<>
struct OFtuple<> : OFtuple_content<>
{

};

OFtuple<> OFmake_tuple();
OFtuple<> OFtie();

// Include the generated implementation of OFtuple.
// This contains the implementation of the general case
// and specializations for different element numbers.
// Each provide appropriate constructors and assignment
// operators as well as the 'swap' method.
// The specialization for two elements also provides
// constructors and assignment operators for OFPair.
// This file also contains the implementations of the
// comparators, OFmake_tuple, OFtie and OFswap for
// OFtuple.
// Note that for OFmake_tuple and OFtie an overload
// is required for every different number of elements.
// The comparators and OFswap instead only need an
// implementation / overload for the maximum number
// of elements currently supported.
#include "dcmtk/ofstd/variadic/tuple.h"

#else // NOT C++11 && NOT DOXYGEN
/** A class template that implements generic tuples.
 *  @headerfile dcmtk/ofstd/oftuple.h "dcmtk/ofstd/oftuple.h"
 *  @tparam Types a sequence of element types that
 *    shall be contained in the tuple. An empty sequence
 *    is allowed.
 *    <table class="note"><tr><td><th class="note">Note</th><td>
 *      When C++11 support is not available, the number
 *      of elements OFtuple may contain is limited,
 *      currently up to <i>50</i> elements are supported.
 *    </td></tr></table>
 *  @details
 *  <h2>@anchor tuple_types Tuple Types</h2>
 *  The term <i>tuple type</i> refers to the generic concept
 *  of tuples of elements, to be described as follows:
 *  <ul>
 *  <li>
 *    A tuple type may contain a specific number of elements of
 *    arbitrary types.
 *  </li>
 *  <li>
 *    The metafunction <i>OFtuple_size</i> is specialized in an
 *    appropriate way to determine the number of elements a tuple
 *    type may contain.
 *  </li>
 *  <li>
 *    The metafunction <i>OFtuple_element</i> is specialized in
 *    an appropriate way to determine the type of one of the
 *    tuple's elements at the provided index.
 *  </li>
 *  <li>
 *    The function-template <i>OFget</i> is overloaded in
 *    appropriate ways to enable access to one of the tuple's
 *    elements at the provided index.
 *  </li>
 *  </ul>
 *  The following types provided by the DCMTK currently fulfill
 *  all the above requirements and may therefore be regarded
 *  as <i>tuple types</i>:
 *  <ul>
 *  <li>any instance of OFtuple</li>
 *  <li>any instance of OFPair</li>
 *  </ul>
 *  When C++11 support is enabled, OFtuple_size, OFtuple_element
 *  and OFget refer to the respective STL declarations and
 *  therefore <i>std::array</i> may then be regarded as a
 *  tuple type as well.
 *  <h3>Defining own tuple types</h3>
 *  It is possible to define one's own tuple types, which
 *  may then be modified by all operations that are defined on
 *  tuple types as well. To define a custom tuple type, you
 *  need to implement the above accession methods, which means
 *  to specialize OFtuple_size and OFtuple_element and overload
 *  OFget in appropriate ways.
 *  @note When C++11 support is enabled, you need to specialize
 *    and overload the respective STL definitions (e.g. std::get)
 *    instead!
 *  @details
 *  <h2>@anchor oftuple_syntax %OFtuple Syntax</h2>
 *  As described in the @ref tuple_types "previous section"
 *  tuple types are queried and modified by the freestanding
 *  metafunctions and function templates <i>OFtuple_size</i>,
 *  <i>OFtuple_element</i> and <i>OFget</i>. Regarding OFtuple,
 *  two additional function templates are provided:
 *  <i>OFmake_tuple</i> and <i>OFtie</i>. Take a look at the
 *  <a href="#related">Related Definitions</a> section for their
 *  description.<br>
 *  However, some operations are defined on OFtuple objects
 *  themselves, these are described in this section. But first
 *  we need to declare the following symbols, which are used in
 *  that description:
 *  <table border>
 *  <tr><th>Symbol</th><th>Definition</th></tr>
 *  <tr>
 *    <td><center><kbd>
 *      T<sub><small>0</small></sub>, T<sub><small>1</small></sub>, ..., T<sub><small>n</small></sub><br>
 *      U<sub><small>0</small></sub>, U<sub><small>1</small></sub>, ..., U<sub><small>n</small></sub>
 *    </kbd></center></td>
 *    <td>The elements' types of two (possibly) different tuples</td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>
 *      t<sub><small>0</small></sub>, t<sub><small>1</small></sub>, ..., t<sub><small>n</small></sub>
 *    </kbd></center></td>
 *    <td>Objects of types <kbd>T<sub><small>0</small></sub>, T<sub><small>1</small></sub>, ..., T<sub><small>n</small></sub></kbd> respectively</td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>
 *      u<sub><small>0</small></sub>, u<sub><small>1</small></sub>, ..., u<sub><small>n</small></sub>
 *    </kbd></center></td>
 *    <td>Objects of types <kbd>U<sub><small>0</small></sub>, U<sub><small>1</small></sub>, ..., U<sub><small>n</small></sub></kbd> respectively</td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>t, rhs</kbd></center></td>
 *    <td>
 *      An object of type <kbd>OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>,...,T<sub><small>n</small></sub>></kbd>
 *    </td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>u</kbd></center></td>
 *    <td>
 *      An object of type <kbd>OFtuple<U<sub><small>0</small></sub>,U<sub><small>1</small></sub>,...,U<sub><small>n</small></sub>></kbd>
 *    </td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>t2</kbd></center></td>
 *    <td>An object of type <kbd>OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>></kbd>, a tuple with exactly two elements</td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>p</kbd></center></td>
 *    <td>An object of type <kbd>OFPair<U<sub><small>0</small></sub>,U<sub><small>1</small></sub>></kbd></td>
 *  </tr>
 *  </table>
 *  The allowed syntax on OFtuple objects is described in the following table:
 *  <table border>
 *  <tr><th>Expression</th><th>Meaning</th></tr>
 *  <tr>
 *     <td><center><kbd>t == u, u == t<br>t != u, u != t</kbd></center></td>
 *      <td>
 *        Compares the tuples <i>t</i> and <i>u</i> for equality / inequality.<br>
 *        You may compare tuples with different element types, as long as both elements at the same
 *        index are comparable (overload <kbd>operator==</kbd> rsp. <kbd>operator!=</kbd>).
 *        - Both tuples are considered <b>equal</b> IFF all pairs of elements that share the same index compare equal to one and another.
 *        - Both tuples are considered <b>inequal</b> IFF at least a pair of elements that share the same index compare inequal to one and another.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>t < u, u > t<br>t > u, u < t<br>t <= u, u >= t<br>t >= u, u <= t</kbd></center></td>
 *      <td>
 *        Tests if the tuples <i>t</i> and <i>u</i> are less / less or equal to each other.<br>
 *        You may compare tuples with different element types, as long as both elements at the same
 *        index are comparable (overload <kbd>operator<</kbd> rsp. <kbd>operator<=</kbd>).
 *        - Both tuples are considered <b>less</b> IFF for at least a pair of elements that share the same index the left hand side's tuple's
 *          element compares less to the other and all previously compared pairs of elements (elements with smaller indices) compared equal
 *          to one and another.
 *        - Both tuples are considered <b>less or equal</b> IFF for at least a pair of elements that share the same index the left hand
 *          side's tuple's element compares less to the other and all previously compared pairs of elements (elements with smaller indices)
 *          compared equal to one and another <i>or</i> all pairs of elements compare equal to one and another.
 *      </td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>
 *      OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>,...,T<sub><small>n</small></sub>>&nbsp;t<br>
 *      OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>,...,T<sub><small>n</small></sub>>&nbsp;t(&nbsp;t<sub><small>0</small></sub>,&nbsp;t<sub><small>1</small></sub>,&nbsp;...,&nbsp;t<sub><small>n</small></sub>&nbsp;)<br>
 *      OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>,...,T<sub><small>n</small></sub>>&nbsp;t(&nbsp;u<sub><small>0</small></sub>,&nbsp;u<sub><small>1</small></sub>,&nbsp;...,&nbsp;u<sub><small>n</small></sub>&nbsp;)<sup><small>C++11</small></sup><br>
 *      OFtuple<T<sub><small>0</small></sub>,T<sub><small>1</small></sub>> t2( p )<br>
 *      OFmake_tuple( t<sub><small>0</small></sub>, t<sub><small>1</small></sub>, ..., t<sub><small>n</small></sub> )
 *    </kbd></center></td>
 *    <td>
 *      Constructs a tuple containing the supplied arguments as its elements (if any). The elements' types
 *      <kbd>T<sub><small>0</small></sub>, T<sub><small>1</small></sub>, ..., T<sub><small>n</small></sub></kbd>
 *      must be specified explicitly unless OFmake_tuple is used, which deduces appropriate types from the given parameters
 *      <kbd>t<sub><small>0</small></sub>, t<sub><small>1</small></sub>, ..., t<sub><small>n</small></sub></kbd>.<br>
 *      If C++11 support is available, <i>t</i> may be move constructed from whatever arguments were supplied, possibly increasing
 *      the performance.
 *      @note You may still supply objects of different types than the respective elements even if C++11 support is not available.
 *        The arguments will then be copy constructed from the supplied objects first (and be copied again to initialized the elements
 *        inside the tuple).
 *    </td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>t = rhs<br>t = u<br>t2 = p</kbd></center></td>
 *    <td>
 *      Assigns the elements from another tuple type to the elements in the left hand side OFtuple object.<br>
 *      If C++11 support is available, the elements may be move assigned instead of being copied, possibly increasing the performance.
 *      @note You may directly assign an object of type OFPair to an OFtuple, but only if the tuple has exactly two elements.
 *    </td>
 *  </tr>
 *  <tr>
 *    <td><center><kbd>t.swap( rhs )<br>OFswap( t, rhs )</kbd></center></td>
 *    <td>
 *      Swaps the elements of two OFtuple objects with identical element types.<br>
 *      Effectively calls <kbd>OFswap()</kbd> on each pair of elements that share the same index. Note however that this is done
 *      sequentially, at no time requiring a whole copy of one the tuples to reside in the memory.
 *    </td>
 *  </tr>
 *  </table>
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  // Include this file to use OFtuple
 *  #include "dcmtk/ofstd/oftuple.h"
 *
 *  // A metatemplate for printing all elements of a tuple
 *  // Note: This works on any tuple type, not just OFtuple
 *  //   based ones.
 *  template<size_t Index>
 *  struct printer
 *  {
 *    // First template-recurse to print the previous elements
 *    // Then append the current one.
 *    template<typename Tuple>
 *    static void print( std::ostream& out, const Tuple& tuple )
 *    {
 *      printer<Index-1>::print( out, tuple );
 *      out << ", " << OFget<Index-1>( tuple );
 *    }
 *  };
 *
 *  // Specialization to print the first element without a
 *  // preceding ','.
 *  template<>
 *  struct printer<1>
 *  {
 *    template<typename Tuple>
 *    static void print( std::ostream& out, const Tuple& tuple )
 *    {
 *      out << OFget<0>( tuple );
 *    }
 *  };
 *
 *  // Specialization so that printing empty tuples does
 *  // not lead to an error.
 *  template<>
 *  struct printer<0>
 *  {
 *    static void print( ... ) {}
 *  };
 *
 *  // A wrapper function template that applies
 *  // OFtuple_size automatically, to ease printing tuples.
 *  template<typename Tuple>
 *  void printTuple( std::ostream& out, const Tuple& tuple )
 *  {
 *    printer<OFtuple_size<Tuple>::value>::print( out, tuple );
 *    out << OFendl;
 *  }
 *
 *  // Construct some tuples. Two OFtuple based ones and one
 *  // OFPair based tuple.
 *  OFtuple<OFString,size_t> t0( "Hello World", 42 ), t1;
 *  OFPair<const char*,int> p( "Hello World", 42 );
 *
 *  // Compare the tuples and print them
 *  if( t0 != t1 )
 *  {
 *    COUT << "The following tuples are inequal:" << OFendl;
 *    COUT << "  ";
 *    printTuple( COUT, t0 );
 *    COUT << "  ";
 *    printTuple( COUT, t1 );
 *  }
 *
 *  // Assign the pair to the tuple
 *  // Note: const char* gets auto-converted to OFString
 *  //   and int gets converted to size_t
 *  //   (probably with a warning, depending on your
 *  //   compiler and settings).
 *  t1 = p;
 *
 *  // Compare the tuples and the pair
 *  // Note: The pair needs to be converted to a tuple
 *  //   first, but comparing tuples with different element
 *  //   types goes well.
 *  if( t0 == t1 && t0 == OFtuple<const char*,int>( p ) )
 *  {
 *    COUT << "The following tuples are equal:" << OFendl;
 *    COUT << "  ";
 *    printTuple( COUT, t0 );
 *    COUT << "  ";
 *    printTuple( COUT, t1 );
 *    COUT << "  ";
 *    printTuple( COUT, p );
 *  }
 *
 *  // Construct a tuple with some more elements and template-recursively print it
 *  // Note: You don't need to declare the element types explicitly when using tuples this way.
 *  printTuple( COUT, OFmake_tuple( "This", "is", "a", "tuple", "with", 7, "elements" ) );
 *  @endcode
 *  <b>Output:</b>
 *  @verbatim
    The following tuples are inequal:
      Hello World, 42
      , 0
    The following tuples are equal:
      Hello World, 42
      Hello World, 42
      Hello World, 42
    This, is, a, tuple, with, 7, elements
    @endverbatim
 */
template<typename... Types>
class OFtuple {};

/** A literal used in conjunction with OFtie to ignore specific elements.
 *  @relates OFtuple
 *  @details
 *  OFignore is a global constant that "tags" a specific tuple element
 *  which should be ignored. This means anything can be assigned to this
 *  element without any effect.
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  typedef OFMap<OFtuple<OFString,OFstring>,OFString> PhoneBook;
 *  PhoneBook pb;
 *  PhoneBook::iterator it;
 *  ...
 *  // We don't care if this was a new entry or an old one existed, we just want to update it
 *  OFtie( it, OFignore ) = pb.insert( PhoneBook::value_type( OFmake_tuple( "Ringo", "John" ), "" );
 *  updatePhonebookEntry( it );
 *  ...
 *  @endcode
 */
const <unspecified> OFignore;

/** Creates a tuple, automatically deducing the element types from the given arguments.
 *  @relates OFtuple
 *  @param args a sequence of arguments. When no arguments are supplied, an object of type
 *    OFtuple<> is created.
 *    <table class="note"><tr><td><th class="note">Note</th><td>
 *      When C++11 support is not available, the number
 *      of elements OFtuple may contain is limited,
 *      currently up to <i>50</i> elements are supported.
 *    </td></tr></table>
 *  @details
 *  OFcreate_tuple deduces the element types from the arguments by applying OFdecay
 *  to each parameter type.
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  OFVector<OFtuple<OFString,OFString,size_t> > personIndex;
 *  personIndex.push_back( OFmake_tuple( "Alexander", "Emily", 17 ) );
 *  personIndex.push_back( OFmake_tuple( "Henke", "Michelle", 0 ) );
 *  @endcode
 */
template<typename... Types>
OFtuple<typename OFdecay<Types>::type...> OFmake_tuple( Types&&... args );

/** Creates a tuple of references to the given arguments.
 *  @relates OFtuple
 *  @param args a sequence of arguments. When no arguments are supplied, an object of type
 *    OFtuple<> is created.
 *    <table class="note"><tr><td><th class="note">Note</th><td>
 *      When C++11 support is not available, the number
 *      of elements OFtuple may contain is limited,
 *      currently up to <i>50</i> elements are supported.
 *    </td></tr></table>
 *  @details
 *  OFtie is meant to link its arguments to an OFtuple object, to ease extracting the
 *  elements from a tuple.
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  OFtuple<OFString,OFBool,unsigned,OFString> getAccount( const size_t accountId )
 *  {
 *     ...
 *  }
 *
 *  ...
 *
 *  OFString name;
 *  OFBool sex;
 *  unsigned age;
 *  OFString data;
 *
 *  OFtie( name, sex, age, data ) = getAccount( currentId );
 *  @endcode
 *  @see OFignore
 */
template<typename... Types>
OFtuple<Types&...> OFtie( Types&... args );

#endif // DOXYGEN

#endif // OFTUPLE_H
