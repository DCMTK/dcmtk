/*
**
** Author: Gerd Ehlers	    Created:  04-16-94
**         Andrew Hewett    29-10-95 - Adapted for Loadable Data Dictionary
**
** Module: dctag.cc
**
** Purpose:
** Implementation of the class DcmTag
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1995-11-23 17:02:54 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctag.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <iostream.h>

#include "dcdict.h"
#include "dctag.h"

#include "dcdeftag.h"


// ********************************

DcmTag::DcmTag() 
{
    dictRef = NULL;
    errorFlag = EC_InvalidTag;
    testConstructDestruct = 1; // for debugging
}

DcmTag::DcmTag(const DcmTagKey& akey) : DcmTagKey(akey) 
{
    dictRef = dcmDataDict.findEntry(akey);
    if (dictRef != NULL) {
    	vr = dictRef->getVR();
    	errorFlag = EC_Normal;
    } else {
    	vr.setVR(EVR_UNKNOWN);
	errorFlag = EC_InvalidTag;
    }
    testConstructDestruct = 1; // for debugging
}
    
DcmTag::DcmTag(Uint16 g, Uint16 e) : DcmTagKey(g, e) 
{
    dictRef = dcmDataDict.findEntry(DcmTagKey(g, e));
    if (dictRef != NULL) {
    	vr = dictRef->getVR();
    	errorFlag = EC_Normal;
    } else {
    	vr.setVR(EVR_UNKNOWN);
	errorFlag = EC_InvalidTag;
    }
    testConstructDestruct = 1; // for debugging
}

DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr) : DcmTagKey(akey), vr(avr) 
{
    dictRef = dcmDataDict.findEntry(akey);
    if (dictRef != NULL) {
    	errorFlag = EC_Normal;
    } else {
	errorFlag = EC_InvalidTag;
    }
    testConstructDestruct = 1; // for debugging
}

DcmTag::DcmTag(const DcmTag& tag) : DcmTagKey(tag), vr(tag.vr) 
{
    dictRef = tag.dictRef;
    errorFlag = tag.errorFlag;
    testConstructDestruct = 1; // for debugging
}



// ********************************


DcmTag::~DcmTag()
{
    if ( testConstructDestruct == 1 ) {			// for debugging
        testConstructDestruct = 2; // for debugging
    } else {
        cerr << "Error: ~DcmTag called more than once ("
             << testConstructDestruct << ")" << endl;
        testConstructDestruct++;
    }
}


// ********************************


DcmTag& DcmTag::operator = ( const DcmTag& tag )
{
    if ( this != &tag ) {
	DcmTagKey::set(tag);
	vr = tag.vr;
	dictRef = tag.dictRef;
	errorFlag = tag.errorFlag;
    } else {
	cerr << "dctag:DcmTag::DcmTag(DcmTag&)  Warning: self-assignment" << endl;
    }

    return *this;
}

// ********************************

DcmTag& DcmTag::operator = ( const DcmTagKey& key )
{
    DcmTagKey::set(key);

    dictRef = dcmDataDict.findEntry(key);
    if (dictRef != NULL) {
    	vr = dictRef->getVR();
    	errorFlag = EC_Normal;
    } else {
    	vr.setVR(EVR_UNKNOWN);
	errorFlag = EC_InvalidTag;
    }
    return *this;
}

// ********************************


DcmVR DcmTag::setVR( const DcmVR& avr )    // nicht-eindeutige VR aufloesen
{
    vr = avr;

    if ( vr.getEVR() == EVR_UNKNOWN ) {
	errorFlag = EC_InvalidVR;
    } else {
	errorFlag = EC_Normal;
    }
    return vr;
}


/*
** CVS/RCS Log:
** $Log: dctag.cc,v $
** Revision 1.2  1995-11-23 17:02:54  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
