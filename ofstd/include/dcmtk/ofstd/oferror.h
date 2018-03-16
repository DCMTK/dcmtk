/*
 *
 *  Copyright (C) 2017-2018, OFFIS e.V.
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
 *  Author: Nikolas Goldhammer
 *
 *  Purpose:
 *      Implementing platform abstracting error code handling.
 *
 */

#ifndef OFERROR_H
#define OFERROR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

// also require STL string, since otherwise OFsystem_error would become incompatible with OFString
#if defined(HAVE_STL_SYSTEM_ERROR) && defined(HAVE_STL_STRING)

#include <system_error>
typedef STD_NAMESPACE error_category OFerror_category;
typedef STD_NAMESPACE error_code OFerror_code;

inline const OFerror_category& OFsystem_category() { return STD_NAMESPACE system_category(); }
inline const OFerror_category& OFgeneric_category() { return STD_NAMESPACE generic_category(); }

#else // fallback implementations

#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstring.h"

// for Doxygen such that it does not ignore the globally declared functions
/// @file oferror.h Declares classes and functions for platform abstracting error code handling.

/** OFerror_category serves as the base class for specific error category types,
 *  such as OFsystem_category. It is possible to make your own error_category class.
 *  The objects of error category classes are treated as singletons, passed by reference.
 *  @note this implementation is meant to be a subset of the C++11's std::error_category
 *  that lacks the following features: error condition support, iostream category,
 *                                     future category.
 *  See: http://en.cppreference.com/w/cpp/error/error_category to compare OFerror_category
 *  against std::error_category.
 *
 *  The following table describes the possible operations on two instances <i>a</i> and <i>b</i>
 *  of %OFerror_category:
 *  <table border>
 *    <tr><th>Expression</th><th>Meaning</th></tr>
 *    <tr>
 *      <td><center><kbd>a != b</kbd></center></td>
 *      <td>Compares two OFerror_category objects and evaluates to OFTrue if both objects refer to different error categories.</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>a == b</kbd></center></td>
 *      <td>Compares two OFerror_category objects and evaluates to OFTrue if both objects refer to the same error category.</td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>a &lt; b</kbd></center></td>
 *      <td>
 *        Implements a total order on OFerror_category objects. The semantics of one category being compared less than another
 *        are intentionally not defined, but, if one category compares less than to another it will stay that way during the
 *        remaining execution of the program and one and the same category will never compare less than to itself.
 *      </td>
 *    </tr>
 *  </Table>
 *  @see OFgeneric_category()
 *  @see OFsystem_category()
 */
class DCMTK_OFSTD_EXPORT OFerror_category
{
public:

    /** Default constructor, used by derived classes.
     */
    inline OFerror_category() {}

    /** Virtual destructor, does nothing.
     */
    virtual ~OFerror_category() {}

    /** Obtains the name of the category, for example "generic".
     *  @return a character pointer that refers to the name of the category.
     */
    virtual const char* name() const = 0;

    /** Constructs an explanatory string for an error code.
     *  @param code an integer that shall be interpreted as an error code.
     *  @return an explanatory error message for the given code as an OFString.
     */
    virtual OFString message( int code ) const = 0;

    // operator implementations, see above table for documentation
#ifndef DOXYGEN
    inline OFBool operator==(const OFerror_category& rhs) const { return this == &rhs; }
    inline OFBool operator!=(const OFerror_category& rhs) const { return this != &rhs; }
    inline OFBool operator<(const OFerror_category& rhs) const { return this < &rhs; }
#endif // NOT DOXYGEN

private:

    // ensure singleton behavior by disabling the copy constructor and assignment
    // operator
#ifndef DOXYGEN
    OFerror_category(const OFerror_category& other);
    OFerror_category& operator=(const OFerror_category& rhs);
#endif // NOT DOXYGEN
};


