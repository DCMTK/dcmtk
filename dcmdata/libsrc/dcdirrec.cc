/*
**
** Author: Gerd Ehlers	    04.06.94 -- Created
**		   Andreas Barth	01.12.95 -- modify to support new streams
**
** Module: dcdirrec.cc
**
** Purpose:
** Implementation of class DcmDirectoryRecord
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-11 13:08:03 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdirrec.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream.h>

#include "dcdirrec.h"
#include "dctk.h"
#include "dcxfer.h"
#include "dcvr.h"
#include "dcvrus.h"
#include "dcdebug.h"

#ifdef HAVE_UNIX_H
#include <unix.h>
#endif


// ********************************


static char* DRTypeNames[] =
{
    "root",
    "CURVE",
    "FILM BOX",
    "FILM SESSION",
    "IMAGE",
    "IMAGE BOX",
    "INTERPRETATION",
    "MODALITY LUT",
    "MRDR",
    "OVERLAY",
    "PATIENT",
    "PRINT QUEUE",
    "PRIVATE",
    "RESULTS",
    "SERIES",
    "STUDY",
    "STUDY COMPONENT",
    "TOPIC",
    "VISIT",
    "VOI LUT"
};

static short DIM_OF_DRTypeNames = 20;


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord()
    : DcmItem( ItemTag )
{
Bdebug((5, "dcdirrec:DcmDirectoryRecord::DcmDirectoryRecord()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    DirRecordType = ERT_Private;
    referencedMRDR = (DcmDirectoryRecord*)NULL;
    numberOfReferences = 0;
offsetInFile = 0;
Edebug(());

}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmTag &tag,
									   const Uint32 len)
: DcmItem(tag, len)
{
	Bdebug((5, "DcmDirectoryRecord::DcmDirectoryRecord(DcmTag&,len=%ld)",
			len ));
	debug(( 8, "Object pointer this=0x%p", this ));

    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    DirRecordType = ERT_Private;
    referencedMRDR = (DcmDirectoryRecord*)NULL;
    numberOfReferences = 0;
	offsetInFile = 0;
	Edebug(());

}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const E_DirRecType recordType,
									   const char * referencedFileID )
: DcmItem(ItemTag)
{
	Bdebug((5, "DcmDirectoryRecord::DcmDirectoryRecord(recordType=%d,char*)",
			recordType ));
	debug(( 8, "Object pointer this=0x%p", this ));

    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    DirRecordType = recordType;
    referencedMRDR = (DcmDirectoryRecord*)NULL;
    numberOfReferences = 0;
	offsetInFile = 0;

    if ( DirRecordType != ERT_root )
		errorFlag = this -> fillElementsAndReadSOP(referencedFileID);
	Edebug(());

}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const char * recordTypeName,
									   const char * referencedFileID	)
: DcmItem(ItemTag)
{
	Bdebug((5, "DcmDirectoryRecord::DcmDirectoryRecord(char*,char*)" ));
	debug(( 8, "Object pointer this=0x%p", this ));

    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    DirRecordType = this -> recordNameToType( recordTypeName );
    referencedMRDR = (DcmDirectoryRecord*)NULL;
    numberOfReferences = 0;
	offsetInFile = 0;

    if ( DirRecordType != ERT_root )
		errorFlag = this -> fillElementsAndReadSOP( referencedFileID );
	Edebug(());
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmDirectoryRecord &old)
: DcmItem( old )
{
	Bdebug((5, "DcmDirectoryRecord::DcmDirectoryRecord(DcmObject&)" ));
	debug(( 8, "Object pointer this=0x%p", this ));
	debug(( 5, "ident()=%d", old.ident() ));

    DcmTag sequTag( DCM_DirectoryRecordSequence );
    if ( old.ident() == EVR_dirRecord )
    {
		DcmSequenceOfItems const *ll = old.lowerLevelList;
		lowerLevelList = new DcmSequenceOfItems( *ll ); // Copy-Constructor

		DirRecordType = old.DirRecordType;
		referencedMRDR = old.referencedMRDR;
		numberOfReferences = old.numberOfReferences;
		offsetInFile = old.offsetInFile;
    }
    else
    {
		lowerLevelList = new DcmSequenceOfItems( sequTag );
		DirRecordType = ERT_Private;
		referencedMRDR = (DcmDirectoryRecord*)NULL;
		numberOfReferences = 0;
		offsetInFile = 0;

		if ( old.ident() != EVR_item )
			cerr << "Warning: DcmDirectoryRecord: wrong use of Copy-Constructor"
				 << endl;
    }
    if ( old.ident() == EVR_item )
    {
		// bestimme einige interne Variablen-Inhalte
		referencedMRDR = this->lookForReferencedMRDR();
		DirRecordType = this->lookForRecordType();
    }

Edebug(());
}


// ********************************


DcmDirectoryRecord::~DcmDirectoryRecord()
{
	Bdebug((5, "dcdirrec:DcmDirectoryRecord::~DcmDirectoryRecord()" ));
	debug(( 8, "Object pointer this=0x%p", this ));

    delete lowerLevelList;
	Edebug(());
}


// ********************************
// ********************************


E_DirRecType DcmDirectoryRecord::recordNameToType(const char * recordTypeName)
{
	Bdebug((4, "dcdirrec:DcmDirectoryRecord::recordNameToType(char*)" ));

    E_DirRecType recType = ERT_Private;
    if (recordTypeName != (char*)NULL)
    {
		short i = 0;
		while (i < DIM_OF_DRTypeNames &&
			   strcmp(DRTypeNames[i], recordTypeName) != 0)
			i++;

		if (i < DIM_OF_DRTypeNames && 
			strcmp(DRTypeNames[i], recordTypeName) == 0)
			recType = (E_DirRecType)i;
		debug((4, "input char*=\"%s\" output enum=%d", recordTypeName, recType));
    }
	Edebug(());

    return recType;
}


// ********************************


char* DcmDirectoryRecord::buildFileName(const char * origName,
										char * destName)
{
    const char* from = origName;
    char* to = destName;
    char c;
    char lastchar = '\0';
    while ( (c = *from++) != 0 )
    {
		if ( c == '\\' )
		{
			if ( lastchar != '\\' )       // eliminiert doppelte '\\'
			{
				*to++ = PATH_SEPARATOR;
			}
		}
		else
			*to++ = c;
		lastchar = c;
    }
    *to = '\0';
    return destName;
}


// ********************************


E_Condition DcmDirectoryRecord::checkHierarchy(const E_DirRecType upperRecord,
											   const E_DirRecType lowerRecord )
{
    E_Condition l_error = EC_IllegalCall;
    switch ( upperRecord )
    {
	case ERT_root:
	    switch ( lowerRecord )
	    {
		case ERT_Patient:
		case ERT_PrintQueue:
		case ERT_Private:
		case ERT_Topic:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_FilmBox:
	    switch ( lowerRecord )
	    {
		case ERT_ImageBox:
		case ERT_Private:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_FilmSession:
	    switch ( lowerRecord )
	    {
		case ERT_FilmBox:
		case ERT_Private:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Patient:
	    switch ( lowerRecord )
	    {
		case ERT_Private:
		case ERT_Study:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_PrintQueue:
	    switch ( lowerRecord )
	    {
		case ERT_FilmSession:
		case ERT_Private:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Results:
	    switch ( lowerRecord )
	    {
		case ERT_Interpretation:
		case ERT_Private:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Series:
	    switch ( lowerRecord )
	    {
		case ERT_Curve:
		case ERT_Image:
		case ERT_ModalityLut:
		case ERT_Overlay:
		case ERT_Private:
		case ERT_VoiLut:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Study:
	    switch ( lowerRecord )
	    {
		case ERT_FilmSession:
		case ERT_Private:
		case ERT_Results:
		case ERT_Series:
		case ERT_StudyComponent:
		case ERT_Visit:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Topic:
	    switch ( lowerRecord )
	    {
		case ERT_Curve:
		case ERT_FilmSession:
		case ERT_Image:
		case ERT_ModalityLut:
		case ERT_Overlay:
		case ERT_Private:
		case ERT_Series:
		case ERT_Study:
		case ERT_VoiLut:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	case ERT_Mrdr:
	    l_error = EC_IllegalCall;
	    break;
	case ERT_Curve:
	case ERT_Image:
	case ERT_ImageBox:
	case ERT_Interpretation:
	case ERT_ModalityLut:
	case ERT_Overlay:
	case ERT_Private:
	case ERT_StudyComponent:
	case ERT_Visit:
	case ERT_VoiLut:
	    switch ( lowerRecord )
	    {
		case ERT_Private:
		    l_error = EC_Normal;
		    break;
		default:
		    l_error = EC_IllegalCall;
		    break;
	    }
	    break;
	default:
	    l_error = EC_Normal;
	    break;
    }
    return l_error;
}


// ********************************
// ********************************


E_Condition DcmDirectoryRecord::setRecordType( E_DirRecType newType )
{
    E_Condition l_error = EC_Normal;

    DcmTag dirRecTag( DCM_DirectoryRecordType );
    DcmCodeString *csP = new DcmCodeString( dirRecTag );
    csP->put( DRTypeNames[ newType ] );
    insert( csP, TRUE );

    return l_error;
}


// ********************************


E_DirRecType DcmDirectoryRecord::lookForRecordType()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::lookForRecordType()" ));

    E_DirRecType localType = ERT_Private;
    if ( !elementList->empty() )
    {
	DcmStack stack;
	if ( this->search( DCM_DirectoryRecordType,
			   stack, ESM_fromHere, FALSE ) == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_CS )
	    {
		DcmCodeString *recType = (DcmCodeString*)stack.top();
		recType->verify( TRUE );	    // erzwinge dealigning
		localType = this->recordNameToType( recType->get() );
debug(( 4, "RecordType-Element(0x%4.4hx,0x%4.4hx) Type=[%s]",
	   recType->getGTag(), recType->getETag(), DRTypeNames[DirRecordType] ));

	    }
	}
    }
Edebug(());

    return localType;
}


// ********************************


E_Condition DcmDirectoryRecord::setReferencedFileID( const char *referencedFileID )
{
    E_Condition l_error = EC_Normal;

    DcmTag refFileTag( DCM_ReferencedFileID );
    DcmCodeString *csP = new DcmCodeString( refFileTag );
    if ( referencedFileID != (char*)NULL )
	csP->put( referencedFileID );
    insert( csP, TRUE );

    return l_error;
}


// ********************************


const char* DcmDirectoryRecord::lookForReferencedFileID()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::lookForReferencedFileID()" ));

    const char *localFile = (char*)NULL;
    if ( !elementList->empty() )
    {
	DcmStack stack;
	if ( this->search( DCM_ReferencedFileID,
			   stack, ESM_fromHere, FALSE ) == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_CS )
	    {
		DcmCodeString *refFile = (DcmCodeString*)stack.top();
		refFile->verify( TRUE );	    // erzwinge dealigning
		localFile = refFile->get();
		if ( localFile != (char*)NULL && *localFile == '\0' )
		    localFile = (char*)NULL;
	    }
	}
    }
debug(( 4, "ReferencedFileID = [%s]", localFile ));
Edebug(());

    return localFile;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::lookForReferencedMRDR()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::lookForReferencedMRDR()" ));

    DcmDirectoryRecord *localMRDR = (DcmDirectoryRecord*)NULL;
    if ( !elementList->empty() )
    {
	DcmStack stack;
	if ( this->search( DCM_DirectoryRecordOffset,
			   stack, ESM_fromHere, FALSE ) == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_up )
	    {
		DcmUnsignedLongOffset *offElem;
		offElem = (DcmUnsignedLongOffset*)stack.top();
		localMRDR = (DcmDirectoryRecord*)offElem->getNextRecord();
debug(( 4, "MRDR Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p n=%p",
	   offElem->getGTag(), offElem->getETag(),
	   *offElem->get(), offElem, localMRDR ));

	    }
	}
    }
Vdebug((4, localMRDR==(DcmDirectoryRecord*)NULL, "no ReferencedMRDR found" ));
Edebug(());

    return localMRDR;
}


// ********************************


const char* DcmDirectoryRecord::getReferencedFileName()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::getReferencedFileName()" ));

    const char *localFile = (char*)NULL;
    if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
	localFile = referencedMRDR->lookForReferencedFileID();
    else
	localFile = this->lookForReferencedFileID();

debug(( 4, "ReferencedFileName = [%s]", localFile ));
Edebug(());

    return localFile;
}


// ********************************


E_Condition DcmDirectoryRecord::setRecordInUseFlag(const Uint16 newFlag )
{
    E_Condition l_error = EC_Normal;

    DcmTag recInUseTag( DCM_RecordInUseFlag );
    DcmUnsignedShort *usP = new DcmUnsignedShort( recInUseTag );
    usP->put( newFlag );
    insert( usP, TRUE );

    return l_error;
}


// ********************************


Uint16 DcmDirectoryRecord::lookForRecordInUseFlag()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::lookForRecordInUseFlag()" ));

    Uint16 localFlag = Uint16(0xffff);     // default value: Record is in use
    if ( !elementList->empty() )
    {
	DcmStack stack;
	if ( this->search( DCM_RecordInUseFlag,
			   stack, ESM_fromHere, FALSE ) == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_US )
		localFlag = ((DcmUnsignedShort*)stack.top())->
			get((const unsigned long)0 );
	}
    }
debug(( 4, "RecordInuseFlag = 0x%4.4hx", localFlag ));
Edebug(());

    return localFlag;
}


// ********************************


Uint32 DcmDirectoryRecord::getFileOffset()
{
    return offsetInFile;
}


// ********************************


Uint32 DcmDirectoryRecord::setFileOffset(Uint32 position )
{
    offsetInFile = position;
    return position;
}


// ********************************
// ********************************


E_Condition DcmDirectoryRecord::setNumberOfReferences(Uint32 newRefNum )
{
    E_Condition l_error = EC_Normal;
    if ( DirRecordType == ERT_Mrdr )
    {
		/*---	// irritiert nur durch nervige Bildschirmausgaben
		  // alten Wert zur Kontrolle auslesen
		  Uint32 oldRefNum = 0L;
		  DcmStack stack;
		  if (  this->search( DCM_NumberOfReferences, stack, ESM_fromHere, FALSE )
		  == EC_Normal )
		  {
		  if ( stack.top()->ident() == EVR_UL )
		  oldRefNum = ((DcmUnsignedLong*)stack.top())->get( 0 );
		  }
		  if ( oldRefNum != newRefNum )
		  {
		  l_error = EC_CorruptedData;
		  cerr << "Warning: DcmDirectoryRecord::setNumberOfReferences()"
		  " inconsistant data detected: oldRef=" << oldRefNum
		  << ", newRef=" << newRefNum << endl;
		  }
		  ---*/
		// neuen Wert eintragen
		DcmTag numRefTag( DCM_NumberOfReferences );
		DcmUnsignedLong *newUL = new DcmUnsignedLong( numRefTag );
		newUL->put( newRefNum );
		insert( newUL, TRUE );
    }
    else
    {
		errorFlag = EC_IllegalCall;
		cerr << "Error: illegal usage of DcmDirectoryRecord::setNumberOfReferences()"
			" - RecordType must be MRDR" << endl;
    }
    return l_error;
}


