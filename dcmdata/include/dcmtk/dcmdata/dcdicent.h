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
 *  Purpose: Interface for a dictionary entry in the loadable DICOM data dictionary
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2010-11-17 15:17:51 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDICENT_H
#define DCDICENT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcvr.h"

#define INCLUDE_CSTRING               /* for strcmp() */
#include "dcmtk/ofstd/ofstdinc.h"

/// constant describing an unlimited VM
#define DcmVariableVM   -1

#define DCM_INRANGE(x,a,b) (((x) >= (a)) && ((x) <= (b)))
#define DCM_IS_ODD(x) (((x) % 2) == 1)
#define DCM_IS_EVEN(x) (((x) % 2) == 0)


/** attribute tag group/element range restrictions
 */
enum DcmDictRangeRestriction
{
    /// integer range
    DcmDictRange_Unspecified,

    /// odd range
    DcmDictRange_Odd,

    /// even range
    DcmDictRange_Even
};


/** each object of this class manages one entry of the
 *  global DICOM data dictionary.
 */
class DcmDictEntry: public DcmTagKey
{
public:

    /** constructor
     *  @param g attribute tag group
     *  @param e attribute tag element
     *  @param vr value representation
     *  @param nam attribute name
     *  @param vmMin lower limit for value multiplicity
     *  @param vmMax upper limit for value multiplicity, DcmVariableVM for unlimited
     *  @param vers standard version name, may be NULL
     *  @param doCopyStrings true if strings should be copied, false if only referenced
     *  @param pcreator private creator name, may be NULL (for standard tags)
     */
    DcmDictEntry(Uint16 g, Uint16 e, DcmVR vr,
        const char* nam, int vmMin, int vmMax,
        const char* vers, OFBool doCopyStrings,
        const char* pcreator);

    /** constructor for repeating tags
     *  @param g attribute tag group lower limit
     *  @param e attribute tag element lower limit
     *  @param ug attribute tag group upper limit
     *  @param ue attribute tag element upper limit
     *  @param vr value representation
     *  @param nam attribute name
     *  @param vmMin lower limit for value multiplicity
     *  @param vmMax upper limit for value multiplicity, DcmVariableVM for unlimited
     *  @param vers standard version name, may be NULL
     *  @param doCopyStrings true if strings should be copied, false if only referenced
     *  @param pcreator private creator name, may be NULL (for standard tags)
     */
    DcmDictEntry(Uint16 g, Uint16 e, Uint16 ug, Uint16 ue, DcmVR vr,
        const char* nam, int vmMin, int vmMax,
        const char* vers, OFBool doCopyStrings,
        const char* pcreator);

    /// copy constructor
    DcmDictEntry(const DcmDictEntry& e);

    /// destructor
    ~DcmDictEntry();

    /* access methods */

    /// returns VR object by value
    DcmVR getVR() const
    {
        return valueRepresentation;
    }

    /// returns VR code
    DcmEVR getEVR() const
    {
        return valueRepresentation.getEVR();
    }

    /// returns standard version string, may be NULL
    const char* getStandardVersion() const
    {
        return standardVersion;
    }

    /// returns tag name
    const char* getTagName() const
    {
        return tagName;
    }

    /// returns private creator code, may be NULL
    const char* getPrivateCreator() const
    {
        return privateCreator;
    }

    /** checks if the private creator code equals the given string
     *  @param c string to compare with, may be NULL
     *  @return true if equal, false otherwise
     */
    int privateCreatorMatch(const char *c) const
    {
      return
      (
        ((privateCreator == NULL) && (c == NULL)) ||
        (privateCreator && c && (0 == strcmp(privateCreator, c)))
      );
    }

    /** checks if the private creator code of this object matches
     *  the one of the given object.
     *  @param arg dictionary entry to compare with
     *  @return true if private creators are equal, false otherwise
     */
    int privateCreatorMatch(const DcmDictEntry& arg) const
    {
      return privateCreatorMatch(arg.privateCreator);
    }

    /// returns lower limit for VM (value multiplicity)
    int getVMMin() const
    {
        return valueMultiplicityMin;
    }

    /// returns upper limit for VM (value multiplicity), DcmVariableVM for unlimited
    int getVMMax() const
    {
        return valueMultiplicityMax;
    }

