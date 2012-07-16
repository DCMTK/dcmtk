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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface for a dictionary entry in the loadable DICOM data dictionary
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
class DCMTK_DCMDATA_EXPORT DcmDictEntry: public DcmTagKey
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
    friend DCMTK_DCMDATA_EXPORT STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmDictEntry& e);

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