// ********************************


Uint32 DcmDirectoryRecord::lookForNumberOfReferences()
{
Bdebug((4, "dcdirrec:DcmDirectoryRecord::lookForNumberOfReferences()" ));

    Uint32 localRefNum = 0L;
    if ( !elementList->empty() )
    {
	DcmStack stack;
	if ( this->search( DCM_NumberOfReferences,
			   stack, ESM_fromHere, FALSE ) == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_UL )
		localRefNum = ((DcmUnsignedLong*)stack.top())->
			get((const unsigned long)0 );
	}
    }
debug(( 4, "NumberOfReferences = %ld", localRefNum ));
Edebug(());

    return localRefNum;
}


// ********************************


Uint32 DcmDirectoryRecord::increaseRefNum()
{
    if ( DirRecordType == ERT_Mrdr )
    {
	if ( numberOfReferences == 0L )
	    this->setRecordInUseFlag( 0xffff ); 	// aktiviere Record
	numberOfReferences++;
	errorFlag = this->setNumberOfReferences( numberOfReferences );
    }
    else
    {
	errorFlag = EC_IllegalCall;
	cerr << "Error: illegal usage of DcmDirectoryRecord::increaseRefNum()"
		" - RecordType must be MRDR" << endl;
    }
    return numberOfReferences;
}


