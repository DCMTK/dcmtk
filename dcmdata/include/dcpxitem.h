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
 *  Purpose: Interface of class DcmPixelItem
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpxitem.h,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcvrobow.h"


//
// CLASS DcmPixelItem
// This is a pseudo item, that has a value with representation OB 
// and has no sub elements. Since a DcmOtherByteOtherWord is defined as a 
// Dicom structure with a value of representation OW/OB it is better to
// derive this class from DcmOtherByteOtherWord.


class DcmPixelItem : public DcmOtherByteOtherWord

{
  protected:
    virtual E_Condition writeTagAndLength(DcmStream & outStream, 
					  const E_TransferSyntax oxfer,	
					  Uint32 & writtenBytes);
  public:
    DcmPixelItem(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelItem(const DcmPixelItem &old);
    virtual ~DcmPixelItem();

    DcmPixelItem &operator=(const DcmPixelItem &obj) { DcmOtherByteOtherWord::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_pixelItem; }

    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
		               size_t *pixelCounter = NULL);
};


#endif // DCPXITEM_H

/*
** CVS/RCS Log:
** $Log: dcpxitem.h,v $
** Revision 1.12  2001-06-01 15:48:43  meichel
** Updated copyright header
**
** Revision 1.11  2000/04/14 15:31:33  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.10  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.9  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/10 10:50:53  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.7  1999/03/31 09:24:45  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/11/12 16:47:43  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/05/22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.4  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
