/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-04-14 15:42:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvr.h,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMVR_H
#define DCMVR_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include "dctypes.h"
#include "ofglobal.h"


/*
** Global flag to enable/disable the generation of VR=UN
*/
extern OFGlobal<OFBool> dcmEnableUnknownVRGeneration; /* default OFTrue */

/*
** Global flag to enable/disable the generation of VR=UT
*/
extern OFGlobal<OFBool> dcmEnableUnlimitedTextVRGeneration; /* default OFTrue */

/*
** VR Enumerations
*/

enum DcmEVR 
{
    EVR_AE=0,
    EVR_AS=1,
    EVR_AT=2,
    EVR_CS=3,
    EVR_DA=4,
    EVR_DS=5,
    EVR_DT=6,
    EVR_FL=7,
    EVR_FD=8,
    EVR_IS=9,
    EVR_LO=10,
    EVR_LT=11,
    EVR_OB=12,
    EVR_OW=13,
    EVR_PN=14,
    EVR_SH=15,
    EVR_SL=16,
    EVR_SQ=17,
    EVR_SS=18,
    EVR_ST=19,
    EVR_TM=20,
    EVR_UI=21,
    EVR_UL=22,
    EVR_US=23,

    EVR_ox=24,  /* OB or OW depending on context */
    EVR_xs=25,  /* SS or US depending on context */
    EVR_na=26,  /* na="not applicable", for data which has no VR */
    EVR_up=27,  /* up="unsigned pointer", used internally for DICOMDIR support */       

    EVR_item=28,        /* used internally */
    EVR_metainfo=29,    /* used internally */
    EVR_dataset=30,     /* used internally */
    EVR_fileFormat=31,  /* used internally */
    EVR_dicomDir=32,    /* used internally */
    EVR_dirRecord=33,   /* used internally */
        
    EVR_pixelSQ=34,     /* used internally */
    EVR_pixelItem=35,   /* used internally */

    EVR_UNKNOWN=36,     /* used internally */
    EVR_UN=37,          /* Unknown Value Representation - defined in supplement 14 */
    EVR_PixelData=38,   /* used internally */
    EVR_OverlayData=39, /* used internally */

    EVR_UT=40,          /* Unlimited Text */
    EVR_UNKNOWN2B=41    /* like EVR_UNKNOWN but without extended length property in explicit VR */
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
 * Revision 1.16  2000-04-14 15:42:56  meichel
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
