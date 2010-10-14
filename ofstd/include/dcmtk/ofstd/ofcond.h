/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Purpose: class OFCondition and helper classes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:50 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCOND_H
#define OFCOND_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"   /* for class OFBool */
#include "dcmtk/ofstd/ofstring.h"  /* for class OFString */
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CASSERT
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


/** abstract base class for condition codes
 */
class OFConditionBase
{
public:

  /// default constructor
  OFConditionBase()
  {
  }

  /// copy constructor
  OFConditionBase(const OFConditionBase& /* arg */)
  {
  }

  /// destructor
  virtual ~OFConditionBase()
  {
  }

  /** this method returns a pointer to a OFConditionBase object containing a clone
   *  of this object. If deletable() is true, the clone must be a deep copy allocated
   *  on the heap.  If deletable() is false, the clone should be a pointer to this.
   *  @return clone of this object, either deep copy or alias.
   */
  virtual const OFConditionBase *clone() const = 0;

  /** returns a combined code and module for this object.
   *  code is lower 16 bits, module is upper 16 bits
   */
  virtual unsigned long codeAndModule() const = 0;

  /// returns the status for this object.
  virtual OFStatus status() const = 0;

  /// returns the error message text for this object.
  virtual const char *text() const = 0;

  /** checks if this object is deletable, e.g. all instances
   *  of this class are allocated on the heap.
   *  @return true if deletable, false otherwise
   */
  virtual OFBool deletable() const = 0;

  /// returns the module identifier for this object.
  unsigned short module() const
  {
    return OFstatic_cast(unsigned short,((codeAndModule() >> 16) & 0xFFFF));
  }

  /// returns the status code identifier for this object.
  unsigned short code() const
  {
    return OFstatic_cast(unsigned short,(codeAndModule() & 0xFFFF));
  }

  /** comparison operator.
   *  Compares status, code and module but not error text.
   *  @param arg argument to compare to
   *  @return true if equal, false otherwise
   */
  OFBool operator==(const OFConditionBase& arg) const
  {
    return ((status() == arg.status()) && (codeAndModule() == arg.codeAndModule()));
  }

  /** comparison operator, not equal.
   *  Compares status, code and module but not error text.
   *  @param arg argument to compare to
   *  @return false if equal, true otherwise
   */
  OFBool operator!=(const OFConditionBase& arg) const
  {
    return ((status() != arg.status()) || (code() != arg.code()) || (module() != arg.module()));
  }

private:

  /// private unimplemented copy assignment operator
  OFConditionBase& operator=(const OFConditionBase& arg);

};



/** this class is used to declare global condition constants.
 *  OFError instances may keep multiple aliased pointers to an instance of
 *  this class. Therefore, instances should be global constants.
 */
class OFConditionConst: public OFConditionBase
{
public:

  /** constructor.
   *  @param aModule module identifier. 0 is reserved for global codes,
   *    other constants are defined elsewhere.
   *  @param aCode status code that is unique for each module
   *  @param aStatus condition status enum
   *  @param aText error text.  The text is not copied, so the pointer must
   *    remain valid for the full lifetime of this object.
   */
  OFConditionConst(unsigned short aModule, unsigned short aCode, OFStatus aStatus, const char *aText)
  : OFConditionBase()
  , theCodeAndModule(OFstatic_cast(unsigned long, aCode) | OFstatic_cast(unsigned long, aModule << 16))
  , theStatus(aStatus)
  , theText(aText)
  {
  }

  /// copy constructor
  OFConditionConst(const OFConditionConst& arg)
  : OFConditionBase(arg)
  , theCodeAndModule(arg.theCodeAndModule)
  , theStatus(arg.theStatus)
  , theText(arg.theText)
  {
  }

  /// destructor
  virtual ~OFConditionConst()
  {
  }

  /** this method returns a pointer to a OFConditionBase object containing a clone
   *  of this object. In this case, deletable() is false and clone just returns a pointer to this.
   *  @return alias of this object
   */
  virtual const OFConditionBase *clone() const;

  /** returns a combined code and module for this object.
   *  code is lower 16 bits, module is upper 16 bits
   */
  virtual unsigned long codeAndModule() const;

  /// returns the status for this object.
  virtual OFStatus status() const;

  /// returns the error message text for this object.
  virtual const char *text() const;

  /** checks if this object is deletable, e.g. all instances
   *  of this class are allocated on the heap.
   *  @return always false for this class
   */
  virtual OFBool deletable() const;

private:

  /// private undefined copy assignment operator
  OFConditionConst& operator=(const OFConditionConst& arg);

  /// code/module identification. Code is lower 16 bits, module is upper 16 bits
  unsigned long theCodeAndModule;

  /// status
  OFStatus theStatus;

  /// condition description
  const char *theText;

};



/** this class is used to declare condition codes with
 *  user defined error messages.
 */
class OFConditionString: public OFConditionBase
{
public:

  /** constructor.
   *  @param aModule module identifier. 0 is reserved for global codes,
   *    other constants are defined elsewhere.
   *  @param aCode status code that is unique for each module
   *  @param aStatus condition status enum
   *  @param aText error text. The text is copied.
   */
  OFConditionString(unsigned short aModule, unsigned short aCode, OFStatus aStatus, const char *aText)
  : OFConditionBase()
  , theCodeAndModule(OFstatic_cast(unsigned long, aCode) | OFstatic_cast(unsigned long, aModule << 16))
  , theStatus(aStatus)
  , theText()
  {
    if (aText) theText = aText;
  }

