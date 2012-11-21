/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
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
 *  Purpose: Wrapper class for the POD struct passwd that supports
 *           efficient memory handling.
 *
 */

#ifndef OFPWD_H
#define OFPWD_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#ifdef HAVE_PWD_H // Only makes sense if we have this header

BEGIN_EXTERN_C
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstring.h"

/** A non-POD version of "struct passwd" for thread- and memory-safe data
 *  access. Wraps the contents of a "struct passwd" instance to a non-POD
 *  object containing RAII-style data (e.g. OFString instead of const char*).
 *  To handle the old pointer behavior, OFPasswd objects can have an invalid
 *  state in which case all members are undefined. You can test whether an
 *  OFPasswd object is invalid or not with the overloaded operators
 *  "operator !" and "operator OFBool". Therefore, it behaves quite the same
 *  way as pointers in this regard.
 *  @note The downside of this non-POD class is that it leads to some
 *    unnecessary string copy operations. The resulting performance penalty
 *    should be insignificant. However, implementing this class based on
 *    auto_ptr / unique_ptr or using c++11 move sematics would prevent that,
 *    if somebody thinks it is necessary.
 */
class OFStandard::OFPasswd
{
public:
    /// default constructor that creates an invalid object.
    OFPasswd();

    /** test if a OFPasswd object is invalid.
     *  @return OFTrue if the object is invalid, otherwise OFFalse.
     */
    OFBool operator!() const;

    /** test if a OFPasswd object is valid.
     *  @return OFTrue if the object is valid, otherwise OFFalse.
     */
    operator OFBool() const;

    /// the username.
    OFString pw_name;

    /// the user password.
    OFString pw_passwd;

    /// the real name.
    OFString pw_gecos;

    /// the home directory.
    OFString pw_dir;

    /// the shell program.
    OFString pw_shell;

    /// the user ID.
    uid_t    pw_uid;

    /// the group ID.
    gid_t    pw_gid;

private:
    /// only OFStandard may instantiate a valid object.
    friend class OFStandard;

    /** the constructor that "sucks out" a struct passwd instance.
     *  @param p the struct passwd instance to clone into this object.
     */
    OFPasswd(passwd* const p);

     /// internal state, OFTrue when valid.
    OFBool ok;
};

#endif // PWD_H

#endif // OFPWD_H
