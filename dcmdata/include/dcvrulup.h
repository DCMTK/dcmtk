/*
**
** Author: Gerd Ehlers      03.06.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrulup.h
**
** Purpose:
** Interface of class DcmUnsignedLongOffset
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-07-15 15:48:56 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrulup.h,v $
** CVS/RCS Revision:    $Revision: 1.6 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

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
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
                       const int level = 0);
    virtual DcmObject*  setNextRecord(DcmObject* record);
    virtual DcmObject*  getNextRecord();
    virtual E_Condition clear();
    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRUSUP_H

/*
** CVS/RCS Log:
** $Log: dcvrulup.h,v $
** Revision 1.6  1998-07-15 15:48:56  joergr
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
**
**
*/
