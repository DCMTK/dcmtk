/*
**
** Author: Gerd Ehlers	    05.05.94 -- Created
**         Andreas Barth	01.12.95 -- New Stream and value storing
** Kuratorium OFFIS e.V.
**
** Module: dcbytstr.h
**
** Purpose:
** Interface of class DcmByteString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-04-18 08:13:28 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcbytstr.h,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"

typedef enum
{
	DCM_MachineString,
	DCM_DicomString,
	DCM_UnknownString
} E_StringMode;

class DcmByteString : public DcmElement 
{
private:
    Uint32 realLength;
    E_StringMode fStringMode;

protected:
    char paddingChar;
    Uint32 maxLength;

    virtual Uint8 * newValueField(void);
    virtual void postLoadValue(void);

    E_Condition makeMachineByteString(void);
    E_Condition makeDicomByteString(void);


public:
    DcmByteString(const DcmTag &tag,
		  const Uint32 len = 0);
    DcmByteString(const DcmByteString& old,
                  const DcmEVR oldIdent = EVR_UNKNOWN);

    virtual ~DcmByteString();

    virtual DcmEVR ident(void) const { return EVR_UNKNOWN; } 
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual unsigned long getVM();

    Uint32 getRealLength(void);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL);

    virtual E_Condition putString(const char *byteStringValue);

    virtual E_Condition getString(char * & byteStringValue);

    virtual E_Condition clear();
    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCBYTSTR_H

/*
** CVS/RCS Log:
** $Log: dcbytstr.h,v $
** Revision 1.8  1997-04-18 08:13:28  andreas
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
** Revision 1.7  1996/08/05 08:45:15  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.6  1996/03/12 15:26:52  hewett
** Removed get method for unsigned char*
**
** Revision 1.5  1996/01/29 13:38:11  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:13  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:52  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
