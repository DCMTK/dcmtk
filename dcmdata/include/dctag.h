/*
**
** Author: Gerd Ehlers	    Created:  16-04-94
**         Andrew Hewett    29-10-95 - Adapted for Loadable Data Dictionary
**
** Module: dctag.h
**
** Purpose:
** Definition of the class DcmTag
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:23:01 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dctag.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCTAG_H
#define DCTAG_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcerror.h"
#include "dctagkey.h"
#include "dcvr.h"
#include "dcdicent.h"

#define DcmTag_ERROR_TagName	"Unknown Tag & Data"


// *** class definition ********************************


class DcmTag : public DcmTagKey {
private:
    DcmVR vr;
    const DcmDictEntry* dictRef;	/* reference to global data dictionary */
	
    E_Condition errorFlag;		/* the current error code */
#ifdef DEBUG
    int testConstructDestruct;		/* for debugging */
#endif

public:
    DcmTag();
    DcmTag(const DcmTagKey& akey);
    DcmTag(Uint16 g, Uint16 e);
    DcmTag(const DcmTagKey& akey, const DcmVR& avr);
    DcmTag(const DcmTag& tag);

    ~DcmTag();

    DcmTag& operator=(const DcmTagKey& key);
    DcmTag& operator=(const DcmTag& tag);

    DcmVR setVR(const DcmVR& avr);	/* set a specific VR */
    DcmVR getVR() const { return vr; }
    DcmEVR getEVR() const { return vr.getEVR(); }
    const char* getVRName() const { return vr.getVRName(); }

    Uint16 getGTag() const { return getGroup(); }
    Uint16 getETag() const { return getElement(); }
    DcmTagKey getXTag() const { return *((DcmTagKey*)(this)); }

    int getMaxVM() const { return (dictRef)?(dictRef->getVMMax()):(1); }
    int getMinVM() const { return (dictRef)?(dictRef->getVMMin()):(1); }
    
    const char*	getTagName() const 
	{ return (dictRef)?(dictRef->getTagName()):(DcmTag_ERROR_TagName); }

    E_Condition error() const { return errorFlag; }

};


// *** global constants ********************************


#define ItemTag (DcmTag(DCM_Item))
#define InternalUseTag (DcmTag(DcmTagKey(0xfffe, 0xfffe)))


#endif /* !DCTAG_H */

/*
** CVS/RCS Log:
** $Log: dctag.h,v $
** Revision 1.3  1996-01-05 13:23:01  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
** Revision 1.2  1995/11/23 16:38:03  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
**
*/
