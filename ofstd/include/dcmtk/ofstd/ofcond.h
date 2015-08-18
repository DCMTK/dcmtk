/*
 *
 *  Copyright (C) 2001-2015, OFFIS e.V.
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
 */


#ifndef OFCOND_H
#define OFCOND_H

#include "dcmtk/config/osconfig.h"  /* include OS specific configuration first */

#include "dcmtk/ofstd/oftypes.h"    /* for class OFBool */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CSTRING             /* for strdup() */
#define INCLUDE_CSTDLIB             /* for free() */
#include "dcmtk/ofstd/ofstdinc.h"


// include this file in doxygen documentation

/** @file ofcond.h
 *  @brief Error handling, codes and strings for all modules
 */


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


/** @name global condition constants.
 *  All constants defined here use module number 0, which is reserved for
 *  global definitions. Other constants are defined in other modules.
 */
//@{
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
/// condition constant: error, could not generate filename
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_CouldNotGenerateFilename;

/// condition constant: error, directory does not exist
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_DirectoryDoesNotExist;
/// condition constant: error, directory is not writable
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_DirectoryNotWritable;
/// condition constant: error, could not generate directory name
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_CouldNotGenerateDirectoryName;
/// condition constant: error, call to setuid() failed
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_setuidFailed;

/// condition constant: error, function not yet implemented
extern DCMTK_OFSTD_EXPORT const OFConditionConst EC_NotYetImplemented;

/// status code constant: error, cannot open character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotOpenEncoding;
/// status code constant: error, cannot close character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotCloseEncoding;
/// status code constant: error, cannot convert character encoding
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotConvertEncoding;
/// status code constant: error, cannot control character encoding converter
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotControlConverter;

/// status code constant: error, cannot create directory
extern DCMTK_OFSTD_EXPORT const unsigned short EC_CODE_CannotCreateDirectory;
//@}


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
