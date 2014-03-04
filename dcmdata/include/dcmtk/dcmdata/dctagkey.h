/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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
 */

#ifndef DCTAGKEY_H
#define DCTAGKEY_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcdefine.h"

/*
** Defines
*/

/// macro for an "undefined" attribute tag that is never used in DICOM
#define DCM_UndefinedTagKey     DcmTagKey(0xffff, 0xffff)

/** class maintaining a attribute tag (group and element number)
 */
class DCMTK_DCMDATA_EXPORT DcmTagKey
{
public:

    /** default constructor
     */
    inline DcmTagKey();

    /** copy constructor
     *  @param key [in] The tag key to initialize from
     */
    inline DcmTagKey(const DcmTagKey& key);

    /** constructor
     *  @param g group
     *  @param e element
     */
    inline DcmTagKey(Uint16 g, Uint16 e);

    /** destructor
     */
    virtual inline ~DcmTagKey();

    /** set value to given tag key
     *  @param key attribute tag to copy
     */
    inline void set(const DcmTagKey& key);

    /** set value to given group and element
     *  @param g group
     *  @param e element
     */
    inline void set(Uint16 g, Uint16 e);

    /** set group to given number
     *  @param g group
     */
    inline void setGroup(Uint16 g);

    /** set element to given number
     *  @param e element
     */
    inline void setElement(Uint16 e);

    /** returns group number
     *  @return returns the group number of the tag key
     */
    inline Uint16 getGroup() const;

    /** returns element number
     *  @return returns the element number of the tag key
     */
    inline Uint16 getElement() const;

    /** returns base tag, i.e. in case of a repeating group tag always the base
     *  group number 0x5000 (curve) or 0x6000 (overlay) is used. For non-repeating
     *  group tags "this" tag key is returned.
     *  @return returns the base tag of the tag key
     */
    DcmTagKey getBaseTag() const;

    /** checks whether the tag key is a valid group length element.
     *  Also calls hasValidGroup().
     *  @return returns OFTrue if tag key is a valid group length element
     */
    inline OFBool isGroupLength() const;

    /** returns true if the tag key is private, i.e. whether it has an odd group
     *  number. Also hasValidGroup() is called.
     *  @return returns OFTrue if group is private and valid.
     */
    inline OFBool isPrivate() const;

    /** returns true, if tag is a private reservation tag of the form (gggg,00xx)
     *  with "gggg" being odd and "xx" in the range of 10 and FF.
     *  @return returns OFTrue if tag key is a private reservation key
     */
    inline OFBool isPrivateReservation() const;

    /** returns true, if group is valid (permitted in DICOM command or data sets).
     *  Referring to the standard, groups 1, 3, 5, 7 and 0xFFFF are illegal.
     *  @return returns OFTrue if tag key has a valid group number.
     */
    inline OFBool hasValidGroup() const;

    /** generate a simple hash code for this attribute tag.
     *  Used for fast look-up in the DICOM dictionary.
     *  @return hash code for this tag
     */
    inline Uint32 hash() const;

    /** assignment operator for initializing this tag key from an existing one
     *  @param key [in] The key to copy from
     *  @return "this" initialization
     */
    inline DcmTagKey& operator = (const DcmTagKey& key);

    /** Comparison operator. Returns true if both group and element number
     *  are the same.
     *  @param key key to compare with
     *  @return true if tag keys are the same
     */
    inline int operator == (const DcmTagKey& key) const;

    /** negation operator. Returns true if either group or element number
     *  are not the same.
     *  @param key key to compare with
     *  @return true if tag keys are not the same
     */
    inline int operator != (const DcmTagKey& key) const;

    /** 'less than' operator. Returns true if the given tag key is greater
     *  than "this".
     *  @param key key to compare with
     *  @return true if given key is greater than "this"
     */
    inline int operator < (const DcmTagKey& key) const;

    /** 'greater than' operator. Returns true if the given tag key is smaller
     *  than "this".
     *  @param key key to compare with
     *  @return true if "this" key is smaller than given one.
     */
    inline int operator > (const DcmTagKey& key) const;

    /** 'less or equal' operator. Returns true if the given tag key is greater
     *  or the same as "this".
     *  @param key key to compare with
     *  @return true if given key is greater or the same as "this"
     */
    inline int operator <= (const DcmTagKey& key) const;

    /** 'greater or equal' operator. Returns true if the given tag key is
     *  smaller or equal as "this".
     *  @param key key to compare with
     *  @return true if "this" key is smaller or equal to given one.
     */
    inline int operator >= (const DcmTagKey& key) const;

    friend DCMTK_DCMDATA_EXPORT STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmTagKey& k);

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
DCMTK_DCMDATA_EXPORT STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmTagKey& k);

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
