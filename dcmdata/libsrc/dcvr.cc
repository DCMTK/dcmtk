
#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>

#include "dcvr.h"

#define DCMVR_PROP_NONSTANDARD	0x01
#define DCMVR_PROP_INTERNAL		0x02

struct DcmVREntry {
    DcmEVR vr;
    const char* vrName;
    int propertyFlags;
};



static DcmVREntry DcmVRDict[] = {

    { EVR_AE, "AE", 0 },
    { EVR_AS, "AS", 0 },
    { EVR_AT, "AT", 0 },
    { EVR_CS, "CS", 0 },
    { EVR_DA, "DA", 0 },
    { EVR_DS, "DS", 0 },
    { EVR_DT, "DT", 0 },
    { EVR_FL, "FL", 0 },
    { EVR_FD, "FD", 0 },
    { EVR_IS, "IS", 0 },
    { EVR_LO, "LO", 0 },
    { EVR_LT, "LT", 0 },
    { EVR_OB, "OB", 0 },
    { EVR_OW, "OW", 0 },
    { EVR_PN, "PN", 0 },
    { EVR_SH, "SH", 0 },
    { EVR_SL, "SL", 0 },
    { EVR_SQ, "SQ", 0 },
    { EVR_SS, "SS", 0 },
    { EVR_ST, "ST", 0 },
    { EVR_TM, "TM", 0 },
    { EVR_UI, "UI", 0 },
    { EVR_UL, "UL", 0 },
    { EVR_US, "US", 0 },

    { EVR_ox, "ox", DCMVR_PROP_NONSTANDARD },
    { EVR_xs, "xs", DCMVR_PROP_NONSTANDARD },
    { EVR_na, "na", DCMVR_PROP_NONSTANDARD },
    { EVR_up, "up", DCMVR_PROP_NONSTANDARD },

    /* unique prefixes have been "invented" for the following internal VRs */
    { EVR_item, "it_EVR_item", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_metainfo, "mi_EVR_metainfo", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dataset, "ds_EVR_dataset", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_fileFormat, "ff_EVR_fileFormat", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dicomDir, "dd_EVR_dicomDir", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_dirRecord, "dr_EVR_dirRecord", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
	    
    { EVR_pixelSQ, "ps_EVR_pixelSQ", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL },
    { EVR_pixelItem, "pi_EVR_pixelItem", 
      DCMVR_PROP_NONSTANDARD | DCMVR_PROP_INTERNAL }
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

const char*
DcmVR::getVRName() const
{
	if (vr == EVR_UNKNOWN) {
		return DcmVR_ERROR_VRName;
	} else {
		return DcmVRDict[vr].vrName;
	}
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
	if (vr == EVR_UNKNOWN) {
		return FALSE;
	} else {
		return !(DcmVRDict[vr].propertyFlags & DCMVR_PROP_NONSTANDARD);
	}
}

int 
DcmVR::isForInternalUseOnly() const
{
	if (vr == EVR_UNKNOWN) {
		return TRUE;
	} else {
		return (DcmVRDict[vr].propertyFlags & DCMVR_PROP_INTERNAL);
	}
}
