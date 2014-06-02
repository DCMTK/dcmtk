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

#ifndef DOXYGEN
struct OFnullopt_t {};
#if __cplusplus >= 201103L
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

/** The class template OFoptional manages an optional contained value, i.e.\ a value tagged with a state that reports its validity.
 *  A common use case for OFoptional is the return value of a function that may fail. OFoptional handles expensive to construct
 *  objects well and is more readable, as the intent is expressed explicitly.
 *  An OFoptional object which actually contains a value is called 'engaged', whereas an OFoptional object not containing a valid
 *  value is called 'disengaged'. The global constant 'OFnullopt' of type 'OFnullopt_t' may be used to explicitly mark an optional
 *  object as disengaged, for example to return a disengaged object from a function if an error occurred.
 *  @tparam T the type of the value to manage initialization state for. The type's destructor must be accessible by OFoptional<T>.
 *  @note There exists a sufficient specialization for reference types (T&) that behaves exactly like a pointer (T*) but allows
 *    OFoptional to be used in a generic context (when it's unknown if T is a reference or not).
 *  @details
 *  ### @anchor optional_syntax %OFoptional Syntax ###
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
{
#ifndef DOXYGEN
    // types for detecting T's default constructibility via sfinae
    struct no_type {};
    struct yes_type {int i;};
    // helper class to create an argument out of size_t
    template<size_t X>
    struct consume{};
    // sfinae overload working for default constructible Xs
    template<typename X>
    static yes_type sfinae(consume<sizeof X()>*);
    // most general sfinae overload, chosen only if everything else fails
    template<typename X>
    static no_type sfinae(...);
    // create a matching istream type if condition is OFTrue
    template<typename Char,typename Traits,OFBool>
    struct if_istream{ typedef STD_NAMESPACE basic_istream<Char,Traits>& type; };
    // otherwise don't
    template<typename Char,typename Traits>
    struct if_istream<Char,Traits,OFFalse> {};
    // use sfinae etc. to enable / disable the respective istream operator overloads
    template<typename Char,typename Traits>
    struct if_constructible : if_istream<Char,Traits,sizeof(sfinae<T>(OFnullptr)) == sizeof(yes_type)> {};
    template<typename Char,typename Traits>
    struct if_not_constructible : if_istream<Char,Traits,sizeof(sfinae<T>(OFnullptr)) == sizeof(no_type)> {};

// implement "construct" when "emplace" is not supported
// needed for the istream operator
#if __cplusplus < 201103L
    void construct() { state() = 1; new (content()) T; }
#endif // C++11

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
#if __cplusplus >= 201103L
            opt.emplace();
#else // C++11
            opt.construct();
#endif // NOT C++11
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
    friend bool operator==( const OFoptional& lhs, OFnullopt_t )
    {
        return !lhs;
    }

    friend bool operator==( OFnullopt_t, const OFoptional& rhs )
    {
        return !rhs;
    }

    friend bool operator==( const OFoptional& lhs, const OFoptional& rhs )
    {
        return ( !lhs && !rhs ) || ( lhs.state() == rhs.state() && *lhs == *rhs );
    }

    template<typename X>
    friend bool operator==( const OFoptional& lhs, const X& rhs )
    {
        return lhs.state() && *lhs == rhs;
    }

    template<typename X>
    friend bool operator==(  const X& lhs, const OFoptional& rhs )
    {
        return rhs.state() && lhs == *rhs;
    }

    friend bool operator!=( const OFoptional& lhs, OFnullopt_t )
    {
        return lhs.state();
    }

    friend bool operator!=( OFnullopt_t, const OFoptional& rhs )
    {
        return rhs.state();
    }

    friend bool operator!=( const OFoptional& lhs, const OFoptional& rhs )
    {
        return lhs.state() != rhs.state() || ( lhs.state() && *lhs != *rhs );
    }

    template<typename X>
    friend bool operator!=( const OFoptional& lhs, const X& rhs )
    {
        return !lhs || *lhs != rhs;
    }

    template<typename X>
    friend bool operator!=(  const X& lhs, const OFoptional& rhs )
    {
        return !rhs || lhs == *rhs;
    }

    friend bool operator<( const OFoptional& lhs, OFnullopt_t )
    {
        return OFFalse;
    }

    friend bool operator<( OFnullopt_t, const OFoptional& rhs )
    {
        return rhs.state();
    }

    friend bool operator<( const OFoptional& lhs, const OFoptional& rhs )
    {
        return lhs.state() < rhs.state() || ( rhs.state() && *lhs < *rhs );
    }

    template<typename X>
    friend bool operator<( const OFoptional& lhs, const X& rhs )
    {
        return !lhs || *lhs < rhs;
    }

    template<typename X>
    friend bool operator<( const X& lhs, const OFoptional& rhs )
    {
        return rhs.state() && lhs < *rhs;
    }

    friend bool operator>( const OFoptional& lhs, OFnullopt_t )
    {
        return lhs.state();
    }

    friend bool operator>( OFnullopt_t, const OFoptional& rhs )
    {
        return OFFalse;
    }

    friend bool operator>( const OFoptional& lhs, const OFoptional& rhs )
    {
        return lhs.state() > rhs.state() || ( lhs.state() && *lhs > *rhs );
    }

    template<typename X>
    friend bool operator>( const OFoptional& lhs, const X& rhs )
    {
        return lhs.state() && *lhs > rhs;
    }

    template<typename X>
    friend bool operator>( const X& lhs, const OFoptional& rhs )
    {
        return !rhs || lhs > *rhs;
    }

    friend bool operator<=( const OFoptional& lhs, OFnullopt_t )
    {
        return !lhs;
    }

    friend bool operator<=( OFnullopt_t, const OFoptional& rhs )
    {
        return OFTrue;
    }

    friend bool operator<=( const OFoptional& lhs, const OFoptional& rhs )
    {
        return !lhs || ( rhs.state() && *lhs <= *rhs );
    }

    template<typename X>
    friend bool operator<=( const OFoptional& lhs, const X& rhs )
    {
        return !lhs || *lhs <= rhs;
    }

    template<typename X>
    friend bool operator<=( const X& lhs, const OFoptional& rhs )
    {
        return rhs.state() && lhs <= *rhs;
    }

    friend bool operator>=( const OFoptional& lhs, OFnullopt_t )
    {
        return OFTrue;
    }

    friend bool operator>=( OFnullopt_t, const OFoptional& rhs )
    {
        return !rhs;
    }

    friend bool operator>=( const OFoptional& lhs, const OFoptional& rhs )
    {
        return !rhs || ( lhs.state() && *lhs >= *rhs );
    }

    template<typename X>
    friend bool operator>=( const OFoptional& lhs, const X& rhs )
    {
        return lhs.state() && *lhs >= rhs;
    }

    template<typename X>
    friend bool operator>=( const X& lhs, const OFoptional& rhs )
    {
        return !rhs || lhs >= *rhs;
    }

    // Default construct an OFoptional object in the disengaged state.
    OFconstexpr OFoptional()
    : m_Content()
    {

    }

    // Explicitly construct a disengaged OFoptional object.
    OFconstexpr OFoptional( OFnullopt_t )
    : m_Content()
    {

    }

    // False friend of the copy constructor, to prevent incorrect behavior
    // (internally calls the real copy constructor).
    OFoptional( OFoptional& rhs )
#if __cplusplus >= 201103L
    // delegate constructor if possible
    : OFoptional( const_cast<const OFoptional&>( rhs ) )
    {

    }
#else // C++11
    : m_Content()
    {
        state() = rhs.state();
        if( state() )
            new (content()) T( *rhs );
    }
#endif // NOT C++11

    // Copy the engaged state from rhs and its contents if rhs is engaged.
    OFoptional( const OFoptional& rhs )
    : m_Content()
    {
        state() = rhs.state();
        if( state() )
            new (content()) T( *rhs );
    }

#if __cplusplus >= 201103L
    // Move constructor, kills rhs if it was engaged before.
    OFoptional( OFoptional&& rhs )
    : m_Content{}
    {
        state() = rhs.state();
        if( state() )
        {
            new (content()) T( std::move( *rhs ) );
            rhs.state() = 0;
            rhs->~T();
        }
    }

    // Variadic constructor that emplaces the content
    template<typename... Args>
    OFoptional( Args&&... args )
    : m_Content{}
    {
        state() = 1;
        new (content()) T( std::forward<Args>( args )... );
    }

    // Move assignment
    OFoptional& operator=( OFoptional&& rhs )
    {
        // Prevent self-assignment
        if( &rhs != this )
        {
            // if both objects are engaged, move content
            // and kill rhs
            if( state() == rhs.state() )
            {
                if( state() )
                {
                    (**this) = std::move( *rhs );
                    rhs.state() = 0;
                    rhs->~T();
                }
            }
            else if( state() ) // suicide if engaged and rhs isn't
            {
                state() = 0;
                (*this)->~T();
            }
            else // if rhs is engaged and we aren't, swap states, move contents and kill rhs.
            {
                OFswap( state(), rhs.state() );
                new (content()) T( std::move( *rhs ) );
                rhs->~T();
            }
        }
        return *this;
    }

    // emplace assignment
    template<typename... Args>
    OFoptional& emplace( Args&&... args )
    {
        // emplace construct if we are disengaged
        if( !state() )
        {
            state() = 1;
            new (content()) T( std::forward<Args>( args )... );
        }
        else (**this) = T( std::forward<Args>( args )... ); // only emplace new content
        return *this;
    }
#else // C++11
    // Construct an engaged OFoptional object containing a copy of x.
    template<typename X>
    OFoptional( const X& x )
    : m_Content()
    {
        state() = 1;
        new (content()) T( x );
    }
#endif // NOT C++11

    // Destroy the contained object if engaged, otherwise do nothing.
    ~OFoptional()
    {
        if( state() ) (*this)->~T();
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
            if( state() == rhs.state() )
            {
                if( state() ) (**this) = *rhs;
            }
            else if( state() ) // suicide if engaged and rhs isn't
            {
                state() = 0;
                (*this)->~T();
            }
            else // if rhs is engaged and we aren't, copy-construct from rhs.
            {
                state() = 1;
                new (content()) T( *rhs );
            }
        }
        return *this;
    }

    // State and content accessing functions, see respective expression for details

// declare cast operator as explicit, if possible
#if __cplusplus >= 201103L
    explicit
#endif // C++11
    operator OFBool() const
    {
        return state();
    }

    OFBool operator!() const
    {
        return !state();
    }

    T& operator*() const
    {
        return *static_cast<T*>( content() );
    }

    T* operator->() const
    {
        return static_cast<T*>( content() );
    }

    // Swap the contents with another OFoptional object.
    void swap( OFoptional& rhs )
    {
        // if both objects are engaged, the contents are swapped.
        if( state() == rhs.state() )
        {
            if( state() ) OFswap( **this, *rhs );
        }
        else
        {
            // if we are engaged and rhs isn't, move assign
            // our contents to rhs.
            if( state() )
            {
                new (rhs.content()) T( OFmove( **this ) );
                (*this)->~T();
            }
            else // else move assign rhs' contents to us
            {
                new (content()) T( OFmove( *rhs ) );
                rhs->~T();
            }
            // finally, swap the states
            OFswap( state(), rhs.state() );
        }
    }

private:
    // State and content are stored in the same data-array to
    // optimize alignment and so on. Mutable, since the content
    // itself can be modified even if it cant be (re-)assigned.
    void* content() const { return m_Content; }
    Uint8& state() const { return m_Content[sizeof(T)]; }
    mutable Uint8 m_Content[sizeof(T)+1];
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
#if __cplusplus >= 201103L
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
#if __cplusplus >= 201103L
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
#if __cplusplus >= 201103L
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
