/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth, Andrew Hewett
 *
 *  Purpose: Definition of the DcmVR class for Value Representation
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:20:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvr.h,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMVR_H
#define DCMVR_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"
#include "ofglobal.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"

/*
** Global flag to enable/disable the generation of VR=UN
*/
extern OFGlobal<OFBool> dcmEnableUnknownVRGeneration; /* default OFTrue */

/*
** Global flag to enable/disable the generation of VR=UT
*/
extern OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration; /* default OFTrue */

/*
** VR Enumerations.
** NB: The order of entries has to conform to the order in DcmVRDict (see dcmvr.cc)!
**     If not an error message is reported and the program aborts (only in DEBUG mode).
*/
enum DcmEVR
{
    EVR_AE,
    EVR_AS,
    EVR_AT,
    EVR_CS,
    EVR_DA,
    EVR_DS,
    EVR_DT,
    EVR_FL,
    EVR_FD,
    EVR_IS,
    EVR_LO,
    EVR_LT,
    EVR_OB,
    EVR_OF,
    EVR_OW,
    EVR_PN,
    EVR_SH,
    EVR_SL,
    EVR_SQ,
    EVR_SS,
    EVR_ST,
    EVR_TM,
    EVR_UI,
    EVR_UL,
    EVR_US,
    EVR_UT,

    EVR_ox,  /* OB or OW depending on context */
    EVR_xs,  /* SS or US depending on context */
    EVR_na,  /* na="not applicable", for data which has no VR */
    EVR_up,  /* up="unsigned pointer", used internally for DICOMDIR support */

    EVR_item,        /* used internally */
    EVR_metainfo,    /* used internally */
    EVR_dataset,     /* used internally */
    EVR_fileFormat,  /* used internally */
    EVR_dicomDir,    /* used internally */
    EVR_dirRecord,   /* used internally */

    EVR_pixelSQ,     /* used internally */
    EVR_pixelItem,   /* used internally */

    EVR_UNKNOWN,     /* used internally */
    EVR_UN,          /* Unknown value representation - defined in supplement 14 */
    EVR_PixelData,   /* used internally */
    EVR_OverlayData, /* used internally */

    EVR_UNKNOWN2B    /* like EVR_UNKNOWN but without extended length property in explicit VR */
};


class DcmVR {
private:
    DcmEVR vr;
protected:
public:
    DcmVR() : vr(EVR_UNKNOWN)
        { }
    DcmVR(DcmEVR evr) : vr(EVR_UNKNOWN)
        { setVR(evr); }
    DcmVR(const char* vrName) : vr(EVR_UNKNOWN)
        { setVR(vrName); }
    DcmVR(const DcmVR& avr) : vr(avr.vr)
        { }

    void setVR(DcmEVR evr);
    void setVR(const char* vrName);
    void setVR(const DcmVR& avr) { vr = avr.vr; }
    DcmEVR getEVR() const { return vr; }
    DcmEVR getValidEVR() const;
    const char* getVRName() const ;
    const char* getValidVRName() const;
    size_t getValueWidth(void) const;

    /* returns true if VR is a standard DICOM VR */
    OFBool isStandard() const;
    /* returns true if VR is for internal use only */
    OFBool isForInternalUseOnly() const;

    /* returns true if VR represents a string */
    OFBool isaString() const;
    /* returns true if VR uses an extended length encoding for explicit transfer syntaxes */
    OFBool usesExtendedLengthEncoding() const;

    /* returns true if the vr is equivalent */
    int isEquivalent(const DcmVR& avr) const;

    /* minimum and maximum length of a value with this VR
    ** (in bytes assuming single byte characters)
    */
    Uint32 getMinValueLength() const;
    Uint32 getMaxValueLength() const;

};


#endif /* !DCMVR_H */


/*
 * CVS/RCS Log:
 * $Log: dcvr.h,v $
 * Revision 1.19  2002-12-06 12:20:19  joergr
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
