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
 *  Purpose: Implementing optional types similar to boost::optional
 *           or the std::optional proposal N3690.
 */

#ifndef OFOPTION_H
#define OFOPTION_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofutil.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/oftraits.h"
#include "dcmtk/ofstd/ofalign.h"

// include <type_traits> for "std::is_default_constructible"
// to recover from compiler insanity (Visual Studio 12+).
#if defined(_MSC_VER) && _MSC_VER >= 1700
#include <type_traits>
#endif

#ifndef DOXYGEN
struct OFnullopt_t {};
#ifdef DCMTK_USE_CXX11_STL
// declare constexpr nullopt if supported.
constexpr OFnullopt_t OFnullopt{};
#else // C++11
// declare extern nullopt if constexpr is not supported.
extern DCMTK_OFSTD_EXPORT OFnullopt_t OFnullopt;
#endif // NOT C++11
#else // NOT DOXYGEN
/** OFnullopt_t is the type of the global constant @ref OFnullopt "@c OFnullopt"
 *  referring to a disengaged OFoptional object independent of the content type.
 *  @typedef <unspecified> OFnullopt_t
 *  OFnullopt_t may be used to improve the performance of overloaded methods, see example:
 *  @code{.cpp}
 *  int add2( const OFoptional<int>& o )
 *  {
 *    return o ? *o + 2 : 2;
 *  }
 *
 *  int add2( OFnullopt_t )
 *  {
 *    // Note: it's not required to check the state when OFnullopt is passed.
 *    return 2;
 *  }
 *  @endcode
 *  @relates OFoptional
 */
unspecified OFnullopt_t;

/** A wildcard global constant to initialize an OFoptional object with disengaged state.
 *  @details
 *  @b Example:
 *  @code{.cpp}
 *  OFoptional<int> div( int lhs, int rhs )
 *  {
 *    return rhs ? OFmake_optional( lhs / rhs ) : OFnullopt;
 *  }
 *  @endcode
 *  @relates OFoptional
 *  @see OFoptional
 */
OFnullpt_t OFnullopt;
#endif // DOXYGEN

/** Default storage traits of OFoptional<T>.
 *  OFdefault_optional_traits manages the state and storage of the contained
 *  object in a portable manner. You may use this class as base when defining
 *  custom traits for a specific type T, so you won't have to re-implement
 *  this functionality entirely.
 *  @relates OFoptional
 *  @tparam T the content type of the respective OFoptional instance.
 *  @sa OFoptional
 *  @sa OFoptional_traits
 *  @details
 *  <h2>@anchor ofoptional_trait_override Example</h2>
 *  This example shows how to override the <i>is_default_constructible</i>
 *  attribute of <kbd>OFoptional_traits</kbd> for the custom class <i>Test</i>.
 *  @code
 *  // Note: Test's default constructor is private
 *  //   which leads to errors when being used within
 *  //   OFoptional, since a default constructor exists
 *  //   but can't be accessed.
 *  class Test
 *  {
 *      Test() {}
 *  public:
 *      static OFoptional<T> instance();
 *           .
 *           .
 *           .
 *  };
 *
 *  // Override OFoptional's behavior to fix this problem
 *  template<>
 *  struct OFoptional_traits<Test>
 *  : OFdefault_optional_traits<Test> // derived from default traits
 *  {
 *      // Tell OFoptional that it can't default-construct
 *      // a Test object.
 *      typedef OFfalse_type is_default_constructible;
 *  };
 *  @endcode
 */
template<typename T>
class OFdefault_optional_traits
{
#ifndef DOXYGEN
    // types for detecting T's default constructibility via sfinae
    struct no_type {};
    struct yes_type {double d;};
#ifdef HAVE_DEFAULT_CONSTRUCTOR_DETECTION_VIA_SFINAE
    // helper class to create an argument out of size_t
    template<size_t>
    struct consume{};
    // sfinae overload working for default constructible Xs
    template<typename X>
    static yes_type sfinae(consume<sizeof *new X>*);
#elif defined(_MSC_VER)
#if _MSC_VER < 1700
    // Workaround bug in Visual Studio.
    // On these broken compilers, the argument is not evaluated
    // unless we require them to evaluate it for choosing which
    // specialization should be instantiated.
    template<size_t,size_t>
    struct consume{};
    template<size_t X>
    struct consume<X,X> { typedef void type; };
    // sfinae overload working for value-initializable Xs, that's as
    // close as we get on these broken compilers
    template<typename X>
    static yes_type sfinae(typename consume<sizeof X(),sizeof X()>::type*);
#else
    // Visual Studio 2012 is completely broken, but it has std::is_default_constructible
    // Note: this tests constructibility, but not if WE can construct this.
    template<typename X>
    static yes_type sfinae(typename OFenable_if<std::is_default_constructible<X>::value>::type*);
#endif // _MSC_VER
#endif // HAVE_DEFAULT_CONSTRUCTOR_DETECTION_VIA_SFINAE
    // most general sfinae overload, chosen only if everything else fails
    template<typename X>
    static no_type sfinae(...);

public:
    struct is_default_constructible
    : OFintegral_constant<OFBool,sizeof(sfinae<T>(OFnullptr)) == sizeof(yes_type)> {};

#ifdef DCMTK_USE_CXX11_STL