// ********************************


Uint32 DcmDirectoryRecord::decreaseRefNum()
{
    if ( DirRecordType == ERT_Mrdr )
    {
	if ( numberOfReferences > 0 )
	{
	    numberOfReferences--;
	    if ( numberOfReferences == 0L )
		this->setRecordInUseFlag( 0x0000 );	// deaktiviere Record
	    errorFlag = this->setNumberOfReferences( numberOfReferences );
	}
	else
	{
	    errorFlag = EC_IllegalCall;
	    cerr << "Warning: DcmDirectoryRecord::decreaseRefNum()"
		    " attempt to decrease value lower than zero" << endl;
	}
    }
    else
    {
	errorFlag = EC_IllegalCall;
	cerr << "Error: illegal usage of DcmDirectoryRecord::decreaseRefNum()"
		" - RecordType must be MRDR" << endl;
    }
    return numberOfReferences;
}


// ********************************
// ********************************


// Die folgende Methode ist in der Datei "dcdirrcf.cc" implementiert:
/*
E_Condition DcmDirectoryRecord::fillTypeElements( E_DirRecType type,
						  DcmFileFormat *fromFile )
{}
*/


// ********************************

//
// Es werden alle Datenelemente gemaess Tabelle B.3.2.2-1 DICOM part 10
// erzeugt.
// Wird ein gueltiger Dateiname (referencedFileID) fuer eine SOP-Instanz
// uebergeben, so werden alle benoetigten keys aus dieser Datei ausgelesen:
//   Das Datenelement MRDRDirectoryRecordOffset entfaellt.
//   directFromFile == TRUE
//
// Ist referencedFileID ein NULL-Zeiger, so wird versucht, die keys indirekt
// ueber den referenzierten MRDR zu besorgen.
//   Das Datenelement ReferencedFileID bekommt die Laenge 0.
//   indirectViaMRDR == TRUE
//
// Existiert kein Verweis auf einen MRDR, so wird davon ausgegangen, das auch
// keine SOP-Instanz mit dem aktuellen Directory Record verknuepft werden soll:
//   Die Datenelemente ReferencedFileID, MRDRDirectoryRecordOffset,
//   ReferencedSOPClassUIDinFile und ReferencedSOPInstanceUIDinFile entfallen.
//   !directFromFile && !indirectViaMRDR
//

