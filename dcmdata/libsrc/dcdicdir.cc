/*
**
** Author: Gerd Ehlers	    04.06.94 -- Created
**		   Andreas Barth	30.11.95 -- Modified for new stream classes
** 
**
** Module: dcdicdir.cc
**
** Purpose:
** Implementation of class DcmDicomDir
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-04-18 08:17:14 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdicdir.cc,v $
** CVS/RCS Revision:	$Revision: 1.10 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_LIBC_H
#include <libc.h>
#endif

#if defined(HAVE_MKTEMP) && !defined(HAVE_PROTOTYPE_MKTEMP)
extern "C" {
char * mktemp(char *);
}
#endif

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream.h>

#include "dcdefine.h"
#include "dcdicdir.h"
#include "dctk.h"
#include "dcuid.h"
#include "dcdirrec.h"
#include "dcxfer.h"
#include "dcdebug.h"
#include "dcdeftag.h"


// ********************************


DcmDicomDir::DcmDicomDir()
{
    Bdebug((5, "dcdicdir:DcmDicomDir::DcmDicomDir()" ));

    errorFlag = EC_Normal;
    modified = FALSE;
    mustCreateNewDir = FALSE;
    dicomDirFileName = new char[ strlen( DEFAULT_DICOMDIR_NAME ) + 1 ];
    strcpy( dicomDirFileName, DEFAULT_DICOMDIR_NAME );
    DcmFileStream inStream(dicomDirFileName, DCM_ReadMode);
    if (inStream.GetError() != EC_Normal)
    {
	mustCreateNewDir = TRUE;
	DirFile = new DcmFileFormat();
    }
    else
    {
	DirFile = new DcmFileFormat();
	DirFile->transferInit();
	DirFile->read(inStream);
	DirFile->transferEnd();
    }
    createNewElements( "" );      // erzeugt Daten-Elemente, die noch fehlen
    RootRec = new DcmDirectoryRecord( ERT_root, (char*)NULL );
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
    Edebug(());

}


// ********************************


DcmDicomDir::DcmDicomDir(char *fileName,
			 char *fileSetID)
{
    Bdebug((5, "DcmDicomDir::DcmDicomDir(char*,char*)" ));

    errorFlag = EC_Normal;
    modified = FALSE;
    mustCreateNewDir = FALSE;
    if ( fileName == (char*)NULL || *fileName == '\0' )
	fileName = DEFAULT_DICOMDIR_NAME;
    dicomDirFileName = new char[ strlen( fileName ) + 1 ];
    strcpy( dicomDirFileName, fileName );
    DcmFileStream inStream(dicomDirFileName, DCM_ReadMode);
    if (inStream.GetError() != EC_Normal)
    {
	debug(( 3, "dcdicdir:DcmDicomDir() creating new DicomDir [%s]",
		dicomDirFileName ));
	mustCreateNewDir = TRUE;
	DirFile = new DcmFileFormat();
    }
    else
    {
	debug(( 5, "dcdicdir:DcmDicomDir() reading DicomDir [%s]",
		dicomDirFileName ));
	DirFile = new DcmFileFormat();
	DirFile->transferInit();
	DirFile->read(inStream);
	DirFile->transferEnd();
    }

    createNewElements( fileSetID );   // erzeugt Daten-Elemente, die noch fehlen
    RootRec = new DcmDirectoryRecord( ERT_root, (char*)NULL );
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
    Edebug(());

}


// ********************************


DcmDicomDir::DcmDicomDir( const DcmDicomDir & /*newDir*/ )
{
    Bdebug((5, "dcdicdir:DcmDicomDir::DcmDicomDir(DcmDicomDir&)" ));

    errorFlag = EC_IllegalCall;
    modified = FALSE;
    mustCreateNewDir = FALSE;
    dicomDirFileName = (char*)NULL;
    mustCreateNewDir = TRUE;
    DirFile = new DcmFileFormat();
    RootRec = new DcmDirectoryRecord( ERT_root, (char*)NULL );
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );
    cerr << "Warning: DcmDicomDir: wrong use of Copy-Constructor" << endl;
    Edebug(());

}


// ********************************


DcmDicomDir::~DcmDicomDir()
{
Bdebug((5, "dcdicdir:DcmDicomDir::~DcmDicomDir()" ));

    if (modified)
		this->write();

    delete DirFile;
    delete dicomDirFileName;
    delete RootRec;
    delete MRDRSeq;

Edebug(());

}


// ********************************

/*
   Erzeugt notwendige Datenelemente. Wird nur in den Konstruktoren aufgerufen.
*/

E_Condition DcmDicomDir::createNewElements( char *fileSetID )
{
Bdebug((3, "dcdicdir:DcmDicomDir::createNewElements(char*)" ));

    E_Condition l_error = EC_Normal;
    DcmUnsignedLongOffset *uloP;
    DcmUnsignedShort *usP;
    DcmCodeString *csP;
    DcmDataset &dset = this->getDataset();    // existiert auf jeden Fall

    DcmTag fileIDTag( DCM_FileSetID );
    csP = new DcmCodeString( fileIDTag );		   // (0004,1130)
    if ( fileSetID != (char*)NULL && *fileSetID != '\0' )
	csP->putString( fileSetID );
    if ( dset.insert( csP, FALSE ) != EC_Normal )
	delete csP;

    // not created or inserted: 			   // (0004,1141)
							   // (0004,1142)

    DcmTag firstRecTag( DCM_RootDirectoryFirstRecord );
    uloP = new DcmUnsignedLongOffset( firstRecTag );	   // (0004,1200)
    uloP->putUint32(Uint32(0));
    if ( dset.insert( uloP, FALSE ) != EC_Normal )
	delete uloP;

    DcmTag lastRecTag( DCM_RootDirectoryLastRecord );
    uloP = new DcmUnsignedLongOffset( lastRecTag );	   // (0004,1202)
    uloP->putUint32(Uint32(0));
    if ( dset.insert( uloP, FALSE ) != EC_Normal )
	delete uloP;

    DcmTag fileConsTag( DCM_FileSetConsistencyFlag );
    usP = new DcmUnsignedShort( fileConsTag );		   // (0004,1212)
    usP->putUint16(Uint16(0x0000));
    dset.insert( usP, TRUE );

Edebug(());

    return l_error;
}