    template<typename... Args>
    void construct( Args&&... args )
    { new (content()) T( std::forward<Args>( args )... ); m_State = OFTrue; }

#else // C++11

#ifdef OFalign
    void construct() { new (content()) T; m_State = OFTrue; }
    template<typename X>
    void construct( const X& x ) { new (content()) T( x ); m_State = OFTrue; }
#else
    void construct() { m_pContent = new T; }
    template<typename X>
    void construct( const X& x ) { m_pContent = new T( x ); }
#endif

#endif // NOT C++11

#ifdef OFalign
    // State and content are stored in the same data-array to
    // optimize alignment and so on. Mutable, since the content
    // itself can be modified even if it cant be (re-)assigned.
    OFdefault_optional_traits() : m_State( OFFalse ) {}
    void destroy() { OFstatic_cast( T*, content() )->~T(); m_State = OFFalse; }
    OFBool state() const { return m_State; }
    void* content() const { return m_Content; }
    mutable OFalign_typename(Uint8[sizeof(T)],T) m_Content;
    OFBool m_State;
#else
    // Allocate content on the heap.
    OFdefault_optional_traits() : m_pContent( OFnullptr ) {}
    void destroy() { delete m_pContent; m_pContent = OFnullptr; }
    OFBool state() const { return m_pContent; }
    void* content() const { return m_pContent; }
    T* m_pContent;
#endif

#endif // NOT DOXYGEN
};

/** Manages storage and state of the object contained in OFoptional<T>.
 *  OFoptional_traits is a customization point for OFoptional that enables you to define a custom storage management policy for
 *  individual types T. If you don't want to implement everything from scratch, use OFdefault_optional_traits as base class for
 *  your implementation.
 *  @relates OFoptional
 *  @tparam the content type of the respective OFoptional instance.
 *  @sa OFoptional
 *  @sa OFdefault_optional_traits
 *  @details
 *  <h2>Example</h2>
 *  The following example shows how to implement custom OFoptional_traits for an enum that already contains a specific element
 *  to denote an invalid state.
 *  @code
 *  enum FILE_ACCESS
 *  {
 *    NONE  = 0000,
 *    READ  = 0400,
 *    WRITE = 0200,
 *        . . .
 *    INVALID_ACCESS_CODE = 01000
 *  };
 *
 *  template<>
 *  struct OFoptional_traits<FILE_ACCESS>
 *  {
 *    // Tell OFoptional that this is default-constructible
 *    typedef OFtrue_type is_default_constructible;
 *    // Initialize storage to the invalid state during construction
 *    OFoptional_traits() : access( INVALID_ACCESS_CODE ) {}
 *    // Constructors
 *    void construct()                 { access = NONE; }
 *    void construct( FILE_ACCESS fa ) { access = fa; }
 *    // There is nothing to destroy, just set the state to "disengaged"
 *    void destroy()                   { access = INVALID_ACCESS_CODE; }
 *    // Tell OFoptional how to distinguish "engaged" and "disengaged" FILE_ACCESS objects
 *    OFBool state() const             { return access != INVALID_ACCESS_CODE; }
 *    // Just return a pointer to the underlying object
 *    void* content() const            { return &access; }
 *    // The actual object
 *    mutable FILE_ACCESS access;
 *  };
 *
 *  COUT << "This should now be the same: " << sizeof( FILE_ACCESS ) << ' ' << sizeof( OFoptional<FILE_ACCESS> ) << OFendl;
 *  @endcode
 */
