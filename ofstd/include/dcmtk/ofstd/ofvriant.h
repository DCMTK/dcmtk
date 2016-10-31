/*
 *
 *  Copyright (C) 2016, OFFIS e.V.
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
 *  Purpose: Implementing tagged unions similar to C++17's std::variant.
 *
 */

#ifndef OFVARIANT_H
#define OFVARIANT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef DCMTK_USE_CXX11_STL
#include <type_traits>
#include <cassert>
#include <limits>

template<std::size_t,typename... Alternatives>
struct OFvariant_traits
{
    using is_constructible = std::false_type;
    static constexpr inline std::size_t alignment() { return 1; }
    static constexpr inline std::size_t size() { return 0; }
    static void index_of();
};

template<std::size_t Index,typename Alternative0,typename... Alternatives>
struct OFvariant_traits<Index,Alternative0,Alternatives...>
: OFvariant_traits<Index+1,Alternatives...>
{
    using OFvariant_traits<Index+1,Alternatives...>::index_of;

    using first_alternative = Alternative0;

    static constexpr inline std::size_t alignment()
    {
        struct test { alignas(OFvariant_traits<Index+1,Alternatives...>::alignment()) first_alternative a; };
        return alignof(test);
    }

    static constexpr inline std::size_t size()
    {
        return sizeof( first_alternative ) > OFvariant_traits<Index+1,Alternatives...>::size()
        ?
            sizeof( first_alternative )
        :
            OFvariant_traits<Index+1,Alternatives...>::size()
        ;
    }

    static std::integral_constant<std::size_t,Index> index_of( first_alternative );
};

template<std::size_t AlternativeCount,typename... IndexAlternatives>
struct OFvariant_select_index_type {};

template<std::size_t AlternativeCount,typename IndexAlternative0,typename... IndexAlternatives>
struct OFvariant_select_index_type<AlternativeCount,IndexAlternative0,IndexAlternatives...>
{
    using type = typename std::conditional
    <
        AlternativeCount <= std::numeric_limits<IndexAlternative0>::max(),
        std::enable_if<true,IndexAlternative0>,
        OFvariant_select_index_type<AlternativeCount,IndexAlternatives...>
    >::type::type;
};

template<typename...>
class OFvariant;

template<std::size_t Index,typename Variant>
struct OFvariant_alternative;

template<std::size_t Index,typename Alternative0,typename... Alternatives>
struct OFvariant_alternative<Index,OFvariant<Alternative0,Alternatives...>>
: OFvariant_alternative<Index - 1,OFvariant<Alternatives...>> {};

template<typename Alternative0,typename... Alternatives>
struct OFvariant_alternative<0,OFvariant<Alternative0,Alternatives...>>
{
    using type = Alternative0;
};

template<typename... Alternatives>
class OFvariant
{
    using traits = OFvariant_traits<0,Alternatives...>;
    using index_type = typename OFvariant_select_index_type<sizeof...(Alternatives),std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t>::type;
    template<typename T>
    using index_of = decltype(traits::index_of(std::declval<T>()));

public:
    using variant = OFvariant;

    OFvariant()
    : m_Content()
    , m_Index( 0 )
    {
        new (m_Content) typename traits::first_alternative;
    }

    template<typename T,index_type Index = index_of<T>()>
    OFvariant( T&& t )
    : m_Content()
    , m_Index( Index )
    {
        new (m_Content) typename OFvariant_alternative<Index,OFvariant>::type( std::forward<T>( t ) );
    }

    OFvariant( OFvariant& rhs )
    : OFvariant( const_cast<const OFvariant&>( rhs ) )
    {

    }

    OFvariant( const OFvariant& rhs )
    : m_Content()
    , m_Index( rhs.index() )
    {
        copy_construct( rhs );
    }

    OFvariant( OFvariant&& rhs )
    : m_Content()
    , m_Index( rhs.index() )
    {
        move_construct( std::move( rhs ) );
    }

    template<typename T,index_type Index = index_of<T>()>
    OFvariant& operator=( T&& t )
    {
        if( m_Index != Index )
        {
            destroy();
            m_Index = Index;
            new (m_Content) typename OFvariant_alternative<Index,OFvariant>::type( std::forward<T>( t ) );
        }
        else
        {
            *reinterpret_cast<typename OFvariant_alternative<Index,OFvariant>::type*>( m_Content ) = std::forward<T>( t );
        }
        return *this;
    }

    OFvariant& operator=( OFvariant& rhs )
    {
        return *this = const_cast<const OFvariant&>( rhs );
    }

    OFvariant& operator=( const OFvariant& rhs )
    {
        if( this != &rhs )
        {
            if( m_Index != rhs.m_Index )
            {
                destroy();
                m_Index = rhs.m_Index;
                copy_construct( rhs );
            }
            else
            {
                using functor = void(OFvariant::*)(const OFvariant&);
                static const functor assignment[] =
                {
                    &OFvariant::template copy_assign_alternative<Alternatives>...
                };
                (this->*assignment[m_Index])( rhs );
            }
        }
        return *this;
    }

    OFvariant& operator=( OFvariant&& rhs )
    {
        if( this != &rhs )
        {
            if( m_Index != rhs.m_Index )
            {
                destroy();
                m_Index = rhs.m_Index;
                move_construct( std::move( rhs ) );
            }
            else
            {
                using functor = void(OFvariant::*)(OFvariant&&);
                static const functor assignment[] =
                {
                    &OFvariant::template move_assign_alternative<Alternatives>...
                };
                (this->*assignment[m_Index])( std::move( rhs ) );
            }
        }
        return *this;
    }