// ********************************


DcmDataset& DcmDicomDir::getDataset()
{
    if ( DirFile == (DcmFileFormat*)NULL )
	DirFile = new DcmFileFormat();
    DcmDataset *localDataset = DirFile->getDataset();

    if ( localDataset == (DcmDataset*)NULL )
    {
	errorFlag = EC_CorruptedData;
	cerr << "Error: DcmDicomDir::getDataset(): missing Dataset in Dicom"
		" Dir File. Must create new Dicom Dir File." << endl;
	if ( DirFile != (DcmFileFormat*)NULL )
	    delete DirFile;
	DirFile = new DcmFileFormat();
	localDataset = DirFile->getDataset();
    }
    return *localDataset;     // muss existieren, sonst Fehler in DcmFileFormat
}


// ********************************


DcmSequenceOfItems& DcmDicomDir::getDirRecSeq( DcmDataset &dset )
{
    DcmSequenceOfItems *localDirRecSeq = (DcmSequenceOfItems*)NULL;
    DcmStack stack;
    if ( dset.search( DCM_DirectoryRecordSequence,
		      stack, ESM_fromHere, FALSE ) == EC_Normal )
    {
	if ( stack.top()->ident() == EVR_SQ )
	    localDirRecSeq = (DcmSequenceOfItems*)stack.top();
    }

    if ( localDirRecSeq == (DcmSequenceOfItems*)NULL )
    {
	errorFlag = EC_CorruptedData;
	if ( !mustCreateNewDir )
	    cerr << "Warning: DcmDicomDir::getDirRecSeq(): missing Directory"
		    " Record Sequence. Must create new one." << endl;
	DcmTag dirSeqTag( DCM_DirectoryRecordSequence );    // (0004,1220)
	localDirRecSeq = new DcmSequenceOfItems( dirSeqTag );
	dset.insert( localDirRecSeq, TRUE );
    }
    return *localDirRecSeq;   // muss existieren, sonst Speichermangel
}


// ********************************


DcmUnsignedLongOffset* DcmDicomDir::lookForOffsetElem( DcmObject *obj,
						       const DcmTagKey &offsetTag )
{
Bdebug((4, "dcdicdir:DcmDicomDir::lookForOffsetElem(obj*,tag=(%x,%x))",
	   offsetTag.getGroup(), offsetTag.getElement() ));

    DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)NULL;
    if ( obj != (DcmObject*)NULL )
    {
	DcmStack stack;
	if ( obj->search( offsetTag, stack, ESM_fromHere, FALSE )
	     == EC_Normal )
	{
	    if ( stack.top()->ident() == EVR_up )
	    {
		offElem = (DcmUnsignedLongOffset*)stack.top();
#ifdef DEBUG
Uint32 uint = 0;
offElem->getUint32(uint);
debug(( 4, "Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p l=%p",
	   offElem->getGTag(), offElem->getETag(),
	   uint, offElem, 
	   offElem->getNextRecord() ));
#endif
	    }
	}
    }
Edebug(());

    return offElem;
}


// ********************************


E_Condition DcmDicomDir::resolveGivenOffsets( DcmObject *startPoint,
					      ItemOffset *itOffsets,
					      const unsigned long numOffsets,
					      const DcmTagKey &offsetTag )
{
  Bdebug((3, "dcdicdir:DcmDicomDir::resolveGivenOffsets(DcmObject*,ItemOffset*,"
	  "num=%ld,offsetTag=(%x,%x))", numOffsets, offsetTag.getGroup(),
	  offsetTag.getElement()));

  E_Condition l_error = EC_Normal;
  if ( startPoint != (DcmObject*)NULL )
    {
      DcmStack stack;
      Uint32 offset;
      while ( startPoint->search( offsetTag, stack,
				  ESM_afterStackTop, TRUE ) == EC_Normal )
	{
	  if ( stack.top()->ident() != EVR_up )
	    continue;
	  DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)stack.top();
	  for (unsigned long i = 0; i < numOffsets; i++ )
	    {
	      l_error = offElem->getUint32(offset);
	      if (offset == itOffsets[ i ].fileOffset )
		{
debug(( 3, "Offset-Element(0x%4.4hx,0x%4.4hx) with offset 0x%8.8lx found",
	offElem->getGTag(), offElem->getETag(), offset));

		  offElem->setNextRecord( itOffsets[ i ].item );
		  break;
		}
	    }
	}
    }
  Edebug(());

  return l_error;
}


// ********************************


