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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmOtherByteOtherWord for data VR OB or OW
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-10-02 11:47:34 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrobow.h,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVROBOW_H
#define DCVROBOW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"

class DcmOtherByteOtherWord : public DcmElement 
{
protected:
    virtual void postLoadValue(void);
    OFCondition alignValue();
    void printPixel(ostream & out, const OFBool showFullData,
                    const int level, const char *pixelFileName,
		            size_t *pixelCounter);

public:
    DcmOtherByteOtherWord( const DcmTag &tag, const Uint32 len = 0);
    DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old );
    virtual ~DcmOtherByteOtherWord();

    DcmOtherByteOtherWord &operator=(const DcmOtherByteOtherWord &obj) { DcmElement::operator=(obj); return *this; }

    virtual OFCondition setVR(DcmEVR vr);
    virtual DcmEVR ident() const;
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM(void) { return 1L; }

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
				 const E_TransferSyntax oldXfer);

    virtual OFCondition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype 
					 = EET_UndefinedLength);

    // put an Unit8 array if VR == OB else return error code
    virtual OFCondition putUint8Array(const Uint8 * byteValue,
				      const unsigned long length);    

    // put an Unit16 array if VR == OW else return error code
    virtual OFCondition putUint16Array(const Uint16 * wordValue,
				       const unsigned long length ); 

    virtual OFCondition putString(const char * value);

    virtual OFCondition getUint8(Uint8 & byteValue,
                                 const unsigned long pos = 0);
    virtual OFCondition getUint16(Uint16 & wordValue,
                                  const unsigned long pos = 0);
    virtual OFCondition getUint8Array(Uint8 * & bytes);
    virtual OFCondition getUint16Array(Uint16 * & words);

    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVROBOW_H

/*
** CVS/RCS Log:
** $Log: dcvrobow.h,v $
** Revision 1.19  2001-10-02 11:47:34  joergr
** Added getUint8/16 routines to class DcmOtherByteOtherWord.
**
** Revision 1.18  2001/09/25 17:19:32  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.17  2001/06/01 15:48:51  meichel
** Updated copyright header
**
** Revision 1.16  2000/11/07 16:56:10  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.15  2000/04/14 15:31:34  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.14  2000/03/08 16:26:24  meichel
** Updated copyright header.
**
** Revision 1.13  2000/03/03 14:05:27  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.12  2000/02/10 10:50:55  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.11  1999/03/31 09:25:03  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.10  1998/11/12 16:47:51  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.9  1997/07/21 08:25:15  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/05/27 13:48:30  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/16 08:31:20  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.6  1997/04/18 08:13:31  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.5  1996/08/05 08:45:33  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:17  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:07  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
