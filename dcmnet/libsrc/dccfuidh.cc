/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmUIDHandler
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfuidh.h"

#include "dcmtk/dcmdata/dcuid.h"    /* for dcmFindUIDFromName() */
#include "dcmtk/dcmdata/dcxfer.h"   /* for gLocalByteOrder */

#define SymbolicUID_LocalEndianExplicit    "LocalEndianExplicit"
#define SymbolicUID_OppositeEndianExplicit "OppositeEndianExplicit"


DcmUIDHandler::DcmUIDHandler()
: uid_()
{
}

DcmUIDHandler::DcmUIDHandler(const OFString& arg)
: uid_(arg)
{
  lookupSymbolicUID();
}

DcmUIDHandler::DcmUIDHandler(const char *arg)
: uid_()
{
  if (arg) uid_ = arg;
  lookupSymbolicUID();
}

DcmUIDHandler::DcmUIDHandler(const DcmUIDHandler& arg)
: uid_(arg.uid_)
{
}

DcmUIDHandler& DcmUIDHandler::operator=(const DcmUIDHandler& arg)
{
  if (&arg != this) uid_ = arg.uid_;
  return *this;
}

DcmUIDHandler& DcmUIDHandler::operator=(const OFString& arg)
{
  uid_ = arg;
  lookupSymbolicUID();
  return *this;
}

DcmUIDHandler& DcmUIDHandler::operator=(const char *arg)
{
  if (arg) uid_ = arg; else uid_.clear();
  lookupSymbolicUID();
  return *this;
}

DcmUIDHandler::~DcmUIDHandler()
{
}

void DcmUIDHandler::lookupSymbolicUID()
{
  if (!uid_.empty())
  {
    const char *c = uid_.c_str();
    if ((*c < '0')||(*c > '9'))
    {
      // not a numerical UID, attempt lookup
      if (*c == '=') ++c; // skip leading '=' as used in dump2dcm etc.

      // check special cases "LocalEndianExplicit" and "OppositeEndianExplicit"
      if (0 == strcmp(SymbolicUID_LocalEndianExplicit, c))
      {
         if (gLocalByteOrder == EBO_BigEndian) uid_ = UID_BigEndianExplicitTransferSyntax;
         else uid_ = UID_LittleEndianExplicitTransferSyntax;
         return;
      }

      if (0 == strcmp(SymbolicUID_OppositeEndianExplicit, c))
      {
         if (gLocalByteOrder == EBO_LittleEndian) uid_ = UID_BigEndianExplicitTransferSyntax;
         else uid_ = UID_LittleEndianExplicitTransferSyntax;
         return;
      }

      // no special case, look-up UID in name table
      const char *uid = dcmFindUIDFromName(c);
      if (uid) uid_ = uid;
    }
  }
}

OFBool DcmUIDHandler::isValidUID() const
{
  if (!uid_.empty())
  {
    const char *c = uid_.c_str();
    do
    {
      if (((*c < '0')||(*c > '9')) && (*c != '.')) return OFFalse;
      ++c;
    } while (*c);
    return OFTrue;
  }
  return OFFalse;
}
