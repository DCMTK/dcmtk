/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: class DcmTag
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctag.cc,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <iostream.h>

#include "dcdict.h"
#include "dctag.h"
#include "dcdeftag.h"


// ********************************

DcmTag::DcmTag()
  : vr(EVR_UNKNOWN),
    dictRef(NULL),
    errorFlag(EC_InvalidTag)
{
}

DcmTag::DcmTag(const DcmTagKey& akey)
  : DcmTagKey(akey),
    vr(EVR_UNKNOWN),
    dictRef(dcmDataDict.findEntry(akey)),
    errorFlag(EC_InvalidTag)
{
    if (dictRef != NULL) {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
}
    
DcmTag::DcmTag(Uint16 g, Uint16 e)
  : DcmTagKey(g, e), 
    vr(EVR_UNKNOWN),
    dictRef(dcmDataDict.findEntry(DcmTagKey(g, e))),
    errorFlag(EC_InvalidTag)
{
    if (dictRef != NULL) {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
}

DcmTag::DcmTag(Uint16 g, Uint16 e, const DcmVR& avr)
  : DcmTagKey(g, e),
    vr(avr),
    dictRef(dcmDataDict.findEntry(DcmTagKey(g, e))),
    errorFlag(EC_InvalidTag)
{
    if (dictRef != NULL)
        errorFlag = EC_Normal;
}

DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr)
  : DcmTagKey(akey),
    vr(avr),
    dictRef(dcmDataDict.findEntry(akey)),
    errorFlag(EC_InvalidTag)
{
    if (dictRef != NULL)
        errorFlag = EC_Normal;
}

DcmTag::DcmTag(const DcmTag& tag)
  : DcmTagKey(tag),
    vr(tag.vr),
    dictRef(tag.dictRef),
    errorFlag(tag.errorFlag)
{
}



// ********************************


DcmTag::~DcmTag()
{
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
** Revision 1.6  1999-03-31 09:25:41  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/07/15 15:52:09  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.4  1996/03/12 15:24:21  hewett
** Added constructor to allow direct setting of the VR.
**
** Revision 1.3  1996/01/05 13:27:44  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.2  1995/11/23 17:02:54  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
*/
