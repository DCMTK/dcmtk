/*
**
** Author: Gerd Ehlers      01.11.94 -- Created
**         Andreas Barth    04.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcpixseq.h
**
** Purpose:
** Interface of class DcmPixelSequence
**
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-07-15 15:48:51 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpixseq.h,v $
** CVS/RCS Revision:    $Revision: 1.11 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcsequen.h"
#include "dcerror.h"

class DcmPixelItem;

class DcmPixelSequence : public DcmSequenceOfItems 
{
private:
    E_TransferSyntax xfer;


// These methods are not sensible for a pix-sequence

    virtual E_Condition insert(DcmItem* /*item*/,
                               unsigned long /*where*/ = DCM_EndOfListIndex,
                               OFBool /*before*/ = OFFalse)
    {
        return EC_IllegalCall;
    }
    virtual DcmItem* getItem(const unsigned long /*num*/)
    { 
        return NULL;
    }
    virtual DcmItem* remove(const unsigned long /*num*/)
    { 
        return NULL;
    }
    virtual DcmItem* remove(DcmItem* /*item*/)
    { 
        return NULL;
    }

protected:
    virtual E_Condition makeSubObject(DcmObject * & newObject, // out
                                      const DcmTag & newTag,
                                      const Uint32 newLength);  // in

public:
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelSequence(const DcmPixelSequence &old);
    virtual ~DcmPixelSequence();

    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
                       const int level = 0);

    virtual E_Condition insert(DcmPixelItem* item,
                               unsigned long where = DCM_EndOfListIndex);

    virtual E_Condition getItem(DcmPixelItem * & item, const unsigned long num);
    virtual E_Condition remove(DcmPixelItem * & item, const unsigned long num);
    virtual E_Condition remove(DcmPixelItem* item);


    E_Condition changeXfer(const E_TransferSyntax newXfer);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer);

    virtual E_Condition read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength 
                             = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/);

};

#endif // DCPIXSEQ_H

/*
** CVS/RCS Log:
** $Log: dcpixseq.h,v $
** Revision 1.11  1998-07-15 15:48:51  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.10  1997/07/21 08:25:10  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/27 13:48:29  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/22 16:57:10  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.7  1997/05/06 09:22:37  hewett
** Added a "before" flag to the insertion of items for compatibility with
** insertion in normal Sequences.
**
** Revision 1.6  1996/08/05 08:45:27  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/01/29 13:38:13  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/24 09:34:55  andreas
** Support for 64 bit long
**
** Revision 1.3  1996/01/05 13:22:58  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
