/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of the class DcmDataset
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:50:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdatset.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDATSET_H
#define DCDATSET_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcitem.h"

class DcmStream;
class DcmRepresentationParameter;

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
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);

    Uint32 calcElementLength(const E_TransferSyntax xfer,
			     const E_EncodingType enctype);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
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

    // methods for different pixel representations

    // choose Representation changes the representation of
    // PixelData Elements in the data set to the given representation
    // If the representation does not exists it creates one.
    E_Condition chooseRepresentation(
	const E_TransferSyntax repType,
	const DcmRepresentationParameter * repParam);

    // checks if all PixelData elements have a conforming representation 
    // (for definition of conforming representation see dcpixel.h).
    // if one PixelData element has no conforming representation
    // OFFalse is returned.
    OFBool hasRepresentation(
	const E_TransferSyntax repType,
	const DcmRepresentationParameter * repParam);

    // removes all but the original representation in all pixel data
    // elements
    void removeAllButOriginalRepresentations();

    // removes all but the current representation and sets the original
    // representation to current
    void removeAllButCurrentRepresentations();
};



#endif // DCDATSET_H


/*
** CVS/RCS Log:
** $Log: dcdatset.h,v $
** Revision 1.10  2000-02-10 10:50:50  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:24:32  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1997/07/21 08:14:38  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.7  1997/05/27 13:48:26  andreas
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
*/
