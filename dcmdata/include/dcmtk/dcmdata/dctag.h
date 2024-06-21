/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: Definition of the class DcmTag
 *
 */

#ifndef DCTAG_H
#define DCTAG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcvr.h"

/*
** Defines
*/

/// default attribute name for unknown attributes
#define DcmTag_ERROR_TagName    "Unknown Tag & Data"

/// macro for the "item" tag (avoid VR lookup)
#define DCM_ItemTag (DcmTag(DCM_Item, EVR_na))
/// macro for the "item delimitation item" tag (avoid VR lookup)
#define DCM_ItemDelimitationItemTag (DcmTag(DCM_ItemDelimitationItem, EVR_na))
/// macro for the "sequence delimitation item" tag (avoid VR lookup)
#define DCM_SequenceDelimitationItemTag (DcmTag(DCM_SequenceDelimitationItem, EVR_na))
/// macro for the "internal use" tag (an attribute that is never used in DICOM)
#define DCM_InternalUseTag (DcmTag(DcmTagKey(0xfffe, 0xfffe), EVR_UNKNOWN))


/** this class encapsulates an attribute tag (group, element) and a VR.
 *  It maintains a private creator identifier for private tags and caches
 *  the attribute name, once it is looked up in the data dictionary.
 *  This class maintains the interface to the DICOM data dictionary,
 *  i.e. performs look-ups of attribute VR and name in the background.
 *  Therefore, creation of DcmTag element is significantly more expensive
 *  than creation of simple DcmTagKey objects unless the VR is passed
 *  in the constructor.
 */
class DCMTK_DCMDATA_EXPORT DcmTag: public DcmTagKey
{
public:
    /// default constructor
    DcmTag();

    /** constructor.
     *  Initializes group/element from given tag key and performs a dictionary
     *  lookup for the VR.  The lookup also considers private tags if the
     *  private creator identifier is defined (not NULL).
     *  @param akey tag key
     *  @param privCreator private creator identifier (optional)
     */
    DcmTag(const DcmTagKey& akey, const char *privCreator = NULL);

    /** constructor.
     *  Initializes group/element from given parameters and performs a
     *  dictionary lookup for the VR.  The lookup also considers private tags
     *  if the private creator identifier is defined (not NULL).
     *  @param g tag group
     *  @param e tag element
     *  @param privCreator private creator identifier (optional)
     */
    DcmTag(Uint16 g, Uint16 e, const char *privCreator = NULL);

    /** constructor.
     *  Initializes group/element and VR from given parameters.
     *  No dictionary lookup needed/performed.
     *  @param akey tag key
     *  @param avr VR
     */
    DcmTag(const DcmTagKey& akey, const DcmVR& avr);

    /** constructor.
     *  Initializes group/element and VR from given parameters.
     *  No dictionary lookup needed/performed.
     *  @param g tag group
     *  @param e tag element
     *  @param avr VR
     */
    DcmTag(Uint16 g, Uint16 e, const DcmVR& avr);

    /** copy constructor
     *  @param tag tag to be copied
     */
    DcmTag(const DcmTag& tag);

    /** constructor.
     *  Initializes tag from given parameter, but uses the specified VR.
     *  This constructor is e.g. useful for attributes with multiple VRs.
     *  No dictionary lookup needed/performed.
     *  @param tag tag to be copied
     *  @param avr VR to be used
     */
    DcmTag(const DcmTag& tag, const DcmVR& avr);

    /// destructor
    ~DcmTag();

    /// copy assignment operator
    DcmTag& operator=(const DcmTag& tag);

    /** comparison operator. Returns true if both group and element number
     *  as well as private creator identifier (if present) are the same.
     *  Please note that the VR is not used for comparison.
     *  @param tag tag to compare with
     *  @return true if tags are the same
     */
    OFBool operator==(const DcmTag& tag) const;

    /** negation operator. Returns true if either group or element number
     *  or private creator identifier (if present) are not the same.
     *  Please note that the VR is not used for comparison.
     *  @param tag tag to compare with
     *  @return true if tags are not the same
     */
    OFBool operator!=(const DcmTag& tag) const;

    /// set specific VR
    DcmVR setVR(const DcmVR& avr);

