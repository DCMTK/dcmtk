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
** Update Date:		$Date: 1997-07-21 08:14:39 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcfilefo.h,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCFILEFO_H
#define DCFILEFO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcerror.h"
#include "dcsequen.h"
#include "dcdatset.h"

class DcmMetaInfo;
class DcmStream;
class DcmRepresentationParameter;

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
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0);

    virtual Uint32  calcElementLength(const E_TransferSyntax xfer,
				      const E_EncodingType enctype);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
			      const E_TransferSyntax oldXfer = EXS_Unknown);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype,
			      const E_GrpLenEncoding glenc,
			      const E_PaddingEncoding padenc = EPD_noChange,
			      const Uint32 padlen = 0,
			      const Uint32 subPadlen = 0,
			      Uint32 instanceLength = 0);

    DcmMetaInfo* getMetaInfo();
    DcmDataset*  getDataset();
    DcmDataset*  getAndRemoveDataset();

    // methods for different pixel representations

    // choose Representation changes the representation of
    // PixelData Elements in the data set to the given representation
    // If the representation does not exists it creates one.
    E_Condition chooseRepresentation(
	const E_TransferSyntax repType,
	const DcmRepresentationParameter * repParam) 
    {
	return getDataset()->chooseRepresentation(repType, repParam);
    }

    // checks if all PixelData elements have a conforming representation 
    // (for definition of conforming representation see dcpixel.h).
    // if one PixelData element has no conforming representation
    // OFFalse is returned.
    OFBool hasRepresentation(
	const E_TransferSyntax repType,
	const DcmRepresentationParameter * repParam)
    {
	return getDataset()->hasRepresentation(repType, repParam);
    }

    // removes all but the original representation in all pixel data
    // elements
    void removeAllButOriginalRepresentations()
    {
	getDataset()->removeAllButOriginalRepresentations();
    }

    // removes all but the current representation in all pixel data
    // elements. Makes the current representation original
    void removeAllButCurrentRepresentations()
    {
	getDataset()->removeAllButCurrentRepresentations();
    }


// The following methods have no meaning in DcmFileFormat and shall not be 
// called. Since it is not possible to delete inherited methods from a class
// stubs are defined that create an error.

    virtual E_Condition  insertItem(DcmItem* item,
				    const unsigned long where = DCM_EndOfListIndex);
    virtual DcmItem*	 remove(const unsigned long num);
    virtual DcmItem*     remove(DcmItem* item);
    virtual E_Condition  clear();

};


#endif // DCFILEFO_H

/*
** CVS/RCS Log:
** $Log: dcfilefo.h,v $
** Revision 1.9  1997-07-21 08:14:39  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/05/27 13:48:27  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/16 08:23:47  andreas
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
** Revision 1.6  1996/08/05 08:45:21  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/07/29 15:46:23  andreas
** Add method getAndRemoveDataset to remove connection of dataset from fileformat
**
** Revision 1.4  1996/01/09 11:06:15  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
