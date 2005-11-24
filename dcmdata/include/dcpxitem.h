/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Update Date:      $Date: 2005-11-24 12:50:57 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcpxitem.h,v $
 *  CVS/RCS Revision: $Revision: 1.20 $
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
#include "dcofsetl.h"    /* for class DcmOffsetList */

// CLASS DcmPixelItem
// This is a pseudo item, that has a value with representation OB
// and has no sub elements. Since a DcmOtherByteOtherWord is defined as a
// Dicom structure with a value of representation OW/OB it is better to
// derive this class from DcmOtherByteOtherWord.

class DcmPixelItem : public DcmOtherByteOtherWord

{
  protected:
    virtual OFCondition writeTagAndLength(DcmOutputStream & outStream,
					  const E_TransferSyntax oxfer,
					  Uint32 & writtenBytes) const;
  public:
    DcmPixelItem(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelItem(const DcmPixelItem &old);
    virtual ~DcmPixelItem();

    DcmPixelItem &operator=(const DcmPixelItem &obj) { DcmOtherByteOtherWord::operator=(obj); return *this; }

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPixelItem(*this);
    }

    virtual DcmEVR ident(void) const { return EVR_pixelItem; }

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** creates in this object an offset table for a compressed pixel sequence.
     *  @param offsetList list of size entries for each individual encoded frame
     *    provided by the compression codec
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createOffsetTable(const DcmOffsetList& offsetList);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype
					 = EET_UndefinedLength);

};


#endif // DCPXITEM_H

/*
** CVS/RCS Log:
** $Log: dcpxitem.h,v $
** Revision 1.20  2005-11-24 12:50:57  meichel
** Fixed bug in code that prepares a byte stream that is fed into the MAC
**   algorithm when creating or verifying a digital signature. The previous
**   implementation was non-conformant when signatures included compressed
**   (encapsulated) pixel data because the item length was included in the byte
**   stream, while it should not. The global variable dcmEnableOldSignatureFormat
**   and a corresponding command line option in dcmsign allow to re-enable the old
**   implementation.
**
** Revision 1.19  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.18  2002/12/06 12:49:12  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.17  2002/08/27 16:55:38  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.16  2002/05/24 14:51:42  meichel
** Moved helper methods that are useful for different compression techniques
**   from module dcmjpeg to module dcmdata
**
** Revision 1.15  2002/04/25 09:41:46  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.14  2001/11/16 15:54:39  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.13  2001/09/25 17:19:28  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.12  2001/06/01 15:48:43  meichel
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
