/*
**
** Author: Gerd Ehlers      26.04.94 -- Created
**         Andreas Barth    02.12.95 -- Modified for new stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcdatset.h
**
** Purpose:
** Interface of the class DcmDataset
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:45:17 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdatset.h,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCDATSET_H
#define DCDATSET_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcitem.h"
#include "dcstream.h"



class DcmDataset : public DcmItem 
{
private:
    E_TransferSyntax Xfer;
    void resolveAmbigous(void);

public:
    DcmDataset();
    DcmDataset(const DcmDataset &old);
    virtual ~DcmDataset();

    inline E_TransferSyntax getOriginalXfer(void) { return Xfer; }

    virtual DcmEVR ident() const { return EVR_dataset; }
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL );
};



#endif // DCDATSET_H


/*
** CVS/RCS Log:
** $Log: dcdatset.h,v $
** Revision 1.5  1996-08-05 08:45:17  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/09 11:06:14  andreas
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
