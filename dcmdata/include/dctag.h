/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Purpose: Definition of the class DcmTag
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-16 15:54:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dctag.h,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCTAG_H
#define DCTAG_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcerror.h"
#include "dctagkey.h"
#include "dcvr.h"
#include "dcdicent.h"

#define DcmTag_ERROR_TagName    "Unknown Tag & Data"


// *** class definition ********************************


class DcmTag : public DcmTagKey {
private:
    DcmVR vr;
    char *tagName;                      /* remains empty unless getTagName() is called */
    OFCondition errorFlag;              /* the current error code */

public:
    DcmTag();
    DcmTag(const DcmTagKey& akey);
    DcmTag(Uint16 g, Uint16 e);
    DcmTag(const DcmTagKey& akey, const DcmVR& avr);
    DcmTag(Uint16 g, Uint16 e, const DcmVR& avr);
    DcmTag(const DcmTag& tag);

    ~DcmTag();

    DcmTag& operator=(const DcmTagKey& key);
    DcmTag& operator=(const DcmTag& tag);

    DcmVR setVR(const DcmVR& avr);      /* set a specific VR */

    DcmVR getVR() const { return vr; }
    DcmEVR getEVR() const { return vr.getEVR(); }
    const char* getVRName() const { return vr.getVRName(); }

    Uint16 getGTag() const { return getGroup(); }
    Uint16 getETag() const { return getElement(); }
    DcmTagKey getXTag() const { return *((DcmTagKey*)(this)); }
    
    const char* getTagName();

    OFCondition error() const { return errorFlag; }

    /** returns true if a data element with the given tag and VR
     *  can be digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    virtual OFBool isSignable() const;

    /** returns true if the VR used for writing is "UN"
     */
    virtual OFBool isUnknownVR() const;

};


// *** global constants ********************************


#define ItemTag (DcmTag(DCM_Item))
#define InternalUseTag (DcmTag(DcmTagKey(0xfffe, 0xfffe)))


#endif /* !DCTAG_H */

/*
** CVS/RCS Log:
** $Log: dctag.h,v $
** Revision 1.15  2001-11-16 15:54:40  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.14  2001/09/25 17:19:29  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.13  2001/06/01 15:48:45  meichel
** Updated copyright header
**
** Revision 1.12  2000/04/14 16:00:58  meichel
** Restructured class DcmTag. Instances don't keep a permanent pointer
**   to a data dictionary entry anymore. Required for MT applications.
**
** Revision 1.11  2000/03/08 16:26:19  meichel
** Updated copyright header.
**
** Revision 1.10  1999/03/31 09:24:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.9  1998/07/15 15:48:54  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.8  1997/05/06 09:26:44  hewett
** The DcmTag::getVMMax() method now returns a maximum value if the attribute
** is unknown.  This makes the default VM=1-n (before it was VM=1).
**
** Revision 1.7  1997/03/26 17:18:01  hewett
** Added member function to obtain a DcmTag's data dictionary reference.
**
** Revision 1.6  1996/04/19 08:37:21  andreas
** correct bug with DEBUG and not DEBUG parts. It was not possible to compile
** the dcmdata library with DEBUG and programs using dcmdata without DEBUG
** (and vice versa)
**
** Revision 1.5  1996/03/13 14:48:32  hewett
** Added useful VR access methods.
**
** Revision 1.4  1996/03/12 15:32:49  hewett
** Added constructor with parameter to explicity set the VR.
**
** Revision 1.3  1996/01/05 13:23:01  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
** Revision 1.2  1995/11/23 16:38:03  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/
