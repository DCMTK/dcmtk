/*
 *
 *  Copyright (C) 2001-2012, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Uli Schlachter
 *
 *  Purpose: class OFCondition and helper classes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2012-02-23 12:00:28 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCOND_H
#define OFCOND_H

#include "dcmtk/config/osconfig.h"  /* include OS specific configuration first */

#include "dcmtk/ofstd/oftypes.h"    /* for class OFBool */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CASSERT
#define INCLUDE_CSTRING             /* for strdup() */
#define INCLUDE_CSTDLIB             /* for free() */
#include "dcmtk/ofstd/ofstdinc.h"


/** this enumeration describes the return status of an operation.
 */
enum OFStatus
{
  /// no error, operation has completed successfully
  OF_ok,

  /// operation has not completed successfully
  OF_error,

  /// application failure
  OF_failure
};


/** A constant data structure which can be used for an OFCondition.
 *  The reason this exists is because we need a trivially constructible class
 *  (= needs no constructor to be run before being usable) that can hold static
 *  condition codes.
 */
struct DCMTK_OFSTD_EXPORT OFConditionConst
{

  /// module identifier. 0 is reserved for global codes.
  unsigned short theModule;

  /// status code that is unique for each module
  unsigned short theCode;

  /// condition status enum
  OFStatus theStatus;

  /// error text
  const char *theText;

  /** comparison operator. Compares status, code and module
   *  but not error text.
   *  @param arg error to compare to
   *  @return true if equal, false otherwise
   */
  inline OFBool operator==(const OFConditionConst& arg) const
  {
    return ((theStatus == arg.theStatus) && (theCode == arg.theCode) && (theModule == arg.theModule));
  }

  /** comparison operator. Compares status, code and module
   *  but not error text.
   *  @param arg error to compare to
   *  @return true if equal, false otherwise
   */
  inline OFBool operator!=(const OFConditionConst& arg) const
  {
    return !(*this == arg);
  }

};


/*  global condition constants.
 *  All constants defined here use module number 0 which is reserved for
 *  global definitions. Other constants are defined elsewhere.
 */

/// condition constant: successful completion
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_Normal;
/// condition constant: error, function called with illegal parameters
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_IllegalParameter;
/// condition constant: failure, memory exhausted
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_MemoryExhausted;

/// condition constant: error, no character encoding library available
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_NoEncodingLibrary;
/// condition constant: error, no character encoding selected
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_NoEncodingSelected;

/// condition constant: error, could not create temporary file
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_CouldNotCreateTemporaryFile;
/// condition constant: error, invalid filename
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_InvalidFilename;

/// status code constant: error, cannot open character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotOpenEncoding;
/// status code constant: error, cannot close character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotCloseEncoding;
/// status code constant: error, cannot convert character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotConvertEncoding;
/// status code constant: error, cannot control character encoding converter
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotControlConverter;

/** use this macro for creating static OFCondition instances. Instead of an
 *  OFCondition instance which needs a constructor, an instance of
 *  OFConditionConst is created. This avoids the problem of static initializers
 *  (and deinitializers) being executed in undefined order (some other static
 *  initializer might want to use this OFCondition / OFConditionConst instance).
 */
#define makeOFConditionConst(name, module, code, status, text) \
  const OFConditionConst name = { (module), (code), (status), (text) }


/** General purpose class for condition codes. Objects of this class can be
 *  efficiently passed by value. To make this possible, the contained string is
 *  not copied when possible.
 */
class DCMTK_OFSTD_EXPORT OFCondition
{
public:

  /** constructor for condition code with text
   *  @param aModule module identifier. 0 is reserved for global codes,
   *    other constants are defined elsewhere.
   *  @param aCode status code that is unique for each module
   *  @param aStatus condition status enum
   *  @param aText error text.
   */
  OFCondition(unsigned short aModule, unsigned short aCode, OFStatus aStatus, const char *aText)
  : theCondition()
  , ownsText(OFTrue)
  {
    theCondition.theModule = aModule;
    theCondition.theCode = aCode;
    theCondition.theStatus = aStatus;
    /* Be nice when someone dares to pass in NULL */
    if (aText != NULL) {
      theCondition.theText = strdup(aText);
      ownsText = OFTrue;
    } else {
      theCondition.theText = "";
      ownsText = OFFalse;
    }
  }

