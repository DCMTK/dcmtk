/*
 *
 * Author: Gerd Ehlers	    Created:  06-04-94
 *                          Modified: 02-07-95
 *
 * Module: dcdicdir.h
 *
 * Purpose:
 * Interface of class DcmDicomDir
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
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
                                                   

typedef struct {
    DcmItem  *item;
    T_VR_UL  fileOffset;
} ItemOffset;


class DcmDicomDir {
protected:
    E_Condition 	errorFlag;
    char		*dicomDirFileName;
    BOOL                modified;              // wird wo gebraucht ?
    BOOL		mustCreateNewDir;
    iDicomStream	*iDS;
    DcmFileFormat	*DirFile;
    DcmDirectoryRecord	*RootRec;
    DcmSequenceOfItems  *MRDRSeq;

    // Manipulation der internen Datenelemente:
    E_Condition createNewElements(   char *fileSetID );             // in
    DcmDataset& getDataset();

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
                                     T_VR_UL numOffsets,            // in
                                     const DcmTagKey &offsetTag );  // in
    E_Condition resolveAllOffsets(   DcmDataset &dset );            // inout
    E_Condition linkMRDRtoRecord(    DcmDirectoryRecord *dRec );    // inout
    E_Condition moveRecordToTree(    DcmDirectoryRecord *startRec,  // in
                                     DcmSequenceOfItems &fromDirSQ, // inout
                                     DcmDirectoryRecord *toRecord );// inout
    E_Condition moveMRDRbetweenSQs(  DcmSequenceOfItems &fromSQ,    // in
                                     DcmSequenceOfItems &toDirSQ ); // inout
    T_VR_UL     lengthUntilSQ(       DcmDataset &dset,              // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    T_VR_UL     lengthOfRecord(      DcmItem *item,                 // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    E_Condition convertGivenPointer( DcmObject *startPoint,         // inout
                                     ItemOffset *itOffsets,         // in
                                     T_VR_UL numOffsets,            // in
                                     const DcmTagKey &offsetTag );  // in
    E_Condition convertAllPointer(   DcmDataset &dset,              // inout
                                     T_VR_UL beginOfFileSet,        // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    E_Condition copyRecordPtrToSQ(   DcmDirectoryRecord *record,    // in
                                     DcmSequenceOfItems &toDirSQ,   // inout
                                     DcmDirectoryRecord **firstRec, // out
                                     DcmDirectoryRecord **lastRec );// out
    E_Condition insertMediaSOPUID(   DcmMetaInfo &metaInfo );       // inout
    E_Condition countMRDRRefs(       DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     T_VR_UL numCounters );         // in
    E_Condition checkMRDRRefCounter( DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     T_VR_UL numCounters );         // in

    // komplette Reorganisation der verwalteten Directory Records (Seiteneffekt)
    E_Condition convertLinearToTree();
    E_Condition convertTreeToLinear( T_VR_UL beginOfFileSet,        // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype,        // in
                                     DcmSequenceOfItems &unresRecs);// inout

public:
    DcmDicomDir();
    DcmDicomDir( char *fileName,
                 char *fileSetID = (char*)NULL );  // only used for new DICOMDIR
    DcmDicomDir( const DcmDicomDir &newDir );
    virtual ~DcmDicomDir();

    virtual void                print(  int level = 0 );
    virtual E_Condition         error();
    virtual DcmFileFormat&      getDirFileFormat();
    virtual DcmDirectoryRecord& getRootRecord();
    virtual DcmSequenceOfItems& getMRDRSequence();
    virtual DcmDirectoryRecord* matchFilename(     char *filename );
    virtual DcmDirectoryRecord* matchOrCreateMRDR( char *filename );
    virtual E_Condition         write(  E_TransferSyntax oxfer
                                            = DICOMDIR_DEFAULT_TRANSFERSYNTAX,
                                        E_EncodingType enctype
                                            = EET_UndefinedLength,
                                        E_GrpLenEncoding gltype
                                            = EGL_withoutGL );
// PENDING: DICOM-konform, aber unvollstaendig
    virtual E_Condition         verify( BOOL autocorrect = FALSE );
};

#endif // DCDICDIR_H