    /// returns VR object by value
    DcmVR getVR() const { return vr; }

    /// returns VR code
    DcmEVR getEVR() const
    {
      return vr.getEVR();
    }

    /// returns name of VR
    const char* getVRName() const
    {
      return vr.getVRName();
    }

    /** returns tag group
     *  @return tag group
     */
    Uint16 getGTag() const
    {
      return getGroup();
    }

    /** returns tag element
     *  @return tag element
     */
    Uint16 getETag() const
    {
      return getElement();
    }

    /** returns a copy of the tag key by value.
     *  @note In most cases, the method getTagKey() should be preferred.
     *  @return copy of tag key, by value
     */
    DcmTagKey getXTag() const
    {
      return * OFstatic_cast(const DcmTagKey *, this);
    }

    /** returns tag key as a const reference
     *  @return tag key
     */
    const DcmTagKey &getTagKey() const
    {
      return *this;
    }

    /** returns name of attribute tag.
     *  If name has not been accessed before, a dictionary lookup under
     *  consideration of the current private creator identifier is performed.
     *  If no attribute name is found, a default name is used.  Never returns
     *  NULL.
     *  @return attribute tag name, never NULL.
     */
    const char* getTagName();

    /** returns the current private creator identifier for this object if any,
     *  NULL otherwise.
     *  @return private creator identifier if present, NULL otherwise
     */
    const char* getPrivateCreator() const;

    /** assigns a private creator identifier and deletes a possibly cached
     *  attribute name since the attribute name could change if a different
     *  private creator identifier is used.
     *  @param privCreator private creator identifier, may be NULL
     */
    void setPrivateCreator(const char *privCreator);

    /** performs a look-up of the VR for the current tag key in the dictionary,
     *  under consideration of the private creator identifier (if defined).
     *  If a dictionary entry is found, the VR of this object is copied
     *  from the dictionary entry, otherwise the VR remains unmodified.
     */
    void lookupVRinDictionary();

    /** returns true if a data element with the given tag and VR can be
     *  digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    OFBool isSignable() const;

    /** returns true if the VR used for writing is "UN"
     */
    OFBool isUnknownVR() const;

    /// returns current status flag
    OFCondition error() const
    {
      return errorFlag;
    }

    // --- static helper functions ---

    /** convert the given string to a DICOM tag value
     *  @param name name or tag of the attribute to be searched for.
     *    If the name of the attribute is given, the spelling has to be consistent
     *    with the spelling used in the data dictionary (e.g. "PatientName").
     *    If the attribute tag is used, the format is "gggg,eeee" (i.e. two
     *    hexadecimal numbers separated by a comma).
     *  @param value variable in which the resulting tag value is stored.
     *    If this functions fails to find the specified tag, this variable
     *    remains unchanged.
     *  @note This method also searches for private tags, but always returns
     *    the first entry that matches (if no standard attribute was found),
     *    i.e. the private creator identifier is ignored (it cannot be specified
     *    for this method).
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    static OFCondition findTagFromName(const char *name,
                                       DcmTag &value);

    // --- re-introduce methods from base class

    using DcmTagKey::operator==;
    using DcmTagKey::operator!=;

protected:

    /** set the members of this tag
     *  @param key attribute tag to be set
     *  @param avr value representation (VR) to be set
     *  @param name attribute name to be set
     *  @param privCreator private creator identifier to be set (might be NULL)
     */
    void setTag(const DcmTagKey &key,
                const DcmVR &avr,
                const char *name,
                const char *privCreator);

private:

    /** replace tagName with copy of given string
     *  @param c new tag name
     */
    void updateTagName(const char *c);

    /** replace privateCreator with copy of given string
     *  @param c new private creator identifier
     */
    void updatePrivateCreator(const char *c);

    /// VR of this attribute tag
    DcmVR vr;

    /// name of this attribute tag, remains NULL unless getTagName() is called
    char *tagName;

    /// private creator identifier, remains NULL unless setPrivateCreator() is
    /// called
    char *privateCreator;

    /// current error code, EC_Normal if a valid VR for the tag is known
    OFCondition errorFlag;

};

#endif /* !DCTAG_H */
