/*
 *
 *  Copyright (C) 1994-2006, OFFIS
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
 *  Purpose: Interface of class DcmPixelSequence
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2006-08-15 15:49:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/dcmtk/dcmdata/dcpixseq.h,v $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcofsetl.h"    /* for class DcmOffsetList */

class DcmPixelItem;

class DcmPixelSequence : public DcmSequenceOfItems
{
private:
    E_TransferSyntax Xfer;


// These methods are not sensible for a pix-sequence

    virtual OFCondition insert(DcmItem* /*item*/,
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
    virtual OFCondition makeSubObject(DcmObject * & newObject, // out
                                      const DcmTag & newTag,
                                      const Uint32 newLength);  // in

public:
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelSequence(const DcmPixelSequence &old);
    virtual ~DcmPixelSequence();

    DcmPixelSequence &operator=(const DcmPixelSequence &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPixelSequence(*this);
    }

    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }

    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual OFCondition insert(DcmPixelItem* item,
                               unsigned long where = DCM_EndOfListIndex);

    virtual OFCondition getItem(DcmPixelItem * & item, const unsigned long num);
    virtual OFCondition remove(DcmPixelItem * & item, const unsigned long num);
    virtual OFCondition remove(DcmPixelItem* item);


    OFCondition changeXfer(const E_TransferSyntax newXfer);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer);

    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength
                             = DCM_MaxReadLength);

    virtual OFCondition write(DcmOutputStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype
					 = EET_UndefinedLength);

  /** appends a single compressed frame to this DICOM pixel sequence
   *  @param offsetList list containing offset table entries.
   *    Upon success, an entry is appended to the list
   *  @param compressedData pointer to compressed image data, must not be NULL
   *  @param compressedLen number of bytes of compressed image data
   *  @param fragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition storeCompressedFrame(
        DcmOffsetList& offsetList,
        Uint8 *compressedData,
        Uint32 compressedLen,
        Uint32 fragmentSize);

};

#endif // DCPIXSEQ_H

/*
** CVS/RCS Log:
** $Log: dcpixseq.h,v $
** Revision 1.28  2006-08-15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.27  2005/12/08 16:28:31  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.26  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.25  2002/12/06 12:49:12  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/08/27 16:55:38  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.23  2002/05/24 14:51:42  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.22  2001/09/25 17:19:28  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.21  2001/06/01 15:48:42  meichel
** Updated copyright header
**
** Revision 1.20  2000/11/07 16:56:08  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.19  2000/04/14 16:02:23  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.18  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.17  2000/03/06 18:11:30  joergr
** Local variable hided member variable (reported by Sun CC 4.2).
**
** Revision 1.16  2000/03/03 14:41:56  joergr
** Corrected bug related to padding of file and item size.
**
** Revision 1.15  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.14  2000/02/10 10:50:53  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.13  1999/03/31 09:24:44  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1998/11/12 16:47:42  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.11  1998/07/15 15:48:51  joergr
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
*/
