/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Module: ofstd
 *
 *  Author: Nikolas Goldhammer
 *
 *  Purpose:
 *      Implementing platform abstracting error code handling.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

// nothing to do if the implementation is provided by the C++11 STL
#ifndef DCMTK_USE_CXX11_STL

#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oferror.h"
#include "dcmtk/ofstd/ofdiag.h"

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// all error categories are derived from the base error category.
struct OFgeneric_error_category : OFerror_category
{
    static const char* unknownError()
    {
        return "Unknown error.";
    }

    // default constructor, since older versions of clang insist
    inline OFgeneric_error_category() {}

    const char* name() const
    {
        return "generic";
    }

    OFString message( int code ) const
    {
        char buffer[256];
        const char* const result = OFStandard::strerror( code, buffer, 256 );
        return OFString( result ? result : unknownError() );
    }
};

struct OFsystem_error_category : OFgeneric_error_category
{
    // default constructor, since older versions of clang insist
    inline OFsystem_error_category() {}

    const char* name() const
    {
        return "system";
    }

    // only Windows has custom error codes, all others can simply use strerror from the generic category
#ifdef _WIN32
    OFString message( int code ) const
    {
        struct RAIICleaner
        {
            RAIICleaner() : buffer( OFnullptr ) {}
            ~RAIICleaner() { LocalFree( buffer ); }
            LPSTR buffer;
        };
        OFString message;
        {
            RAIICleaner cleanup;
            if
            (
                FormatMessageA
                (
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                    OFnullptr,
                    code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    OFreinterpret_cast(LPSTR, &cleanup.buffer),
                    0,
                    OFnullptr
                ) > 0
            )
            {
                message = cleanup.buffer;
                // remove trailing "\r\n" FormatMessage() may potentially add although we never asked for this
                if( message.length() >= 2 && message.substr( message.length() - 2 ) == "\r\n" )
                    message = message.substr( 0, message.length() - 2 );
            }
        }
        if( message.empty() )
            message = unknownError();
        return message;
    }
#endif
};

OFerror_code::OFerror_code()
: m_Code( 0 )
, m_Category( &OFsystem_category() )
{

}

OFerror_code::OFerror_code( int code, const OFerror_category& category )
: m_Code( code )
, m_Category( &category )
{

}

void OFerror_code::assign( int code, const OFerror_category& category )
{
    m_Code = code;
    m_Category = &category;
}

void OFerror_code::clear()
{
    m_Code = 0;
    m_Category = &OFsystem_category();
}

int OFerror_code::value() const
{
    return m_Code;
}

const OFerror_category& OFerror_code::category() const
{
    return *m_Category;
}

OFString OFerror_code::message() const
{
    return m_Category->message( value() );
}

OFBool OFerror_code::operator!=( const OFerror_code& rhs ) const
{
    return m_Code != rhs.m_Code || m_Category != rhs.m_Category;
}

OFBool OFerror_code::operator==( const OFerror_code& rhs ) const
{
    return m_Code == rhs.m_Code && m_Category == rhs.m_Category;
}

OFBool OFerror_code::operator<( const OFerror_code& rhs ) const
{
    return m_Category < rhs.m_Category || ( m_Category == rhs.m_Category && m_Code < rhs.m_Code );
}

OFerror_code::operator OFBool() const
{
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_PERFORMANCE_WARNING
    return m_Code;
#include DCMTK_DIAGNOSTIC_POP
}

DCMTK_OFSTD_EXPORT const OFerror_category& OFsystem_category()
{
    static const OFsystem_error_category system_category_const;
    return system_category_const;
}

DCMTK_OFSTD_EXPORT const OFerror_category& OFgeneric_category()
{
    static const OFgeneric_error_category generic_category_const;
    return generic_category_const;
}

// helper class to ensure both categories are initialized before main(),
// such that there will not be any race conditions in the singletons
struct InitErrorCategories
{
    static InitErrorCategories Init;

    InitErrorCategories()
    {
        OFstatic_cast(void, OFsystem_category());
        OFstatic_cast(void, OFgeneric_category());
    }
};

InitErrorCategories InitErrorCategories::Init;

#endif //DCMTK_USE_CXX11_STL
