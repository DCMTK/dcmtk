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
 *  Purpose: Interface of class DcmMetaInfo
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:05:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcmetinf.h,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMETINF_H
#define DCMETINF_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
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
    OFBool preambleUsed;
    E_TransferState fPreambleTransferState;
    E_TransferSyntax Xfer;

    void setPreamble(void);
    OFBool checkAndReadPreamble(DcmStream & inStream,
			      E_TransferSyntax & newxfer);	 // out
    OFBool nextTagIsMeta(DcmStream & inStream);
    E_Condition readGroupLength(DcmStream & inStream,		// inout
				const E_TransferSyntax xfer,    // in
                                const DcmTagKey & xtag,         // in
				const E_GrpLenEncoding glenc,   // in
				Uint32 & headerLen, 		// out
				Uint32 & bytesRead,		// out
				const Uint32 maxReadLength 
				= DCM_MaxReadLength);     // in

public:
    DcmMetaInfo();
    DcmMetaInfo(const DcmMetaInfo &old);
    virtual ~DcmMetaInfo();

    virtual DcmEVR ident(void) const { return EVR_metainfo; }
    virtual void print(ostream & out = COUT, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual	void transferInit(void);
    virtual void transferEnd(void);

    inline E_TransferSyntax getOriginalXfer(void) { return Xfer; }

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength);

};

#endif // DCMETINF_H

/*
** CVS/RCS Log:
** $Log: dcmetinf.h,v $
** Revision 1.11  2000-03-03 14:05:24  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:51  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:24:41  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1997/09/22 14:56:12  hewett
** Added a method to retreive the original transfer syntax of a read
** meta-header (getOriginalXfer).  This functionality is needed by
** the DCMCHECK package.
**
** Revision 1.7  1997/07/21 08:25:09  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/05/16 08:23:47  andreas
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
** Revision 1.5  1996/08/05 08:45:24  andreas
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
*/

