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
 *  Purpose: Wrapper class for the POD struct hostent that supports
 *           efficient memory handling.
 *
 */

#ifndef OFNETDB_H
#define OFNETDB_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

BEGIN_EXTERN_C
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofstring.h"

/** A non-POD version of "struct hostent" for thread- and memory-safe data
 *  access. Wraps the contents of a "struct hostent" instance to a non-POD
 *  object containing RAII-style data (e.g. OFString instead of const char*).
 *  To handle the old pointer behavior, OFHostent objects can have an invalid
 *  state in which case all members are undefined. You can test whether an
 *  OFHostent object is invalid or not with the overloaded operators
 *  "operator !" and "operator OFBool". Therefore, it behaves quite the same
 *  way as pointers in this regard.
 *  @note The downside of this non-POD class is that it leads to some
 *    unnecessary string copy operations. The resulting performance penalty
 *    should be insignificant. However, implementing this class based on
 *    auto_ptr / unique_ptr or using c++11 move sematics would prevent that,
 *    if somebody thinks it is necessary.
 */
class DCMTK_OFSTD_EXPORT OFStandard::OFHostent
{
public:
    /// default constructor that creates an invalid object.
    OFHostent();

    /** test if a OFHostent object is invalid.
     *  @return OFTrue if the object is invalid, otherwise OFFalse.
     */
    OFBool operator!() const;

    /** test if a OFHostent object is valid.
     *  @return OFTrue if the object is valid, otherwise OFFalse.
     */
    operator OFBool() const;

    /// official name of host.
    OFString           h_name;

    /// a vector containing all known aliases.
    OFVector<OFString> h_aliases;

    /// vector containing the addresses.
    OFVector<OFString> h_addr_list;

    /// host address type.
    int                h_addrtype;

    /// the length of each address (all have the same length).
    int                h_length;

private:
    /// only OFStandard may instantiate a valid object.
    friend class OFStandard;

    /** the constructor that "sucks out" a struct hostent instance.
     *  @param h the struct hostent instance to clone into this object.
     */
    OFHostent(hostent* const h);

    /// internal state, OFTrue when valid.
    OFBool ok;
};

#endif // OFNETDB_H
