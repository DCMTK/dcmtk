/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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

#ifndef DCCFUIDH_H
#define DCCFUIDH_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h" /* for class OFString */
#include "dcmtk/dcmnet/dndefine.h"

/** helper class that manages a single DICOM UID.
 *  Unlike class DcmUniqueIdentifier, this class does not manage an 
 *  attribute tag or means of encoding/decoding UIDs. Instead it allows 
 *  to convert between numeric and symbolic representation of a UID and 
 *  implements a syntactical check.
 *
 *  This class supports two special Transfer Syntax names, 
 *  "LocalEndianExplicit" and "OppositeEndianExplicit". These are 
 *  resolved to the explicit VR transfer syntaxes of the local byte 
 *  order and the opposite byte order of the system the application is 
 *  running on.
 */
class DCMTK_DCMNET_EXPORT DcmUIDHandler
{
public:
  /// default constructor
  DcmUIDHandler();

  /// construct from OFString
  DcmUIDHandler(const OFString& arg);

  /// construct from C string
  DcmUIDHandler(const char *arg);

  /// copy constructor
  DcmUIDHandler(const DcmUIDHandler& arg);

  /// copy assignment operator
  DcmUIDHandler& operator=(const DcmUIDHandler& arg);

  /// assign from OFString
  DcmUIDHandler& operator=(const OFString& arg);

  /// assign from C string
  DcmUIDHandler& operator=(const char *arg);

  /// destructor
  ~DcmUIDHandler();

  /// equal operator
  OFBool operator==(const DcmUIDHandler& arg) const
  {
    return uid_ == arg.uid_;
  }

  /// not equal operator
  OFBool operator!=(const DcmUIDHandler& arg) const
  {
    return uid_ != arg.uid_;
  }

  /** checks if the UID maintained by this object
   *  is a syntactically valid UID string
   */
  OFBool isValidUID() const;

  /** returns UID as C string
   *  @return UID
   */
  const char *c_str() const
  {
    return uid_.c_str();
  }

  /** returns UID as const OFString reference
   *  @return UID
   */
  const OFString& str() const
  {
    return uid_;
  }

private:

  /** this method checks if the UID value passed to this object
   *  is a symbolic UID and in this case attempts to resolve by name.
   *  This method is called from the constructor, must never be virtual.
   */
  void lookupSymbolicUID();

  /// the UID maintained by this object.
  OFString uid_;
};


#endif