  /// copy constructor
  OFConditionString(const OFConditionString& arg)
  : OFConditionBase(arg)
  , theCodeAndModule(arg.theCodeAndModule)
  , theStatus(arg.theStatus)
  , theText(arg.theText)
  {
  }

  /// destructor
  virtual ~OFConditionString()
  {
  }

  /** this method returns a pointer to a OFConditionBase object containing a clone
   *  of this object. The clone is a deep copy allocated on the heap.
   *  @return deep copy of this object
   */
  virtual const OFConditionBase *clone() const;

  /** returns a combined code and module for this object.
   *  code is lower 16 bits, module is upper 16 bits
   */
  virtual unsigned long codeAndModule() const;

  /// returns the status for this object.
  virtual OFStatus status() const;

  /// returns the error message text for this object.
  virtual const char *text() const;

  /** checks if this object is deletable, e.g. all instances
   *  of this class are allocated on the heap.
   *  @return true if deletable, false otherwise
   */
  virtual OFBool deletable() const;

private:
  /// private undefined copy assignment operator
  OFConditionString& operator=(const OFConditionString& arg);

  /// code/module identification. Code is lower 16 bits, module is upper 16 bits
  unsigned long theCodeAndModule;

  /// status
  OFStatus theStatus;

  /// condition description
  OFString theText;
};


// global constant used by OFCondition default constructor.
extern const OFConditionConst ECC_Normal;


/** General purpose class for condition codes. Objects of this class can be
 *  efficiently passed by value since they only contain a single pointer and
 *  no virtual methods. The condition code is maintained by the object of class
 *  OFConditionBase pointed to.
 */
class OFCondition
{
public:

  /** constructor for condition code with user-defined error text
   *  @param base pointer to error base, which must be allocated on the heap.
   *     The object pointed to is deleted upon destruction of this object.
   *     Pointer must not be NULL.
   */
  OFCondition(OFConditionString *base)
  : theCondition(base)
  {
    assert(theCondition);
  }

  /** constructor for condition code with global const condition object
   *  @param base reference to condition base, which must be guaranteed
   *     to exist for the lifetime of this (and every derived) object
   *     since it is only referenced but not copied.
   */
#ifdef OFCONDITION_STRICT_MODE
  // in strict mode OFCondition has no default constructor.
  OFCondition(const OFConditionConst& base)
#else
  OFCondition(const OFConditionConst& base = ECC_Normal)
#endif
  : theCondition(&base)
  {
    assert(theCondition);
  }

  /// copy constructor
  OFCondition(const OFCondition& arg)
  : theCondition(arg.theCondition->clone())
  {
    assert(theCondition);
  }

  /// destructor
  ~OFCondition()
  {
    if (theCondition->deletable())
    {
      delete OFconst_cast(OFConditionBase *, theCondition); // cast away const
    }
  }

  /// copy assignment operator
  OFCondition& operator=(const OFCondition& arg)
  {
    if (&arg != this)
    {
      if (theCondition->deletable())
      {
        delete OFconst_cast(OFConditionBase *, theCondition); // cast away const
      }
      theCondition = arg.theCondition->clone();
      assert(theCondition);
    }
    return *this;
  }

  /// returns the module identifier for this object.
  inline unsigned short module() const
  {
    return theCondition->module();
  }

  /// returns the status code identifier for this object.
  inline unsigned short code() const
  {
    return theCondition->code();
  }

  /// returns the status for this object.
  inline OFStatus status() const
  {
    return theCondition->status();
  }

  /// returns the error message text for this object.
  inline const char *text() const
  {
    return theCondition->text();
  }

  /// returns true if status is OK
  inline OFBool good() const
  {
    OFStatus s = theCondition->status();
    return (s == OF_ok);
  }

  /// returns true if status is not OK, i. e. error or failure
  inline OFBool bad() const
  {
    OFStatus s = theCondition->status();
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
    return (*theCondition == *arg.theCondition);
  }

  /** comparison operator. Compares status, code and module
   *  but not error text.
   *  @param arg error to compare to
   *  @return true if equal, false otherwise
   */
  inline OFBool operator!=(const OFCondition& arg) const
  {
    return (*theCondition != *arg.theCondition);
  }

private:

  /// pointer to the condition base object
  const OFConditionBase *theCondition;

};


/*  global condition constants.
 *  All constants defined here use module number 0 which is reserved for
 *  global definitions. Other constants are defined elsewhere.
 */

/// condition constant: successful completion
extern const OFCondition EC_Normal;

/// condition constant: error, function called with illegal parameters
extern const OFCondition EC_IllegalParameter;

/// condition constant: failure, memory exhausted
extern const OFCondition EC_MemoryExhausted;


/** this macro is a shortcut for creating user-specific error messages.
 */
#define makeOFCondition(A, B, C, D) OFCondition(new OFConditionString((A), (B), (C), (D)))


#endif

/*
 * CVS/RCS Log:
 * $Log: ofcond.h,v $
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