template<typename T>
struct OFoptional_traits
#ifndef DOXYGEN
: OFdefault_optional_traits<T>
#endif
{
#ifdef DOXYGEN
    /** <b>Required</b>: default construction policy.
     *  You need to define an appropriate integral constant as "is_default_constructible"
     *  that denotes if OFoptional<T> may attempt to default construct the underlying object
     *  under certain circumstances. You may use <kbd>OFdefault_optional_traits<T>::is_default_constructible</kbd>
     *  to specify this member, which uses SFINAE mechanisms to query a type's default constructibility.
     */
    <unspecified> is_default_constructible;

    /// <b>Requried</b>: default constructor, must initialize the state to "disengaged".
    OFoptional_traits();

    /** <b>Required</b>: type constructors, construct the contained object.
     *  You need to define at least one method of this kind, that takes appropriate
     *  parameter(s) to construct the underlying object. Must set the state to "engaged".
     *  If <kbd>is_default_constructible</kbd> evaluates to <i>OFTrue</i>, an overload
     *  taking zero arguments is also required.
     */
    void construct(...);

    /// <b>Required</b>: type destructor, destroys the underlying object and must set the state to "disengaged".
    void destroy();

    /// <b>Required</b>: state query, must return <i>OFTrue</i> when "engaged" and <i>OFFalse</i> otherwhise.
    OFBool state() const;

    /// <b>Required</b>: content access, must return a pointer to the contained object.
    void* content() const;
#endif
};