  /** constructor for condition code from constant data
   *  @param aConst OFConditionConst to use
   */
  OFCondition(const OFConditionConst& aConst = EC_Normal)
  : theCondition(aConst)
  , ownsText(OFFalse)
  {
  }

  /** copy constructor
   *  @param arg OFCondition to copy
   */
  OFCondition(const OFCondition& arg)
  : theCondition(arg.theCondition)
  , ownsText(arg.ownsText)
  {
    // Do we need our own copy of the text?
    if (ownsText)
    {
      theCondition.theText = strdup(theCondition.theText);
    }
  }

  /// destructor
  ~OFCondition()
  {
    if (ownsText)
    {
      free(OFconst_cast(char *, theCondition.theText)); // cast away const
    }
  }

  /** copy assignment operator
   *  @param arg The OFCondition instance to copy
   *  @return *this
   */
  OFCondition& operator=(const OFCondition& arg)
  {
    if (&arg != this)
    {
      if (ownsText)
      {
        free(OFconst_cast(char *, theCondition.theText)); // cast away const
      }
      theCondition = arg.theCondition;
      ownsText = arg.ownsText;
      if (ownsText)
      {
        theCondition.theText = strdup(arg.theCondition.theText);
      }
    }
    return *this;
  }

  /** get the module identifier for this object.
   *  @return the module identifier for this object.
   *  @see code()
   */
  inline unsigned short module() const
  {
    return theCondition.theModule;
  }

  /** get the status code identifier for this object. This uniquely identifies
   *  the error code within the module.
   *  @return the status code identifier for this object.
   *  @see module()
   */
  inline unsigned short code() const
  {
    return theCondition.theCode;
  }

  /** get the error status this object represents.
   *  @return the status for this object.
   */
  inline OFStatus status() const
  {
    return theCondition.theStatus;
  }

  /** get a human readable text representation of this error code. The returned
   *  string is owned by this OFCondition instance and must not be modified or
   *  freed.
   *  @return the error message text for this object.
   */
  inline const char *text() const
  {
    return theCondition.theText;
  }

  /** internal function only, don't use yourself.
   *  @return an equivalent OFConditionConst for this object.
   */
  inline const OFConditionConst& condition() const
  {
    return theCondition;
  }

  /** check if the status is OK.
   *  @return true if status is OK, else false
   */
  inline OFBool good() const
  {
    OFStatus s = theCondition.theStatus;
    return (s == OF_ok);
  }

  /** check if the status is not OK, i.e.\ error or failure.
   *  @return true if status is not OK, else false
   */
  inline OFBool bad() const
  {
    OFStatus s = theCondition.theStatus;
    return (s != OF_ok);
  }

#ifdef OFCONDITION_IMPLICIT_BOOL_CONVERSION
  /* Implicit conversion from OFCondition to bool might
   * not always be a good idea since it can hide unwanted constructs.
   * Therefore, we disable this operator by default.
   */

  /** conversion operator to bool.
   *  @return true if status is OK, false otherwise
   */
  inline operator OFBool() const
  {
    return good();
  }
#endif

  /** comparison operator. Compares status, code and module
   *  but not error text.
   *  @param arg error to compare to
   *  @return true if equal, false otherwise
   */
  inline OFBool operator==(const OFCondition& arg) const
  {
    return theCondition == arg.theCondition;
  }

  /** comparison operator. Compares status, code and module
   *  but not error text.
   *  @param arg error to compare to
   *  @return true if equal, false otherwise
   */
  inline OFBool operator!=(const OFCondition& arg) const
  {
    return theCondition != arg.theCondition;
  }

private:

  /// The condition information
  OFConditionConst theCondition;

  /// Does theCondition.theText point to our own heap string which must be freed?
  OFBool ownsText;

};


/** returns true if lhs refers to the same OFCondition as rhs
 *  @param lhs left-hand side condition
 *  @param rhs right-hand side condition
 *  @return true if OFCondition::operator==() returns true
 */
inline OFBool operator== (const OFConditionConst& lhs, const OFCondition& rhs)
{
  return lhs == rhs.condition();
}

