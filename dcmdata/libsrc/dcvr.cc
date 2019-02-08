/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
** global flags
*/
OFGlobal<OFBool> dcmEnableUnknownVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOtherFloatVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOtherDoubleVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOtherLongVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUniversalResourceIdentifierOrLocatorVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnlimitedCharactersVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableOther64bitVeryLongVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableSigned64bitVeryLongVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnsigned64bitVeryLongVRGeneration(OFTrue);
OFGlobal<OFBool> dcmEnableUnknownVRConversion(OFFalse);

/*
** global functions
*/
void dcmEnableGenerationOfNewVRs()
{
    dcmEnableUnknownVRGeneration.set(OFTrue);
    dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
    dcmEnableOtherFloatVRGeneration.set(OFTrue);
    dcmEnableOtherDoubleVRGeneration.set(OFTrue);
    dcmEnableOtherLongVRGeneration.set(OFTrue);
    dcmEnableUniversalResourceIdentifierOrLocatorVRGeneration.set(OFTrue);
    dcmEnableUnlimitedCharactersVRGeneration.set(OFTrue);
    dcmEnableOther64bitVeryLongVRGeneration.set(OFTrue);
    dcmEnableSigned64bitVeryLongVRGeneration.set(OFTrue);
    dcmEnableUnsigned64bitVeryLongVRGeneration.set(OFTrue);
}

void dcmDisableGenerationOfNewVRs()
{
    dcmEnableUnknownVRGeneration.set(OFFalse);
    dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
    dcmEnableOtherFloatVRGeneration.set(OFFalse);
    dcmEnableOtherDoubleVRGeneration.set(OFFalse);
    dcmEnableOtherLongVRGeneration.set(OFFalse);
    dcmEnableUniversalResourceIdentifierOrLocatorVRGeneration.set(OFFalse);
    dcmEnableUnlimitedCharactersVRGeneration.set(OFFalse);
    dcmEnableOther64bitVeryLongVRGeneration.set(OFFalse);
    dcmEnableSigned64bitVeryLongVRGeneration.set(OFFalse);
    dcmEnableUnsigned64bitVeryLongVRGeneration.set(OFFalse);
}


/*
** VR property table
*/

#define DCMVR_PROP_NONE                   0x00
#define DCMVR_PROP_NONSTANDARD            0x01
#define DCMVR_PROP_INTERNAL               0x02
#define DCMVR_PROP_EXTENDEDLENGTHENCODING 0x04
#define DCMVR_PROP_ISASTRING              0x08
#define DCMVR_PROP_ISAFFECTEDBYCHARSET    0x10
#define DCMVR_PROP_ISLENGTHINCHAR         0x20
#define DCMVR_PROP_UNDEFINEDLENGTH        0x40

struct DcmVREntry {
    DcmEVR vr;                      // Enumeration Value of Value representation
    const char* vrName;             // Name of Value representation
    const OFString* delimiterChars; // Delimiter characters, switch to default charset
    size_t fValWidth;               // Length of minimal unit, used for swapping
    int propertyFlags;              // Normal, internal, non-standard VR, etc.
    Uint32 minValueLength;          // Minimum length of a single value (bytes or characters)
    Uint32 maxValueLength;          // Maximum length of a single value (bytes or characters)
};

static const OFString noDelimiters;             // none
static const OFString bsDelimiter("\\");        // backslash
static const OFString pnDelimiters("\\^=");     // person name

