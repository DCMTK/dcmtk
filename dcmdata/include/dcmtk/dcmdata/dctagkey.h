/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Basis class for dicom tags.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-21 07:52:40 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMTAGKEY_H
#define DCMTAGKEY_H 1

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"

/*
** Defines
*/

/// macro for an "undefined" attribute tag that is never used in DICOM
#define DCM_UndefinedTagKey     DcmTagKey(0xffff, 0xffff)

/** class maintaining a attribute tag (group and element number)
 */
class DcmTagKey
{
public:

    /** default constructor
     */
    DcmTagKey();

    /** copy constructor
     *  @param key [in] The tag key to initialize from
     */
    DcmTagKey(const DcmTagKey& key);

    /** constructor
     *  @param g group
     *  @param e element
     */
    DcmTagKey(Uint16 g, Uint16 e);

    /** destructor
     */
    virtual ~DcmTagKey();

    /** set value to given tag key
     *  @param key attribute tag to copy
     */
    void set(const DcmTagKey& key);

    /** set value to given group and element
     *  @param g group
     *  @param e element
     */
    void set(Uint16 g, Uint16 e);

    /** set group to given number
     *  @param g group
     */
    void setGroup(Uint16 g);

    /** set element to given number
     *  @param e element
     */
    void setElement(Uint16 e);

    /** returns group number
     *  @return returns the group number of the tag key
     */
    Uint16 getGroup() const;

    /** returns element number
     *  @return Returns the element number of the tag key
     */
    Uint16 getElement() const;

    /** checks whether the tag key is a valid group length element.
     *  Also calls hasValidGroup().
     *  @return returns OFTrue if tag key is a valid group length element
     */
    OFBool isGroupLength() const;

    /** returns true if the tag key is private, ie. whether it has an odd group
     *  number. Also hasValidGroup() is called.
     *  @return returns OFTrue if group is private and valid.
     */
    OFBool isPrivate() const;

    /** returns true, if tag is a private reservation tag
     *  of the form (gggg,00xx) with gggg being odd and
     *  xx in the range of 10 and FF.
     *  @return returns OFTrue if tag key is a private reservation key
     */
    OFBool isPrivateReservation() const;

    /** returns true, if group is valid (permitted in DICOM files).
     *  Referring to the standard, groups 1, 3, 5, 7 and 0xFFFF are illegal.
     *  @return returns OFTrue if tag key has a valid group number.
     */
    OFBool hasValidGroup() const;

    /** generate a simple hash code for this attribute tag,
     *  used for fast look-up in the DICOM dictionary
     *  @return hash code for this tag
     */
    Uint32 hash() const; // generate simple hash code

    /** assignment operator for initializing this tag
     *  key from an existing one.
     *  @param key [in] The key to copy from
     *  @return "this" initialization
     */
    DcmTagKey& operator = (const DcmTagKey& key);

    /** Comparison operator. Returns true if both group and
     *  element number are the same.
     *  @param key key to compare with
     *  @return true if tag keys are the same
     */
    int operator == (const DcmTagKey& key) const;

    /** negation operator. Returns true if either group or element number
     *  are not the same.
     *  @param key key to compare with
     *  @return true if tag keys are not the same
     */
    int operator != (const DcmTagKey& key) const;

    /** 'less than' operator. Returns true if the given tag key is greater
     *  than "this".
     *  @param key key to compare with
     *  @return true if given key is greater than "this"
     */
    int operator < (const DcmTagKey& key) const;

    /** 'greater than' operator. Returns true if the given tag key is smaller
     *  than "this".
     *  @param key key to compare with
     *  @return true if "this" key is smaller than given one.
     */
    int operator > (const DcmTagKey& key) const;

    /** 'less or equal' operator. Returns true if the given tag key is greater
     *  or the same as "this".
     *  @param key key to compare with
     *  @return true if given key is greater or the same as "this"
     */
    int operator <= (const DcmTagKey& key) const;

    /** 'greater or equal' operator. Returns true if the given tag key is
     *  smaller or equal as "this".
     *  @param key key to compare with
     *  @return true if "this" key is smaller or equal to given one.
     */
    int operator >= (const DcmTagKey& key) const;

    friend STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmTagKey& k);

    /** convert tag key to string having the form "(gggg,eeee)".
     *  @return the string representation of this tag key
     */
    OFString toString() const;

    /** returns true if a data element with the given tag key can
     *  be digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    OFBool isSignableTag() const;

protected:

    /// less-than operation comparing only group numbers
    int groupLT(const DcmTagKey& key) const;

    /// greater-than operation comparing only group numbers
    int groupGT(const DcmTagKey& key) const;

    /// comparison operation comparing only group numbers
    int groupEQ(const DcmTagKey& key) const;

    /// less-than operation comparing only element numbers
    int elementLT(const DcmTagKey& key) const;

    /// greater-than operation comparing only element numbers
    int elementGT(const DcmTagKey& key) const;

    /// comparison operation comparing only element numbers
    int elementEQ(const DcmTagKey& key) const;

private:

    /// tag group number
    Uint16 group;
    /// tag element number
    Uint16 element;

};

/** stream output operator for tag keys
 *  @param s output stream
 *  @param k tag key
 *  @return reference to output stream
 */
STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmTagKey& k);

/*
** inline versions of functions
*/

/* constructors and destructor */

inline
DcmTagKey::DcmTagKey()
  : group(0xffff),
    element(0xffff)
{
}

