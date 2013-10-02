/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Purpose: class DcmVR: Value Representation
 *
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dctypes.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

/*
 * global flags
 */
OFGlobal<OFBool> dcmEnableUnknownVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOtherFloatStringVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOtherDoubleStringVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnknownVRConversion(OFFalse);

/*
** VR property table
*/

#define DCMVR_PROP_NONE         0x00
#define DCMVR_PROP_NONSTANDARD  0x01
#define DCMVR_PROP_INTERNAL     0x02
#define DCMVR_PROP_EXTENDEDLENGTHENCODING 0x04
#define DCMVR_PROP_ISASTRING 0x08

struct DcmVREntry {
    DcmEVR vr;                  // Enumeration Value of Value representation
    const char* vrName;         // Name of Value representation
    size_t fValWidth;           // Length of minimal unit, used for swapping
    int propertyFlags;          // Normal, internal, non-standard vr
    Uint32 minValueLength;      // Minimum length of a single value (bytes)
    Uint32 maxValueLength;      // Maximum length of a single value (bytes)
};


static const DcmVREntry DcmVRDict[] = {

    { EVR_AE, "AE", sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_AS, "AS", sizeof(char), DCMVR_PROP_ISASTRING, 4, 4 },
    { EVR_AT, "AT", sizeof(Uint16), DCMVR_PROP_NONE, 4, 4 },
    { EVR_CS, "CS", sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_DA, "DA", sizeof(char), DCMVR_PROP_ISASTRING, 8, 10 },
    { EVR_DS, "DS", sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_DT, "DT", sizeof(char), DCMVR_PROP_ISASTRING, 0, 26},
    { EVR_FL, "FL", sizeof(Float32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_FD, "FD", sizeof(Float64), DCMVR_PROP_NONE, 8, 8 },
    { EVR_IS, "IS", sizeof(char), DCMVR_PROP_ISASTRING, 0, 12 },
    { EVR_LO, "LO", sizeof(char), DCMVR_PROP_ISASTRING, 0, 64 },
    { EVR_LT, "LT", sizeof(char), DCMVR_PROP_ISASTRING, 0, 10240 },
    { EVR_OB, "OB", sizeof(Uint8), DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_OD, "OD", sizeof(Float64), DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_OF, "OF", sizeof(Float32), DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_OW, "OW", sizeof(Uint16), DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_PN, "PN", sizeof(char), DCMVR_PROP_ISASTRING, 0, 64 },
    { EVR_SH, "SH", sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_SL, "SL", sizeof(Sint32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_SQ, "SQ", 0, DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_SS, "SS", sizeof(Sint16), DCMVR_PROP_NONE, 2, 2 },
    { EVR_ST, "ST", sizeof(char), DCMVR_PROP_ISASTRING, 0, 1024 },
    { EVR_TM, "TM", sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_UI, "UI", sizeof(char), DCMVR_PROP_ISASTRING, 0, 64 },
    { EVR_UL, "UL", sizeof(Uint32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_US, "US", sizeof(Uint16), DCMVR_PROP_NONE, 2, 2 },
    { EVR_UT, "UT", sizeof(char), DCMVR_PROP_ISASTRING|DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_ox, "ox", sizeof(Uint8), DCMVR_PROP_NONSTANDARD | DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_xs, "xs", sizeof(Uint16), DCMVR_PROP_NONSTANDARD, 2, 2 },
    { EVR_lt, "lt", sizeof(Uint16), DCMVR_PROP_NONSTANDARD | DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },
    { EVR_na, "na", 0, DCMVR_PROP_NONSTANDARD, 0, 0 },
    { EVR_up, "up", sizeof(Uint32), DCMVR_PROP_NONSTANDARD, 4, 4 },

    /* unique prefixes have been "invented" for the following internal VRs */
    { EVR_item, "it_EVR_item", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_metainfo, "mi_EVR_metainfo", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dataset, "ds_EVR_dataset", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_fileFormat, "ff_EVR_fileFormat", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dicomDir, "dd_EVR_dicomDir", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dirRecord, "dr_EVR_dirRecord", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },

    { EVR_pixelSQ, "ps_EVR_pixelSQ", sizeof(Uint8),
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },
    /* Moved from internal use to non standard only: necessary to distinguish from "normal" OB */
    { EVR_pixelItem, "pi", sizeof(Uint8),
      DCMVR_PROP_NONSTANDARD, 0, DCM_UndefinedLength },

    { EVR_UNKNOWN, "??", sizeof(Uint8), /* EVR_UNKNOWN (i.e. "future" VRs) should be mapped to UN or OB */
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL | DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },

    /* Unknown Value Representation */
    { EVR_UN, "UN", sizeof(Uint8), DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, DCM_UndefinedLength },

    /* Pixel Data - only used in ident() */
    { EVR_PixelData, "PixelData", 0, DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },
    /* Overlay Data - only used in ident() */
    { EVR_OverlayData, "OverlayData", 0, DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },

    { EVR_UNKNOWN2B, "??", sizeof(Uint8), /* illegal VRs, we assume no extended length coding */
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },

};

static const int DcmVRDict_DIM = sizeof(DcmVRDict) / sizeof(DcmVREntry);


/*
** Check the consistency of the DcmVRDict
*/

#ifdef DEBUG

#include "dcmtk/ofstd/ofstream.h"

class DcmVRDict_checker {
private:
    int error_found;
public:
    DcmVRDict_checker();
};

DcmVRDict_checker::DcmVRDict_checker()
  : error_found(OFFalse)
{
    for (int i = 0; i < DcmVRDict_DIM; i++) {
        if (DcmVRDict[i].vr != i) {
            error_found = OFTrue;
            DCMDATA_FATAL("DcmVRDict: Internal ERROR: inconsistent indexing: " << DcmVRDict[i].vrName);
            abort();
        }
    }
}

const DcmVRDict_checker DcmVRDict_startup_check();

#endif

/*
** DcmVR member functions
*/

void
DcmVR::setVR(DcmEVR evr)
{
    if ((OFstatic_cast(int, evr) >= 0) && (OFstatic_cast(int, evr) < DcmVRDict_DIM)) {
        vr = evr;
    } else {
        vr = EVR_UNKNOWN;
    }
}

void
DcmVR::setVR(const char* vrName)
{
    vr = EVR_UNKNOWN;   /* default */
    if (vrName != NULL)
    {
        int found = OFFalse;
        int i = 0;
        for (i = 0;  (!found && (i < DcmVRDict_DIM)); i++)
        {
            if (strncmp(vrName, DcmVRDict[i].vrName, 2) == 0)
            {
                found = OFTrue;
                vr = DcmVRDict[i].vr;
            }
        }
        /* Workaround: There have been reports of systems transmitting
         * illegal VR strings in explicit VR (i.e. "??") without using
         * extended length fields. This is particularly bad because the
         * DICOM committee has announced that all future VRs will use
         * extended length. In order to distinguish between these two
         * variants, we treat all unknown VRs consisting of uppercase
         * letters as "real" future VRs (and thus assume extended length).
         * All other VR strings are treated as "illegal" VRs.
         */
        register char c1 = *vrName;
        register char c2 = (c1) ? (*(vrName + 1)) : ('\0');
        if ((c1 == '?') && (c2 == '?')) vr = EVR_UNKNOWN2B;
        if (!found && ((c1 < 'A') || (c1 > 'Z') || (c2 < 'A') || (c2 > 'Z'))) vr = EVR_UNKNOWN2B;
    }
}

DcmEVR
DcmVR::getValidEVR() const
{
    DcmEVR evr = EVR_UNKNOWN;

    if (isStandard()) {
        evr = vr;
    } else {
        switch (vr) {
            case EVR_up:
                evr = EVR_UL;
                break;
            case EVR_xs:
                evr = EVR_US;
                break;
            case EVR_lt:
                evr = EVR_OW;
                break;
            case EVR_ox:
            case EVR_pixelSQ:
                evr = EVR_OB;
                break;
            default:
                evr = EVR_UN;   /* handle as Unknown VR */
                break;
        }
    }

    /*
    ** If the generation of post-1993 VRs is not globally enabled then use OB instead.
    ** We may not want to generate these "new" VRs if other software cannot handle it.
    */
    switch (evr) {
        case EVR_UN:
            if (!dcmEnableUnknownVRGeneration.get())
            {
                DCMDATA_TRACE("DcmVR::getValidEVR() VR=\"UN\" replaced by \"OB\" since support is disabled");
                evr = EVR_OB; /* handle UN as if OB */
            }
            break;
        case EVR_UT:
            if (!dcmEnableUnlimitedTextVRGeneration.get())
            {
                DCMDATA_TRACE("DcmVR::getValidEVR() VR=\"UT\" replaced by \"OB\" since support is disabled");
                evr = EVR_OB; /* handle UT as if OB */
            }
            break;
        case EVR_OF:
            if (!dcmEnableOtherFloatStringVRGeneration.get())
            {
                DCMDATA_TRACE("DcmVR::getValidEVR() VR=\"OF\" replaced by \"OB\" since support is disabled");
                evr = EVR_OB; /* handle OF as if OB */
            }
            break;
        case EVR_OD:
            if (!dcmEnableOtherDoubleStringVRGeneration.get())
            {
                DCMDATA_TRACE("DcmVR::getValidEVR() VR=\"OD\" replaced by \"OB\" since support is disabled");
                evr = EVR_OB; /* handle OD as if OB */
            }
            break;
        default:
            /* in all other cases, do nothing */
            break;
    }

    return evr;
}

size_t
DcmVR::getValueWidth(void) const
{
    return DcmVRDict[vr].fValWidth;
}


const char*
DcmVR::getVRName() const
{
    return DcmVRDict[vr].vrName;
}

const char*
DcmVR::getValidVRName() const
{
    DcmVR avr(getValidEVR());
    return avr.getVRName();
}

OFBool
DcmVR::isStandard() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_NONSTANDARD) ? OFFalse : OFTrue;
}

OFBool
DcmVR::isForInternalUseOnly() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_INTERNAL) ? OFTrue : OFFalse;
}

