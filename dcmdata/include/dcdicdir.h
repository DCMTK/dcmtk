/*
**
** Author: Gerd Ehlers	    04.06.94 -- Creation
**         Andreas Barth    30.11.95 -- new stream 
** Kuratorium OFFIS e.V.
**
** Module: dcdicdir.h
**
** Purpose:
** Interface of class DcmDicomDir
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:45:18 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdicdir.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#ifndef DCDICDIR_H
#define DCDICDIR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"
#include "dcfilefo.h"
#include "dcsequen.h"
#include "dcdirrec.h"
#include "dcvrulup.h"


#define DEFAULT_DICOMDIR_NAME  "DICOMDIR"
#define TEMPNAME_TEMPLATE      "DDXXXXXX"
#define DICOMDIR_BACKUP_SUFFIX ".$$$"
#define DICOMDIR_DEFAULT_TRANSFERSYNTAX  EXS_LittleEndianExplicit
                                                   

typedef struct 
{
    DcmItem *item;
    Uint32  fileOffset;
} ItemOffset;


class DcmDicomDir 
{
  protected:
    E_Condition	errorFlag;
    char * dicomDirFileName;
    BOOL modified;              // wird wo gebraucht ?
    BOOL mustCreateNewDir;
    DcmFileFormat * DirFile;
    DcmDirectoryRecord * RootRec;
    DcmSequenceOfItems * MRDRSeq;

    // Manipulation der internen Datenelemente:
    E_Condition createNewElements(char * fileSetID);             // in
    DcmDataset& getDataset(void);

    // Seiteneffekt-freie Methoden zur Manipulation und Konversion:
    DcmSequenceOfItems&    getDirRecSeq(      DcmDataset &dset );   // inout
    DcmUnsignedLongOffset* lookForOffsetElem( DcmObject *obj,       // in
                                              const DcmTagKey &offsetTag );// in
    DcmDirectoryRecord*    recurseMatchFile(  DcmDirectoryRecord* startRec,// in
                                              char *filename );            // in
    DcmDirectoryRecord*    searchMatchFile(   DcmSequenceOfItems& recSeq,  // in
                                              char *filename );            // in
    E_Condition resolveGivenOffsets( DcmObject *startPoint,         // inout
                                     ItemOffset *itOffsets,         // in
                                     const unsigned long numOffsets,  // in
                                     const DcmTagKey &offsetTag );  // in
    E_Condition resolveAllOffsets(   DcmDataset &dset );            // inout
    E_Condition linkMRDRtoRecord(    DcmDirectoryRecord *dRec );    // inout
    E_Condition moveRecordToTree(    DcmDirectoryRecord *startRec,  // in
                                     DcmSequenceOfItems &fromDirSQ, // inout
                                     DcmDirectoryRecord *toRecord );// inout
    E_Condition moveMRDRbetweenSQs(  DcmSequenceOfItems &fromSQ,    // in
                                     DcmSequenceOfItems &toDirSQ ); // inout
    Uint32     lengthUntilSQ(       DcmDataset &dset,              // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    Uint32     lengthOfRecord(      DcmItem *item,                 // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    E_Condition convertGivenPointer( DcmObject *startPoint,         // inout
                                     ItemOffset *itOffsets,         // in
                                     const unsigned long numOffsets,  // in
                                     const DcmTagKey &offsetTag );  // in
    E_Condition convertAllPointer(   DcmDataset &dset,              // inout
                                     Uint32 beginOfFileSet,        // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    E_Condition copyRecordPtrToSQ(   DcmDirectoryRecord *record,    // in
                                     DcmSequenceOfItems &toDirSQ,   // inout
                                     DcmDirectoryRecord **firstRec, // out
                                     DcmDirectoryRecord **lastRec );// out
    E_Condition insertMediaSOPUID(   DcmMetaInfo &metaInfo );       // inout
    E_Condition countMRDRRefs(       DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     const unsigned long numCounters );   // in
    E_Condition checkMRDRRefCounter( DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     const unsigned long numCounters );    // in

    // komplette Reorganisation der verwalteten Directory Records (Seiteneffekt)
    E_Condition convertLinearToTree();
    E_Condition convertTreeToLinear( Uint32 beginOfFileSet,        // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype,        // in
                                     DcmSequenceOfItems &unresRecs);// inout

public:
    DcmDicomDir();
    DcmDicomDir( char *fileName,
                 char *fileSetID = (char*)NULL );  // only used for new DICOMDIR
    DcmDicomDir( const DcmDicomDir &newDir );
    virtual ~DcmDicomDir();

    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual E_Condition         error();
    virtual DcmFileFormat&      getDirFileFormat();
    virtual DcmDirectoryRecord& getRootRecord();
    virtual DcmSequenceOfItems& getMRDRSequence();
    virtual DcmDirectoryRecord* matchFilename(     char *filename );
    virtual DcmDirectoryRecord* matchOrCreateMRDR( char *filename );
    virtual E_Condition         write(const E_TransferSyntax oxfer
                                            = DICOMDIR_DEFAULT_TRANSFERSYNTAX,
                                      const E_EncodingType enctype
                                            = EET_UndefinedLength,
                                      const E_GrpLenEncoding gltype
                                            = EGL_withoutGL );
// PENDING: DICOM-konform, aber unvollstaendig
    virtual E_Condition         verify( BOOL autocorrect = FALSE );
};

#endif // DCDICDIR_H

/*
** CVS/RCS Log:
** $Log: dcdicdir.h,v $
** Revision 1.3  1996-08-05 08:45:18  andreas
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
**
*/

