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
** Update Date:		$Date: 1997-05-27 13:48:26 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdatset.h,v $
** CVS/RCS Revision:	$Revision: 1.7 $
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

    Uint32 calcElementLength(const E_TransferSyntax xfer,
			     const E_EncodingType enctype);

    virtual BOOL canWriteXfer(const E_TransferSyntax newXfer,
			      const E_TransferSyntax oldXfer = EXS_Unknown);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype,
			      const E_GrpLenEncoding glenc,
			      const E_PaddingEncoding padenc = EPD_noChange,
			      const Uint32 padlen = 0,
			      const Uint32 subPadlen = 0,
			      Uint32 instanceLength = 0);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);

};



#endif // DCDATSET_H


/*
** CVS/RCS Log:
** $Log: dcdatset.h,v $
** Revision 1.7  1997-05-27 13:48:26  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.6  1997/05/16 08:23:45  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.5  1996/08/05 08:45:17  andreas
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