/* returns true if VR represents a string */
OFBool
DcmVR::isaString() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_ISASTRING) ? OFTrue : OFFalse;
}

/*
 * returns true if VR uses an extended length encoding
 * for explicit transfer syntaxes
 */
OFBool
DcmVR::usesExtendedLengthEncoding() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_EXTENDEDLENGTHENCODING) ? OFTrue : OFFalse;
}

Uint32
DcmVR::getMinValueLength() const
{
    return (DcmVRDict[vr].minValueLength);
}

Uint32
DcmVR::getMaxValueLength() const
{
    return (DcmVRDict[vr].maxValueLength);
}

/* returns true if the VR is equivalent */
OFBool
DcmVR::isEquivalent(const DcmVR& avr) const
{
    DcmEVR evr = avr.getEVR();
    if (vr == evr) return OFTrue;

    OFBool result = OFFalse;
    switch (vr)
    {
      case EVR_ox:
          result = (evr == EVR_OB || evr == EVR_OW);
          break;
      case EVR_lt:
          result = (evr == EVR_OW || evr == EVR_US || evr == EVR_SS);
          break;
      case EVR_OB:
          result = (evr == EVR_ox);
          break;
      case EVR_OW:
          result = (evr == EVR_ox || evr == EVR_lt);
          break;
      case EVR_up:
          result = (evr == EVR_UL);
          break;
      case EVR_UL:
          result = (evr == EVR_up);
          break;
      case EVR_xs:
          result = (evr == EVR_SS || evr == EVR_US);
          break;
      case EVR_SS:
      case EVR_US:
          result = (evr == EVR_xs || evr == EVR_lt);
          break;
      default:
          break;
    }
    return result;
}