    ~OFvariant()
    {
        destroy();
    }

    std::size_t index() const
    {
        return m_Index;
    }

private:
    template<typename T,typename... Xs>
    friend T* OFget( OFvariant<Xs...>* );
    template<typename T,typename... Xs>
    friend const T* OFget( const OFvariant<Xs...>* );
    template<typename ReturnType,typename FN,typename... Xs>
    friend ReturnType OFvisit( FN&&, OFvariant<Xs...>& );
    template<typename ReturnType,typename FN,typename... Xs>
    friend ReturnType OFvisit( FN&&, const OFvariant<Xs...>& );

    template<typename Alternative>
    void copy_construct_alternative( const OFvariant& rhs )
    {
        new (m_Content) Alternative( *reinterpret_cast<const Alternative*>( rhs.m_Content ) );
    }

    template<typename Alternative>
    void move_construct_alternative( OFvariant&& rhs )
    {
        new (m_Content) Alternative( std::move( *reinterpret_cast<Alternative*>( rhs.m_Content ) ) );
    }

    template<typename Alternative>
    void copy_assign_alternative( const OFvariant& rhs )
    {
        *reinterpret_cast<Alternative*>( m_Content ) = *reinterpret_cast<const Alternative*>( rhs.m_Content );
    }

    template<typename Alternative>
    void move_assign_alternative( OFvariant&& rhs )
    {
        *reinterpret_cast<Alternative*>( m_Content ) = std::move( *reinterpret_cast<Alternative*>( rhs.m_Content ) );
    }

    template<typename Alternative>
    void destructor()
    {
        reinterpret_cast<Alternative*>( m_Content )->~Alternative();
    }

    template<typename ReturnType,typename Alternative,typename FN>
    ReturnType visit_alternative( FN&& fn )
    {
        return fn( *reinterpret_cast<Alternative*>( m_Content ) );
    }

    template<typename ReturnType,typename Alternative,typename FN>
    ReturnType const_visit_alternative( FN&& fn ) const
    {
        return fn( *reinterpret_cast<const Alternative*>( m_Content ) );
    }

    void copy_construct( const OFvariant& rhs )
    {
        using functor = void(OFvariant::*)(const OFvariant&);
        static const functor constructor[] =
        {
            &OFvariant::template copy_construct_alternative<Alternatives>...
        };
        assert( m_Index < sizeof...(Alternatives) );
        (this->*constructor[m_Index])( rhs );
    }

    void move_construct( OFvariant&& rhs )
    {
        using functor = void(OFvariant::*)(OFvariant&&);
        static const functor constructor[] =
        {
            &OFvariant::template move_construct_alternative<Alternatives>...
        };
        assert( m_Index < sizeof...(Alternatives) );
        (this->*constructor[m_Index])( std::move( rhs ) );
    }

    void destroy()
    {
        using functor = void(OFvariant::*)();
        static const functor destructor[] =
        {
            &OFvariant::template destructor<Alternatives>...
        };
        assert( m_Index < sizeof...(Alternatives) );
        (this->*destructor[m_Index])();
    }

    alignas(traits::alignment()) std::uint8_t m_Content[traits::size()];
    index_type m_Index;
};

template<std::size_t Index,typename X,typename T0,typename... Tn>
struct OFvariant_index_of_type
: OFvariant_index_of_type<Index + 1,X,Tn...> {};

template<std::size_t Index,typename X,typename... Tn>
struct OFvariant_index_of_type<Index,X,X,Tn...>
: std::integral_constant<std::size_t,Index> {};

template<typename T,typename... Alternatives>
constexpr bool OFholds_alternative( const OFvariant<Alternatives...>& v )
{
    return v.index() == OFvariant_index_of_type<0,T,Alternatives...>::value;
}

template<typename T,typename... Alternatives>
T* OFget( OFvariant<Alternatives...>* v )
{
    if( OFholds_alternative<T>( *v ) )
        return reinterpret_cast<T*>( v->m_Content );
    return nullptr;
}

template<typename T,typename... Alternatives>
const T* OFget( const OFvariant<Alternatives...>* v )
{
    if( OFholds_alternative<T>( *v ) )
        return reinterpret_cast<const T*>( v->m_Content );
    return nullptr;
}

template<typename ReturnType,typename FN,typename... Alternatives>
ReturnType OFvisit( FN&& fn, OFvariant<Alternatives...>& v )
{
    using functor = ReturnType(OFvariant<Alternatives...>::*)(FN&&);
    static const functor visit[] =
    {
        &OFvariant<Alternatives...>::template visit_alternative<ReturnType,Alternatives,FN>...
    };
    return (v.*visit[v.index()])( std::forward<FN>( fn ) );
}

template<typename ReturnType,typename FN,typename... Alternatives>
ReturnType OFvisit( FN&& fn, const OFvariant<Alternatives...>& v )
{
    using functor = ReturnType(OFvariant<Alternatives...>::*)(FN&&) const;
    static const functor visit[] =
    {
        &OFvariant<Alternatives...>::template const_visit_alternative<ReturnType,Alternatives,FN>...
    };
    return (v.*visit[v.index()])( std::forward<FN>( fn ) );
}

#elif !defined(DOXYGEN) // fallback implementation

// Include the actual implementation (that emulates variadic templates)
#include "dcmtk/ofstd/variadic/variant.h"

#else // NOT C++11 && NOT DOXYGEN

// TODO

#endif // DOXYGEN

/// TODO
struct OFmonostate {};

#endif // OFVARIANT_H
