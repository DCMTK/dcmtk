/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDicomDir
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-15 13:58:43 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDICDIR_H
#define DCDICDIR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcvrulup.h"


#define DEFAULT_DICOMDIR_NAME  "DICOMDIR"
#define TEMPNAME_TEMPLATE      "DDXXXXXX"
#define DICOMDIR_BACKUP_SUFFIX ".$$$"
#define DICOMDIR_DEFAULT_TRANSFERSYNTAX  EXS_LittleEndianExplicit

/** helper structure for item offsets
 */
typedef struct
{
    /// DICOM item containing a directory record
    DcmItem *item;
    /// offset in file for this item
    Uint32  fileOffset;
} ItemOffset;


/** this class implements support for DICOMDIR files, which are special DICOM files
 *  containing a list of directory records, with a logical tree structure being
 *  implemented through references between records as byte offsets in file.
 */
class DcmDicomDir
{
public:
    /// default constructor
    DcmDicomDir();

    /** constructor
     *  @param fileName filename to read a DICOMDIR from. If NULL, an attempt is
     *  made to read file DEFAULT_DICOMDIR_NAME ("DICOMDIR").
     *  @param fileSetID file set ID of this file set, used only for new DICOMDIRs
     */
    DcmDicomDir( const char *fileName,
                 const char *fileSetID = NULL );  // only used for new DICOMDIR

    /// destructor. If DICOMDIR was modified, writes new file.
    virtual ~DcmDicomDir();

    /** print all elements of the item to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /// returns current status flag
    virtual OFCondition error();

    /** returns reference to DICOM file from which DICOMDIR was read.
     *  Note, this file may be obsolete if the DICOMDIR was modified
     *  @return reference to DICOM file
     */
    virtual DcmFileFormat& getDirFileFormat();

    /** returns file name from which DICOMDIR was read.
     *  @return filename of DICOMDIR
     */
    virtual const char* getDirFileName();

    /// returns root directory record
    virtual DcmDirectoryRecord& getRootRecord();

    /// returns container in which all MRDR (multi-reference directory records) are stored
    virtual DcmSequenceOfItems& getMRDRSequence();

    /** look up directory record for the given referenced file ID (not OS file path)
     *  @return directory record if found, NULL otherwise
     */
    virtual DcmDirectoryRecord* matchFilename( const char *filename );

    /** look up MRDR for the given referenced file ID (not OS file path).
     *  If there is none yet, create one.
     *  @return MRDR for given referenced file ID
     */
    virtual DcmDirectoryRecord* matchOrCreateMRDR( const char *filename );

