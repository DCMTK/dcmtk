/*
**
** Author: Gerd Ehlers      Created:  26.04.94
**         Andreas Barth    26.11.95 -- support of value width
**
** Module: dcvr.cc
**
** Purpose:
** Implementation of the DcmVR class for Value Representation
**
**
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.3 $
** Status:	  $State: Exp $
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>

#include "dcvr.h"

#define DCMVR_PROP_NONSTANDARD	0x01
#define DCMVR_PROP_INTERNAL		0x02

struct DcmVREntry {
    DcmEVR vr;				// Enumeration Value of Value representation
    const char* vrName;		// Name of Value representation
	size_t fValWidth;		// Length of minimal unit, used for swapping
    int propertyFlags;		// Normal, internal, non-standard vr
};



static DcmVREntry DcmVRDict[] = {

    { EVR_AE, "AE", sizeof(char), 0 },
    { EVR_AS, "AS", sizeof(char), 0 },
    { EVR_AT, "AT", sizeof(Uint16), 0 },
    { EVR_CS, "CS", sizeof(char), 0 },
    { EVR_DA, "DA", sizeof(char), 0 },
    { EVR_DS, "DS", sizeof(char), 0 },
    { EVR_DT, "DT", sizeof(char), 0 },
    { EVR_FL, "FL", sizeof(Float32), 0 },
    { EVR_FD, "FD", sizeof(Float64), 0 },
    { EVR_IS, "IS", sizeof(char), 0 },
    { EVR_LO, "LO", sizeof(char), 0 },
    { EVR_LT, "LT", sizeof(char), 0 },
    { EVR_OB, "OB", sizeof(Uint8), 0 },
    { EVR_OW, "OW", sizeof(Uint16), 0 },
    { EVR_PN, "PN", sizeof(char), 0 },
    { EVR_SH, "SH", sizeof(char), 0 },
    { EVR_SL, "SL", sizeof(Sint32), 0 },
    { EVR_SQ, "SQ", 0, 0 },
    { EVR_SS, "SS", sizeof(Sint16), 0 },
    { EVR_ST, "ST", sizeof(char), 0 },
    { EVR_TM, "TM", sizeof(char), 0 },
    { EVR_UI, "UI", sizeof(char), 0 },
    { EVR_UL, "UL", sizeof(Uint32), 0 },
    { EVR_US, "US", sizeof(Uint16), 0 },

    { EVR_ox, "ox", sizeof(Uint8), DCMVR_PROP_NONSTANDARD },
    { EVR_xs, "xs", sizeof(Uint16), DCMVR_PROP_NONSTANDARD },
    { EVR_na, "na", 0, DCMVR_PROP_NONSTANDARD },
    { EVR_up, "up", sizeof(Uint32), DCMVR_PROP_NONSTANDARD },

    /* unique prefixes have been "invented" for the following internal VRs */
    { EVR_item, "it_EVR_item", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_metainfo, "mi_EVR_metainfo", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dataset, "ds_EVR_dataset", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_fileFormat, "ff_EVR_fileFormat", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dicomDir, "dd_EVR_dicomDir", 0,
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dirRecord, "dr_EVR_dirRecord", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
	    
    { EVR_pixelSQ, "ps_EVR_pixelSQ", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_pixelItem, "pi_EVR_pixelItem", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },

    { EVR_UNKNOWN, "??", 0, 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
};

static int DcmVRDict_DIM = sizeof(DcmVRDict) / sizeof(DcmVREntry);


/*
** Check the consistency of the DcmVRDict
*/

#ifdef DEBUG

#include <iostream.h>

class DcmVRDict_checker {
private:
	int error_found;
public:
	DcmVRDict_checker();
};

DcmVRDict_checker::DcmVRDict_checker() 
{
	error_found = FALSE;
	for (int i=0; i<DcmVRDict_DIM; i++) {
		if (DcmVRDict[i].vr != i) {
			error_found = TRUE;
			cerr << "DcmVRDict:: Internal ERROR: inconsistent indexing: "
				<< DcmVRDict[i].vrName << endl;
		}
	}
}


DcmVRDict_checker DcmVRDict_startup_check;

#endif

/*
** DcmVR member functions
*/

void
DcmVR::setVR(DcmEVR evr) 
{
    if ( (evr >= 0) && (evr < DcmVRDict_DIM)) {
	vr = evr;
    } else {
	vr = EVR_UNKNOWN;
    }
}

void
DcmVR::setVR(const char* vrName)
{
	vr = EVR_UNKNOWN;	/* default */
	if ( vrName != NULL) {
		int found = FALSE;
		int i = 0;
		for (i=0;  (!found && (i < DcmVRDict_DIM)); i++) {
			if (strncmp(vrName, DcmVRDict[i].vrName, 2) == 0) {
				found = TRUE;
				vr = DcmVRDict[i].vr;
			}
		}
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
		default:
			evr = EVR_OB;	/* handle as if OB */
			break;
		}
	}
	return evr;
}

const size_t
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
	
int 
DcmVR::isStandard() const
{
	return !(DcmVRDict[vr].propertyFlags & DCMVR_PROP_NONSTANDARD);
}

int 
DcmVR::isForInternalUseOnly() const
{
	return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_INTERNAL);
}