E_Condition DcmDicomDir::resolveAllOffsets( DcmDataset &dset )	 // inout
{
Bdebug((3, "dcdicdir:DcmDicomDir::resolveAllOffsets(DcmDataset&)" ));

    E_Condition l_error = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );
    unsigned long maxitems = localDirRecSeq.card();
    ItemOffset *itOffsets = new ItemOffset[ maxitems + 1 ];

    for (unsigned long i = 0; i < maxitems; i++ )
    {
	DcmDirectoryRecord *rec;
	rec = (DcmDirectoryRecord*)localDirRecSeq.getItem( i );
	long filePos = rec->getFileOffset();
	itOffsets[ i ].item = rec;
	itOffsets[ i ].fileOffset = (Uint32)filePos;
debug(( 3, "Item-Offset[%d]=0x%8.8lx", i, filePos ));

    }
    resolveGivenOffsets( &dset, itOffsets, maxitems,
	DCM_RootDirectoryFirstRecord );
    resolveGivenOffsets( &dset, itOffsets, maxitems,
	DCM_RootDirectoryLastRecord );

    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
	DCM_NextDirectoryRecordOffset );
    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
	DCM_LowerLevelDirectoryOffset );
    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
	DCM_DirectoryRecordOffset );

    delete itOffsets;
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::linkMRDRtoRecord( DcmDirectoryRecord *dRec )
{
Bdebug((3, "dcdicdir:DcmDicomDir::linkMRDRtoRecord(DcmDirectoryRecord*)" ));

    E_Condition l_error = EC_Normal;

    if ( dRec != (DcmDirectoryRecord*)NULL )
    {
	// trage referenzierten MRDR in protected attribute ein:
	// geht nur, weil DcmDicomDir friend class von DcmDirectoryRecord ist
	dRec->referencedMRDR = dRec->lookForReferencedMRDR();
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::moveRecordToTree( DcmDirectoryRecord *startRec,
					   DcmSequenceOfItems &fromDirSQ,
					   DcmDirectoryRecord *toRecord )
{
Bdebug((2, "dcdicdir:DcmDicomDir::moveRecordToTree(*startRec,&fromDirSQ,"
	   "*toRecord)" ));

    E_Condition l_error = EC_Normal;

    if (
	 toRecord  == (DcmDirectoryRecord*)NULL
       )
	l_error = EC_IllegalCall;
    else if ( startRec != (DcmDirectoryRecord*)NULL )
    {
	DcmDirectoryRecord *lowerRec = (DcmDirectoryRecord*)NULL;
	DcmDirectoryRecord *nextRec = (DcmDirectoryRecord*)NULL;

	DcmUnsignedLongOffset *offElem;
	offElem = lookForOffsetElem( startRec,
			       DCM_LowerLevelDirectoryOffset );
	if ( offElem != (DcmUnsignedLongOffset*)NULL )
	    lowerRec = (DcmDirectoryRecord*)offElem->getNextRecord();
	offElem = lookForOffsetElem( startRec,
			       DCM_NextDirectoryRecordOffset );
	if ( offElem != (DcmUnsignedLongOffset*)NULL )
	    nextRec = (DcmDirectoryRecord*)offElem->getNextRecord();

debug(( 2, "Record(0x%4.4hx,0x%4.4hx) p=%p has lower=%p and next=%p Record",
	   startRec->getGTag(), startRec->getETag(),
	   startRec, lowerRec, nextRec ));

	linkMRDRtoRecord( startRec );

	// benutze protected method zum Einfuegen ohne Typ-Ueberpuefung:
	if ( toRecord->masterInsertSub( startRec ) == EC_Normal )
	{					  // geht nur, weil friend class
	     DcmItem *dit = fromDirSQ.remove( startRec );
	     if ( dit == (DcmItem*)NULL )
		cerr << "Error: DcmDicomDir::moveRecordToTree() DirRecord is"
			" part of unknown Sequence" << endl;
	}
	else
	    cerr << "Error: DcmDicomDir::moveRecordToTree() cannot insert"
		    " DirRecord (=NULL?)" << endl;

	moveRecordToTree( lowerRec, fromDirSQ, startRec );
	moveRecordToTree( nextRec, fromDirSQ, toRecord );
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::moveMRDRbetweenSQs( DcmSequenceOfItems &fromSQ,
					     DcmSequenceOfItems &toSQ )
{
Bdebug((3, "dcdicdir:DcmDicomDir::moveMRDRbetweenSQs(&fromSQ,&toSQ)" ));

    E_Condition l_error = EC_Normal;

    unsigned long num = fromSQ.card();
    for (unsigned long i = 0, j = 0; i < num; i++ )
    {
	DcmDirectoryRecord *dRec;
	dRec = (DcmDirectoryRecord*)fromSQ.getItem( j );
	if (   dRec != (DcmDirectoryRecord*)NULL
	    && dRec->getRecordType() == ERT_Mrdr
	   )
	{
debug(( 2, "move MRDR from fromSQ[%d] to toSQ", j ));

	    toSQ.insert( dRec );
	    fromSQ.remove( j );
	}
	else
	    j++;
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::convertLinearToTree()
{
Bdebug((1, "dcdicdir:DcmDicomDir::convertLinearToTree()" ));

    DcmDataset &dset = this->getDataset();    // existiert auf jeden Fall
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );
    E_Condition l_error = resolveAllOffsets( dset );

    // Suche ersten Directory Record:
    DcmDirectoryRecord *firstRootRecord = (DcmDirectoryRecord*)NULL;
    DcmUnsignedLongOffset *offElem = lookForOffsetElem( &dset,
		   DCM_RootDirectoryFirstRecord );
    if ( offElem != (DcmUnsignedLongOffset*)NULL )
		firstRootRecord = (DcmDirectoryRecord*)offElem->getNextRecord();

    // bilde aus linearer Anordnung der Records eine Baumstruktur:
    moveRecordToTree( firstRootRecord, localDirRecSeq, &this->getRootRecord() );

    // Verschiebe MRDRs von localDirRecSeq nach globaler MRDRSeq:
    moveMRDRbetweenSQs( localDirRecSeq, this->getMRDRSequence() );

    // loese MRDR-Referenzen bei allen uebrig gebliebenen Items auf
    for (unsigned long i = localDirRecSeq.card(); i > 0; i-- )
	linkMRDRtoRecord( (DcmDirectoryRecord*)localDirRecSeq.getItem(i-1) );

Edebug(());

    return l_error;
}


// ********************************
// ********************************

/*
   siehe auch DcmItem::calcHeaderLength()
*/

Uint32 DcmDicomDir::lengthUntilSQ(DcmDataset &dset,
								  E_TransferSyntax oxfer,
								  E_EncodingType enctype )
{
Bdebug((4, "dcdicdir:DcmDicomDir::lengthUntilSQ(&dset,oxfer=%d,enctype=%d)",
	   oxfer, enctype ));

    Uint32 templen = 0L;
    unsigned long num = dset.card();
    for (unsigned long i = 0; i < num; i++ )
    {
	DcmObject *dO = dset.getElement( i );

	// Die folgende Methode ist leider protected:
	// templen += DcmItem::calcHeaderLength( dO->getVR(), oxfer );

	// deshalb ist deren Rumpf hier her kopiert worden
	DcmXfer xferSyn( oxfer );
	if ( xferSyn.isExplicitVR() )
	{
	    switch ( dO->getVR() )
	    {
		case EVR_ox :
		case EVR_OB :
		case EVR_OW :
		case EVR_SQ :
		    templen += 12;  // fuer Tag, Length, VR und reserved
		    break;
		default:
		    templen += 8;   // fuer Tag, Length und VR
		    break;
	    }
	}
	else
	    templen += 8;	    // fuer Tag und Length

	if ( dO->getTag().getXTag() == DCM_DirectoryRecordSequence )
	    break;

	Uint32 sublength = dO->getLength( oxfer, enctype );
	templen += sublength;
	if ( sublength==DCM_UndefinedLength )
	{
	    DcmVR subvr( dO->getVR() );
	    cerr << "Warning:DcmDicomDir::lengthUntilSQ() subelem \""
		 << subvr.getVRName() << "\" has undefined Length" << endl;
	}

	if (	( dO->getVR() == EVR_SQ )
	     && ( enctype == EET_UndefinedLength )
	   )
	    templen += 8;	    // fuer ItemDelimitationItem
debug(( 4, "subLength of \"%s\" is %d, actual sum of Length is %d",
	   DcmVR(dO->getVR()).getVRName(), sublength, templen ));

    }
debug(( 4, "Length of Dataset until SQ=%ld", templen ));
Edebug(());

    return templen;
}


// ********************************


Uint32 DcmDicomDir::lengthOfRecord( DcmItem *item,
				     E_TransferSyntax oxfer,
				     E_EncodingType enctype )
{
    Uint32 templen = 0;
    if ( item != (DcmItem*)NULL )
    {
	templen = item->getLength( oxfer, enctype );

	templen += 8;		    // fuer Tag und Length

	if ( enctype == EET_UndefinedLength )
	    templen += 8;	    // fuer ItemDelimitationItem
    }
    return templen;
}


// ********************************


E_Condition DcmDicomDir::convertGivenPointer( DcmObject *startPoint,
					      ItemOffset *itOffsets,
					      const unsigned long numOffsets,
					      const DcmTagKey &offsetTag )
{
Bdebug((3, "dcdicdir:DcmDicomDir::convertGivenPointer(*startPoint,*ItemOffset,"
	"numOffsets=%ld,tag=%d)", numOffsets, offsetTag.getGroup(), 
	offsetTag.getElement() ));

    E_Condition l_error = EC_Normal;
    if ( startPoint != (DcmObject*)NULL )
    {
	DcmStack stack;
	while ( startPoint->search( offsetTag, stack,
				    ESM_afterStackTop, TRUE ) == EC_Normal )
	{
	    if ( stack.top()->ident() != EVR_up )
		continue;
	    DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)stack.top();

	    for (unsigned long i = 0; i < numOffsets; i++ )
	    {
		if ( offElem->getNextRecord() == itOffsets[i].item )
		{
		    offElem->putUint32( itOffsets[i].fileOffset );

debug(( 2, "Offset-Element(0x%4.4hx,0x%4.4hx) with offset 0x%8.8lx updated",
	   offElem->getGTag(), offElem->getETag(), itOffsets[i].fileOffset ));

		    break;
		}
	    }
	}
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::convertAllPointer( DcmDataset &dset,	       // inout
					    Uint32 beginOfDataSet,     // in
					    E_TransferSyntax oxfer,    // in
					    E_EncodingType enctype )   // in
{
Bdebug((3, "dcdicdir:DcmDicomDir::convertAllPointer(&dset,beginOfDataSet=%ld,"
	   "oxfer=%d,enctype=%d)", beginOfDataSet, oxfer, enctype ));

    E_Condition l_error = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );

    Uint32 offs_Item1 =   beginOfDataSet
			 + lengthUntilSQ( dset, oxfer, enctype );
debug(( 2, "Length: offs_Item1(%d) = beginOfDataSet(%d) + lengthUntilSQ()",
	   offs_Item1, beginOfDataSet ));

    unsigned long num = localDirRecSeq.card();
    ItemOffset *itOffsets = new ItemOffset[ num ];

    Uint32 item_pos = offs_Item1;
    for (unsigned long i = 0; i < num; i++ )
    {
	DcmDirectoryRecord *rec;
	rec = (DcmDirectoryRecord*)localDirRecSeq.getItem( i );
	rec->setFileOffset( item_pos );
	itOffsets[ i ].item = rec;
	itOffsets[ i ].fileOffset = item_pos;
debug(( 2, "Item-Offset[%d]=0x%8.8lx  p=%p", i, item_pos, rec ));

	item_pos = lengthOfRecord( rec, oxfer, enctype ) + item_pos;
    }

    E_Condition e1, e2, e3, e4, e5;
    e1 = convertGivenPointer( &dset, itOffsets, num,
	       DCM_RootDirectoryFirstRecord );
    e2 = convertGivenPointer( &dset, itOffsets, num,
	       DCM_RootDirectoryLastRecord );

    e3 = convertGivenPointer( &localDirRecSeq, itOffsets, num,
	       DCM_NextDirectoryRecordOffset );
    e4 = convertGivenPointer( &localDirRecSeq, itOffsets, num,
	       DCM_LowerLevelDirectoryOffset );
    e5 = convertGivenPointer( &localDirRecSeq, itOffsets, num,
	       DCM_DirectoryRecordOffset );
    if (    e1 == EC_InvalidVR
	 || e2 == EC_InvalidVR
	 || e3 == EC_InvalidVR
	 || e4 == EC_InvalidVR
	 || e5 == EC_InvalidVR
       )
	l_error = EC_InvalidVR;
    delete itOffsets;
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::copyRecordPtrToSQ( DcmDirectoryRecord *record,
					    DcmSequenceOfItems &toDirSQ,
					    DcmDirectoryRecord **firstRec,
					    DcmDirectoryRecord **lastRec )
{
Bdebug((2, "dcdicdir:DcmDicomDir::copyRecordPtrToSQ(*record,&toDirSQ,"
	   "**firstRec,**lastRec)" ));

    DcmDirectoryRecord *nextRec = (DcmDirectoryRecord*)NULL;
    DcmDirectoryRecord *lastReturnItem = (DcmDirectoryRecord*)NULL;

    if ( record != (DcmDirectoryRecord*)NULL )
    {
	unsigned long lastIndex = record->cardSub();
	unsigned long lastDirIndex = toDirSQ.card();
debug(( 3, "there are %ld subRecords and %ld toDirItems ",
	   lastIndex, lastDirIndex ));

	for (unsigned long i = lastIndex; i > 0; i-- )
	{
debug(( 3, "testing subRecord no %ld of %ld:", i, lastIndex ));

	    DcmDirectoryRecord *subRecord = record->getSub( i-1 );

	    if ( subRecord != (DcmDirectoryRecord*)NULL )
	    {
		DcmUnsignedLongOffset *uloP;
		toDirSQ.insert( subRecord, lastDirIndex );
		if ( i == lastIndex )
		    lastReturnItem = subRecord; 	// letztes Item merken
							// nextPointer anpassen
		DcmTag nextRecTag( DCM_NextDirectoryRecordOffset );
		uloP = new DcmUnsignedLongOffset( nextRecTag );
		uloP->putUint32(Uint32(0));
		uloP->setNextRecord( nextRec );
		subRecord->insert( uloP, TRUE );
#ifdef DEBUG
Uint32 uint = 0;
uint = uloP->getUint32(uint);
debug(( 2, "Next Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p next=%p",
	   uloP->getGTag(), uloP->getETag(),
	   uint, uloP, nextRec ));
#endif
		copyRecordPtrToSQ( subRecord, toDirSQ, firstRec, lastRec );

							// lowerPointer anpassen
		DcmTag lowerRefTag( DCM_LowerLevelDirectoryOffset );
		uloP = new DcmUnsignedLongOffset( lowerRefTag );
		uloP->putUint32(Uint32(0));
		uloP->setNextRecord( *firstRec );
		subRecord->insert( uloP, TRUE );
#ifdef DEBUG
uint = uloP->getUint32(uint);
debug(( 2, "Lower Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p lower=%p",
	   uloP->getGTag(), uloP->getETag(),
	   uint, uloP, *firstRec ));
#endif
		nextRec = subRecord;
	    }
	}  // for ( i ...
    }
    if ( lastRec != (DcmDirectoryRecord**)NULL )
	*lastRec = lastReturnItem;
    // zeigt auf ersten Record der untergeordneten Ebene
    if ( firstRec != (DcmDirectoryRecord**)NULL )
	*firstRec = nextRec;
Edebug(());

    return EC_Normal;
}


// ********************************


E_Condition DcmDicomDir::convertTreeToLinear(Uint32 beginOfDataSet,
					      E_TransferSyntax oxfer,
					      E_EncodingType enctype,
					      DcmSequenceOfItems &unresRecs )
{
Bdebug((1, "dcdicdir:DcmDicomDir::convertTreeToLinear(beginOfDataSet=%ld)",
	   beginOfDataSet  ));

    E_Condition l_error = EC_Normal;
    DcmDataset &dset = this->getDataset();    // existiert auf jeden Fall
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );

    // Kopiere Items, auf die kein Zeiger existiert, in temporaere Liste
    unsigned long numUnresItems = localDirRecSeq.card();
    for (unsigned long i = numUnresItems; i > 0; i-- )
    {
debug(( 2, "copy pointer of unresolved Record no %ld of %ld to unresRecs-SQ:",
	   i, numUnresItems ));

	unresRecs.insert( localDirRecSeq.getItem(i-1), 0 );
    }
debug(( 5, "copied %d pointer of unresolvedRecords", numUnresItems ));

    // Konvertiere Items in die Root Directory Entity zurueck:
    DcmDirectoryRecord *firstRootRecord[1], *lastRootRecord[1];
    copyRecordPtrToSQ( &this->getRootRecord(),
		       localDirRecSeq,
		       firstRootRecord,
		       lastRootRecord );

    // Setze Zeiger auf ersten Directory Record:
    DcmUnsignedLongOffset *offElem = lookForOffsetElem( &dset,
		   DCM_RootDirectoryFirstRecord );
    if ( offElem != (DcmUnsignedLongOffset*)NULL )
	offElem->setNextRecord( *firstRootRecord );

    // Setze Zeiger auf letzen Directory Record:
    offElem = lookForOffsetElem( &dset,
		    DCM_RootDirectoryLastRecord );
    if ( offElem != (DcmUnsignedLongOffset*)NULL )
	offElem->setNextRecord( *lastRootRecord );

    // Kopiere MRDRs in localDirRecSeq
    unsigned long numMRDRItems = this->getMRDRSequence().card();
    for (unsigned long j = numMRDRItems; j > 0; j-- )
    {
debug(( 2, "copy pointer of MRDR no %ld of %ld to localDirRecSeq:",
	   j, numUnresItems ));

	localDirRecSeq.insert( this->getMRDRSequence().getItem(j-1), 0 );
    }

    // konvertiere im Fehlerfall ein zweites Mal:
    if (    convertAllPointer( dset, beginOfDataSet, oxfer, enctype )
	 == EC_InvalidVR )
	if (	convertAllPointer( dset, beginOfDataSet, oxfer, enctype )
	     == EC_InvalidVR )
	{
	    cerr << "ERROR: dcdicdir: there are some Offsets incorrect"
		    " in file " << dicomDirFileName << endl;
	    l_error = EC_CorruptedData;
	}
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::insertMediaSOPUID( DcmMetaInfo &metaInfo )  // inout
{
Bdebug((5, "dcdicdir:DcmDicomDir::insertMediaSOPUID(DcmMetaInfo&)" ));

    E_Condition l_error = EC_Normal;
    DcmTag medSOPClassTag( DCM_MediaStorageSOPClassUID );
    DcmUniqueIdentifier *mediaStorageSOPClassUID =
				  new DcmUniqueIdentifier( medSOPClassTag );
    const char* valueUID = UID_BasicDirectoryStorageSOPClass;
    mediaStorageSOPClassUID->putString( valueUID );
    metaInfo.insert( mediaStorageSOPClassUID, TRUE );

debug(( 2, "MediaStoredSOPClassUID %s in MetaInfo inserted.", valueUID ));
Edebug(());

    return l_error;
}


// ********************************


void DcmDicomDir::print(ostream & out, const BOOL showFullData,
			const int level)
{
    int i;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Dicom DIR" << endl;

    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Meta-Info and General Directory Information" << endl;
    this->getDirFileFormat().print(out, showFullData);

    out << endl;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Item Hierarchy (root Record not shown)" << endl;
    this->getRootRecord().lowerLevelList->print(out, showFullData, 1);  // friend class

    out << endl;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# used Multi Referenced Directory Records" << endl;
    this->getMRDRSequence().print(out, showFullData, 1 );
}


// ********************************


E_Condition DcmDicomDir::error()
{
    return errorFlag;
}


// ********************************


DcmFileFormat& DcmDicomDir::getDirFileFormat()
{
    return *DirFile;
}


// ********************************


DcmDirectoryRecord& DcmDicomDir::getRootRecord()
{
    return *RootRec;
}


// ********************************


DcmSequenceOfItems& DcmDicomDir::getMRDRSequence()
{
    return *MRDRSeq;
}


// ********************************
// ********************************


DcmDirectoryRecord* DcmDicomDir::recurseMatchFile( DcmDirectoryRecord* startRec,
						   char *filename )
{
Bdebug((4, "dcdicdir:DcmDicomDir::recurseMatchFile(*startRec,*filename)" ));

    DcmDirectoryRecord* retRec = (DcmDirectoryRecord*)NULL;
    if (    filename != (char*)NULL
	 && *filename != '\0'
	 && startRec != (DcmDirectoryRecord*)NULL
       )
    {
	unsigned long lastIndex = startRec->cardSub();
	for (unsigned long i = 0; i < lastIndex; i++ )
	{
	    DcmDirectoryRecord *subRecord = startRec->getSub( i );
	    const char* subName = subRecord->lookForReferencedFileID(); // friend

	    if ( subName != (char*)NULL && !strcmp( filename, subName ) )
	    {
debug(( 2, "Record p=%p with matching filename [%s] found.",
	   subRecord, subName ));

		retRec = subRecord;
		break;
	    }
	    else
		retRec = recurseMatchFile( subRecord, filename );
	}
    }
Edebug(());

    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::searchMatchFile( DcmSequenceOfItems& recSeq,
						  char *filename )
{
Bdebug((4, "dcdicdir:DcmDicomDir::searchMatchFile(&recSeq,*filename)" ));

    DcmDirectoryRecord* retRec = (DcmDirectoryRecord*)NULL;
    if ( filename != (char*)NULL && *filename != '\0' )
    {
	unsigned long lastIndex = recSeq.card();
	for (unsigned long i = 0; i < lastIndex; i++ )
	{
	    DcmDirectoryRecord *record;
	    record = (DcmDirectoryRecord*)recSeq.getItem( i );
	    const char* subName = record->lookForReferencedFileID(); // friend

	    if ( subName != (char*)NULL && !strcmp( filename, subName ) )
	    {
debug(( 2, "Record p=%p with matching filename [%s] found.",
	   record, subName ));

		retRec = record;
		break;
	    }
	}
    }
Edebug(());

    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchFilename( char *filename )
{
Bdebug((4, "dcdicdir:DcmDicomDir::matchFilename(*filename)" ));

    DcmDirectoryRecord* retRec = (DcmDirectoryRecord*)NULL;
    if ( filename != (char*)NULL && *filename != '\0' )
    {
	retRec = recurseMatchFile( &this->getRootRecord(), filename );
	if ( retRec == (DcmDirectoryRecord*)NULL )
	{
	    retRec = searchMatchFile( this->getMRDRSequence(), filename );
	    if ( retRec == (DcmDirectoryRecord*)NULL )
	    {
		DcmDataset &dset = this->getDataset();
		retRec = searchMatchFile( this->getDirRecSeq(dset), filename );
	    }
	}
    }
Vdebug((2, retRec==NULL, "No Record with matching filename [%s] found.",
			 filename ));
Edebug(());

    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchOrCreateMRDR( char *filename )
{
Bdebug((4, "dcdicdir:DcmDicomDir::matchOrCreateMRDR(*filename)" ));

    DcmDirectoryRecord* newMRDR = (DcmDirectoryRecord*)NULL;
    DcmDirectoryRecord* matchRec = this->matchFilename( filename );
    if ( matchRec != (DcmDirectoryRecord*)NULL )
    {
	if ( matchRec->getRecordType() == ERT_Mrdr )
	    newMRDR = matchRec;
	else if ( matchRec->getRecordType() != ERT_root )
	{
	    newMRDR = new DcmDirectoryRecord( ERT_Mrdr, filename );
	    if ( matchRec->assignToMRDR( newMRDR ) != EC_IllegalCall )
		this->getMRDRSequence().insert( newMRDR );
	    else
	    {
		delete newMRDR;
		newMRDR = (DcmDirectoryRecord*)NULL;
		cerr << "Error: Internal error, can't Create MRDR." << endl;
	    }
Vdebug((1, newMRDR!=NULL, "New MRDR p=%p with matching filename [%s] created,"
			  " original Record p=%p with same filename modified.",
			  newMRDR, filename, matchRec ));

	    modified = TRUE;
	}
    }
Vdebug((1, newMRDR==NULL, "No MRDR with matching filename [%s] found.",
			  filename ));
Edebug(());

    return newMRDR;
}


// ********************************
// ********************************


E_Condition DcmDicomDir::write(const E_TransferSyntax oxfer,
			       const E_EncodingType enctype,
			       const E_GrpLenEncoding gltype)
{
    Bdebug((1, "DcmDicomDir::write(oxfer=%d,enctype=%d,gltype=%d)",
	    oxfer, enctype, gltype ));

    if ( oxfer != DICOMDIR_DEFAULT_TRANSFERSYNTAX )
	cerr << "Error: DcmDicomDir::write(): wrong TransferSyntax used"
	     << " - only LittleEndianExplicit allowed!"
	     << endl;
    errorFlag = EC_Normal;
    E_TransferSyntax outxfer = DICOMDIR_DEFAULT_TRANSFERSYNTAX;
    char *newname = new char[ strlen( TEMPNAME_TEMPLATE ) + 1 ];
    strcpy( newname, TEMPNAME_TEMPLATE );
#ifdef HAVE_MKTEMP
    mktemp( newname );
#else
    /* DANGER - just use the name as it is - DANGER */
#endif
    debug(( 1, "use tempory filename \"%s\"", newname ));

    DcmDataset &dset = this->getDataset();	 // existiert auf jeden Fall
    // existiert daher auch:
    DcmMetaInfo &metainfo = *(this->getDirFileFormat().getMetaInfo());
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );
    DcmTag unresSeqTag( DCM_DirectoryRecordSequence );
    DcmSequenceOfItems localUnresRecs( unresSeqTag );

    // fuege Media Stored SOP Class UID in MetaInfo ein
    insertMediaSOPUID( metainfo );

    this->getDirFileFormat().validateMetaInfo( outxfer );
    DcmFileStream outStream(newname, DCM_WriteMode);
    metainfo.transferInit();
    metainfo.write(outStream, META_HEADER_DEFAULT_TRANSFERSYNTAX, enctype);
    metainfo.transferEnd();

    Uint32 beginOfDataset = outStream.Tell();
    Uint32 bODset = beginOfDataset;

    // in schreibbares Format umwandeln
    errorFlag = convertTreeToLinear(bODset, outxfer, 
				    enctype, localUnresRecs);

    dset.transferInit();
    dset.write(outStream, outxfer, enctype, gltype);
    dset.transferEnd();

    if ( !mustCreateNewDir )
    {
#ifndef DICOMDIR_WITHOUT_BACKUP
	char *backupname = new char[ 1 + strlen( dicomDirFileName )
				   + strlen( DICOMDIR_BACKUP_SUFFIX ) ];
	strcpy( backupname, dicomDirFileName );

#ifndef HAVE_LONG_FILE_NAMES
	char *suffix = strrchr( backupname, '.' );
	if ( suffix )
	    *suffix = '\0';
#endif

	strcat( backupname, DICOMDIR_BACKUP_SUFFIX );
	unlink( backupname );
	if (errorFlag == EC_Normal && 
	    rename( dicomDirFileName, backupname ) != 0)
	    errorFlag = EC_InvalidStream;
	delete backupname;
#else
	if ( unlink( dicomDirFileName ) != 0 )
	    errorFlag = EC_InvalidStream;
#endif
    }
    if (errorFlag == EC_Normal &&
	rename( newname, dicomDirFileName ) != 0)
	errorFlag = EC_InvalidStream;
    delete newname;
    modified = FALSE;

    // entferne alle Records aus der Sequence localDirRecSeq
    while ( localDirRecSeq.card() > 0 )
	localDirRecSeq.remove((const unsigned long)(0));

    // verschiebe Records, auf die kein Zeiger existiert, zurueck
    while ( localUnresRecs.card() > 0 )
    {
	DcmItem *unresRecord = 
	    localUnresRecs.remove((const unsigned long)(0));
	localDirRecSeq.insert( unresRecord );
    }
    Edebug(());

    return errorFlag;
}


// ********************************
// ********************************


E_Condition DcmDicomDir::countMRDRRefs( DcmDirectoryRecord *startRec,
					ItemOffset *refCounter,
					const unsigned long numCounters )
{
Bdebug((4, "dcdicdir:DcmDicomDir::countMRDRRefs(*startRec,*refCounter,"
	   "numCounters=%ld)", numCounters ));

    E_Condition l_error = EC_Normal;
    if ( refCounter == (ItemOffset*)NULL )
	l_error = EC_IllegalCall;
    else if ( startRec != (DcmDirectoryRecord*)NULL )
    {
	unsigned long lastIndex = startRec->cardSub();
	for (unsigned long i = 0; i < lastIndex; i++ )
	{
	    DcmDirectoryRecord *subRecord = startRec->getSub( i );
	    DcmDirectoryRecord *refMRDR = subRecord->lookForReferencedMRDR();
							    // friend class
	    if ( refMRDR != (DcmDirectoryRecord*)NULL )
	    {
		unsigned long j;
		for ( j = 0; j < numCounters; j++ )
		    if ( refMRDR == refCounter[ j ].item )
		    {
			++refCounter[ j ].fileOffset;	    // Reference counter
			break;
		    }
debug(( 3, "MRDR p=%p found, which is %ld times referenced and %ld times"
	   " counted.", refMRDR, refMRDR->numberOfReferences, j ));

	    }
	    countMRDRRefs( subRecord, refCounter, numCounters );
	}
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::checkMRDRRefCounter( DcmDirectoryRecord *startRec,
					      ItemOffset *refCounter,
					      const unsigned long numCounters )
{
Bdebug((4, "dcdicdir:DcmDicomDir::checkMRDRRefCounter(*startRec,*refCounter,"
	   "numCounters=%ld)", numCounters ));

    E_Condition l_error = EC_Normal;
    if ( refCounter == (ItemOffset*)NULL )
	l_error = EC_IllegalCall;
    else if ( startRec != (DcmDirectoryRecord*)NULL )
    {
	unsigned long lastIndex = startRec->cardSub();
	for (unsigned long i = 0; i < lastIndex; i++ )
	{
	    DcmDirectoryRecord *subRecord = startRec->getSub( i );
	    DcmDirectoryRecord *refMRDR = subRecord->lookForReferencedMRDR();
							    // friend class
	    if ( refMRDR != (DcmDirectoryRecord*)NULL )
	    {
		unsigned long j;
		for ( j = 0; j < numCounters; j++ )
		    if ( refMRDR == refCounter[ j ].item )
		    {
			++refCounter[ j ].fileOffset;	    // Reference counter
			break;
		    }
debug(( 3, "MRDR p=%p found, which is %ld times referenced and %ld times"
	   " counted.", refMRDR, refMRDR->numberOfReferences, j ));

	    }
	    E_Condition err1 = checkMRDRRefCounter( subRecord,
						    refCounter,
						    numCounters );
	    if ( l_error == EC_Normal && err1 != EC_Normal )
		l_error = err1; 	// der zuerst aufgetretene Fehler zaehlt
	}
    }
Edebug(());

    return l_error;
}


// ********************************

/*
   Strategie fuer verify (mit autocorrect==TRUE):
   - lege Tabelle an mit Zeigern auf MRDRs und Referenzzaehlern mit der Groesse
     getDirRecSeq( getDataset() ).card() + getMRDRSequence().card()
   - durchlaufe den Record-Baum und erhoehe bei jedem Auftreten eines
     MRDR-Verweises, den entsprechenden Zaehler in obiger Tabelle
   - setze in allen MRDRs, auf die laut Tabelle kein Verweis existiert, das
     activation flag auf INAKTIV
PENDING:
   - ueberprufe fuer alle inaktiven MRDRs, ob deren referenzierte Dateien
     von keinem anderen Record referenziert werden und loesche dann
     gegebenenfalls die Dateien
   - loesche alle inaktiven MRDRs aus der Sequenz getMRDRSequence()
   - uebertrage alle aktiven MRDRs aus der Sequenz getDirRecSeq( getDataset() )
     in die Sequenz getMRDRSequence()
   - loesche die gesamte Sequenz getDirRecSeq( getDataset() ), unter Inkaufnahme
     eines Datenverlustes: es kann nicht automatisch entschieden werden, an
     welche Position innerhalb der Record-Hierearchie die zu loeschenden
     Records eingefuegt werden muessen
*/

E_Condition DcmDicomDir::verify( BOOL autocorrect )
{
Bdebug((3, "dcdicdir:DcmDicomDir::verify(BOOL autocorrect=%d", autocorrect ));

    errorFlag = EC_Normal;
    if ( autocorrect == TRUE )
    ;
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq(this->getDataset());
    unsigned long maxMRDRs = localDirRecSeq.card() + 
		this->getMRDRSequence().card();
    ItemOffset *refCounter = new ItemOffset[ maxMRDRs ];

    // lege MRDR-Tabelle fuer MRDRs aus der MRDRSeq und aus der DirRecSeq an:
    unsigned long i;
    for ( i = 0; i < this->getMRDRSequence().card(); i++ )
    {
	DcmDirectoryRecord *rec;
	rec = (DcmDirectoryRecord*)this->getMRDRSequence().getItem( i );
	refCounter[i].item = rec;
	refCounter[i].fileOffset = 0L;
    }
    for (unsigned long j = 0; j < localDirRecSeq.card() && i < maxMRDRs; j++ )
    {
	DcmDirectoryRecord *rec;
	rec = (DcmDirectoryRecord*)localDirRecSeq.getItem( j );
	if ( rec->getRecordType() == ERT_Mrdr )
	{
	    refCounter[i].item = rec;
	    refCounter[i].fileOffset = 0L;
	    i++;
	}
    }
//    maxMRDRs = i;	       // passe Tabellengroesse an reale Werte an

    // zaehle fuer jeden MRDR die Anzahl der auf ihn erfolgten Verweise:
    countMRDRRefs( &this->getRootRecord(), refCounter, maxMRDRs );

    // ueberpruefe die Korrektheit der gespeicherten Referenz-Zaehler:
    E_Condition err3 = EC_Normal;
    for (unsigned long k = 0; k < maxMRDRs; k++ )
    {
	DcmDirectoryRecord *refMRDR = (DcmDirectoryRecord*)refCounter[k].item;
	Uint32 refNum = refMRDR->lookForNumberOfReferences();	// friend
	if ( refCounter[k].fileOffset != refNum )
	{
debug(( 1, "Error: Refcounter of MRDR p=%p has incorrect value=%ld (must be"
	   " %ld)", refMRDR, refNum, refCounter[k].fileOffset ));
Vdebug((1, refCounter[k].fileOffset==refMRDR->numberOfReferences,
	   "but internal record class value numberOfReferences is correct" ));

	    if ( autocorrect )		   // korrigiere Referenzzaehler, friend
		refMRDR->setNumberOfReferences( refCounter[k].fileOffset );
	    else
		err3 = EC_CorruptedData;
	}
	// setze inactivation flag fuer MRDRs ohne Referenz
	if ( autocorrect && refCounter[k].fileOffset == 0L )
	    refMRDR->setRecordInUseFlag( 0xffff );
    }

    delete refCounter;
    E_Condition err1 = this->getDirFileFormat().verify( autocorrect );
					      // keine automatische Korrektur:
    E_Condition err2 = this->getRootRecord().verify( FALSE );
    if (    errorFlag == EC_Normal
	 && (	 err1 != EC_Normal
	      || err2 != EC_Normal
	      || err3 != EC_Normal
	    )
       )
	errorFlag = EC_CorruptedData;
Edebug(());

    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcdicdir.cc,v $
** Revision 1.10  1997-04-18 08:17:14  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.9  1997/04/15 16:12:09  hewett
** Modified code which printed a message on stderr when creating a new DICOM
** to use the debug routines instead.
**
** Revision 1.8  1997/03/26 17:03:40  hewett
** Smoothed out some poor indentation.
**
** Revision 1.7  1997/02/06 12:11:20  hewett
** Updated for Macintosh CodeWarrior 11.  Corrected for incompatibilities
** in the timeval structure between unix.h and winsock.h
**
** Revision 1.6  1996/09/27 08:23:07  hewett
** Preliminary support for IBM AIX systems using the IBM C Set++ compiler.
** Corrected mismatch between method definition in .h and .cc files.
**
** Revision 1.5  1996/08/05 08:46:09  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:24  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/23 17:29:24  andreas
** Support for old fashioned make without @^
** Support for machines that have mktemp but do not define it.
**
** Revision 1.2  1996/01/05 13:27:34  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