    /// returns true if element has a single valid VM value
    OFBool isFixedSingleVM() const
    {
        return ((valueMultiplicityMin != DcmVariableVM) &&
                      (valueMultiplicityMin == valueMultiplicityMax));
    }

    /// returns true if element has a fixed VM range
    OFBool isFixedRangeVM() const
    {
        return ((valueMultiplicityMin != DcmVariableVM) &&
                      (valueMultiplicityMax != DcmVariableVM));
    }

    /// returns true if element has a variable VM range (no upper limit)
    OFBool isVariableRangeVM() const
    {
        return ((valueMultiplicityMin != DcmVariableVM) &&
                      (valueMultiplicityMax == DcmVariableVM));
    }

    /** converts entry into repeating tag entry by defining an upper limit
     *  for group and element, taken from the given tag key.
     *  @param key tag key containing upper limit for group and element
     */
    void setUpper(const DcmTagKey& key)
    {
        upperKey = key;
    }

    /** converts entry into repeating tag entry by defining an upper limit
     *  for tag group
     *  @param ug upper limit for tag group
     */
    void setUpperGroup(Uint16 ug)
    {
        upperKey.setGroup(ug);
    }

    /** converts entry into repeating tag entry by defining an upper limit
     *  for tag element
     *  @param ue upper limit for tag element
     */
    void setUpperElement(Uint16 ue)
    {
        upperKey.setElement(ue);
    }

    /// returns upper limit for tag group
    Uint16 getUpperGroup() const
    {
        return upperKey.getGroup();
    }

    /// returns upper limit for tag element
    Uint16 getUpperElement() const
    {
        return upperKey.getElement();
    }

    /// returns attribute tag as DcmTagKey object by value
    DcmTagKey getKey() const
    {
        return * OFstatic_cast(const DcmTagKey *, this);
    }

    /// returns upper limits for attribute tag as DcmTagKey object by value
    DcmTagKey getUpperKey() const
    {
        return upperKey;
    }

    /// returns true if entry is has a repeating group
    int isRepeatingGroup() const
    {
        return (getGroup() != getUpperGroup());
    }

    /// returns true if entry is has a repeating element
    int isRepeatingElement() const
    {
        return (getElement() != getUpperElement());
    }

    /// returns true if entry is repeating (group or element)
    int isRepeating() const
    {
        return (isRepeatingGroup() || isRepeatingElement());
    }

    /// returns group range restriction
    DcmDictRangeRestriction getGroupRangeRestriction() const
    {
        return groupRangeRestriction;
    }

    /// sets group range restriction
    void setGroupRangeRestriction(DcmDictRangeRestriction rr)
    {
        groupRangeRestriction = rr;
    }

    /// returns element range restriction
    DcmDictRangeRestriction getElementRangeRestriction() const
    {
        return elementRangeRestriction;
    }

    /// sets element range restriction
    void setElementRangeRestriction(DcmDictRangeRestriction rr)
    {
        elementRangeRestriction = rr;
    }

    /* containment */

    /** checks if the given tag key and private creator code are covered
     *  by this object.
     *  @param key tag key
     *  @param privCreator private creator, may be NULL
     *  @return true if this entry contains the given tag for the given private creator
     */
    int contains(const DcmTagKey& key, const char *privCreator) const /* this contains key */
    {
        if ((getGroupRangeRestriction() == DcmDictRange_Even) &&
            DCM_IS_ODD(key.getGroup()))
            return OFFalse;
        else if ((getGroupRangeRestriction() == DcmDictRange_Odd) &&
            DCM_IS_EVEN(key.getGroup()))
            return OFFalse;
        else if ((getElementRangeRestriction() == DcmDictRange_Even) &&
            DCM_IS_ODD(key.getElement()))
            return OFFalse;
        else if ((getElementRangeRestriction() == DcmDictRange_Odd) &&
            DCM_IS_EVEN(key.getElement()))
            return OFFalse;
        else if (! privateCreatorMatch(privCreator))
            return OFFalse;
        else
        {
            const OFBool groupMatches=DCM_INRANGE(key.getGroup(), getGroup(), getUpperGroup());
            OFBool found=groupMatches && DCM_INRANGE(key.getElement(), getElement(), getUpperElement());
            if (!found && groupMatches && privCreator)
                found=DCM_INRANGE(key.getElement() & 0xFF, getElement(), getUpperElement());
            return found;
        }
    }

