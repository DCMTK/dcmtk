/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-12-06 12:49:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcmetinf.h,v $
 *  CVS/RCS Revision: $Revision: 1.18 $
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


/** a class representing the DICOM file meta information header
 */
class DcmMetaInfo
  : public DcmItem
{

  public:

    /** default constructor
     */
    DcmMetaInfo();

    /** copy constructor
     *  @param old item to be copied
     */
    DcmMetaInfo(const DcmMetaInfo &old);

    /** destructor
     */
    virtual ~DcmMetaInfo();

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident() const;

    E_TransferSyntax getOriginalXfer() const;

    /** print meta information header to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual void transferInit();
    virtual void transferEnd();

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write meta information header to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);


  private:

    void setPreamble();

    OFBool checkAndReadPreamble(DcmInputStream &inStream,
                                E_TransferSyntax &newxfer);  // out

    OFBool nextTagIsMeta(DcmInputStream &inStream);

    OFCondition readGroupLength(DcmInputStream &inStream,       // inout
                                const E_TransferSyntax xfer,    // in
                                const DcmTagKey &xtag,          // in
                                const E_GrpLenEncoding glenc,   // in
                                Uint32 &headerLen,              // out
                                Uint32 &bytesRead,              // out
                                const Uint32 maxReadLength = DCM_MaxReadLength);   // in

    char filePreamble[DCM_PreambleLen + DCM_MagicLen];
    OFBool preambleUsed;
    E_TransferState fPreambleTransferState;
    E_TransferSyntax Xfer;
};

#endif // DCMETINF_H

/*
** CVS/RCS Log:
** $Log: dcmetinf.h,v $
** Revision 1.18  2002-12-06 12:49:11  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.17  2002/08/27 16:55:35  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.16  2002/04/25 09:40:56  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.15  2001/09/25 17:19:27  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.14  2001/06/01 15:48:41  meichel
** Updated copyright header
**
** Revision 1.13  2000/04/14 15:31:32  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.12  2000/03/08 16:26:16  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:24  meichel
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