/** OFerror_code is a platform abstracting wrapper for platform specific error codes.
 *  Each OFerror_code object holds an error code originating from the operating system
 *  or some low-level interface and a pointer to an object of type OFerror_category,
 *  which corresponds to the said interface. The error code values may be not unique across
 *  different error categories.
 *  @note This implementation is meant to be a subset of the C++11's std::error_code
 *  that lacks the following features: error condition support, iostream category,
 *                                     future category.
 *  See: http://en.cppreference.com/w/cpp/error/error_code to compare OFerror_code against
 *  std::error_code.
 *
 *  The following table describes the possible operations on two instances <i>a</i> and <i>b</i>
 *  of %OFerror_code:
 *  <table border>
 *    <tr><th>Expression</th><th>Meaning</th></tr>
 *    <tr>
 *      <td><center><kbd>if(a), while(a), ...</kbd></center></td>
 *      <td>
 *        Test whether the error code refers to an error message or means that everything went fine.
 *        OFTrue if the object refers to an error, OFFalse otherwise.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>a != b</kbd></center></td>
 *      <td>
 *        Compares two OFerror_code objects, evaluates to OFTrue if both objects refer to a different
 *        error code, that is, if either the code or the category (or both) differ, OFFalse otherwise.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>a == b</kbd></center></td>
 *      <td>
 *        Compares two OFerror_code objects, evaluates to OFTrue if both objects refer to the same
 *        error code, that is, if both the code and the category are equal, OFFalse otherwise.
 *      </td>
 *    </tr>
 *    <tr>
 *      <td><center><kbd>a &lt; b</kbd></center></td>
 *      <td>
 *        Implements a total order on OFerror_code objects. Will return OFTrue if the category of <i>a</i>
 *        compares less than to the category of <i>b</i>. Will return OFFalse if the category of <i>b</i>
 *        compares less than to the category of <i>a</i>. If both objects refer to the same category, they
 *        will be ordered using the actual error code integer.
 *      </td>
 *    </tr>
 *  </Table>
 */
class DCMTK_OFSTD_EXPORT OFerror_code
{
public:

    /** Default constructor.
     *  Initializes an error code with value 0 (success)
     *  and category OFsystem_category().
     */
    OFerror_code();

    /** Constructs an error code from the given arguments.
     *  @param code the actual error code.
     *  @param category a reference to the OFerror_category that shall be used.
     */
    OFerror_code( int code, const OFerror_category& category );

    /** Replaces the contents with the given error code and category.
     *  @param code the actual error code.
     *  @param category a reference to the OFerror_category that shall be used.
     */
    void assign( int code, const OFerror_category& category );

    /** Sets the error code to value 0 (success) and the category
     *  to OFsystem_category().
     */
    void clear();

    /** Obtains the actual error code.
     *  @return the error code as an integer.
     */
    int value() const;

    /** Obtains the category linked to this error code.
     *  @return a reference to the linked error category.
     */
    const OFerror_category& category() const;

    /** Constructs an explanatory string for this error code using the
     *  linked error category.
     *  @return the error message as an OFString.
     */
    OFString message() const;

    // declare overloaded operators, see above table for documentation
#ifndef DOXYGEN
#ifdef HAVE_CXX11
    explicit
#endif
    operator OFBool() const;
    OFBool operator!=( const OFerror_code& rhs ) const;
    OFBool operator==( const OFerror_code& rhs ) const;
    OFBool operator<( const OFerror_code& rhs ) const;
#endif // NOT DOXYGEN

private:

    /// holds the error value.
    int m_Code;

    /// holds the error category
    const OFerror_category* m_Category;
};

/** Retrieves a reference to an OFerror_category object used for operating system
 *  specific error codes.
 *  The object is required to override the virtual function OFerror_category::name()
 *  to return a pointer to the string "system".
 *  @return a reference to the system error category.
 */
DCMTK_OFSTD_EXPORT const OFerror_category& OFsystem_category();

/** Retrieves a reference to an OFerror_category object used for generic error codes.
 *  The object is required to override the virtual function OFerror_category::name()
 *  to return a pointer to the string "generic".
 *  @return a reference to the generic error category.
 */
DCMTK_OFSTD_EXPORT const OFerror_category& OFgeneric_category();

#endif // NOT HAVE_STL_SYSTEM_ERROR && HAVE_STL_STRING

#endif // OFERROR_H
