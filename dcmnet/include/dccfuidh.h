/*
 *
 *  Copyright (C) 1994-2003, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:27:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dccfuidh.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCCFUIDH_H
#define DCCFUIDH_H

#include "osconfig.h"
#include "ofstring.h" /* for class OFString */

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
class DcmUIDHandler
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

/*
 * CVS/RCS Log
 * $Log: dccfuidh.h,v $
 * Revision 1.1  2003-06-10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
