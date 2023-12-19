/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
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
 *  Purpose: Implementing the currently used subset of C++17' std::filesystem
 *
 */

#include "dcmtk/config/osconfig.h"

#include <cstring>
#include <cassert>
#include <climits>

#include "dcmtk/ofstd/offilsys.h"

#ifdef _WIN32
#include <io.h>
#else // _WIN32
BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>  // for struct DIR, opendir()
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>    // for struct DIR, opendir()
#endif
END_EXTERN_C
/* Check if we are using glibc in a version where readdir() is known to be
 * thread-safe and where readdir_r() is deprecated.
 */
#if defined(__GLIBC__) && (((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 24)) || (__GLIBC__ >= 3))
#define READDIR_IS_THREADSAFE
#endif
#endif // _WIN32

OFpath::OFpath()
: m_NativeString()
{

}

OFpath::OFpath( const OFpath& rhs )
: m_NativeString( rhs.native() )
{

}

OFpath::OFpath( OFrvalue_ref(OFpath) rhs )
: m_NativeString()
{
    m_NativeString.swap( OFrvalue_access( rhs ).m_NativeString );
}

OFpath::OFpath( const char* const cstr, format fmt )
: m_NativeString( cstr )
{
#ifdef _WIN32
    convertSeparator(fmt);
#endif
}

OFpath::OFpath( const OFString& string, format fmt )
: m_NativeString( string )
{
#ifdef _WIN32
    convertSeparator(fmt);
#endif
}

#ifdef _WIN32
void OFpath::convertSeparator( format fmt )
{
    size_t pos = OFString_npos;
    switch( fmt )
    {
    case native_format:
        return;
    case auto_format:
        pos = m_NativeString.find_first_of( "\\/" );
        if( pos == OFString_npos || m_NativeString[pos] == preferred_separator )
            return;
        break;
    default:
    case generic_format:
        pos = m_NativeString.find( '/' );
        if( pos == OFString_npos )
            return;
        break;
    }
    m_NativeString[pos] = preferred_separator;
    for( pos = m_NativeString.find( '/', pos + 1 ); pos != OFString_npos; pos = m_NativeString.find( '/', pos + 1 ) )
        m_NativeString[pos] = preferred_separator;
}
#endif

OFpath& OFpath::operator=( const OFpath& rhs )
{
    m_NativeString = rhs.native();
    return *this;
}

OFpath& OFpath::operator=( OFrvalue_ref(OFpath) rhs )
{
    if( this != &OFrvalue_access( rhs ) )
    {
        m_NativeString.clear();
        m_NativeString.swap( OFrvalue_access( rhs ).m_NativeString );
    }
    return *this;
}

OFBool OFpath::empty() const
{
    return m_NativeString.empty();
}

OFBool OFpath::is_absolute() const
{
#if _WIN32
    const std::size_t pos = findRootName();
    return OFString_npos != pos &&
    (
        ( pos + 1 ) == m_NativeString.size() ||
        preferred_separator == m_NativeString[pos+1]
    );
#else
    return has_root_directory();
#endif
}

OFBool OFpath::is_relative() const
{
    return !is_absolute();
}

OFBool OFpath::has_root_name() const
{
    return OFString_npos != findRootName();
}

OFBool OFpath::has_root_directory() const
{
#if _WIN32
    size_t pos = findRootName();
    pos = ( OFString_npos != pos ? pos + 1 : 0 );
    return pos < m_NativeString.size() && preferred_separator == m_NativeString[pos];
#else
    return !empty() && preferred_separator == *m_NativeString.begin();
#endif
}

OFBool OFpath::has_filename() const
{
#if _WIN32
    OFString::const_iterator it = m_NativeString.end();
    if( it == m_NativeString.begin() )
        return OFFalse;
    --it;
    if( *it == preferred_separator )
        return OFFalse;
    if( *it != ':' )
        return OFTrue;
    while( it != m_NativeString.begin() )
        if( *--it == preferred_separator )
            return OFTrue;
    return OFFalse;
#else
    return !empty() && preferred_separator != *(m_NativeString.end() - 1);
#endif
}


