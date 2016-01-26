/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth, Andrew Hewett
 *
 *  Purpose: Definition of the DcmVR class for Value Representation
 *
 */

#ifndef DCMVR_H
#define DCMVR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofglobal.h"
#include "dcmtk/dcmdata/dcdefine.h"

// include this file in doxygen documentation

/** @file dcvr.h
 *  @brief definition and handling of value representations (VR)
 */

/** Global flag to enable/disable the generation of VR=UN, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=OB is used instead.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableUnknownVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=UT, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UN (if enabled) or alternatively VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=OF, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UN (if enabled) or alternatively VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableOtherFloatVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=OD, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UN (if enabled) or alternatively VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableOtherDoubleVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=OL, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UN (if enabled) or alternatively VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableOtherLongVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=UR, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UT (if enabled), VR=UN (if enabled) or alternatively
 *  VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableUniversalResourceIdentifierOrLocatorVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=UC, which has been
 *  introduced after the first edition of the DICOM standard (1993).
 *  If disabled, the VR=UN (if enabled) or alternatively VR=OB is used.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableUnlimitedCharactersVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the automatic re-conversion of defined
 *  length UN elements read in an explicit VR transfer syntax, if the real
 *  VR is defined in the data dictionary.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableUnknownVRConversion; /* default OFFalse */


/** Global function that enables the generation of "new" value representations,
 *  i.e. the ones that have been introduced after the first edition of the DICOM
 *  standard (1993). See above dcmEnableXXXVRGeneration() flags for details.
 */
DCMTK_DCMDATA_EXPORT void dcmEnableGenerationOfNewVRs();

/** Global function that disables the generation of "new" value representations,
 *  i.e. the ones that have been introduced after the first edition of the DICOM
 *  standard (1993). See above dcmEnableXXXVRGeneration() flags for details.
 */
DCMTK_DCMDATA_EXPORT void dcmDisableGenerationOfNewVRs();


/*
** VR Enumerations.
** NB: The order of entries has to conform to the order in DcmVRDict (see dcmvr.cc)!
**     If not an error message is reported and the program aborts (only in DEBUG mode).
*/
enum DcmEVR
{
    /// application entity title
    EVR_AE,

    /// age string
    EVR_AS,

    /// attribute tag
    EVR_AT,

    /// code string
    EVR_CS,

    /// date string
    EVR_DA,

    /// decimal string
    EVR_DS,

    /// date time string
    EVR_DT,

    /// float single-precision
    EVR_FL,

    /// float double-precision
    EVR_FD,

    /// integer string
    EVR_IS,

    /// long string
    EVR_LO,

    /// long text
    EVR_LT,

    /// other byte
    EVR_OB,

    /// other double
    EVR_OD,

    /// other float
    EVR_OF,

    /// other long
    EVR_OL,

    /// other word
    EVR_OW,

    /// person name
    EVR_PN,

    /// short string
    EVR_SH,

    /// signed long
    EVR_SL,

    /// sequence of items
    EVR_SQ,

    /// signed short
    EVR_SS,

    /// short text
    EVR_ST,

    /// time string
    EVR_TM,

    /// unlimited characters
    EVR_UC,

    /// unique identifier
    EVR_UI,

    /// unsigned long
    EVR_UL,

    /// universal resource identifier or universal resource locator (URI/URL)
    EVR_UR,

    /// unsigned short
    EVR_US,

    /// unlimited text
    EVR_UT,

    /// OB or OW depending on context
    EVR_ox,

    /// SS or US depending on context
    EVR_xs,

    /// US, SS or OW depending on context, used for LUT Data (thus the name)
    EVR_lt,

    /// na="not applicable", for data which has no VR
    EVR_na,

    /// up="unsigned pointer", used internally for DICOMDIR support
    EVR_up,

    /// used internally for items
    EVR_item,

    /// used internally for meta info datasets
    EVR_metainfo,

    /// used internally for datasets
    EVR_dataset,

    /// used internally for DICOM files
    EVR_fileFormat,

    /// used internally for DICOMDIR objects
    EVR_dicomDir,

    /// used internally for DICOMDIR records
    EVR_dirRecord,

    /// used internally for pixel sequences in a compressed image
    EVR_pixelSQ,

    /// used internally for pixel items in a compressed image
    EVR_pixelItem,

    /// used internally for elements with unknown VR (encoded with 4-byte length field in explicit VR)
    EVR_UNKNOWN,

    /// unknown value representation
    EVR_UN,

    /// used internally for uncompressed pixel data
    EVR_PixelData,