    /** writes DICOMDIR to file
     *  @param oxfer must be DICOMDIR_DEFAULT_TRANSFERSYNTAX
     *  @param enctype encoding type for sequences
     *  @param glenc encoding type for group lengths
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition write(
      const E_TransferSyntax oxfer = DICOMDIR_DEFAULT_TRANSFERSYNTAX,
      const E_EncodingType enctype = EET_UndefinedLength,
      const E_GrpLenEncoding glenc = EGL_withoutGL );

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify( OFBool autocorrect = OFFalse );

  protected:

    // Manipulation of member variables
    OFCondition createNewElements(const char* fileSetID);  // in
    DcmDataset& getDataset(void);

    // side-effect free methods for manipulation and conversion
    DcmSequenceOfItems&    getDirRecSeq(      DcmDataset &dset );    // inout
    DcmUnsignedLongOffset* lookForOffsetElem( DcmObject *obj,        // in
                                              const DcmTagKey &offsetTag ); // in
    DcmDirectoryRecord*    recurseMatchFile(  DcmDirectoryRecord* startRec, // in
                                              const char *filename );       // in
    DcmDirectoryRecord*    searchMatchFile(   DcmSequenceOfItems& recSeq,   // in
                                              const char *filename );       // in
    OFCondition resolveGivenOffsets( DcmObject *startPoint,          // inout
                                     ItemOffset *itOffsets,          // in
                                     const unsigned long numOffsets, // in
                                     const DcmTagKey &offsetTag );   // in
    OFCondition resolveAllOffsets(   DcmDataset &dset );             // inout
    OFCondition linkMRDRtoRecord(    DcmDirectoryRecord *dRec );     // inout
    OFCondition moveRecordToTree(    DcmDirectoryRecord *startRec,   // in
                                     DcmSequenceOfItems &fromDirSQ,  // inout
                                     DcmDirectoryRecord *toRecord ); // inout
    OFCondition moveMRDRbetweenSQs(  DcmSequenceOfItems &fromSQ,     // in
                                     DcmSequenceOfItems &toDirSQ );  // inout
    Uint32     lengthUntilSQ(        DcmDataset &dset,               // in
                                     E_TransferSyntax oxfer,         // in
                                     E_EncodingType enctype );       // in
    Uint32     lengthOfRecord(       DcmItem *item,                  // in
                                     E_TransferSyntax oxfer,         // in
                                     E_EncodingType enctype );       // in
    OFCondition convertGivenPointer( DcmObject *startPoint,          // inout
                                     ItemOffset *itOffsets,          // in
                                     const unsigned long numOffsets, // in
                                     const DcmTagKey &offsetTag );   // in
    OFCondition convertAllPointer(   DcmDataset &dset,               // inout
                                     Uint32 beginOfFileSet,          // in
                                     E_TransferSyntax oxfer,         // in
                                     E_EncodingType enctype );       // in
    OFCondition copyRecordPtrToSQ(   DcmDirectoryRecord *record,     // in
                                     DcmSequenceOfItems &toDirSQ,    // inout
                                     DcmDirectoryRecord **firstRec,  // out
                                     DcmDirectoryRecord **lastRec ); // out
    OFCondition insertMediaSOPUID(   DcmMetaInfo &metaInfo );        // inout
    OFCondition countMRDRRefs(       DcmDirectoryRecord *startRec,   // in
                                     ItemOffset *refCounter,         // inout
                                     const unsigned long numCounters );  // in
    OFCondition checkMRDRRefCounter( DcmDirectoryRecord *startRec,   // in
                                     ItemOffset *refCounter,         // inout
                                     const unsigned long numCounters );  // in

    // complete re-organization of the managed directory records (side effect)
    OFCondition convertLinearToTree();

    OFCondition convertTreeToLinear( Uint32 beginOfFileSet,         // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype,        // in
                                     E_GrpLenEncoding glenc,        // in
                                     DcmSequenceOfItems &unresRecs);// inout

  private:

    /// private undefined copy assignment operator
    DcmDicomDir &operator=(const DcmDicomDir &);

    /// private undefined copy constructor
    DcmDicomDir( const DcmDicomDir &newDir );

    /// condition flag
    OFCondition errorFlag;

    /// file name the DICOMDIR was read from, or DEFAULT_DICOMDIR_NAME
    char * dicomDirFileName;

    /** flag indicating whether or not this DICOMDIR has been modified after being read from file.
     *  If true, the destructor of this class will write the modified DICOMDIR back to file
     */
    OFBool modified;

    /// flag indicating whether the DICOM file in DirFile needs to be re-created upon writing
    OFBool mustCreateNewDir;

    /** DICOM file into which a DICOMDIR is read first before the logical tree
     *  structure is created.
     */
    DcmFileFormat * DirFile;

    /// root directory record of this DICOMDIR
    DcmDirectoryRecord * RootRec;

    /// container in which all MRDR (multi-reference directory records) for this DICOMDIR are kept
    DcmSequenceOfItems * MRDRSeq;
};

#endif // DCDICDIR_H

/*
** CVS/RCS Log:
** $Log: dcdicdir.h,v $
** Revision 1.28  2010-11-15 13:58:43  uli
** Fixed some errors in doxygen comments.
**
** Revision 1.27  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.26  2010-08-18 15:13:26  joergr
** Added const specifier to char pointers where appropriate. Thanks to forum
** user "takeos" for the report.
**
** Revision 1.25  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.24  2010-02-22 11:39:53  uli
** Remove some unneeded includes.
**
** Revision 1.23  2009-11-04 09:58:07  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.22  2007-11-29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.21  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.20  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.19  2005/12/08 16:28:07  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.18  2005/11/07 16:59:24  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.17  2003/08/14 09:00:56  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.16  2002/12/06 12:49:09  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.15  2001/09/25 17:19:25  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.14  2001/06/01 15:48:36  meichel
** Updated copyright header
**
** Revision 1.13  2000/04/14 15:31:31  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.12  2000/03/08 16:26:12  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:23  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:50  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:24:34  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1998/07/15 15:48:44  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.7  1997/09/11 15:02:16  hewett
** Changed DcmDicomDir constructor to take const char* arguments.
**
** Revision 1.6  1997/07/21 08:25:06  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.5  1997/05/16 08:31:19  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.4  1997/04/24 12:08:28  hewett
** Fixed DICOMDIR generation bug affecting inclusion of Group Length
** attributes (file offsets were not being computed correctly).
**
** Revision 1.3  1996/08/05 08:45:18  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.2  1996/01/05 13:22:54  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/