OFBool OFpath::has_extension() const
{
    return OFString_npos != findExtension();
}

const OFString& OFpath::native() const
{
    return m_NativeString;
}

const char* OFpath::c_str() const
{
    return m_NativeString.c_str();
}

OFrvalue<OFpath> OFpath::root_name() const
{
#if _WIN32
    const size_t pos = findRootName();
    if( OFString_npos != pos )
        return OFpath( m_NativeString.substr( 0, pos + 1 ) );
#endif
    return OFpath();
}

OFrvalue<OFpath> OFpath::filename() const
{
    const size_t pos = findFilename();
    if( OFString_npos != pos )
        return OFpath( m_NativeString.substr( pos ) );
    return OFpath();
}

OFrvalue<OFpath> OFpath::extension() const
{
    const size_t pos = findExtension();
    if( OFString_npos != pos )
        return OFpath( m_NativeString.substr( pos ) );
    return OFpath();
}

OFpath& OFpath::operator/=( const OFpath& rhs )
{
    // self append
    if( this == &rhs )
        return *this /= OFpath( rhs );
#if _WIN32
    // Comments are the descriptions from en.cppreference.com, put to whatever code segment handles it:
    // If p.is_absolute() || (p.has_root_name() && p.root_name() != root_name()),
    // then replaces the current path with p as if by operator=(p) and finishes.
    std::size_t pos = rhs.findRootName();
    if( OFString_npos != pos ) // .. p.has_root_name()
    {
        ++pos;
        if
        (
            // .. p.is_absolute()
            ( pos < rhs.m_NativeString.size() && rhs.m_NativeString[pos] == preferred_separator ) ||
            // .. p.root_name() != root_name()
            ( pos > m_NativeString.size() || 0 != m_NativeString.compare( 0, pos - 1, rhs.m_NativeString, 0, pos - 1 ) )
        )
        {
            // then replaces the current path with p as if by operator=(p) and finishes.
            m_NativeString = rhs.m_NativeString;
            return *this;
        }
    }
    else pos = 0;
    // Otherwise, if p.has_root_directory(), then removes any root directory and the
    // entire relative path from the generic format pathname of *this, then appends the native format
    // pathname of p, omitting any root-name from its generic format, to the native format of *this.
    if( pos < rhs.m_NativeString.size() && rhs.m_NativeString[pos] == '\\' ) // .. p.has_root_directory()
    {
        // we shall remove ONLY the root directory and relative path, not the root name
        // so find it and, if it exists, keep it
        const size_t root = findRootName();
        if( OFString_npos == root )
        {
            // no root name, so replace the entire string
            m_NativeString = rhs.m_NativeString;
        }
        else
        {
            // removes any root directory and the entire relative path from the generic format pathname of *this
            // appends the native format pathname of p, omitting any root-name from its generic format
            m_NativeString.replace( root + 1, OFString_npos, rhs.m_NativeString, pos, OFString_npos );
        }
    }
    else
    {
        // If has_filename() || (!has_root_directory() && is_absolute())
        if( !empty() && preferred_separator != *(m_NativeString.end() - 1) )
        {
            // then appends path::preferred_separator to the generic format of *this
            m_NativeString.reserve( m_NativeString.size() + rhs.m_NativeString.size() - pos + 1 );
            m_NativeString += preferred_separator;
        }
        // appends the native format pathname of p, omitting any root-name from its generic format
        m_NativeString += rhs.m_NativeString.substr( pos );
    }
#else
    // version for filesystems without root names, pretty straight forward
    if( !rhs.is_absolute() )
    {
        if( has_filename() )
        {
            m_NativeString.reserve( m_NativeString.size() + rhs.m_NativeString.size() + 1 );
            m_NativeString += preferred_separator;
        }
        m_NativeString += rhs.m_NativeString;
    }
    else m_NativeString = rhs.m_NativeString;
#endif
    return *this;
}

