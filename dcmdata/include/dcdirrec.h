/*
**
** Author: Gerd Ehlers    04.06.94 -- Created
**         Andreas Barth  30.11.95 -- modify to support new streams
**
** Module: dcdirrec.h
**
** Purpose:
** Interface of class DcmDirectoryRecord
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 08:00:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdirrec.h,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCDIRREC_H
#define DCDIRREC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
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

private:
    char* recordsOriginFile;

protected:
    DcmSequenceOfItems* lowerLevelList;
    E_DirRecType DirRecordType;
    DcmDirectoryRecord* referencedMRDR;
    Uint32 numberOfReferences;
    Uint32 offsetInFile;

    // Seiteneffekt-freie Konversions-Methoden:
    E_DirRecType recordNameToType(const char *recordTypeName);
    char *	buildFileName(const char* origName, char* destName );
    E_Condition checkHierarchy(const E_DirRecType upperRecord,
			       const E_DirRecType lowerRecord );

    // Zugriff auf Datenelemente innerhalb des Directory Records:
    E_Condition 	setRecordType(E_DirRecType newType );
    E_DirRecType	lookForRecordType();
    E_Condition 	setReferencedFileID( const char *referencedFileID );
    const char*		lookForReferencedFileID();
    DcmDirectoryRecord* lookForReferencedMRDR();
    const char*		getReferencedFileName();      // lokal oder in MRDR
    E_Condition 	setRecordInUseFlag(const Uint16 newFlag );
    Uint16		lookForRecordInUseFlag();
    Uint32		getFileOffset();
    Uint32		setFileOffset(Uint32 position );


    // Zugriff auf MRDR-Datenelement:
    E_Condition 	setNumberOfReferences(	Uint32 newRefNum );
    Uint32		lookForNumberOfReferences();
    Uint32		increaseRefNum();
    Uint32		decreaseRefNum();

    // Verschiedenes:
    E_Condition 	fillElementsAndReadSOP(const char *referencedFileID,
					       const char * sourceFileName);
    E_Condition 	masterInsertSub(DcmDirectoryRecord* dirRec,
					const unsigned long where 
					= DCM_EndOfListIndex);
    E_Condition 	purgeReferencedFile();

public:
    DcmDirectoryRecord();
    DcmDirectoryRecord(const DcmTag &tag,
		       const Uint32 len);
    DcmDirectoryRecord(const E_DirRecType recordType,
		       const char *referencedFileID,   // Dicom-Format with '\\'
		       const char * sourceFileName);   // OS Format
    
    DcmDirectoryRecord(const char *recordTypeName,
		       const char *referencedFileID,   // Dicom-Format with '\\'
		       const char * sourceFileName);   // OS Format

    DcmDirectoryRecord(const DcmDirectoryRecord &oldDirRec );
    virtual ~DcmDirectoryRecord();

    virtual DcmEVR ident() const;
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
    virtual E_Condition search(const DcmTagKey &xtag,	    // in
			       DcmStack &resultStack,	    // inout
			       E_SearchMode mode = ESM_fromHere,// in
			       OFBool searchIntoSub = OFTrue );     // in
    virtual E_Condition searchErrors(DcmStack &resultStack );   // inout

    virtual E_DirRecType getRecordType();
    virtual DcmDirectoryRecord* getReferencedMRDR();
    virtual E_Condition assignToMRDR( DcmDirectoryRecord *mrdr );// in
    virtual E_Condition assignToSOPFile(const char *referencedFileID,
					const char * sourceFileName);
								       	 
    // Manipulation of the Lower-Level Directory Entities:

    virtual unsigned long cardSub();
    virtual E_Condition insertSub(DcmDirectoryRecord* dirRec,
				  unsigned long where = DCM_EndOfListIndex,
				  OFBool before = OFFalse);
    virtual DcmDirectoryRecord* getSub(const unsigned long num);
    virtual DcmDirectoryRecord* removeSub(const unsigned long num);
    virtual DcmDirectoryRecord* removeSub( DcmDirectoryRecord* dirRec );
    virtual E_Condition deleteSubAndPurgeFile(const unsigned long num);
    virtual E_Condition deleteSubAndPurgeFile( DcmDirectoryRecord* dirRec );
    virtual E_Condition clearSub();

    // note the records origin file
    virtual void setRecordsOriginFile(const char* fname);
    virtual const char* getRecordsOriginFile();

};



#endif // DCDIRREC_H

/*
** CVS/RCS Log:
** $Log: dcdirrec.h,v $
** Revision 1.9  1997-07-21 08:00:45  andreas
** - Corrected error in DcmDirectoryRecord::write since this routine can
**   change the length of the record after this is calculated in the
**   sequence.
** - DcmDirectoryRecord can be build with a referenced Name and a source
**   filename. These name now can differ (lower case - upper case
**   characters).
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/07/07 07:42:04  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
**
** Revision 1.7  1997/05/16 08:31:21  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.6  1997/05/09 13:12:02  hewett
** Added a member variable and associated get/set methods to DcmDirectoryRecord
** to allow the origin of a directory record to be noted.
**
** Revision 1.5  1997/05/06 09:22:35  hewett
** Added a "before" flag to the insertion of items for compatibility with
** insertion in normal Sequences.
**
** Revision 1.4  1997/03/26 16:56:25  hewett
** Removed the DcmDirectoryRecord member function fillTypeElements since it
** did nothing useful.  Smoothed out some poor indentation.
**
** Revision 1.3  1996/08/05 08:45:20  andreas
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