    /// used internally for overlay data
    EVR_OverlayData,

    /// used internally for elements with unknown VR with 2-byte length field in explicit VR
    EVR_UNKNOWN2B
};


/** a class representing a DICOM Value Representation
 */
class DCMTK_DCMDATA_EXPORT DcmVR
{
public:

    /// default constructor
    DcmVR()
    : vr(EVR_UNKNOWN)
    {
    }

    /** constructor
     *  @param evr enumerated VR value
     */
    DcmVR(DcmEVR evr)
    : vr(EVR_UNKNOWN)
    {
      // the set method is safeguarded against incorrect passing of integer values
      setVR(evr);
    }

    /** constructor
     *  @param vrName symbolic name of value representation
     */
    DcmVR(const char* vrName)
    : vr(EVR_UNKNOWN)
    {
      setVR(vrName);
    }

    /** copy constructor
     *  @param avr VR value
     */
    DcmVR(const DcmVR& avr)
    : vr(avr.vr)
    {
    }

    /** assign new VR value
     *  @param evr enumerated VR value
     */
    void setVR(DcmEVR evr);

    /** assign new VR value by name
     *  @param vrName symbolic name of value representation
     */
    void setVR(const char* vrName);

    /** assign new VR value
     *  @param avr VR value
     */
    void setVR(const DcmVR& avr) { vr = avr.vr; }

    /** copy assignment operator
     *  @param arg vr to assign from
     */
    DcmVR& operator=(const DcmVR& arg)
    {
      vr = arg.vr;
      return *this;
    }

    /** get enumerated VR managed by this object
     *  @return enumerated VR
     */
    DcmEVR getEVR() const { return vr; }

    /** get enumerated standard VR managed by this object.
     *  If this object manages a non-standard, internal VR such as EVR_ox,
     *  this method returns the enumerated VR to which the internal VR will
     *  be mapped when writing the DICOM object.
     *
     *  Please note that some VR, e.g. EVR_pixelItem, won't be written as
     *  EVR_UNKNOWN, although this method will return that value for them.
     *  This means that e.g. usesExtendedLengthEncoding() for the returned VR
     *  might not be correct.
     *
     *  Also note that DcmItem::checkAndUpdateVR() will in some cases influence
     *  the VR which is written out.
     *  @return enumerated VR
     */
    DcmEVR getValidEVR() const;

    /** get symbolic VR name for this object
     *  @return VR name string, never NULL
     */
    const char* getVRName() const ;

    /** get symbolic standard VR name for this object
     *  If this object manages a non-standard, internal VR such as EVR_ox,
     *  this method returns the name of the VR to which the internal VR will
     *  be mapped when writing the DICOM object.
     *  @return VR name string, never NULL
     */
    const char* getValidVRName() const;

    /** compute the size for non-empty values of this VR.
     *  For fixed size VRs such as OW, US, SL, the method returns the size
     *  of each value, in bytes.  For variable length VRs (strings), it returns 1.
     *  For internal VRs it returns 0.
     *  @return size of values of this VR
     */
    size_t getValueWidth() const;

    /** returns true if VR is a standard DICOM VR
     *  @return true if VR is a standard DICOM VR
     */
    OFBool isStandard() const;

    /** returns true if VR is for internal use only
     *  @return true if VR is for internal use only
     */
    OFBool isForInternalUseOnly() const;

    /** returns true if VR represents a string
     *  @return true if VR represents a string
     */
    OFBool isaString() const;

    /** returns true if VR uses an extended length encoding for explicit transfer syntaxes
     *  @return true if VR uses an extended length encoding for explicit transfer syntaxes
     */
    OFBool usesExtendedLengthEncoding() const;

    /** check if VRs are equivalent
     *  VRs are considered equivalent if equal or if one of them is an internal VR
     *  and the other one is a possible standard VR to which the internal one maps.
     *  @param avr VR to compare with
     *  @return true if VRs are equivalent, false otherwise
     */
    OFBool isEquivalent(const DcmVR& avr) const;

    /* minimum and maximum length of a value with this VR
    ** (in bytes assuming single byte characters)
    */

    /** return minimum length of a value with this VR (in bytes), assuming single byte characters
     *  @return minimum length of a value
     */
    Uint32 getMinValueLength() const;

    /** return maximum length of a value with this VR (in bytes), assuming single byte characters
     *  @return maximum length of a value
     */
    Uint32 getMaxValueLength() const;

private:
    /// the enumerated VR value
    DcmEVR vr;
};


#endif /* !DCMVR_H */
