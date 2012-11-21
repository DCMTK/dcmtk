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
 *  Purpose: Wrapper class for the POD struct group that supports
 *           efficient memory handling.
 *
 */

#ifndef OFGRP_H
#define OFGRP_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#ifdef HAVE_GRP_H // Only makes sense if we have this header

BEGIN_EXTERN_C
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofstring.h"


/** A non-POD version of "struct group" for thread- and memory-safe data
 *  access. Wraps the contents of a "struct group" instance to a non-POD
 *  object containing RAII-style data (e.g. OFString instead of const char*).
 *  To handle the old pointer behavior, OFGroup objects can have an invalid
 *  state in which case all members are undefined. You can test whether an
 *  OFGroup object is invalid or not with the overloaded operators
 *  "operator !" and "operator OFBool". Therefore, it behaves quite the same
 *  way as pointers in this regard.
 *  @note The downside of this non-POD class is that it leads to some
 *    unnecessary string copy operations. The resulting performance penalty
 *    should be insignificant. However, implementing this class based on
 *    auto_ptr / unique_ptr or using c++11 move sematics would prevent that,
 *    if somebody thinks it is necessary.
 */
class OFStandard::OFGroup
{
public:
    /// default constructor that creates an invalid object.
    OFGroup();

    /** test if a OFGroup object is invalid.
     *  @return OFTrue if the object is invalid, otherwise OFFalse.
     */
    OFBool operator!() const;

    /** test if a OFGroup object is valid.
     *  @return OFTrue if the object is valid, otherwise OFFalse.
     */
    operator OFBool() const;

    /// the group name.
    OFString           gr_name;

    /// the group password.
    OFString           gr_passwd;

    /// usernames of group members.
    OFVector<OFString> gr_mem;

    /// the group ID.
    gid_t              gr_gid;

private:
    /// only OFStandard may instantiate a valid object.
    friend class OFStandard;

    /** the constructor that "sucks out" a struct group instance.
     *  @param g the struct group instance to clone into this object.
     */
    OFGroup(group* const g);

    /// internal state, OFTrue when valid.
    OFBool ok;
};

#endif // HAVE_GRP_H

#endif // OFGRP_H