    /** checks if this entry contains the given name
     *  @param name attribute name, must not be NULL
     *  @return true if tagName matches the given string
     */
    int contains(const char *name) const /* this contains named key */
    {
        return !strcmp( tagName, name );
    }

    /* set relations */

    /** checks if this entry describes a true subset of tag range
     *  described by the given entry.
     *  @param e entry to compare with
     *  @return true if this object is subset of e
     */
    int subset(const DcmDictEntry& e) const /* this is a subset of key */
    {
        return ( (getGroup() >= e.getGroup()) &&
                 (getUpperGroup() <= e.getUpperGroup()) &&
                 (getElement() >= e.getElement()) &&
                 (getUpperElement() <= e.getUpperElement()) &&
                 privateCreatorMatch(e.privateCreator)
            );
    }

    /** checks if this entry describes the same tag range as the given entry.
     *  @param e entry to compare with
     *  @return true if objects describe the same tag range
     */
    int setEQ(const DcmDictEntry& e) const /* this is set equal to key */
    {
        return ( (getGroup() == e.getGroup()) &&
                 (getUpperGroup() == e.getUpperGroup()) &&
                 (getElement() == e.getElement()) &&
                 (getUpperElement() == e.getUpperElement()) &&
                 (getGroupRangeRestriction() == e.getGroupRangeRestriction()) &&
                 (getElementRangeRestriction() == e.getElementRangeRestriction()) &&
                 privateCreatorMatch(e.privateCreator)
            );
    }

    /// friend operator<<
    friend STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmDictEntry& e);

private:

    /// private undefined copy assignment operator
    DcmDictEntry &operator=(const DcmDictEntry &);

    /** upper limit of repeating group and element (lower limit is inherited
     *   from DcmTagKey)
     */
    DcmTagKey upperKey;

    /// value representation
    DcmVR valueRepresentation;

    /// attribute name
    const char *tagName;

    /// lower limit for VM
    int valueMultiplicityMin;

    /// upper limit for VM
    int valueMultiplicityMax;

    /// standard version name, may be NULL
    const char *standardVersion;

    /// true if strings are copies (i.e. should be deleted upon destruction)
    OFBool stringsAreCopies;

    /// restriction (even, odd, unrestricted) for group range
    DcmDictRangeRestriction groupRangeRestriction;

    /// restriction (even, odd, unrestricted) for element range
    DcmDictRangeRestriction elementRangeRestriction;

    /// private creator name, may be NULL
    const char *privateCreator;
};

#endif /* !DCDICENT_H */


/*
** CVS/RCS Log:
** $Log: dcdicent.h,v $
** Revision 1.26  2010-11-17 15:17:51  meichel
** Fixed issue with data dictionary causing private tags with group number
**   range and flexible element number not to be found in the dictionary.
**
** Revision 1.25  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.24  2010-06-25 09:15:19  uli
** Fixed issues with compiling with HAVE_STD_STRING.
**
** Revision 1.23  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.22  2009-11-04 09:58:07  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.21  2006-08-15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.20  2005/12/08 16:28:08  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.19  2004/01/16 14:07:03  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.18  2003/08/14 09:00:56  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.17  2002/11/27 12:07:21  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.16  2002/07/23 14:21:25  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.15  2002/04/16 13:41:44  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.14  2001/06/01 15:48:36  meichel
** Updated copyright header
**
** Revision 1.13  2000/03/08 16:26:13  meichel
** Updated copyright header.
**
** Revision 1.12  1999/03/31 09:24:35  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1998/07/15 15:48:45  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.10  1997/08/26 13:44:59  hewett
** Modified constructors to take const parameters.
**
** Revision 1.9  1997/07/31 14:40:35  meichel
** Created copy constructor for class DcmDictEntry, required by dcmcheck.
**
** Revision 1.8  1997/07/21 08:25:07  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.7  1997/04/18 08:04:39  andreas
** - Minor corrections: correct some warnings of the SUN-C++ Compiler
**   concerning the assignments of wrong types and inline compiler
**   errors
**
** Revision 1.6  1997/04/15 16:25:05  hewett
** Corrected data dictionary bug whereby the even/odd range restrictions
** were not being taken into consideration when searching the dictionary.
**
** Revision 1.5  1996/09/24 16:24:58  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/09/18 16:37:09  hewett
** Added capability to search data dictionary by tag name.
**
** Revision 1.3  1996/03/20 16:43:49  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
*/
