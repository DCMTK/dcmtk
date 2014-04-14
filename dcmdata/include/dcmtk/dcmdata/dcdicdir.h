/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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
 */

#ifndef DCDICDIR_H
#define DCDICDIR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcvrulup.h"


#define DEFAULT_DICOMDIR_NAME   "DICOMDIR"
#define DICOMDIR_BACKUP_SUFFIX  ".$$$"
#define DICOMDIR_TEMP_SUFFIX    ".tmp"
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
class DCMTK_DCMDATA_EXPORT DcmDicomDir
{
public:
    /// default constructor
    DcmDicomDir();

    /** constructor
     *  @param fileName filename to read a DICOMDIR from. If empty, an attempt is
     *    made to read file DEFAULT_DICOMDIR_NAME ("DICOMDIR").
     *  @param fileSetID file set ID of this file set, used only for new DICOMDIRs
     */
    DcmDicomDir( const OFFilename &fileName,
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
    virtual const OFFilename &getDirFileName();

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
                                     const OFMap<Uint32, DcmDirectoryRecord *> &itOffsets, // in
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
    OFFilename dicomDirFileName;

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
