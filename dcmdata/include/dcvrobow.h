/*
**
** Author: Gerd Ehlers      05.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrobow.h
**
** Purpose:
** Interface of class DcmOtherByteOtherWord
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:45:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrobow.h,v $
** CVS/RCS Revision:	$Revision: 1.5 $
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
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual unsigned long getVM(void) { return 1L; }

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL);

    virtual E_Condition put(const Uint8 * byteValue,
			    const unsigned long length);      // number of bytes

    virtual E_Condition put(const Uint16 * wordValue,
			    const unsigned long length );      // number of words

    virtual E_Condition put(const char * value);

    virtual E_Condition get(Uint8 * & bytes);
    virtual E_Condition get(Uint16 * & words);

    Uint8 * getBytes(void);		       // Restriction of C++
    Uint16 * getWords(void);	       //	   -""-

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVROBOW_H

/*
** CVS/RCS Log:
** $Log: dcvrobow.h,v $
** Revision 1.5  1996-08-05 08:45:33  andreas
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
