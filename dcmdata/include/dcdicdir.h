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
 *  Purpose: Interface of class DcmDicomDir
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:50:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdicdir.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
                                                   

typedef struct 
{
    DcmItem *item;
    Uint32  fileOffset;
} ItemOffset;


class DcmDicomDir 
{
  private:

 // --- declarations to avoid compiler warnings
 
    DcmDicomDir &operator=(const DcmDicomDir &);

  protected:
    E_Condition errorFlag;
    char * dicomDirFileName;
    OFBool modified;              // wird wo gebraucht ?
    OFBool mustCreateNewDir;
    DcmFileFormat * DirFile;
    DcmDirectoryRecord * RootRec;
    DcmSequenceOfItems * MRDRSeq;

    // Manipulation der internen Datenelemente:
    E_Condition createNewElements(const char* fileSetID);  // in
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
    E_Condition convertTreeToLinear( Uint32 beginOfFileSet,         // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype,        // in
                                     E_GrpLenEncoding glenc,        // in
                                     DcmSequenceOfItems &unresRecs);// inout

public:
    DcmDicomDir();
    DcmDicomDir( const char *fileName,
                 const char *fileSetID = (char*)NULL );  // only used for new DICOMDIR
    DcmDicomDir( const DcmDicomDir &newDir );
    virtual ~DcmDicomDir();

    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);
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
                                      const E_GrpLenEncoding glenc
                                            = EGL_withoutGL );
// PENDING: DICOM-konform, aber unvollstaendig
    virtual E_Condition         verify( OFBool autocorrect = OFFalse );
};

#endif // DCDICDIR_H

/*
** CVS/RCS Log:
** $Log: dcdicdir.h,v $
** Revision 1.10  2000-02-10 10:50:50  joergr
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

