/*
**
** Author: Gerd Ehlers      26.04.94 -- Created
**         Andreas Barth    02.12.95 -- Modified for new stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcfilefo.h
**
** Purpose:
** Interface of class DcmFileFormat
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:22:55 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcfilefo.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCFILEFO_H
#define DCFILEFO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcsequen.h"
#include "dcmetinf.h"
#include "dcdatset.h"




class DcmFileFormat : public DcmSequenceOfItems 
{
private:
    E_Condition checkValue(DcmMetaInfo * metainfo,
			   DcmDataset * dataset,
			   const DcmTagKey & atagkey,
			   DcmObject * obj,
			   const E_TransferSyntax oxfer);
    E_TransferSyntax lookForXfer(DcmMetaInfo * metainfo );

public:
    DcmFileFormat();
    DcmFileFormat(DcmDataset * dataset);
    DcmFileFormat(const DcmFileFormat &old);
    virtual ~DcmFileFormat();

    virtual E_Condition validateMetaInfo(E_TransferSyntax oxfer);

    virtual DcmEVR ident() const { return EVR_fileFormat; }
    virtual void print(int level = 0);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength = DCM_MaxReadLength);
    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual DcmMetaInfo* getMetaInfo();
    virtual DcmDataset*  getDataset();


// The following methods have no meaning in DcmFileFormat and shall not be 
// called. Since it is not possible to delete inherited methods from a class
// stubs are defined that create an error.

    virtual E_Condition  insertItem(DcmItem* item,
				    unsigned long where = DCM_EndOfListIndex);
    virtual DcmItem*	 remove(unsigned long num);
    virtual DcmItem*     remove(DcmItem* item);
    virtual E_Condition  clear();
};


#endif // DCFILEFO_H

/*
** CVS/RCS Log:
** $Log: dcfilefo.h,v $
** Revision 1.3  1996-01-05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