static const DcmVREntry DcmVRDict[] = {

    { EVR_AE, "AE", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_AS, "AS", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 4, 4 },
    { EVR_AT, "AT", &noDelimiters, sizeof(Uint16), DCMVR_PROP_NONE, 4, 4 },
    { EVR_CS, "CS", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_DA, "DA", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 8, 10 },
    { EVR_DS, "DS", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_DT, "DT", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 26 },
    { EVR_FL, "FL", &noDelimiters, sizeof(Float32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_FD, "FD", &noDelimiters, sizeof(Float64), DCMVR_PROP_NONE, 8, 8 },
    { EVR_IS, "IS", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 12 },
    { EVR_LO, "LO", &bsDelimiter, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_ISAFFECTEDBYCHARSET | DCMVR_PROP_ISLENGTHINCHAR, 0, 64 },
    { EVR_LT, "LT", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_ISAFFECTEDBYCHARSET | DCMVR_PROP_ISLENGTHINCHAR, 0, 10240 },
    { EVR_OB, "OB", &noDelimiters, sizeof(Uint8), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967294U },
    { EVR_OD, "OD", &noDelimiters, sizeof(Float64), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967288U },
    { EVR_OF, "OF", &noDelimiters, sizeof(Float32), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967292U },
    { EVR_OL, "OL", &noDelimiters, sizeof(Uint32), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967292U },
    { EVR_OV, "OV", &noDelimiters, sizeof(Uint64), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967288U },
    { EVR_OW, "OW", &noDelimiters, sizeof(Uint16), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967294U },
    { EVR_PN, "PN", &pnDelimiters, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_ISAFFECTEDBYCHARSET | DCMVR_PROP_ISLENGTHINCHAR, 0, 64 },
    { EVR_SH, "SH", &bsDelimiter, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_ISAFFECTEDBYCHARSET | DCMVR_PROP_ISLENGTHINCHAR, 0, 16 },
    { EVR_SL, "SL", &noDelimiters, sizeof(Sint32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_SQ, "SQ", &noDelimiters, 0, DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967294U },
    { EVR_SS, "SS", &noDelimiters, sizeof(Sint16), DCMVR_PROP_NONE, 2, 2 },
    { EVR_ST, "ST", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_ISAFFECTEDBYCHARSET | DCMVR_PROP_ISLENGTHINCHAR, 0, 1024 },
    { EVR_SV, "SV", &noDelimiters, sizeof(Sint64), DCMVR_PROP_EXTENDEDLENGTHENCODING, 8, 8 },
    { EVR_TM, "TM", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 16 },
    { EVR_UC, "UC", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_ISAFFECTEDBYCHARSET, 0, 4294967294U },
    { EVR_UI, "UI", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING, 0, 64 },
    { EVR_UL, "UL", &noDelimiters, sizeof(Uint32), DCMVR_PROP_NONE, 4, 4 },
    { EVR_UR, "UR", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING|DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, 4294967294U },
    { EVR_US, "US", &noDelimiters, sizeof(Uint16), DCMVR_PROP_NONE, 2, 2 },
    { EVR_UT, "UT", &noDelimiters, sizeof(char), DCMVR_PROP_ISASTRING | DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_ISAFFECTEDBYCHARSET, 0, 4294967294U },
    { EVR_UV, "UV", &noDelimiters, sizeof(Uint64), DCMVR_PROP_EXTENDEDLENGTHENCODING, 8, 8 },
    { EVR_ox, "ox", &noDelimiters, sizeof(Uint8), DCMVR_PROP_NONSTANDARD | DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, 4294967294U },
    { EVR_xs, "xs", &noDelimiters, sizeof(Uint16), DCMVR_PROP_NONSTANDARD, 2, 2 },
    { EVR_lt, "lt", &noDelimiters, sizeof(Uint16), DCMVR_PROP_NONSTANDARD | DCMVR_PROP_EXTENDEDLENGTHENCODING, 0, 4294967294U },
    { EVR_na, "na", &noDelimiters, 0, DCMVR_PROP_NONSTANDARD, 0, 0 },
    { EVR_up, "up", &noDelimiters, sizeof(Uint32), DCMVR_PROP_NONSTANDARD, 4, 4 },

    /* unique prefixes have been "invented" for the following internal VRs */
    { EVR_item, "it_EVR_item", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_metainfo, "mi_EVR_metainfo", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dataset, "ds_EVR_dataset", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_fileFormat, "ff_EVR_fileFormat", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dicomDir, "dd_EVR_dicomDir", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },
    { EVR_dirRecord, "dr_EVR_dirRecord", &noDelimiters, 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, 0 },

    { EVR_pixelSQ, "ps_EVR_pixelSQ", &noDelimiters, sizeof(Uint8),
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },
    /* Moved from internal use to non standard only: necessary to distinguish from "normal" OB */
    { EVR_pixelItem, "pi", &noDelimiters, sizeof(Uint8),
      DCMVR_PROP_NONSTANDARD, 0, DCM_UndefinedLength },

    { EVR_UNKNOWN, "??", &noDelimiters, sizeof(Uint8), /* EVR_UNKNOWN (i.e. "future" VRs) should be mapped to UN or OB */
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL | DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, DCM_UndefinedLength },

    /* Unknown Value Representation */
    { EVR_UN, "UN", &noDelimiters, sizeof(Uint8), DCMVR_PROP_EXTENDEDLENGTHENCODING | DCMVR_PROP_UNDEFINEDLENGTH, 0, 4294967294U },

    /* Pixel Data - only used in ident() */
    { EVR_PixelData, "PixelData", &noDelimiters, 0, DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },
    /* Overlay Data - only used in ident() */
    { EVR_OverlayData, "OverlayData", &noDelimiters, 0, DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },

    { EVR_UNKNOWN2B, "??", &noDelimiters, sizeof(Uint8), /* illegal VRs, we assume no extended length coding */
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL, 0, DCM_UndefinedLength },

};

static const int DcmVRDict_DIM = OFstatic_cast(int, sizeof(DcmVRDict) / sizeof(DcmVREntry));


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
        for (i = 0; (!found && (i < DcmVRDict_DIM)); i++)
        {
            /* We only compare the first two characters of the passed string and
             * never accept a VR that is labeled for internal use only.
             */
            if ((strncmp(vrName, DcmVRDict[i].vrName, 2) == 0) &&
                !(DcmVRDict[i].propertyFlags & DCMVR_PROP_INTERNAL))
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
        char c1 = *vrName;
        char c2 = (c1) ? (*(vrName + 1)) : ('\0');
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
    DcmEVR oldVR = evr;
    switch (evr) {
        case EVR_UN:
            if (!dcmEnableUnknownVRGeneration.get())
                evr = EVR_OB; /* handle UN as if OB */
            break;
        case EVR_UT:
            if (!dcmEnableUnlimitedTextVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle UT as if UN */
                else
                    evr = EVR_OB; /* handle UT as if OB */
            }
            break;
        case EVR_OF:
            if (!dcmEnableOtherFloatVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle OF as if UN */
                else
                    evr = EVR_OB; /* handle OF as if OB */
            }
            break;
        case EVR_OD:
            if (!dcmEnableOtherDoubleVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle OD as if UN */
                else
                    evr = EVR_OB; /* handle OD as if OB */
            }
            break;
        case EVR_OL:
            if (!dcmEnableOtherLongVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle OL as if UN */
                else
                    evr = EVR_OB; /* handle OL as if OB */
            }
            break;
        case EVR_UR:
            if (!dcmEnableUniversalResourceIdentifierOrLocatorVRGeneration.get())
            {
                if (dcmEnableUnlimitedTextVRGeneration.get())
                    evr = EVR_UT; /* handle UR as if UT */
                else if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle UR as if UN */
                else
                    evr = EVR_OB; /* handle UR as if OB */
            }
            break;
        case EVR_UC:
            if (!dcmEnableUnlimitedCharactersVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle UC as if UN */
                else
                    evr = EVR_OB; /* handle UC as if OB */
            }
            break;
        case EVR_OV:
            if (!dcmEnableOther64bitVeryLongVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle OV as if UN */
                else
                    evr = EVR_OB; /* handle OV as if OB */
            }
            break;
        case EVR_SV:
            if (!dcmEnableSigned64bitVeryLongVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle SV as if UN */
                else
                    evr = EVR_OB; /* handle SV as if OB */
            }
            break;
        case EVR_UV:
            if (!dcmEnableUnsigned64bitVeryLongVRGeneration.get())
            {
                if (dcmEnableUnknownVRGeneration.get())
                    evr = EVR_UN; /* handle UV as if UN */
                else
                    evr = EVR_OB; /* handle UV as if OB */
            }
            break;
        default:
            /* in all other cases, do nothing */
            break;
    }
    if (oldVR != evr)
    {
        DCMDATA_TRACE("DcmVR::getValidEVR() VR=\"" << DcmVR(oldVR).getVRName()
            << "\" replaced by \"" << DcmVR(evr).getVRName() << "\" since support is disabled");
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

/* returns true if VR supports undefined length */
OFBool
DcmVR::supportsUndefinedLength() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_UNDEFINEDLENGTH) ? OFTrue : OFFalse;
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

OFBool
DcmVR::isAffectedBySpecificCharacterSet() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_ISAFFECTEDBYCHARSET) ? OFTrue : OFFalse;
}

const OFString&
DcmVR::getDelimiterChars() const
{
    return *DcmVRDict[vr].delimiterChars;
}

/* returns true if VR length is in char */
OFBool
DcmVR::isLengthInChar() const
{
    return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_ISLENGTHINCHAR) ? OFTrue : OFFalse;
}