E_Condition DcmDirectoryRecord::fillElementsAndReadSOP
                                                 (const char *referencedFileID)
{
    Bdebug((3, "dcdirrec:DcmDirectoryRecord::fillElementsAndReadSOP(char*)" ));

    E_Condition l_error = EC_Normal;
    char *fileName = (char*)NULL;
    DcmFileStream * fileStream = NULL;
    DcmFileFormat *refFile = (DcmFileFormat*)NULL;

    BOOL directFromFile = FALSE;
    BOOL indirectViaMRDR = FALSE;
    if (referencedFileID != (char*)NULL && *referencedFileID != '\0' )
	directFromFile = TRUE;
    else if (DirRecordType != ERT_Mrdr &&		   // fuer MRDR verboten
	     referencedMRDR != NULL)
    {
	indirectViaMRDR = TRUE;
	referencedFileID = referencedMRDR->lookForReferencedFileID();
    }

    if ( referencedFileID != (char*)NULL && *referencedFileID != '\0' )
    {
	fileName = new char[ strlen( referencedFileID ) + 1 ];
	buildFileName( referencedFileID, fileName );
	fileStream = new DcmFileStream(fileName, DCM_ReadMode);
	if (!fileStream || fileStream->GetError() != EC_Normal )
	{
	    cerr << "Error: DcmDirectoryRecord::readSOPandFileElements():"
		" DicomFile \"" << fileName << "\" not found." << endl;
	    l_error = EC_InvalidStream;
	    directFromFile = FALSE;
	    indirectViaMRDR = FALSE;
	}
	else if ( DirRecordType != ERT_Mrdr )
	{
	    refFile = new DcmFileFormat();
	    refFile->transferInit();
	    refFile->read(*fileStream);
	}
    }
    else
    {
	directFromFile = FALSE;
	indirectViaMRDR = FALSE;
    }
    DcmStack stack;
    DcmUnsignedLongOffset *uloP;
    DcmUniqueIdentifier *uiP;

    DcmTag nextOffTag( DCM_NextDirectoryRecordOffset ); // (0004,1400)
    uloP = new DcmUnsignedLongOffset( nextOffTag );
    uloP->put(Uint32(0));
    if ( insert( uloP, FALSE ) != EC_Normal )
	delete uloP;

    this->setRecordInUseFlag( 0xffff ); 		    // (0004,1410)

    DcmTag lowerOffTag( DCM_LowerLevelDirectoryOffset );
    uloP = new DcmUnsignedLongOffset( lowerOffTag );	    // (0004,1420)
    uloP->put(Uint32(0));
    if ( insert( uloP, FALSE ) != EC_Normal )
	delete uloP;

    this->setRecordType( DirRecordType );		    // (0004,1430)

    DcmTag privRecTag( DCM_PrivateRecordUID );		    // (0004,1432)
    if ( DirRecordType == ERT_Private )
    {
	uiP = new DcmUniqueIdentifier( privRecTag );
	if ( insert( uiP, FALSE ) != EC_Normal )
	    delete uiP;
    }
    else
	delete this->remove( privRecTag );

    /*  // alternative Realisierung
	else if ( this->search( DCM_PrivateRecordUID,
	stack, ESM_fromHere, FALSE )
	== EC_Normal )
	delete this->remove( stack.top() );
	*/

    if ( directFromFile )				    // (0004,1500)
	this->setReferencedFileID( referencedFileID );
    /*---				      // neueste Aenderung des Standards:
      else if ( indirectViaMRDR )
      this->setReferencedFileID( (char*)NULL );
      ---*/
    else
    {
	DcmTag refFileTag( DCM_ReferencedFileID );
	delete this->remove( refFileTag );
    }

    DcmTag mrdrOffTag( DCM_DirectoryRecordOffset );	    // (0004,1504)
    if ( indirectViaMRDR )
    {
	// erzeuge Zeiger-Attribut auf MRDR
	DcmUnsignedLongOffset *uloP = new DcmUnsignedLongOffset( mrdrOffTag );
	uloP->put(Uint32(0));
	uloP->setNextRecord( referencedMRDR );
	insert( uloP, TRUE );
    }
    else
	delete this->remove( mrdrOffTag );

    DcmTag refSOPClassTag( DCM_ReferencedSOPClassUIDInFile );
    DcmTag refSOPInstTag( DCM_ReferencedSOPInstanceUIDInFile );
    DcmTag refFileXferTag( DCM_ReferencedTransferSyntaxUIDInFile );

    if (    DirRecordType != ERT_Mrdr
	    && ( directFromFile || indirectViaMRDR )
	)
    {
	if ( refFile == (DcmFileFormat*)NULL )
	    cerr << "Error: internal Error in"
		" DcmDirectoryRecord::fillElementsAndReadSOP()" << endl;

	uiP = new DcmUniqueIdentifier( refSOPClassTag );    // (0004,1510)
	if ( refFile->search( DCM_SOPClassUID, stack )
	     == EC_Normal )
	{
	    const char *uid = ((DcmUniqueIdentifier*)stack.top())->get();
	    uiP->put( uid );
	}
	else
	{
	    cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP(): "
		"I can't find DCM_SOPClassUID in Dataset ["
		 << fileName << "] !" << endl;
	    l_error = EC_CorruptedData;
	}
	insert( uiP, TRUE );

	uiP = new DcmUniqueIdentifier( refSOPInstTag );     // (0004,1511)
	if (	refFile->search( DCM_SOPInstanceUID, stack )
		== EC_Normal
		|| refFile->search( DCM_MediaStorageSOPInstanceUID, stack )
		== EC_Normal
	    )
	{
	    const char *uid = ((DcmUniqueIdentifier*)stack.top())->get();
	    uiP->put( uid );
	}
	else
	{
	    cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP():"
		" I can't find DCM_SOPInstanceUID neither in Dataset"
		" or MetaInfo of file ["
		 << fileName << "] !" << endl;
	    l_error = EC_CorruptedData;
	}
	insert( uiP, TRUE );

	uiP = new DcmUniqueIdentifier( refFileXferTag );     // (0004,1512)
	if ( refFile->search( DCM_TransferSyntaxUID, stack )
	     == EC_Normal )
	{
	    const char *uid = ((DcmUniqueIdentifier*)stack.top())->get();
	    uiP->put( uid );
	}
	else
	{
	    cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP():"
		" I can't find DCM_TransferSyntaxUID in MetaInfo of file ["
		 << fileName << "] !" << endl;
	    l_error = EC_CorruptedData;
	}
	insert( uiP, TRUE );
    }
    else  // nicht nur dann: if ( DirRecordType == ERT_Mrdr )
    {
	// entferne SOP-UIDs aus Directory Record
	delete this->remove( refSOPClassTag );
	delete this->remove( refSOPInstTag );
	delete this->remove( refFileXferTag );
    }

    // Erzeuge typ-abhaengige Elemente:

    fillTypeElements( DirRecordType, refFile );

    if ( refFile != (DcmFileFormat*)NULL )
	delete refFile;
    if (fileStream != NULL)
	delete fileStream;
    if ( fileName != (char*)NULL )
	delete fileName;
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDirectoryRecord::masterInsertSub(DcmDirectoryRecord* dirRec,
												const unsigned long where)
{
    // Einfuegen ohne Typ-Ueberpruefung
Bdebug((4, "dcdirrec:DcmDirectoryRecord::masterInsertSub(DcmItem*,where=%ld)",
	   where ));

    errorFlag = lowerLevelList->insert( dirRec, where );
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::purgeReferencedFile()
{
Bdebug((2, "dcdirrec:DcmDirectoryRecord::purgeReferencedFile()" ));

    E_Condition l_error = EC_Normal;
    if ( DirRecordType != ERT_root )
    {
	char *localFileName = (char*)NULL;

	// loesche Dateinamen aus directory record
	const char *fileName = this->lookForReferencedFileID();
	if ( fileName != (char*)NULL )
	{
	    localFileName = new char[ strlen( fileName ) + 1 ];
	    buildFileName( fileName, localFileName );
	    this->setReferencedFileID( (char*)NULL );
	}
debug(( 2, "trying to purge file %s from file system", localFileName ));

	if ( localFileName != (char*)NULL )
	{				// Dateiname vorhanden
	    if ( unlink( localFileName ) != 0 )
	    {
		l_error = EC_InvalidStream;
		cerr << "Error: DcmDirectoryRecord::purgeReferencedFile()"
			" cannot purge file ["
		     << localFileName << "] from file system." << endl;
	    }
	    delete localFileName;
	}
	else				// keine referenzierte Datei vorhanden
	    ;  // do nothing
    }
    else
	l_error = EC_IllegalCall;
Edebug(());

    return l_error;
}


// ********************************
// ******** public methods ********
// ********************************


DcmEVR DcmDirectoryRecord::ident() const
{
    return EVR_dirRecord;
}


// ********************************


void DcmDirectoryRecord::print(const int level )
{
    int i;
    char *type = DRTypeNames[ DirRecordType ];
    char *info = new char[ strlen(type) + 50 ];

    sprintf( info, "\"Directory Record\" %s #=%ld ", type, (long)card() );
    DcmObject::printInfoLine( level, info );
    delete info;
    const char* refFile = this->getReferencedFileName();
    for ( i=1; i<level; i++)
	cout << "    ";
    cout << "#    offset= $" << this->getFileOffset();
    if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
	cout << "   referenced MRDR=$"
	     << referencedMRDR->getFileOffset();
    if ( DirRecordType == ERT_Mrdr )
	cout << "   No of references="
	     << numberOfReferences;
    if ( refFile != (char*)NULL )
	cout << "   refFileID=\"" << refFile << "\"";
    cout << endl;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
	    dO->print( level + 1 );
	} while ( elementList->seek( ELP_next ) );
    }
    if ( lowerLevelList->card() > 0 )
	lowerLevelList->print( level + 1 );
    DcmTag delimItemTag( DCM_ItemDelimitationItem );
    if ( Length == DCM_UndefinedLength )
	DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"ItemDelimitationItem\"" );
    else
	DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"ItemDelimitationItem for re-encoding\"" );
}


