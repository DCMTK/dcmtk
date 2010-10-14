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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: Definition of the class DcmTag
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCTAG_H
#define DCTAG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmdata/dcvr.h"

/// default attribute name for unknown attributes
#define DcmTag_ERROR_TagName    "Unknown Tag & Data"


/** this class encapsulates an attribute tag (group, element) and a VR.
 *  It maintains a private creator name for private tags and caches
 *  the attribute name, once it is looked up in the data dictionary.
 *  This class maintains the interface to the DICOM data dictionary,
 *  i.e. performs look-ups of attribute VR and name in the background.
 *  Therefore, creation of DcmTag element is significantly more expensive
 *  than creation of simple DcmTagKey objects unless the VR is passed
 *  in the constructor.
 */
class DcmTag: public DcmTagKey
{
public:
    /// default constructor
    DcmTag();

    /** constructor.
     *  Initializes group/element from given tag key and performs
     *  a dictionary lookup for the VR.  The lookup only considers
     *  standard tags, tags with private creator are ignored.
     *  @param akey tag key
     */
    DcmTag(const DcmTagKey& akey);

    /** constructor.
     *  Initializes group/element from given parameters and performs
     *  a dictionary lookup for the VR.  The lookup only considers
     *  standard tags, tags with private creator are ignored.
     *  @param g tag group
     *  @param e tag element
     */
    DcmTag(Uint16 g, Uint16 e);

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

    /// copy constructor
    DcmTag(const DcmTag& tag);

    /// destructor
    ~DcmTag();

    /// copy assignment operator
    DcmTag& operator=(const DcmTag& tag);

    /// set specific VR
    DcmVR setVR(const DcmVR& avr);

    /// returns VR object by value
    DcmVR getVR() const { return vr; }

    /// returns VR code
    DcmEVR getEVR() const { return vr.getEVR(); }

    /// returns name of VR
    const char* getVRName() const { return vr.getVRName(); }

    /** returns tag group
     *  @return tag group
     */
    Uint16 getGTag() const { return getGroup(); }

    /** returns tag element
     *  @return tag element
     */
    Uint16 getETag() const { return getElement(); }

    /** returns a copy of the tag key by value
     *  @return copy of tag key, by value
     */
    DcmTagKey getXTag() const
    {
      return * OFstatic_cast(const DcmTagKey *, this);
    }

    /** returns name of attribute tag.
     *  If name has not been accessed before, a dictionary lookup
     *  under consideration of the current private creator code
     *  is performed.  If no attribute name is found, a default
     *  name is used.  Never returns NULL.
     *  @return attribute tag name, never NULL.
     */
    const char* getTagName();

    /** returns the current private creator string for this object
     *  if any, NULL otherwise.
     *  @return creator code if present, NULL otherwise
     */
    const char* getPrivateCreator() const;

    /** assigns a private creator code and deletes a possibly
     *  cached attribute name since the attribute name could
     *  change if a different private creator code is used.
     *  @param privCreator private creator code, may be NULL
     */
    void setPrivateCreator(const char *privCreator);

    /** performs a look-up of the VR for the current tag key in the dictionary,
     *  under consideration of the private creator (if defined).
     *  If a dictionary entry is found, the VR of this object is copied
     *  from the dictionary entry, otherwise the VR remains unmodified.
     */
    void lookupVRinDictionary();

    /** returns true if a data element with the given tag and VR
     *  can be digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    OFBool isSignable() const;

    /** returns true if the VR used for writing is "UN"
     */
    OFBool isUnknownVR() const;

    /// returns current status flag
    OFCondition error() const { return errorFlag; }

    // --- static helper functions ---

    /** convert the given string to a DICOM tag value
     *  @param name name or tag of the attribute to be searched for.
     *    If the name of the attribute is given the spelling has to be consistent
     *    with the spelling used in the data dictionary (e.g. "PatientName").
     *    If the tag values are used the format is "gggg,eeee" (i.e. two hexa-
     *    decimal numbers separated by a comma).
     *  @param value variable in which the resulting tag value is stored.
     *    If this functions fails to find the specified tag, this variable
     *    remains unchanged.
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    static OFCondition findTagFromName(const char *name,
                                       DcmTag &value);
private:

    /** replace tagName with copy of given string
     *  @param c new tag name
     */
    void updateTagName(const char *c);

    /** replace privateCreator with copy of given string
     *  @param c new private creator
     */
    void updatePrivateCreator(const char *c);

    /// VR of this attribute tag
    DcmVR vr;

    /// name of this attribute tag, remains NULL unless getTagName() is called
    char *tagName;

    /// private creator code, remains NULL unless setPrivateCreator() is called
    char *privateCreator;

    /// current error code, EC_Normal if a valid VR for the tag is known
    OFCondition errorFlag;

};


// *** global constants ********************************


#define ItemTag (DcmTag(DCM_Item))
#define InternalUseTag (DcmTag(DcmTagKey(0xfffe, 0xfffe)))


#endif /* !DCTAG_H */

/*
** CVS/RCS Log:
** $Log: dctag.h,v $
** Revision 1.24  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.23  2010-08-09 13:02:57  joergr
** Updated data dictionary to 2009 edition of the DICOM standard. From now on,
** the official "keyword" is used for the attribute name which results in a
** number of minor changes (e.g. "PatientsName" is now called "PatientName").
**
** Revision 1.22  2009-11-04 09:58:07  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.21  2005-12-08 16:28:44  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.20  2003/08/14 09:00:56  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.19  2002/07/23 14:21:27  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.18  2002/05/24 09:49:13  joergr
** Renamed some parameters/variables to avoid ambiguities.
**
** Revision 1.17  2002/04/30 13:12:12  joergr
** Added static helper function to convert strings (tag names or group/element
** numbers) to DICOM tag objects.
**
** Revision 1.16  2001/11/19 15:23:10  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.15  2001/11/16 15:54:40  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.14  2001/09/25 17:19:29  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.13  2001/06/01 15:48:45  meichel
** Updated copyright header
**
** Revision 1.12  2000/04/14 16:00:58  meichel
** Restructured class DcmTag. Instances don't keep a permanent pointer
**   to a data dictionary entry anymore. Required for MT applications.
**
** Revision 1.11  2000/03/08 16:26:19  meichel
** Updated copyright header.
**
** Revision 1.10  1999/03/31 09:24:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.9  1998/07/15 15:48:54  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.8  1997/05/06 09:26:44  hewett
** The DcmTag::getVMMax() method now returns a maximum value if the attribute
** is unknown.  This makes the default VM=1-n (before it was VM=1).
**
** Revision 1.7  1997/03/26 17:18:01  hewett
** Added member function to obtain a DcmTag's data dictionary reference.
**
** Revision 1.6  1996/04/19 08:37:21  andreas
** correct bug with DEBUG and not DEBUG parts. It was not possible to compile
** the dcmdata library with DEBUG and programs using dcmdata without DEBUG
** (and vice versa)
**
** Revision 1.5  1996/03/13 14:48:32  hewett
** Added useful VR access methods.
**
** Revision 1.4  1996/03/12 15:32:49  hewett
** Added constructor with parameter to explicity set the VR.
**
** Revision 1.3  1996/01/05 13:23:01  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
** Revision 1.2  1995/11/23 16:38:03  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/
