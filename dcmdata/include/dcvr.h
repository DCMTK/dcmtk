/*
**
** Author: Gerd Ehlers      Created:  26.04.94
**         Andreas Barth    26.11.95 -- support of value width
**
** Module: dcvr.h
**
** Purpose:
** Definition of the DcmVR class for Value Representation
**
**
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.3 $
** Status:	  $State: Exp $
**
*/

#ifndef DCMVR_H
#define DCMVR_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include "dctypes.h"

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

    EVR_ox=24,	/* OB or OW depending on context */
    EVR_xs=25,	/* SS or US depending on context */
    EVR_na=26,	/* na="not applicable", for data which has no VR */
    EVR_up=27,	/* up="unsigned pointer", used internally for DICOMDIR support */	

    EVR_item=28,	/* used internally */
    EVR_metainfo=29,	/* used internally */
    EVR_dataset=30,	/* used internally */
    EVR_fileFormat=31,	/* used internally */
    EVR_dicomDir=32,	/* used internally */
    EVR_dirRecord=33,	/* used internally */
	
    EVR_pixelSQ=34,	/* used internally */
    EVR_pixelItem=35,	/* used internally */

    EVR_UNKNOWN=36	/* used internally */
};


class DcmVR {
private:
    DcmEVR vr;
protected:
public:
    DcmVR() { vr = EVR_UNKNOWN; }
    DcmVR(DcmEVR evr);
    DcmVR(const char* vrName);
    DcmVR(const DcmVR& avr) { vr = avr.vr; }
	
    void setVR(DcmEVR evr);
    void setVR(const char* vrName);
    void setVR(const DcmVR& avr) { vr = avr.vr; }
    DcmEVR getEVR() const { return vr; }
    DcmEVR getValidEVR() const;
    const char* getVRName() const ;
    const char* getValidVRName() const;
	const size_t getValueWidth(void) const;
	
    /* returns true if VR is a standard DICOM VR */
    int isStandard() const;
    /* returns true if VR is for internal use only */
    int isForInternalUseOnly() const;

};

inline
DcmVR::DcmVR(DcmEVR evr)
{
    setVR(evr);
}

inline
DcmVR::DcmVR(const char* vrName)
{
    setVR(vrName);
}


#endif /* !DCMVR_H */




