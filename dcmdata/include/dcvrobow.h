/*
**
** Author: Gerd Ehlers      05.05.94 -- First Creation
**
** Kuratorium OFFIS e.V.
**
** Module: dcvrobow.h
**
** Purpose:
** Interface of class DcmOtherByteOtherWord for data VR OB or OW
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 08:25:15 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrobow.h,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVROBOW_H
#define DCVROBOW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmOtherByteOtherWord : public DcmElement 
{
protected:
    virtual void postLoadValue(void);
    E_Condition alignValue();

public:
    DcmOtherByteOtherWord( const DcmTag &tag, const Uint32 len = 0);
    DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old );
    virtual ~DcmOtherByteOtherWord();

    virtual E_Condition setVR(DcmEVR vr);
    virtual DcmEVR ident() const;
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0);
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
** Revision 1.9  1997-07-21 08:25:15  andreas
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
**
**
*/
