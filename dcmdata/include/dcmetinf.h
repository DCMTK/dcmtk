/*
**
** Author: Gerd Ehlers	    15.05.94 -- Created
**         Andreas Barth    02.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcmetinf.h
**
** Purpose:
** Interface of class DcmMetaInfo
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:45:24 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcmetinf.h,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
*/

#ifndef DCMETINF_H
#define DCMETINF_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"


#define DCM_Magic                       "DICM"
#define DCM_MagicLen                    4
#define DCM_PreambleLen                 128
#define META_HEADER_DEFAULT_TRANSFERSYNTAX EXS_LittleEndianExplicit



class DcmMetaInfo : public DcmItem 
{
private:
    char filePreamble[DCM_PreambleLen+DCM_MagicLen];
    BOOL preambleUsed;
    E_TransferState fPreambleTransferState;
    E_TransferSyntax Xfer;

    void setPreamble(void);
    BOOL checkAndReadPreamble(DcmStream & inStream,
			      E_TransferSyntax & newxfer);	 // out
    BOOL nextTagIsMeta(DcmStream & inStream);
    E_Condition readGroupLength(DcmStream & inStream,			// inout
				const E_TransferSyntax xfer,    // in
                                const DcmTagKey & xtag,         // in
                                const E_GrpLenEncoding gltype,  // in
				Uint32 & headerLen, 		// out
				Uint32 & bytesRead,		// out
				const Uint32 maxReadLength 
				= DCM_MaxReadLength);     // in

public:
    DcmMetaInfo();
    DcmMetaInfo(const DcmMetaInfo &old);
    virtual ~DcmMetaInfo();

    virtual DcmEVR ident(void) const { return EVR_metainfo; }
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual	void transferInit(void);
    virtual void transferEnd(void);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength = DCM_MaxReadLength);
    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL);

};

#endif // DCMETINF_H

/*
** CVS/RCS Log:
** $Log: dcmetinf.h,v $
** Revision 1.5  1996-08-05 08:45:24  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/09 11:06:16  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:57  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/

