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
** Update Date:		$Date: 1996-01-05 13:22:52 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcbytstr.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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

    virtual DcmEVR ident() const { return EVR_UNKNOWN; } 
    virtual void print(int level = 0);
    virtual unsigned long getVM();

	Uint32 getRealLength(void);

    virtual E_Condition write(DcmStream & outStream,
							  const E_TransferSyntax oxfer,
							  const E_EncodingType enctype = EET_UndefinedLength,
							  const E_GrpLenEncoding gltype = EGL_withoutGL);

    virtual E_Condition put(const char *byteStringValue);

	virtual E_Condition get(char * & byteStringValue);
	virtual E_Condition get(unsigned char * & byteStringValue);
    char * get();

    virtual E_Condition clear();
    virtual E_Condition verify(BOOL autocorrect = FALSE);
};


#endif // DCBYTSTR_H

/*
** CVS/RCS Log:
** $Log: dcbytstr.h,v $
** Revision 1.3  1996-01-05 13:22:52  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
