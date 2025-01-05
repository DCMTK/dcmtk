/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Helper functions to convert between std::string and OFString
 *
 */

#ifndef OFSTRHLP_H
#define OFSTRHLP_H

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */
#include "dcmtk/ofstd/ofstring.h"      /* for class OFString */
#include <string>                      /* for std::string */

/** convert OFString to std::string.
 *  This helper function converts an OFString object into an equivalent std::string.
 *  @param arg string to convert
 *  @return string as std::string object
 */
inline STD_NAMESPACE string OFString_to_std_string(const OFString& arg)
{
#ifdef HAVE_STL_STRING
  // OFString is just a typedef for std::string
  return arg;
#else
  return STD_NAMESPACE string(arg.c_str(), arg.length());
#endif
}

/** convert std::string to OFString.
 *  This helper function converts a std::string object into an equivalent OFString.
 *  @param arg string to convert
 *  @return string as OFString object
 */
inline OFString std_string_to_OFString(const STD_NAMESPACE string& arg)
{
#ifdef HAVE_STL_STRING
  // OFString is just a typedef for std::string
  return arg;
#else
  return OFString(arg.c_str(), arg.length());
#endif
}

#endif /* OFSTRHLP_H */
