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
 *  Author:  Gerd Ehlers, Andreas Barth, Andrew Hewett
 *
 *  Purpose: Definition of the DcmVR class for Value Representation
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMVR_H
#define DCMVR_H 1

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofglobal.h"

/** Global flag to enable/disable the generation of VR=UN
 */
extern OFGlobal<OFBool> dcmEnableUnknownVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the generation of VR=UT
 */
extern OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration; /* default OFTrue */

/** Global flag to enable/disable the automatic re-conversion of defined
 *  length UN elements read in an explicit VR transfer syntax, if the real
 *  VR is defined in the data dictionary.
 */
extern OFGlobal<OFBool> dcmEnableUnknownVRConversion; /* default OFFalse */

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

    /// other float
    EVR_OF,

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

    /// unique identifier
    EVR_UI,

    /// unsigned long
    EVR_UL,

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

    /// up="unsigned pointer", used internally for DICOMDIR suppor
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

    /// used internally for uncompressed pixeld data
    EVR_PixelData,

    /// used internally for overlay data
    EVR_OverlayData,

    /// used internally for elements with unknown VR with 2-byte length field in explicit VR
    EVR_UNKNOWN2B
};


/** a class representing a DICOM Value Representation
 */
class DcmVR
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


/*
 * CVS/RCS Log:
 * $Log: dcvr.h,v $
 * Revision 1.28  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.27  2010-03-01 09:08:45  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.26  2008-06-23 12:09:13  joergr
 * Fixed inconsistencies in Doxygen API documentation.
 *
 * Revision 1.25  2007/11/29 14:30:35  meichel
 * Updated doxygen API documentation
 *
 * Revision 1.24  2005/12/08 16:28:50  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.23  2005/11/15 18:28:02  meichel
 * Added new global flag dcmEnableUnknownVRConversion that enables the automatic
 *   re-conversion of defined length UN elements read in an explicit VR transfer
 *   syntax, if the real VR is defined in the data dictionary. Default is OFFalse,
 *   i.e. to retain the previous behavior.
 *
 * Revision 1.22  2005/11/15 16:59:24  meichel
 * Added new pseudo VR type EVR_lt that is used for LUT Data when read in
 *   implicit VR, which may be US, SS or OW. DCMTK always treats EVR_lt like OW.
 *
 * Revision 1.21  2003/06/12 13:31:46  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.20  2003/03/21 13:06:46  meichel
 * Minor code purifications for warnings reported by MSVC in Level 4
 *
 * Revision 1.19  2002/12/06 12:20:19  joergr
 * Added support for new value representation Other Float String (OF).
 *
 * Revision 1.18  2002/11/27 12:07:24  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.17  2001/06/01 15:48:47  meichel
 * Updated copyright header
 *
 * Revision 1.16  2000/04/14 15:42:56  meichel
 * Global VR generation flags are now derived from OFGlobal and, thus,
 *   safe for use in multi-thread applications.
 *
 * Revision 1.15  2000/03/08 16:26:21  meichel
 * Updated copyright header.
 *
 * Revision 1.14  2000/02/29 11:48:38  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.13  2000/02/23 15:11:42  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.12  2000/02/01 10:12:03  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.11  1999/06/10 10:44:51  meichel
 * Replaced some #if statements by more robust #ifdef
 *
 * Revision 1.10  1999/03/31 09:24:54  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
