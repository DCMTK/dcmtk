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
 *  Purpose: Interface of class DcmUnsignedLongOffset
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-25 17:19:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrulup.h,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcvrul.h"


class DcmUnsignedLongOffset : public DcmUnsignedLong 
{
  private:

 // --- declarations to avoid compiler warnings
 
    DcmUnsignedLongOffset &operator=(const DcmUnsignedLongOffset &);

  protected:
    DcmObject* nextRecord;

  public:
    DcmUnsignedLongOffset(const DcmTag &tag, const Uint32 len = 0);
    DcmUnsignedLongOffset(const DcmUnsignedLongOffset& old);
    virtual ~DcmUnsignedLongOffset(void);

    virtual DcmEVR ident(void) const;
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
		               size_t *pixelCounter = NULL);
    virtual DcmObject*  setNextRecord(DcmObject* record);
    virtual DcmObject*  getNextRecord();
    virtual OFCondition clear();
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRUSUP_H

/*
** CVS/RCS Log:
** $Log: dcvrulup.h,v $
** Revision 1.13  2001-09-25 17:19:35  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.12  2001/06/01 15:48:54  meichel
** Updated copyright header
**
** Revision 1.11  2000/04/14 15:31:36  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.10  2000/03/08 16:26:27  meichel
** Updated copyright header.
**
** Revision 1.9  2000/03/03 14:05:28  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/10 10:50:56  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.7  1999/03/31 09:25:10  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/07/15 15:48:56  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.5  1997/07/21 08:25:17  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.4  1996/08/05 08:45:40  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.3  1996/01/05 13:23:11  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
