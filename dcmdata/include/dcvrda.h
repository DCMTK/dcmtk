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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDate
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:49 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrda.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRDA_H
#define DCVRDA_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include "dctypes.h"
#include "dcbytstr.h"



class DcmDate : public DcmByteString 
{
  public:
    DcmDate(const DcmTag &tag, const Uint32 len = 0);
    DcmDate( const DcmDate &newDA );
    virtual ~DcmDate();

    DcmDate &operator=(const DcmDate &obj) { DcmByteString::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_DA; }
};


#endif // DCVRDA_H

/*
** CVS/RCS Log:
** $Log: dcvrda.h,v $
** Revision 1.7  2001-06-01 15:48:49  meichel
** Updated copyright header
**
** Revision 1.6  2000/03/08 16:26:22  meichel
** Updated copyright header.
**
** Revision 1.5  1999/03/31 09:24:58  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:47  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1996/01/05 13:23:04  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