size_t OFpath::findRootName() const
{
#if _WIN32
    const size_t pos = m_NativeString.find_first_of( ":\\" );
    if( OFString_npos != pos && m_NativeString[pos] == ':' )
        return pos;
#endif
    return OFString_npos;
}

size_t OFpath::findFilename() const
{
#if _WIN32
    const size_t pos = m_NativeString.find_last_of( ":\\" );
#else
    const size_t pos = m_NativeString.find_last_of( "/" );
#endif
    if( pos == OFString_npos )
        return empty() ? OFString_npos : 0;
    else
        return pos < m_NativeString.size() ? pos + 1 : OFString_npos;
}

size_t OFpath::findExtension() const
{
#if _WIN32
    const size_t pos = m_NativeString.find_last_of( ".:\\" );
#else
    const size_t pos = m_NativeString.find_last_of( "./" );
#endif
    if
    (
        pos && OFString_npos != pos && m_NativeString[pos] == '.'
    )
    {
        switch( m_NativeString[pos-1] )
        {
#if _WIN32
        case '\\':
        case ':':
#else
        case '/':
#endif
            return OFString_npos;
        case '.':
            if( pos < m_NativeString.size() - 1 )
                return pos;
            if( pos == 1 )
                return OFString_npos;
            switch( m_NativeString[pos-2] )
            {
#if _WIN32
            case '\\':
            case ':':
#else
            case '/':
#endif
                return OFString_npos;
            default:
                break;
            }
        default:
            break;
        }
        return pos;
    }
    return OFString_npos;
}

const OFpath& OFdirectory_entry::path() const
{
    return m_Path;
}

OFdirectory_entry::operator const OFpath&() const
{
    return OFdirectory_entry::path();
}

OFdirectory_iterator_proxy::OFdirectory_iterator_proxy( const OFdirectory_entry& rhs )
: m_Entry( rhs )
{

}

class OFdirectory_iterator::NativeDirectoryEntry : public OFdirectory_entry
{
public:
    NativeDirectoryEntry( const OFpath& path );
    ~NativeDirectoryEntry();
    OFBool skipInvalidFiles();
    OFBool next();

private:
    OFBool good() const;
    const char* filename() const;
    const OFpath m_Parent;

#ifdef HAVE__FINDFIRST // HAVE__FINDFIRST
    ::_finddata_t m_FileData;
    OFintptr_t m_hFile;
#else // HAVE__FINDFIRST
    ::DIR* m_pDIR;
    ::dirent* m_pDirent;
#if defined(HAVE_READDIR_R) && !defined(READDIR_IS_THREADSAFE)
    Uint8 m_Buffer[sizeof(::dirent) + _POSIX_PATH_MAX + 1];
#endif
#endif // HAVE__FINDFIRST
};

OFdirectory_iterator::NativeDirectoryEntry::NativeDirectoryEntry( const OFpath& path )
: OFdirectory_entry()
, m_Parent( path )
#ifdef HAVE__FINDFIRST
, m_FileData()
, m_hFile( _findfirst( OFconst_cast( char*, (path / "*").c_str() ), &m_FileData ) )
#else // HAVE__FINDFIRST
, m_pDIR( ::opendir( path.c_str() ) )
, m_pDirent()
#if defined(HAVE_READDIR_R) && !defined(READDIR_IS_THREADSAFE)
, m_Buffer()
#endif
#endif // HAVE__FINDFIRST
{
#ifndef HAVE__FINDFIRST
    if( m_pDIR && !next() )
    {
        ::closedir( m_pDIR );
        m_pDIR = OFnullptr;
    }
#endif
}

OFdirectory_iterator::NativeDirectoryEntry::~NativeDirectoryEntry()
{
#ifdef HAVE__FINDFIRST
    ::_findclose( m_hFile );
#else
    if( m_pDIR ) ::closedir( m_pDIR );
#endif
}

