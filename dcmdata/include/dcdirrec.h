/*
 *
 * Author: Gerd Ehlers	    Created:  06-04-94
 *			    Modified: 02-07-95
 *
 * Module: dcdirrec.h
 *
 * Purpose:
 * Interface of class DcmDirectoryRecord
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCDIRREC_H
#define DCDIRREC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcitem.h"
#include "dcsequen.h"
#include "dcfilefo.h"



typedef enum {
    ERT_root = 0,
    ERT_Curve = 1,
    ERT_FilmBox = 2,
    ERT_FilmSession = 3,
    ERT_Image = 4,
    ERT_ImageBox = 5,
    ERT_Interpretation = 6,
    ERT_ModalityLut = 7,
    ERT_Mrdr = 8,
    ERT_Overlay = 9,
    ERT_Patient = 10,
    ERT_PrintQueue = 11,
    ERT_Private = 12,
    ERT_Results = 13,
    ERT_Series = 14,
    ERT_Study = 15,
    ERT_StudyComponent = 16,
    ERT_Topic = 17,
    ERT_Visit = 18,
    ERT_VoiLut = 19
} E_DirRecType;


class DcmDicomDir;


class DcmDirectoryRecord : public DcmItem {

    friend class DcmDicomDir;

protected:
    DcmSequenceOfItems* lowerLevelList;
    E_DirRecType	DirRecordType;
    DcmDirectoryRecord* referencedMRDR;
    T_VR_UL		numberOfReferences;

    // Seiteneffekt-freie Konversions-Methoden:
    E_DirRecType	recordNameToType( const char *recordTypeName );
    char*		buildFileName( const char* origName, char* destName );
    E_Condition 	checkHierarchy( 	E_DirRecType upperRecord,
						E_DirRecType lowerRecord );

    // Zugriff auf Datenelemente innerhalb des Directory Records:
    E_Condition 	setRecordType(		E_DirRecType newType );
    E_DirRecType	lookForRecordType();
    E_Condition 	setReferencedFileID( const char *referencedFileID );
    const char*		lookForReferencedFileID();
    DcmDirectoryRecord* lookForReferencedMRDR();
    const char*		getReferencedFileName();      // lokal oder in MRDR
    E_Condition 	setRecordInUseFlag(	T_VR_US newFlag );
    T_VR_US		lookForRecordInUseFlag();
    long		getFileOffset();
    long		setFileOffset(long position );


    // Zugriff auf MRDR-Datenelement:
    E_Condition 	setNumberOfReferences(	T_VR_UL newRefNum );
    T_VR_UL		lookForNumberOfReferences();
    T_VR_UL		increaseRefNum();
    T_VR_UL		decreaseRefNum();

    // Verschiedenes:
// PENDING: benutze hierfuer DcmModuleTable
    E_Condition 	fillTypeElements(	E_DirRecType type,
						DcmFileFormat *fromFile );
    E_Condition 	fillElementsAndReadSOP( const char *referencedFileID );
    E_Condition 	masterInsertSub(	DcmDirectoryRecord* dirRec,
						T_VR_UL where = UNDEF_LEN );
    E_Condition 	purgeReferencedFile();

public:
    DcmDirectoryRecord();
    DcmDirectoryRecord( const DcmTag &tag,
			T_VR_UL len,
			iDicomStream *iDStream );
    DcmDirectoryRecord( E_DirRecType recordType,
			char *referencedFileID );     // Dicom-Format mit '\\'
    DcmDirectoryRecord( char *recordTypeName,
			char *referencedFileID );     // Dicom-Format mit '\\'
    DcmDirectoryRecord( const DcmDirectoryRecord &oldDirRec );
    virtual ~DcmDirectoryRecord();

    virtual DcmEVR 		ident() const;
    virtual void		print(	   int level = 0 );

    virtual E_Condition 	read(	   E_TransferSyntax xfer,
					   E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition 	write(	   oDicomStream &oDS,
					   E_TransferSyntax oxfer,
					   E_EncodingType enctype = EET_UndefinedLength,
					   E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition 	readBlock( E_TransferSyntax xfer,
					   E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition 	writeBlock(oDicomStream &oDS,
					   E_TransferSyntax oxfer,
					   E_EncodingType enctype = EET_UndefinedLength,
					   E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition 	verify(    BOOL autocorrect = FALSE );
    virtual E_Condition 	search(    const DcmTag &tag, 		    // in
					   DcmStack &resultStack,	    // inout
					   E_SearchMode mode = ESM_fromHere,// in
					   BOOL searchIntoSub = TRUE );     // in
    virtual E_Condition 	search(    const DcmTagKey &xtag,	    // in
					   DcmStack &resultStack,	    // inout
					   E_SearchMode mode = ESM_fromHere,// in
					   BOOL searchIntoSub = TRUE );     // in
    virtual E_Condition 	searchErrors(	 DcmStack &resultStack );   // inout

    virtual E_DirRecType	getRecordType();
    virtual DcmDirectoryRecord* getReferencedMRDR();
    virtual E_Condition 	assignToMRDR( DcmDirectoryRecord *mrdr );// in
    virtual E_Condition 	assignToSOPFile( const char *referencedFileID );  // in

// Manipulation der Lower-Level Directory Entities:

    virtual T_VR_UL		cardSub();
    virtual E_Condition 	insertSub( DcmDirectoryRecord* dirRec,
					   T_VR_UL where = UNDEF_LEN );
    virtual DcmDirectoryRecord* getSub(    T_VR_UL num );
    virtual DcmDirectoryRecord* removeSub( T_VR_UL num );
    virtual DcmDirectoryRecord* removeSub( DcmDirectoryRecord* dirRec );
    virtual E_Condition 	deleteSubAndPurgeFile( T_VR_UL num );
    virtual E_Condition 	deleteSubAndPurgeFile( DcmDirectoryRecord* dirRec );
    virtual E_Condition 	clearSub();
};



#endif // DCDIRREC_H