/** returns true if lhs refers to the same OFCondition as rhs
 *  @param lhs left-hand side condition
 *  @param rhs right-hand side condition
 *  @return true if OFCondition::operator==() returns true
 */
inline OFBool operator== (const OFCondition& lhs, const OFConditionConst& rhs)
{
  return lhs.condition() == rhs;
}

/** returns true if lhs refers to a different OFCondition as rhs
 *  @param lhs left-hand side condition
 *  @param rhs right-hand side condition
 *  @return true if OFCondition::operator!=() returns true
 */
inline OFBool operator!= (const OFConditionConst& lhs, const OFCondition& rhs)
{
  return lhs != rhs.condition();
}

/** returns true if lhs refers to a different OFCondition as rhs
 *  @param lhs left-hand side condition
 *  @param rhs right-hand side condition
 *  @return true if OFCondition::operator!=() returns true
 */
inline OFBool operator!= (const OFCondition& lhs, const OFConditionConst& rhs)
{
  return lhs.condition() != rhs;
}


/** this macro is a shortcut for creating user-specific error messages.
 */
#define makeOFCondition(A, B, C, D) OFCondition((A), (B), (C), (D))


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofcond.h,v $
 * Revision 1.20  2012-02-23 12:00:28  joergr
 * Fixed Doxygen documentation issue: Appended "\" in case "." does not mark
 * the end of a sentence.
 *
 * Revision 1.19  2012-02-17 10:03:04  joergr
 * Added missing "stdlib" include, required for free() on some Linux systems.
 *
 * Revision 1.18  2012-02-15 14:50:46  uli
 * Removed dependency on static initialization order from OFCondition.
 * All static condition objects are now created via makeOFConditionConst()
 * in a way that doesn't need a constructor to run. This should only break
 * code which defines its own condition objects, all other changes are
 * backwards compatible.
 *
 * Revision 1.17  2012-02-15 11:31:50  uli
 * Removed OFCONDITION_STRICT_MODE since DCMTK didn't compile in this mode.
 *
 * Revision 1.16  2011-12-14 08:54:00  uli
 * Make it possible to correctly build ofstd as a DLL.
 *
 * Revision 1.15  2011-11-28 11:15:45  joergr
 * Added general condition constant for invalid filenames.
 *
 * Revision 1.14  2011-11-16 13:50:36  uli
 * Added a new class for managing temporary files.
 *
 * Revision 1.13  2011-10-28 09:32:47  joergr
 * Restructured code of OFCharacterEncoding in order to allow particular classes
 * to access more low-level functions, e.g. for opening multiple conversion
 * descriptors at the same time. This will be needed for ISO 2022 support.
 *
 * Revision 1.12  2011-10-24 13:03:07  joergr
 * Changed name of status code constant for "illegal character encoding".
 *
 * Revision 1.11  2011-10-21 09:14:57  joergr
 * Added class for managing and converting between different character encodings
 * based on the libiconv toolkit.
 *
 * Revision 1.10  2010-10-14 13:15:50  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.9  2005/12/08 16:05:50  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.8  2003/12/05 10:37:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright date where appropriate.
 *
 * Revision 1.7  2003/07/09 13:57:43  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.6  2003/07/04 13:31:51  meichel
 * Fixed issues with compiling with HAVE_STD_STRING
 *
 * Revision 1.5  2003/06/12 13:15:59  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.4  2001/11/09 15:44:39  joergr
 * Removed ";" from macro definition to avoid compiler warnings reported by
 * Sun CC 2.0.1.
 *
 * Revision 1.3  2001/10/12 10:42:26  meichel
 * Introduced conditional define OFCONDITION_STRICT_MODE in which the
 *   compatibility options related to the transition to OFCondition are disabled:
 *   No OFCondition default constructor, no typedefs for E_Condition, CONDITION,
 *   no macros for SUCCESS and condition aliases.
 *
 * Revision 1.2  2001/09/25 17:07:24  meichel
 * Disabled implicit conversion to bool, added default constructor
 *   to class OFCondition.
 *
 * Revision 1.1  2001/08/23 16:08:37  meichel
 * Initial release of class OFCondition, a generic approach for condition codes
 *
 *
 */