/** The class template OFoptional manages an optional contained value, i.e.\ a value tagged with a state that reports its validity.
 *  A common use case for OFoptional is the return value of a function that may fail. OFoptional handles expensive to construct
 *  objects well and is more readable, as the intent is expressed explicitly.
 *  An OFoptional object which actually contains a value is called 'engaged', whereas an OFoptional object not containing a valid
 *  value is called 'disengaged'. The global constant 'OFnullopt' of type 'OFnullopt_t' may be used to explicitly mark an optional
 *  object as disengaged, for example to return a disengaged object from a function if an error occurred.
 *  @tparam T the type of the value to manage initialization state for. The type's destructor must be accessible by OFoptional<T>.
 *  @note There exists a sufficient specialization for reference types (T&) that behaves exactly like a pointer (T*) but allows
 *    OFoptional to be used in a generic context (when it's unknown if T is a reference or not).
 *  @sa OFoptional_traits
 *  @sa OFdefault_optional_traits
 *  @details
 *  <h2>@anchor optional_syntax %OFoptional Syntax</h2>
 *  OFoptional can be used like a pointer to the contained value, except OFoptional also manages the contained value's storage.
 *  Several operators have been overloaded to simplify the usage of OFoptional. Instead of looking at every overload's specification,
 *  it is more appropriate to describe the possible syntax in a general manner. Therefore we declare the following symbols that are
 *  used below to describe OFoptional's syntax and behavior:
 *  <table border>
 *    <tr><th>Symbol</th><th>Definition</th></tr>
 *    <tr>
 *      <td><center><kbd>T</kbd></center></td>
 *      <td>The content type to be used</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>t</kbd></center></td>
 *      <td>An object of type <tt>T</tt></td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o, lhs, rhs</kbd></center></td>
 *      <td>An object of type <kbd>OFoptional<T></kbd></td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>os</kbd></center></td>
 *      <td>An STL compatible output stream</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>is</kbd></center></td>
 *      <td>An STL compatible input stream</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>x</kbd></center></td>
 *      <td>
 *        May be:
 *        - an object of type <kbd>T</kbd>
 *        - an object of type <kbd>OFoptional<T></kbd>
 *        - <kbd>OFnullopt</kbd>
 *      </td>
 *    </tr>
 *  </table>
 *  The following table describes possible operations on OFoptional and related objects:
 *  <table border>
 *    <tr><th>Expression</th><th>Meaning</th></tr>
 *    <tr>
 *      <td><center><kbd>os << o</kbd></center></td>
 *      <td>Prints content of <i>o</i> to <i>os</i>. Prints <kbd>nullopt</kbd> if <i>o</i> is disengaged.</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>is >> o</kbd></center></td>
 *      <td>
 *        Reads the content of <i>o</i> from <i>is</i>.
 *        If the content cannot be read from <i>is</i>, <i>o</i> becomes disengaged.<br>
 *        If <i>o</i> is disengaged before the expression is evaluated, the content of <i>o</i> is default
 *        constructed unless <kbd>T</kbd> is not default constructible or <kbd>T</kbd>'s default constructor is
 *        not accessible by <kbd>OFoptional<T></kbd>. If default construction is not possible, the expression has
 *        no effect (<i>o</i> remains disengaged).
 *        @note Detecting if T is default constructible within OFoptional<T> does not work correctly on all
 *          compilers. Especially all versions of Microsoft Visual Studio are impaired. For example a private
 *          default constructor of T might be detected as <i>not accessible</i> although OFoptional<T>
 *          was declared a friend of T. Specialize <kbd>OFoptional_traits</kbd> for a particular type T to
 *          override constructibility detection as required. See @ref ofoptional_trait_override "this example".
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o == x, x == o<br>o != x, x != o</kbd></center></td>
 *      <td>
 *        - If <i>x</i> is an object of type <kbd>T</kbd> and <i>o</i> is engaged, <i>x</i> is compared to the content of <i>o</i>
 *          in the appropriate fashion. Otherwise <i>o</i> is considered <b>inequal</b> to <i>x</i>.
 *        - If <i>x</i> is another <kbd>OFoptional</kbd> object, the two objects are considered <b>equal</b> IFF both objects are
 *          disengaged or both objects are engaged and their contents are equal.
 *        - If <i>x</i> is <kbd>OFnullopt</kbd>, <i>x</i> and <i>o</i> are considered <b>equal</b> IFF <i>o</i> is disengaged.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o < x, x > o<br>o > x, x < o<br>o <= x, x >= o<br>o >= x, x <= o</kbd></center></td>
 *      <td>
 *        - If <i>x</i> is an object of type <kbd>T</kbd>, <i>x</i> is compared to the content of <i>o</i> if <i>o</i> is engaged.
 *          Otherwise <i>o</i> is considered <b>less</b> than <i>x</i>.
 *        - If <i>x</i> is another <kbd>OFoptional</kbd> object, the state of both objects is compared whereas <kbd>disengaged</kbd>
 *          is considered <b>less</b> than <kbd>engaged</kbd>. If both objects are engaged, their contents are compared to evaluate
 *          the expression.
 *        - If <i>x</i> is <kbd>OFnullopt</kbd>, <i>o</i> is considered <b>equal</b> to <i>x</i> if <i>o</i> is disengaged and
 *          <b>greater</b> than <i>x</i> otherwise.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>
 *        OFoptional<T> o<br>
 *        OFoptional<T> o(x)<br>
 *        OFoptional<T>&nbsp;o(x<sub><small>0</small></sub>,...,x<sub><small>n</small></sub>)<sup><small>C++11</small></sup><br>
 *        OFmake_optional(t)
 *      </kbd></center></td>
 *      <td>
 *        Construct an OFoptional object. The content type <tt>T</tt> must be specified explicitly unless <tt>OFmake_optional</tt>
 *        is used, which deduces an appropriate type for <tt>T</tt> from the given parameter <i>t</i>.<br>
 *        If C++11 support is available, OFoptional can be <i>move constructed</i> from <i>x</i>, allowing the content to be moved
 *        into <i>o</i> instead of being copied. C++11 support also enables <i>in-place construction</i> of <i>o</i>, copying or
 *        moving the given arguments as appropriate to construct <i>o</i>'s content in the most efficient way.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>!o</kbd></center></td>
 *      <td>Evaluates to <kbd>OFTrue</kbd> if <i>o</i> is disengaged, evaluates to <kbd>OFFalse</kbd> otherwise.</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>if(o), while(o), ...</kbd></center></td>
 *      <td>
 *          When used in an appropriate context, <i>o</i> is evaluated to <kbd>OFTrue</kbd> if <i>o</i> is
 *          engaged and to <kbd>OFFalse</kbd> if <i>o</i> is disengaged.<br>
 *          If C++11 support is available, the conversion operator is marked as <tt>explicit</tt>, which prevents
 *          <i>o</i> to be interpreted as a boolean value in an inappropriate context. You should therefore use
 *          <i>o</i> with caution when C++11 support is unavailable, as <i>o</i> might be converted to a boolean value
 *          automatically where it shouldn't.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>*o<br>o.value()</kbd></center></td>
 *      <td>
 *        Access the content of <i>o</i>. The expression evaluates to a reference to <i>o</i>'s content if <i>o</i>
 *        is engaged. Otherwise the results are undefined.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o.value_or(t)</kbd></center></td>
 *      <td>
 *        Access the content of <i>o</i>. The expression evaluates to a copy of <i>o</i>'s content if <i>o</i>
 *        is engaged. Otherwise a copy of <i>t</i> is returned.<br>
 *        If C++11 support is available, <i>t</i> may be <i>moved</i> instead of being copied, if possible.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o-></kbd></center></td>
 *      <td>
 *        Access a member of <i>o</i>'s content. Members of compound data-types can be accessed via this syntax if <i>o</i>
 *        is engaged. Otherwise the results are undefined.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>
 *        o = x<br>
 *        o.emplace(x<sub><small>0</small></sub>,...,x<sub><small>n</small></sub>)<sup><small>C++11</small></sup>
 *      </kbd></center></td>
 *      <td>
 *        Assign <i>x</i> to <i>o</i>. If <i>x</i> is another OFoptional object, the state is copied from <i>x</i> and
 *        the content is only assigned if <i>x</i> is engaged. If <i>x</i> is <tt>OFnullopt</tt> <i>o</i> simply becomes disengaged.<br>
 *        If C++11 support is available, OFoptional can be <i>move assigned</i> from <i>x</i>, allowing the content to be moved
 *        into <i>o</i> instead of being copied. C++11 support also enables <i>in-place assignment</i> of <i>o</i> via the member
 *        method <i>emplace</i>, copying or moving the given arguments as appropriate to assign <i>o</i>'s content in the most
 *        efficient way.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>o.swap(rhs)<br>OFswap(lhs, rhs)</kbd></center></td>
 *      <td>
 *        Swap state and contents of two OFoptional objects.
 *        - Does nothing if both objects are disengaged.
 *        - Calls <tt>OFswap(*o, *rhs)</tt> resp. <tt>OFswap(*lhs, *rhs)</tt> if both objects are engaged.
 *        - Swaps the states if both objects' states differ and assigns the content of the engaged object
 *          to the previously disengaged object. If C++11 support is available, the content is <i>moved</i>
 *          from one object to the other. Otherwise the content is copied and afterwards destroyed in the now
 *          disengaged object.
 *      </td>
 *    </tr>
 *  </table>
 *  @note OFoptional is implemented based on the C++14 proposal for std::optional
 *  [N3690, p. 503ff](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3690.pdf) and is similar to
 *  [Boost.Optional](http://www.boost.org/doc/libs/release/libs/optional/doc/html/index.html).
 *  @details
 *  <h3>Usage example:</h3>
 *  @code{.cpp}
 *  OFoptional<int> my_atoi( const char* str )
 *  {
 *    int result;
 *    return parseValue( result, str ) ? OFoptional<int>( result ) : OFnullopt;
 *  }
 *
 *  // Classical atoi, can't distinguish '0' from an error.
 *  int i = atoi( "0" );
 *  if( !i ) // ERROR
 *
 *  // Using OFoptional to distinguish '0' and parser error.
 *  OFoptional<int> i = my_atoi( "0" );
 *  if( !i ) // OK
 *  {
 *    // read optional value from standard input.
 *    COUT << "Invalid value, please input a new value: ";
 *    CIN >> i;
 *    // if the user entered garbage, use 42 as default.
 *    i = i.value_or( 42 );
 *  }
 *
 *  // modify content of optional value and print the result.
 *  *i += 3;
 *  COUT << i << OFendl;
 *  @endcode
 */