// ********************************


E_Condition DcmDirectoryRecord::read(DcmStream & inStream,
									 const E_TransferSyntax xfer,
									 const E_GrpLenEncoding gltype,
									 const Uint32 maxReadLength)
{
	if (fTransferState == ERW_notInitialized)
		errorFlag = EC_IllegalCall;
	else
	{

		if(fTransferState != ERW_ready)
			errorFlag = DcmItem::read(inStream, xfer, gltype, maxReadLength);

		if (fTransferState == ERW_ready &&
			DirRecordType == ERT_Private)     // minimiert mehrfaches Auswerten
		{    
			DirRecordType = this->lookForRecordType();
			if ( DirRecordType == ERT_Mrdr )
				numberOfReferences = this->lookForNumberOfReferences();
		}
	}
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::write(DcmStream & outStream,
									  const E_TransferSyntax oxfer,
									  const E_EncodingType enctype,
									  const E_GrpLenEncoding gltype )
{
	if (fTransferState == ERW_notInitialized)
		errorFlag = EC_IllegalCall;
	else
	{
		if (fTransferState == ERW_init )
		{
			this->setRecordType( DirRecordType );
		}
		errorFlag = DcmItem::write(outStream, oxfer, enctype, gltype);
	}
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::verify(const BOOL autocorrect )
{
    E_Condition err1, err2;
    errorFlag = EC_Normal;
    if ( autocorrect == TRUE && DirRecordType != ERT_root )
	errorFlag = fillElementsAndReadSOP( this->getReferencedFileName() );

    err1 = DcmItem::verify( autocorrect );
    err2 = lowerLevelList->verify( autocorrect );
    if (    errorFlag == EC_Normal
	 && (	 err1 != EC_Normal
	      || err2 != EC_Normal
	    )
       )
	errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::search( const DcmTag &tag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
Bdebug((5, "dcdirrec:DcmDirectoryRecord::search(tag=(%4.4x,%4.4x),Stack&(%ld),"
	   "mode=%d,sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
	   mode, searchIntoSub ));
debug(( 5, "local Info: Tag=(%4.4x,%4.4x) \"%s\" p=%p",
	   getGTag(), getETag(), DcmVR(getVR()).getVRName(), this ));
debug(( 5, "switching to method DcmItem::search()" ));

    E_Condition l_error = DcmItem::search( tag,
					   resultStack,
					   mode,
					   searchIntoSub );
    if ( l_error != EC_Normal )
    {
debug(( 5, "no element found: err(%d). now searching in lowerLevelList",
	   l_error ));

	if ( mode != ESM_afterStackTop || resultStack.top() == this )
	    resultStack.push( lowerLevelList );
	l_error = lowerLevelList->search( tag,
					  resultStack,
					  mode,
					  searchIntoSub );
	if ( l_error != EC_Normal )
	    resultStack.pop();
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDirectoryRecord::search( const DcmTagKey &xtag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
Bdebug((5, "dcdirrec:DcmDirectoryRecord::search(xtag=(%x,%x),Stack&,mode=%d,sub=%d)",
	   xtag.getGroup(), xtag.getElement(), mode, searchIntoSub ));

    DcmTag tag( xtag );
    E_Condition l_error = DcmDirectoryRecord::search( tag,
						      resultStack,
						      mode,
						      searchIntoSub );
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDirectoryRecord::searchErrors( DcmStack &resultStack )
{
    E_Condition err1, err2, l_error = EC_Normal;
    err1 = DcmItem::searchErrors( resultStack );
    err2 = lowerLevelList->searchErrors( resultStack );
    if ( err1 != EC_Normal || err2 != EC_Normal )
	l_error = EC_CorruptedData;
    return l_error;
}


// ********************************
// ********************************


E_DirRecType DcmDirectoryRecord::getRecordType()
{
    return DirRecordType;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::getReferencedMRDR()
{
    return referencedMRDR;
}


// ********************************


E_Condition DcmDirectoryRecord::assignToSOPFile( const char *referencedFileID )
{
Bdebug((2, "dcdirrec:DcmDirectoryRecord::assignToSOPFile(char*)" ));

    errorFlag = EC_Normal;

    if ( DirRecordType != ERT_root )
    {
debug(( 2, "old Referenced File ID was %s", this->getReferencedFileName() ));
debug(( 2, "new Referenced File ID is  %s", referencedFileID ));

	// aktualisiere geg. den alten Referenz-Zaehler
	if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
	    referencedMRDR->decreaseRefNum();
	referencedMRDR = (DcmDirectoryRecord*)NULL;

	errorFlag = this->fillElementsAndReadSOP( referencedFileID );
    }
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::assignToMRDR( DcmDirectoryRecord *mrdr )
{
Bdebug((2, "dcdirrec:DcmDirectoryRecord::assignToMRDR(*mrdr=%p)", mrdr ));

    errorFlag = EC_Normal;
    if (   DirRecordType != ERT_root
	&& mrdr != (DcmDirectoryRecord*)NULL   // neuer MRDR vorhanden
	&& mrdr != referencedMRDR	       // alter MRDR != neuer MRDR
       )
    {
debug(( 2, "old Referenced File ID was %s", this->getReferencedFileName() ));
debug(( 2, "new Referenced File ID is  %s", mrdr->lookForReferencedFileID() ));

	// setze internen Zeiger auf mrdr und aktualisiere geg. den alten Wert
	if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
	    referencedMRDR->decreaseRefNum();
	referencedMRDR = mrdr;

	// erhoehe Referenz-Zaehler in referencedMRDR um einen
	referencedMRDR->increaseRefNum();

	// setze Laenge der Referenced File ID auf Null und fuelle Datenelemente
	errorFlag = this->fillElementsAndReadSOP( (char*)NULL );
    }
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return errorFlag;
}


// ********************************
// ********************************


unsigned long DcmDirectoryRecord::cardSub()
{
    return lowerLevelList->card();
}


// ********************************


E_Condition DcmDirectoryRecord::insertSub( DcmDirectoryRecord* dirRec,
					   const unsigned long where )
{
Bdebug((3, "dcdirrec:DcmDirectoryRecord::insertSub(DcmItem*,where=%ld)",
	   where ));

    if ( dirRec != (DcmDirectoryRecord*)NULL )
    {
	if (	checkHierarchy( DirRecordType, dirRec->DirRecordType )
	     == EC_Normal )
	    errorFlag = lowerLevelList->insert( dirRec, where );
	else
	{
	    errorFlag = EC_IllegalCall;
debug(( 1, "dcdirrec: ( %s -> %s ) hierarchy not allowed.",
	   DRTypeNames[ this->getRecordType() ],
	   DRTypeNames[ dirRec->getRecordType() ] ));

	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::getSub(const unsigned long num )
{
    DcmDirectoryRecord *retRec = (DcmDirectoryRecord*)
						lowerLevelList->getItem( num );
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::removeSub(const unsigned long num )
{
    DcmDirectoryRecord *retRec = (DcmDirectoryRecord*)
						lowerLevelList->remove( num );
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::removeSub( DcmDirectoryRecord* dirRec )
{
    DcmDirectoryRecord *retRec = (DcmDirectoryRecord*)
					      lowerLevelList->remove( dirRec );
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


E_Condition DcmDirectoryRecord::deleteSubAndPurgeFile(const unsigned long num )
{
Bdebug((2, "dcdirrec:DcmDirectoryRecord::deleteSubAndPurgeFile(num=%d)",
	   num ));

    DcmDirectoryRecord *subDirRec = (DcmDirectoryRecord*)
						lowerLevelList->remove( num );
    errorFlag = lowerLevelList->error();
    if ( subDirRec != (DcmDirectoryRecord*)NULL )
    {
	DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

	if ( localSubRefMRDR != (DcmDirectoryRecord*)NULL )
	{				    // Datei wird indirekt referenziert
	    localSubRefMRDR->decreaseRefNum();
	}
	else				    // Datei direkt entfernen
	    errorFlag = subDirRec->purgeReferencedFile();
debug(( 2, "now purging lower records:" ));

	while ( subDirRec->cardSub() > 0 )  // alle sub sub records loeschen
	    subDirRec->deleteSubAndPurgeFile((const unsigned long)(0));
	delete subDirRec;		    // loesche sub directory record
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::deleteSubAndPurgeFile( DcmDirectoryRecord* dirRec )
{
Bdebug((2, "dcdirrec:DcmDirectoryRecord::deleteSubAndPurgeFile(*dirRec)" ));

    DcmDirectoryRecord *subDirRec = (DcmDirectoryRecord*)
					      lowerLevelList->remove( dirRec );
    errorFlag = lowerLevelList->error();
    if ( subDirRec != (DcmDirectoryRecord*)NULL )
    {
	DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

	if ( localSubRefMRDR != (DcmDirectoryRecord*)NULL )
	{				    // Datei wird indirekt referenziert
	    localSubRefMRDR->decreaseRefNum();
	}
	else				    // Datei direkt entfernen
	    errorFlag = subDirRec->purgeReferencedFile();
debug(( 2, "now purging lower records:" ));

	while ( subDirRec->cardSub() > 0 )  // alle sub sub records loeschen
	    subDirRec->deleteSubAndPurgeFile((const unsigned long)(0));
	delete subDirRec;		    // loesche sub directory record
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::clearSub()
{
Bdebug((3, "dcdirrec:DcmDirectoryRecord::clear()"));

    errorFlag = lowerLevelList->clear();
Edebug(());

    return errorFlag;
}


// ********************************

/* alt:

    virtual E_Condition 	searchSub( DcmTag &tag, 		    // in
					   DcmStack &resultStack,	    // inout
					   E_SearchMode mode = ESM_fromHere,// in
					   BOOL searchIntoSub = TRUE );     // in
    virtual E_Condition 	searchSub( DcmTagKey xtag,			    // in
					   DcmStack &resultStack,	    // inout
					   E_SearchMode mode = ESM_fromHere,// in
					   BOOL searchIntoSub = TRUE );     // in


E_Condition DcmDirectoryRecord::searchSub( DcmTag &tag,
					   DcmStack &resultStack,
					   E_SearchMode mode,
					   BOOL searchIntoSub )
{
Bdebug((5, "dcdirrec:DcmDirectoryRecord::searchSub(tag=(%4.4x,%4.4x),Stack&,mode=%d,sub=%d)",
	   tag.getGTag(), tag.getETag(), mode, searchIntoSub ));
debug(( 5, "local Info: Tag=(%4.4x,%4.4x) \"%s\" ",
	   getGTag(), getETag(), DcmVR(getVR()).getVRName() ));

    errorFlag = lowerLevelList->search( tag, resultStack, mode, searchIntoSub );
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::searchSub( DcmTagKey xtag,
					   DcmStack &resultStack,
					   E_SearchMode mode,
					   BOOL searchIntoSub )
{
Bdebug((5, "dcdirrec:DcmDirectoryRecord::searchSub(xtag=%d,Stack&,mode=%d,sub=%d)",
	   xtag, mode, searchIntoSub ));

    DcmTag tag( xtag );
    E_Condition l_error = searchSub( tag, resultStack, mode, searchIntoSub );
Edebug(());

    return l_error;
}


// ********************************

*/

