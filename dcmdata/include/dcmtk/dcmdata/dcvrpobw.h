/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *  Interface of class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall
 *  not be used directly in applications. No identification exists.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2007-06-07 09:00:59 $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRPOBW_H
#define DCVRPOBW_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrobow.h"

class DcmPolymorphOBOW : public DcmOtherByteOtherWord
{
private:
    OFBool changeVR;
    DcmEVR currentVR;    // current VR of value field (can change)

public:
    DcmPolymorphOBOW(
        const DcmTag & tag,
        const Uint32 len = 0);

    DcmPolymorphOBOW(
        const DcmPolymorphOBOW & old);

    virtual ~DcmPolymorphOBOW();

    DcmPolymorphOBOW &operator=(
        const DcmPolymorphOBOW &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPolymorphOBOW(*this);
    }

    virtual OFCondition read(
        DcmInputStream & inStream,
        const E_TransferSyntax ixfer,
        const E_GrpLenEncoding glenc,
        const Uint32 maxReadLength);

    virtual OFCondition write(
        DcmOutputStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(
        DcmOutputStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    virtual void transferInit();
    virtual void transferEnd();

    // get data as Uint8 Array
    virtual OFCondition getUint8Array(
        Uint8 * & bytes);

    // get data as Uint16 Array
    virtual OFCondition getUint16Array(
        Uint16 * & words);

    // put an Unit8 array. It is converted to OW if VR == OW
    virtual OFCondition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);

    // put an Unit16 array. It is converted to OB if VR == OB
    virtual OFCondition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length );

    // create an empty Uint8 array of given number of bytes and set it
    virtual OFCondition createUint8Array(
        const Uint32 numBytes,
        Uint8 * & bytes);

    // create an empty Uint16 array of given number of words and set it
    virtual OFCondition createUint16Array(
        const Uint32 numWords,
        Uint16 * & words);
};
#endif

/*
** CVS/RCS Log:
** $Log: dcvrpobw.h,v $
** Revision 1.14  2007-06-07 09:00:59  joergr
** Fixed incorrect comment.
**
** Revision 1.13  2005/12/08 16:29:06  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.12  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.11  2002/09/12 14:07:16  joergr
** Added method "createUint8Array" which works similar to the 16 bit variant.
**
** Revision 1.10  2002/08/27 16:55:40  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.9  2001/09/25 17:19:33  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.8  2001/06/01 15:48:52  meichel
** Updated copyright header
**
** Revision 1.7  2001/05/10 12:52:56  meichel
** Added public createUint16Array() method in class DcmPolymorphOBOW.
**
** Revision 1.6  2000/11/07 16:56:11  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.5  2000/03/08 16:26:25  meichel
** Updated copyright header.
**
** Revision 1.4  1999/03/31 09:25:05  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.3  1998/11/12 16:47:53  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.2  1997/07/31 06:59:00  andreas
** Error correction and additonal functionality for
** DcmPolymorphOBOW to support getting and putting of Uint8 and
** Uint16 data independent of the VR.
**
** Revision 1.1  1997/07/21 07:54:00  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
**
*/