template<typename T>
class OFoptional
#ifndef DOXYGEN
: OFoptional_traits<T>
#endif
{
#ifndef DOXYGEN
    // create a matching istream type if condition is OFTrue
    template<typename Char,typename Traits,OFBool>
    struct if_istream{ typedef STD_NAMESPACE basic_istream<Char,Traits>& type; };
    // otherwise don't
    template<typename Char,typename Traits>
    struct if_istream<Char,Traits,OFFalse> {};
    // use sfinae etc. to enable / disable the respective istream operator overloads
    template<typename Char,typename Traits>
    struct if_constructible : if_istream<Char,Traits,OFoptional_traits<T>::is_default_constructible::value> {};
    template<typename Char,typename Traits>
    struct if_not_constructible : if_istream<Char,Traits,!OFoptional_traits<T>::is_default_constructible::value> {};

public:
    // generic output stream operator overload
    template<typename Char,typename Traits>
    friend STD_NAMESPACE basic_ostream<Char,Traits>& operator<<( STD_NAMESPACE basic_ostream<Char,Traits>& os, const OFoptional& opt )
    {
        if( opt )
            return os << *opt;
        else
            return os << "nullopt";
    }

    // input stream operator overload for default constructible Ts
    template<typename Char,typename Traits>
    friend typename if_constructible<Char,Traits>::type operator>>( STD_NAMESPACE basic_istream<Char,Traits>& is, OFoptional& opt )
    {
        if( !opt )
        {
            opt.construct();
            if( (is >> *opt).fail() )
                opt = OFnullopt;
            return is;
        }
        return is >> *opt;
    }

    // input stream operator overload for non default constructible Ts
    template<typename Char,typename Traits>
    friend typename if_not_constructible<Char,Traits>::type operator>>( STD_NAMESPACE basic_istream<Char,Traits>& is, OFoptional& opt )
    {
        if( opt ) is >> *opt;
        return is;
    }

    // comparison operator overloads, see expression itself for details
    template<typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator==( OFnullopt_t, const O& rhs )
    {
        return !rhs;
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator==( const X& lhs, const O& rhs )
    {
        return rhs.state() && lhs == *rhs;
    }

    template<typename O>
    friend typename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator!=( OFnullopt_t, const O& rhs )
    {
        return rhs.state();
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator!=( const X& lhs, const O& rhs )
    {
        return !rhs || lhs != *rhs;
    }

    template<typename O>
    friend typename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator<( OFnullopt_t, const O& rhs )
    {
        return rhs.state();
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator<( const X& lhs, const O& rhs )
    {
        return rhs.state() && lhs < *rhs;
    }

    template<typename O>
    friend typename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator>( OFnullopt_t, const O& rhs )
    {
        return OFFalse;
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator>( const X& lhs, const O& rhs )
    {
        return !rhs || lhs > *rhs;
    }

    template<typename O>
    friend typename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator<=( OFnullopt_t, const O& rhs )
    {
        return OFTrue;
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator<=( const X& lhs, const O& rhs )
    {
        return rhs.state() && lhs <= *rhs;
    }

    template<typename O>
    friend typename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator>=( OFnullopt_t, const O& rhs )
    {
        return !rhs;
    }

    template<typename X,typename O>
    friend OFTypename OFenable_if<OFis_same<O,OFoptional>::value,bool>::type
    operator>=( const X& lhs, const O& rhs )
    {
        return !rhs || lhs >= *rhs;
    }

    bool operator==( OFnullopt_t )
    {
        return !*this;
    }

    bool operator==( const OFoptional& rhs )
    {
        return ( !*this && !rhs ) || ( OFoptional_traits<T>::state() == rhs.state() && **this == *rhs );
    }

    template<typename X>
    bool operator==( const X& rhs )
    {
        return OFoptional_traits<T>::state() && **this == rhs;
    }

    bool operator!=( OFnullopt_t )
    {
        return OFoptional_traits<T>::state();
    }

    bool operator!=( const OFoptional& rhs )
    {
        return OFoptional_traits<T>::state() != rhs.state() || ( rhs.state() && **this != *rhs );
    }

    template<typename X>
    bool operator!=( const X& rhs )
    {
        return !*this || **this != rhs;
    }

    bool operator<( OFnullopt_t )
    {
        return OFFalse;
    }

    bool operator<( const OFoptional& rhs )
    {
        return OFoptional_traits<T>::state() < rhs.state() || ( rhs.state() && **this < *rhs );
    }

    template<typename X>
    bool operator<( const X& rhs )
    {
        return !*this || **this < rhs;
    }

    bool operator>( OFnullopt_t )
    {
        return OFoptional_traits<T>::state();
    }

    bool operator>( const OFoptional& rhs )
    {
        return OFoptional_traits<T>::state() > rhs.state() || ( OFoptional_traits<T>::state() && **this > *rhs );
    }

    template<typename X>
    bool operator>( const X& rhs )
    {
        return OFoptional_traits<T>::state() && **this > rhs;
    }

    bool operator<=( OFnullopt_t )
    {
        return !*this;
    }

    bool operator<=( const OFoptional& rhs )
    {
        return !*this || ( rhs.state() && **this <= *rhs );
    }

    template<typename X>
    bool operator<=( const X& rhs )
    {
        return !*this || **this <= rhs;
    }

    bool operator>=( OFnullopt_t )
    {
        return OFTrue;
    }

    bool operator>=( const OFoptional& rhs )
    {
        return !rhs || ( OFoptional_traits<T>::state() && **this >= *rhs );
    }

    template<typename X>
    bool operator>=( const X& rhs )
    {
        return OFoptional_traits<T>::state() && **this >= rhs;
    }

    // Default construct an OFoptional object in the disengaged state.
    OFconstexpr OFoptional()
    : OFoptional_traits<T>()
    {

    }

    // Explicitly construct a disengaged OFoptional object.
    OFconstexpr OFoptional( OFnullopt_t )
    : OFoptional_traits<T>()
    {

    }

    // False friend of the copy constructor, to prevent incorrect behavior
    // (internally calls the real copy constructor).
    OFoptional( OFoptional& rhs )
#ifdef DCMTK_USE_CXX11_STL
    // delegate constructor if possible
    : OFoptional( const_cast<const OFoptional&>( rhs ) )
    {

    }
#else // C++11
    : OFoptional_traits<T>()
    {
        if( rhs.state() )
            OFoptional_traits<T>::construct( *rhs );
    }
#endif // NOT C++11

    // Copy the engaged state from rhs and its contents if rhs is engaged.
    OFoptional( const OFoptional& rhs )
    : OFoptional_traits<T>()
    {
        if( rhs.state() )
            OFoptional_traits<T>::construct( *rhs );
    }

#ifdef DCMTK_USE_CXX11_STL
    // Move constructor, kills rhs if it was engaged before.
    OFoptional( OFoptional&& rhs )
    : OFoptional_traits<T>()
    {
        if( rhs.state() )
        {
            OFoptional_traits<T>::construct( std::move( *rhs ) );
            rhs.destroy();
        }
    }

    // Variadic constructor that emplaces the content
    template<typename... Args>
    OFoptional( Args&&... args )
    : OFoptional_traits<T>()
    {
        OFoptional_traits<T>::construct( std::forward<Args>( args )... );
    }

    // Move assignment
    OFoptional& operator=( OFoptional&& rhs )
    {
        // Prevent self-assignment
        if( &rhs != this )
        {
            // if both objects are engaged, move content
            // and kill rhs
            if( OFoptional_traits<T>::state() == rhs.state() )
            {
                if( OFoptional_traits<T>::state() )
                {
                    (**this) = std::move( *rhs );
                    rhs.destroy();
                }
            }
            else if( OFoptional_traits<T>::state() ) // suicide if engaged and rhs isn't
            {
                OFoptional_traits<T>::destroy();
            }
            else // if rhs is engaged and we aren't, swap states, move contents and kill rhs.
            {
                OFoptional_traits<T>::construct( std::move( *rhs ) );
                rhs.destroy();
            }
        }
        return *this;
    }

    // emplace assignment
    template<typename... Args>
    OFoptional& emplace( Args&&... args )
    {
        // emplace construct if we are disengaged
        if( !OFoptional_traits<T>::state() )
            OFoptional_traits<T>::construct( std::forward<Args>( args )... );
        else
            (**this) = T( std::forward<Args>( args )... ); // only emplace new content
        return *this;
    }
#else // C++11
    // Construct an engaged OFoptional object containing a copy of x.
    template<typename X>
    OFoptional( const X& x )
    : OFoptional_traits<T>()
    {
        OFoptional_traits<T>::construct( x );
    }
#endif // NOT C++11

    // Destroy the contained object if engaged, otherwise do nothing.
    ~OFoptional()
    {
        if( OFoptional_traits<T>::state() )
            OFoptional_traits<T>::destroy();
    }

    // False friend of the assignment operator to prevent wrong behavior
    // (internally calls the real assignment operator).
    OFoptional& operator=( OFoptional& rhs )
    {
        return (*this) = const_cast<const OFoptional&>( rhs );
    }

    // copy assignment
    OFoptional& operator=( const OFoptional& rhs )
    {
        // Prevent self-assignment
        if( &rhs != this )
        {
            // if both objects are engaged, copy content
            if( OFoptional_traits<T>::state() == rhs.state() )
            {
                if( OFoptional_traits<T>::state() ) (**this) = *rhs;
            }
            else if( OFoptional_traits<T>::state() ) // suicide if engaged and rhs isn't
            {
                OFoptional_traits<T>::destroy();
            }
            else // if rhs is engaged and we aren't, copy-construct from rhs.
            {
                OFoptional_traits<T>::construct( *rhs );
            }
        }
        return *this;
    }

    // State and content accessing functions, see respective expression for details

// declare cast operator as explicit, if possible
#ifdef DCMTK_USE_CXX11_STL
    explicit
#endif // C++11
    operator OFBool() const
    {
        return OFoptional_traits<T>::state();
    }

    OFBool operator!() const
    {
        return !OFoptional_traits<T>::state();
    }

    T& operator*() const
    {
        return *OFstatic_cast( T*, OFoptional_traits<T>::content() );
    }

    T* operator->() const
    {
        return OFstatic_cast( T*, OFoptional_traits<T>::content() );
    }

    // Swap the contents with another OFoptional object.
    void swap( OFoptional& rhs )
    {
        // if both objects are engaged, the contents are swapped.
        if( OFoptional_traits<T>::state() == rhs.state() )
        {
            if( OFoptional_traits<T>::state() ) OFswap( **this, *rhs );
        }
        else
        {
            // if we are engaged and rhs isn't, move assign
            // our contents to rhs.
            if( OFoptional_traits<T>::state() )
            {
                rhs.construct( OFmove( **this ) );
                OFoptional_traits<T>::destroy();
            }
            else // else move assign rhs' contents to us
            {
                OFoptional_traits<T>::construct( OFmove( *rhs ) );
                rhs.destroy();
            }
        }
    }
#endif // NOT DOXYGEN
};

#ifndef DOXYGEN
// Specialization for references that behaves like a pointer
template<typename T>
class OFoptional<T&>
{
public:
    // generic ostream operator overload
    template<typename Char,typename Traits>
    friend STD_NAMESPACE basic_ostream<Char,Traits>& operator<<( STD_NAMESPACE basic_ostream<Char,Traits>& os, const OFoptional& opt )
    {
        if( opt )
            return os << *opt;
        else
            return os << "nullopt";
    }

    // generic istream operator overload since references aren't default constructible
    template<typename Char,typename Traits>
    friend STD_NAMESPACE basic_istream<Char,Traits>& operator>>( STD_NAMESPACE basic_istream<Char,Traits>& is, OFoptional& opt )
    {
        if( opt ) is >> *opt;
        return is;
    }

    // constructors and assignment operators, pretty self-explanatory
    OFconstexpr OFoptional()
    : m_pT( OFnullptr )
    {

    }

    OFconstexpr OFoptional( T& t )
    : m_pT( &t )
    {

    }

    OFconstexpr OFoptional( OFnullopt_t )
    : m_pT( OFnullptr )
    {

    }

    OFconstexpr OFoptional( T* t )
    : m_pT( t )
    {

    }

    OFoptional( const OFoptional& rhs )
    : m_pT( rhs.m_pT )
    {

    }

    OFoptional& operator=( const OFoptional& rhs )
    {
        if( &rhs != this )
            m_pT = rhs.m_pT;
        return *this;
    }

    // Move semantics if C++11 is supported
#ifdef DCMTK_USE_CXX11_STL
    OFoptional( OFoptional&& rhs )
    : m_pT( rhs.m_pT )
    {
        rhs.m_pT = OFnullptr;
    }

    OFoptional& operator=( OFoptional&& rhs )
    {
        if( &rhs != this )
        {
            m_pT = rhs.m_pT;
            rhs.m_pT = OFnullptr;
        }
        return *this;
    }

    // Can't really emplace-construct a reference, so this is
    // just a duplicate of the normal assignment operator,
    // for convenience.
    OFoptional& emplace( T& t )
    {
        m_pT = &t;
        return *this;
    }

    // Default destructor, to disable some warnings about pointer
    // data members.
    ~OFoptional() = default;
#else // C++11
    inline ~OFoptional() {}
#endif

    // State and content accessing functions, see respective expression for details

// declare cast operator as explicit, if possible
#ifdef DCMTK_USE_CXX11_STL
    explicit
#endif // C++11
    operator bool() const
    {
        return m_pT;
    }

    bool operator !() const
    {
        return !m_pT;
    }

    T& operator*() const
    {
        return *m_pT;
    }

    T* operator->() const
    {
        return m_pT;
    }

    // Swap the pointer, what else?
    void swap( OFoptional& rhs )
    {
        OFswap( m_pT, rhs.m_pT );
    }

private:
    // The pointer that behaves like a pointer...
    T* m_pT;
};

// Move semantics and std::swap overload if C++11 is supported
#ifdef DCMTK_USE_CXX11_STL
template<typename T>
OFoptional<typename OFdecay<T>::type> OFmake_optional( T&& t )
{
    return OFoptional<typename OFdecay<T>::type>( std::forward<T>( t ) );
}
namespace std
{
    template<typename T>
    void swap( OFoptional<T>& lhs, OFoptional<T>& rhs ) { lhs.swap( rhs ); }
}
#else // C++11
template<typename T>
OFoptional<typename OFdecay<T>::type> OFmake_optional( const T& t )
{
    return OFoptional<typename OFdecay<T>::type>( t );
}
template<typename T>
void OFswap( OFoptional<T>& lhs, OFoptional<T>& rhs ) { lhs.swap( rhs ); }
#endif // NOT C++11
#endif // NOT DOXYGEN

#endif // OFOPTION_H