inline
DcmTagKey::DcmTagKey(const DcmTagKey& key)
  : group(key.group),
    element(key.element)
{
}

inline
DcmTagKey::DcmTagKey(Uint16 g, Uint16 e)
  : group(g),
    element(e)
{
}

inline
DcmTagKey::~DcmTagKey()
{
}

/* access methods */

inline void
DcmTagKey::set(const DcmTagKey& key)
{
    group = key.group;
    element = key.element;
}

inline void
DcmTagKey::set(Uint16 g, Uint16 e)
{
    group = g;
    element = e;
}

inline void
DcmTagKey::setGroup(Uint16 g)
{
    group = g;
}

inline void
DcmTagKey::setElement(Uint16 e)
{
    element = e;
}

inline Uint16
DcmTagKey::getGroup() const
{
    return group;
}

inline Uint16
DcmTagKey::getElement() const
{
    return element;
}

inline OFBool
DcmTagKey::isGroupLength() const
{
    return (element == 0) && hasValidGroup();
}

inline OFBool
DcmTagKey::isPrivate() const
{
    return ((group & 1) != 0 ) && hasValidGroup();
}

inline OFBool
DcmTagKey::isPrivateReservation() const
{
    // private reservation has element number ranging from 0x0010 to 0x00FF
    return isPrivate() && (element >= 0x10) && (element <= 0xFF);
}

inline OFBool
DcmTagKey::hasValidGroup() const
{
    // group numbers 1, 3, 5, 7 and 0xFFFF are illegal in DICOM
    if (((group & 1) != 0) && ((group <= 7) || (group == 0xFFFF)))
        return OFFalse;
    else
        return OFTrue;
}

inline DcmTagKey&
DcmTagKey::operator=(const DcmTagKey& key)
{
    set(key);
    return *this;
}

/* Simple Hash Function */

inline Uint32
DcmTagKey::hash() const
{
    // generate simple hash code
    return (((getGroup() << 16) & 0xffff0000) | (getElement() & 0xffff));
}

/* Comparisons */

inline int
DcmTagKey::groupLT(const DcmTagKey& key) const
{
    return (getGroup() < key.getGroup());
}

inline int
DcmTagKey::groupGT(const DcmTagKey& key) const
{
    return (getGroup() > key.getGroup());
}

inline int
DcmTagKey::groupEQ(const DcmTagKey& key) const
{
    return getGroup() == key.getGroup();
}

inline int
DcmTagKey::elementLT(const DcmTagKey& key) const
{
    return (getElement() < key.getElement());
}

inline int
DcmTagKey::elementGT(const DcmTagKey& key) const
{
    return (getElement() > key.getElement());
}

inline int
DcmTagKey::elementEQ(const DcmTagKey& key) const
{
    return getElement() == key.getElement();
}

inline int
DcmTagKey::operator == (const DcmTagKey& key) const
{
    return ( groupEQ(key) && elementEQ(key) );
}

inline int
DcmTagKey::operator != (const DcmTagKey& key) const
{
    return !(*this == key);
}

inline int
DcmTagKey::operator < (const DcmTagKey& key) const
{
    return (groupLT(key) || (groupEQ(key) && elementLT(key)));
}

inline int
DcmTagKey::operator > (const DcmTagKey& key) const
{
    return (groupGT(key) || (groupEQ(key) && elementGT(key)));
}

inline int
DcmTagKey::operator <= (const DcmTagKey& key) const
{
    return (*this < key) || (*this == key);
}

inline int
DcmTagKey::operator >= (const DcmTagKey& key) const
{
    return (*this > key) || (*this == key);
}

#endif


/*
** CVS/RCS Log:
** $Log: dctagkey.h,v $
** Revision 1.23  2010-10-21 07:52:40  joergr
** Added virtual destructor in order to avoid warnings reported by gcc with
** additional flags.
**
** Revision 1.22  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.20  2009-04-20 16:00:08  joergr
** Added method that checks whether the tag key is a valid group length element.
**
** Revision 1.19  2009-01-15 16:06:27  onken
** Added convenience methods for private tag handling. Added doxygen
** documentation for not or badly documented member functions.
**
** Revision 1.18  2007-11-29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.17  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.16  2005/12/08 16:28:45  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.15  2004/01/16 14:08:00  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.14  2003/11/13 14:06:36  meichel
** Fixed definition of DCM_UndefinedTagKey
**
** Revision 1.13  2003/11/05 15:56:31  meichel
** Added declaration of operator<< for DcmTagKeys.
**   Fixes compilation issue on Visual C++ 6.0 SP 0.
**
** Revision 1.12  2002/04/16 13:41:44  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.11  2001/11/19 15:23:11  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.10  2001/11/16 15:54:40  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.9  2001/06/01 15:48:45  meichel
** Updated copyright header
**
** Revision 1.8  2000/11/07 16:56:10  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.7  2000/03/08 16:26:19  meichel
** Updated copyright header.
**
** Revision 1.6  2000/02/07 14:45:16  meichel
** Removed const qualifier from DcmTagKey::toString(), avoids warning on Irix.
**
** Revision 1.5  1999/03/31 09:24:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1999/03/17 11:08:54  meichel
** added method DcmTagKey::toString()
**
** Revision 1.3  1998/07/15 15:48:54  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.2  1997/08/26 13:45:54  hewett
** Added simple hash function method.
**
** Revision 1.1  1995/11/23 16:38:04  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/