OFBool OFdirectory_iterator::NativeDirectoryEntry::good() const
{
#ifdef HAVE__FINDFIRST
    return -1 != m_hFile;
#else
    return m_pDIR;
#endif
}

const char* OFdirectory_iterator::NativeDirectoryEntry::filename() const
{
#ifdef HAVE__FINDFIRST
    return m_FileData.name;
#else
    return m_pDirent->d_name;
#endif
}

OFBool OFdirectory_iterator::NativeDirectoryEntry::next()
{
#ifdef HAVE__FINDFIRST // HAVE__FINDFIRST
    return !::_findnext( m_hFile, &m_FileData );
#else // HAVE__FINDFIRST
#if defined(HAVE_READDIR_R) && !defined(READDIR_IS_THREADSAFE)
#ifdef HAVE_OLD_READDIR_R
    return (m_pDirent = ::readdir_r( m_pDIR, OFreinterpret_cast(dirent*, m_Buffer ) ) );
#else // HAVE_OLD_READDIR_R
    return !::readdir_r( m_pDIR, OFreinterpret_cast(dirent*, m_Buffer ), &m_pDirent ) && m_pDirent;
#endif // HAVE_OLD_READDIR_R
#else // HAVE_READDIR_R && !READDIR_IS_THREADSAFE
    return (m_pDirent = ::readdir( m_pDIR ) );
#endif // HAVE_READDIR_R && !READDIR_IS_THREADSAFE
#endif // HAVE__FINDFIRST
}

OFBool OFdirectory_iterator::NativeDirectoryEntry::skipInvalidFiles()
{
    while( good() && ( !::strcmp( filename(), "." ) || !::strcmp( filename(), ".." ) ) )
    {
        if( !next() )
            return OFFalse;
    }
    if( good() )
    {
        OFdirectory_entry::m_Path = m_Parent / filename();
        return OFTrue;
    }
    return OFFalse;
}

OFdirectory_iterator::OFdirectory_iterator()
: m_pEntry()
{

}

OFdirectory_iterator::OFdirectory_iterator( const OFpath& path )
: m_pEntry( new NativeDirectoryEntry( path ) )
{
    assert( m_pEntry );
    if( !m_pEntry->skipInvalidFiles() )
        m_pEntry.reset();
}

OFdirectory_iterator::OFdirectory_iterator( const OFdirectory_iterator& rhs )
: m_pEntry( rhs.m_pEntry )
{

}

OFdirectory_iterator& OFdirectory_iterator::operator=( const OFdirectory_iterator& rhs )
{
    m_pEntry = rhs.m_pEntry;
    return *this;
}

OFdirectory_iterator::~OFdirectory_iterator()
{

}

OFBool OFdirectory_iterator::operator==( const OFdirectory_iterator& rhs ) const
{
    return m_pEntry == rhs.m_pEntry;
}

OFBool OFdirectory_iterator::operator!=( const OFdirectory_iterator& rhs ) const
{
    return m_pEntry != rhs.m_pEntry;
}

OFdirectory_iterator& OFdirectory_iterator::operator++()
{
    assert( m_pEntry );
    if( !m_pEntry->next() || !m_pEntry->skipInvalidFiles() )
        m_pEntry.reset();
    return *this;
}

OFrvalue<OFdirectory_iterator_proxy> OFdirectory_iterator::operator++(int)
{
    assert( m_pEntry );
    OFdirectory_iterator_proxy proxy( *m_pEntry );
    ++(*this);
    return proxy;
}

OFdirectory_iterator::pointer OFdirectory_iterator::operator->() const
{
    assert( m_pEntry );
    return m_pEntry.get();
}

OFdirectory_iterator::reference OFdirectory_iterator::operator*() const
{
    assert( m_pEntry );
    return *m_pEntry;
}
