/*
 *
 *  Copyright (C) 1994-2010, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmUIDHandler
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-06-25 09:15:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dccfuidh.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dccfuidh.h"

#include "dcmtk/dcmdata/dcuid.h"    /* for dcmFindUIDFromName() */
#include "dcmtk/dcmdata/dcxfer.h"   /* for gLocalByteOrder */

#define INCLUDE_CSTRING             /* for strcmp() */
#include "dcmtk/ofstd/ofstdinc.h"

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
  if (uid_.length() > 0)
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
  if (uid_.length() > 0)
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


/*
 * CVS/RCS Log
 * $Log: dccfuidh.cc,v $
 * Revision 1.3  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.2  2005-12-08 15:44:33  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2003/06/10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
