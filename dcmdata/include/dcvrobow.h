/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrobow.h,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
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
    E_Condition alignValue();
    void printPixel(ostream & out, const OFBool showFullData,
                    const int level, const char *pixelFileName,
		            size_t *pixelCounter);

public:
    DcmOtherByteOtherWord( const DcmTag &tag, const Uint32 len = 0);
    DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old );
    virtual ~DcmOtherByteOtherWord();

    DcmOtherByteOtherWord &operator=(const DcmOtherByteOtherWord &obj) { DcmElement::operator=(obj); return *this; }

    virtual E_Condition setVR(DcmEVR vr);
    virtual DcmEVR ident() const;
    virtual void print(ostream & out = COUT, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM(void) { return 1L; }

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
				 const E_TransferSyntax oldXfer);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);


    // put an Unit8 array if VR == OB else return error code
    virtual E_Condition putUint8Array(const Uint8 * byteValue,
				      const unsigned long length);    

    // put an Unit16 array if VR == OW else return error code
    virtual E_Condition putUint16Array(const Uint16 * wordValue,
				       const unsigned long length ); 

    virtual E_Condition putString(const char * value);

    virtual E_Condition getUint8Array(Uint8 * & bytes);
    virtual E_Condition getUint16Array(Uint16 * & words);

    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVROBOW_H

/*
** CVS/RCS Log:
** $Log: dcvrobow.h,v $
** Revision 1.14  2000-03-08 16:26:24  meichel
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
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
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
