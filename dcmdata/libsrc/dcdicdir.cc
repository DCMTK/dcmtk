/*
**
** Author: Gerd Ehlers      04.06.94 -- Created
**                 Andreas Barth        30.11.95 -- Modified for new stream classes
** 
**
** Module: dcdicdir.cc
**
** Purpose:
** Implementation of class DcmDicomDir
**
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-07-15 15:51:49 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdicdir.cc,v $
** CVS/RCS Revision:    $Revision: 1.21 $
** Status:              $State: Exp $
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
#include <unix.h>       /* for unlink() under Metrowerks C++ (Macintosh) */
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
  : errorFlag(EC_Normal),
    dicomDirFileName(NULL),
    modified(OFFalse),
    mustCreateNewDir(OFFalse),
    DirFile(NULL),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    dicomDirFileName = new char[ strlen( DEFAULT_DICOMDIR_NAME ) + 1 ];
    strcpy( dicomDirFileName, DEFAULT_DICOMDIR_NAME );
    DcmFileStream inStream(dicomDirFileName, DCM_ReadMode);
    if (inStream.GetError() != EC_Normal)
    {
        mustCreateNewDir = OFTrue;
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
    RootRec = new DcmDirectoryRecord( ERT_root,NULL, NULL);
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
}


// ********************************


DcmDicomDir::DcmDicomDir(const char *fileName, const char *fileSetID)
  : errorFlag(EC_Normal),
    dicomDirFileName(NULL),
    modified(OFFalse),
    mustCreateNewDir(OFFalse),
    DirFile(NULL),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    if ( fileName == (char*)NULL || *fileName == '\0' )
        fileName = DEFAULT_DICOMDIR_NAME;
    dicomDirFileName = new char[ strlen( fileName ) + 1 ];
    strcpy( dicomDirFileName, fileName );
    DcmFileStream inStream(dicomDirFileName, DCM_ReadMode);
    if (inStream.GetError() != EC_Normal)
    {
        debug(3, ( "dcdicdir:DcmDicomDir() creating new DicomDir [%s]",
                dicomDirFileName ));
        mustCreateNewDir = OFTrue;
        DirFile = new DcmFileFormat();
    }
    else
    {
        debug(5, (  "dcdicdir:DcmDicomDir() reading DicomDir [%s]",
                dicomDirFileName ));
        DirFile = new DcmFileFormat();
        DirFile->transferInit();
        DirFile->read(inStream);
        DirFile->transferEnd();
    }

    createNewElements( fileSetID );   // erzeugt Daten-Elemente, die noch fehlen
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, NULL);
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
}


// ********************************


DcmDicomDir::DcmDicomDir( const DcmDicomDir & /*newDir*/ )
  : errorFlag(EC_IllegalCall),
    dicomDirFileName(NULL),
    modified(OFFalse),
    mustCreateNewDir(OFTrue),
    DirFile(NULL),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    DirFile = new DcmFileFormat();
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, NULL );
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );
    cerr << "Warning: DcmDicomDir: wrong use of Copy-Constructor" << endl;
}


// ********************************


DcmDicomDir::~DcmDicomDir()
{
    if (modified)
                this->write();

    delete DirFile;
    delete dicomDirFileName;
    delete RootRec;
    delete MRDRSeq;
}


// ********************************

/*
   Erzeugt notwendige Datenelemente. Wird nur in den Konstruktoren aufgerufen.
*/

E_Condition DcmDicomDir::createNewElements( const char* fileSetID )
{
    E_Condition l_error = EC_Normal;
    DcmUnsignedLongOffset *uloP;
    DcmUnsignedShort *usP;
    DcmCodeString *csP;
    DcmDataset &dset = this->getDataset();    // existiert auf jeden Fall

    DcmTag fileIDTag( DCM_FileSetID );
    csP = new DcmCodeString( fileIDTag );                  // (0004,1130)
    if ( fileSetID != (char*)NULL && *fileSetID != '\0' )
        csP->putString( fileSetID );
    if ( dset.insert( csP, OFFalse ) != EC_Normal )
        delete csP;

    // not created or inserted:                            // (0004,1141)
                                                           // (0004,1142)

    DcmTag firstRecTag( DCM_RootDirectoryFirstRecord );
    uloP = new DcmUnsignedLongOffset( firstRecTag );       // (0004,1200)
    uloP->putUint32(Uint32(0));
    if ( dset.insert( uloP, OFFalse ) != EC_Normal )
        delete uloP;

    DcmTag lastRecTag( DCM_RootDirectoryLastRecord );
    uloP = new DcmUnsignedLongOffset( lastRecTag );        // (0004,1202)
    uloP->putUint32(Uint32(0));
    if ( dset.insert( uloP, OFFalse ) != EC_Normal )
        delete uloP;

    DcmTag fileConsTag( DCM_FileSetConsistencyFlag );
    usP = new DcmUnsignedShort( fileConsTag );             // (0004,1212)
    usP->putUint16(Uint16(0x0000));
    dset.insert( usP, OFTrue );
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
                      stack, ESM_fromHere, OFFalse ) == EC_Normal )
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
        dset.insert( localDirRecSeq, OFTrue );
    }
    return *localDirRecSeq;   // muss existieren, sonst Speichermangel
}


// ********************************


DcmUnsignedLongOffset* DcmDicomDir::lookForOffsetElem( DcmObject *obj,
                                                       const DcmTagKey &offsetTag )
{
    DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)NULL;
    if ( obj != (DcmObject*)NULL )
    {
        DcmStack stack;
        if ( obj->search( offsetTag, stack, ESM_fromHere, OFFalse )
             == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_up )
            {
                offElem = (DcmUnsignedLongOffset*)stack.top();
#ifdef DEBUG
Uint32 l_uint = 0;
offElem->getUint32(l_uint);
debug(4, ( "DcmDicomDir::lookForOffsetElem() Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p l=%p",
           offElem->getGTag(), offElem->getETag(),
           l_uint, offElem, 
           offElem->getNextRecord() ));
#endif
            }
        }
    }
    return offElem;
}


// ********************************


E_Condition DcmDicomDir::resolveGivenOffsets( DcmObject *startPoint,
                                              ItemOffset *itOffsets,
                                              const unsigned long numOffsets,
                                              const DcmTagKey &offsetTag )
{
  E_Condition l_error = EC_Normal;
  if ( startPoint != (DcmObject*)NULL )
    {
      DcmStack stack;
      Uint32 offset;
      while ( startPoint->search( offsetTag, stack,
                                  ESM_afterStackTop, OFTrue ) == EC_Normal )
        {
          if ( stack.top()->ident() != EVR_up )
            continue;
          DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)stack.top();
          for (unsigned long i = 0; i < numOffsets; i++ )
            {
              l_error = offElem->getUint32(offset);
              if (offset == itOffsets[ i ].fileOffset )
                {
debug(3, ( "DcmDicomDir::resolveGivenOffset() Offset-Element(0x%4.4hx,0x%4.4hx) with offset 0x%8.8lx found",
        offElem->getGTag(), offElem->getETag(), offset));

                  offElem->setNextRecord( itOffsets[ i ].item );
                  break;
                }
            }
        }
    }

  return l_error;
}


// ********************************


E_Condition DcmDicomDir::resolveAllOffsets( DcmDataset &dset )   // inout
{
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
debug(3, ( "DcmDicomDir::resolveAllOffsets() Item-Offset[%d]=0x%8.8lx", i, filePos ));

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

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::linkMRDRtoRecord( DcmDirectoryRecord *dRec )
{
    E_Condition l_error = EC_Normal;

    if ( dRec != (DcmDirectoryRecord*)NULL )
    {
        // trage referenzierten MRDR in protected attribute ein:
        // geht nur, weil DcmDicomDir friend class von DcmDirectoryRecord ist
        dRec->referencedMRDR = dRec->lookForReferencedMRDR();
    }

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::moveRecordToTree( DcmDirectoryRecord *startRec,
                                           DcmSequenceOfItems &fromDirSQ,
                                           DcmDirectoryRecord *toRecord )
{
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

debug(2,( "DcmDicomDir::moveRecordToTree() Record(0x%4.4hx,0x%4.4hx) p=%p has lower=%p and next=%p Record",
           startRec->getGTag(), startRec->getETag(),
           startRec, lowerRec, nextRec ));

        linkMRDRtoRecord( startRec );

        // benutze protected method zum Einfuegen ohne Typ-Ueberpuefung:
        if ( toRecord->masterInsertSub( startRec ) == EC_Normal )
        {                                         // geht nur, weil friend class
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

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::moveMRDRbetweenSQs( DcmSequenceOfItems &fromSQ,
                                             DcmSequenceOfItems &toSQ )
{
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
            toSQ.insert( dRec );
            fromSQ.remove( j );
        }
        else
            j++;
    }

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::convertLinearToTree()
{
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

    return l_error;
}


// ********************************
// ********************************

Uint32 DcmDicomDir::lengthUntilSQ(DcmDataset &dset,
                                  E_TransferSyntax oxfer,
                                  E_EncodingType enctype )
{
    Uint32 templen = 0L;
    unsigned long num = dset.card();
    for (unsigned long i = 0; i < num; i++ )
    {
        DcmObject *dO = dset.getElement( i );

        DcmXfer xf(oxfer);
        templen += xf.sizeofTagHeader(dO->getVR());

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

        if ( dO->getVR() == EVR_SQ  && enctype == EET_UndefinedLength )
            templen += 8;           // fuer ItemDelimitationItem

    }
debug(4, ( "DcmDicomDir::lengthUntilSQ() Length of Dataset until SQ=%ld", templen ));

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

        templen += 8;               // fuer Tag und Length

        if ( enctype == EET_UndefinedLength )
            templen += 8;           // fuer ItemDelimitationItem
    }
    return templen;
}


// ********************************


E_Condition DcmDicomDir::convertGivenPointer( DcmObject *startPoint,
                                              ItemOffset *itOffsets,
                                              const unsigned long numOffsets,
                                              const DcmTagKey &offsetTag )
{
    E_Condition l_error = EC_Normal;
    if ( startPoint != (DcmObject*)NULL )
    {
        DcmStack stack;
        while ( startPoint->search( offsetTag, stack,
                                    ESM_afterStackTop, OFTrue ) == EC_Normal )
        {
            if ( stack.top()->ident() != EVR_up )
                continue;
            DcmUnsignedLongOffset *offElem = (DcmUnsignedLongOffset*)stack.top();

            for (unsigned long i = 0; i < numOffsets; i++ )
            {
                if ( offElem->getNextRecord() == itOffsets[i].item )
                {
                    offElem->putUint32( itOffsets[i].fileOffset );
                    break;
                }
            }
        }
    }

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::convertAllPointer( DcmDataset &dset,          // inout
                                            Uint32 beginOfDataSet,     // in
                                            E_TransferSyntax oxfer,    // in
                                            E_EncodingType enctype )   // in
{
    E_Condition l_error = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );

    Uint32 offs_Item1 =   beginOfDataSet
                         + lengthUntilSQ( dset, oxfer, enctype );
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

    return l_error;
}


// ********************************


E_Condition DcmDicomDir::copyRecordPtrToSQ( DcmDirectoryRecord *record,
                                            DcmSequenceOfItems &toDirSQ,
                                            DcmDirectoryRecord **firstRec,
                                            DcmDirectoryRecord **lastRec )
{
    DcmDirectoryRecord *nextRec = (DcmDirectoryRecord*)NULL;
    DcmDirectoryRecord *lastReturnItem = (DcmDirectoryRecord*)NULL;

    if ( record != (DcmDirectoryRecord*)NULL )
    {
        unsigned long lastIndex = record->cardSub();
        for (unsigned long i = lastIndex; i > 0; i-- )
        {
            debug(3, ( "DcmDicomDir::copyRecordPtrToSQ() testing subRecord no %ld of %ld:", i, lastIndex ));

            DcmDirectoryRecord *subRecord = record->getSub( i-1 );

            if ( subRecord != (DcmDirectoryRecord*)NULL )
            {
                DcmUnsignedLongOffset *uloP;
                if ( i == lastIndex )
                    lastReturnItem = subRecord;         // letztes Item merken
                                                        // nextPointer anpassen
                DcmTag nextRecTag( DCM_NextDirectoryRecordOffset );
                uloP = new DcmUnsignedLongOffset( nextRecTag );
                uloP->putUint32(Uint32(0));
                uloP->setNextRecord( nextRec );
                subRecord->insert( uloP, OFTrue );
#ifdef DEBUG
Uint32 l_uint = 0;
uloP->getUint32(l_uint);
debug(2, ( "DcmDicomDir::copyRecordPtrToSQ() Next Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p next=%p",
           uloP->getGTag(), uloP->getETag(),
           l_uint, uloP, nextRec ));
#endif
                copyRecordPtrToSQ( subRecord, toDirSQ, firstRec, lastRec );

                                                        // lowerPointer anpassen
                DcmTag lowerRefTag( DCM_LowerLevelDirectoryOffset );
                uloP = new DcmUnsignedLongOffset( lowerRefTag );
                uloP->putUint32(Uint32(0));
                uloP->setNextRecord( *firstRec );
                subRecord->insert( uloP, OFTrue );
#ifdef DEBUG
uloP->getUint32(l_uint);
debug(2, ( "DcmDicomDir::copyRecordPtrToSQ() Lower Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p lower=%p",
           uloP->getGTag(), uloP->getETag(),
           l_uint, uloP, *firstRec ));
#endif

                /* insert a begining */
                toDirSQ.prepend( subRecord );

                nextRec = subRecord;
            }
        }  // for ( i ...
    }
    if ( lastRec != (DcmDirectoryRecord**)NULL )
        *lastRec = lastReturnItem;
    // zeigt auf ersten Record der untergeordneten Ebene
    if ( firstRec != (DcmDirectoryRecord**)NULL )
        *firstRec = nextRec;
    return EC_Normal;
}


// ********************************


E_Condition DcmDicomDir::convertTreeToLinear(Uint32 beginOfDataSet,
                                             E_TransferSyntax oxfer,
                                             E_EncodingType enctype,
                                             E_GrpLenEncoding glenc,
                                             DcmSequenceOfItems &unresRecs )
{
    E_Condition l_error = EC_Normal;
    DcmDataset &dset = this->getDataset();    // existiert auf jeden Fall
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );

    // Kopiere Items, auf die kein Zeiger existiert, in temporaere Liste
    unsigned long numUnresItems = localDirRecSeq.card();
    for (unsigned long i = numUnresItems; i > 0; i-- )
    {
debug(2, ( "DcmDicomDir::convertTreeToLinear() copy pointer of unresolved Record no %ld of %ld to unresRecs-SQ:",
           i, numUnresItems ));

        unresRecs.insert( localDirRecSeq.getItem(i-1), 0 );
    }
debug(5, ( "DcmDicomDir::convertTreeToLinear() copied %d pointer of unresolvedRecords", numUnresItems ));

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
debug(2, ( "DcmDicomDir::convertTreeToLinear() copy pointer of MRDR no %ld of %ld to localDirRecSeq:",
           j, numUnresItems ));

        localDirRecSeq.insert( this->getMRDRSequence().getItem(j-1), 0 );
    }

    // compute group lengths before computing byte offsets
    dset.computeGroupLengthAndPadding(glenc, EPD_noChange, oxfer, enctype);

    // convert maximum twice
    if (    convertAllPointer( dset, beginOfDataSet, oxfer, enctype )
         == EC_InvalidVR )
        if (    convertAllPointer( dset, beginOfDataSet, oxfer, enctype )
             == EC_InvalidVR )
        {
            cerr << "ERROR: dcdicdir: there are some Offsets incorrect"
                    " in file " << dicomDirFileName << endl;
            l_error = EC_CorruptedData;
        }
    return l_error;
}


// ********************************


E_Condition DcmDicomDir::insertMediaSOPUID( DcmMetaInfo &metaInfo )  // inout
{
    E_Condition l_error = EC_Normal;
    DcmTag medSOPClassTag( DCM_MediaStorageSOPClassUID );
    DcmUniqueIdentifier *mediaStorageSOPClassUID =
                                  new DcmUniqueIdentifier( medSOPClassTag );
    const char* valueUID = UID_BasicDirectoryStorageSOPClass;
    mediaStorageSOPClassUID->putString( valueUID );
    metaInfo.insert( mediaStorageSOPClassUID, OFTrue );
    return l_error;
}


// ********************************


void DcmDicomDir::print(ostream & out, const OFBool showFullData,
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
debug(2, ( "DcmDicomDir::recurseMatchFile() Record p=%p with matching filename [%s] found.",
           subRecord, subName ));

                retRec = subRecord;
                break;
            }
            else
                retRec = recurseMatchFile( subRecord, filename );
        }
    }
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::searchMatchFile( DcmSequenceOfItems& recSeq,
                                                  char *filename )
{
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
debug(2, ( "DcmDicomDir::searchMatchFile() Record p=%p with matching filename [%s] found.",
           record, subName ));

                retRec = record;
                break;
            }
        }
    }

    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchFilename( char *filename )
{
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
Cdebug(2, retRec==NULL, ("DcmDicomDir::matchFilename() No Record with matching filename [%s] found.",
                         filename ));
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchOrCreateMRDR( char *filename )
{
    DcmDirectoryRecord* newMRDR = (DcmDirectoryRecord*)NULL;
    DcmDirectoryRecord* matchRec = this->matchFilename( filename );
    if ( matchRec != (DcmDirectoryRecord*)NULL )
    {
        if ( matchRec->getRecordType() == ERT_Mrdr )
            newMRDR = matchRec;
        else if ( matchRec->getRecordType() != ERT_root )
        {
            newMRDR = new DcmDirectoryRecord( ERT_Mrdr, filename, NULL);
            if ( matchRec->assignToMRDR( newMRDR ) != EC_IllegalCall )
                this->getMRDRSequence().insert( newMRDR );
            else
            {
                delete newMRDR;
                newMRDR = (DcmDirectoryRecord*)NULL;
                cerr << "Error: Internal error, can't Create MRDR." << endl;
            }
Cdebug(1, newMRDR!=NULL, ("DcmDicomDir::matchOrCreateMRDR() New MRDR p=%p with matching filename [%s] created,"
                          " original Record p=%p with same filename modified.",
                          newMRDR, filename, matchRec ));

            modified = OFTrue;
        }
    }
Cdebug(1, newMRDR==NULL, ("DcmDicomDir::matchOrCreateMRDR() No MRDR with matching filename [%s] found.",
                          filename ));

    return newMRDR;
}


// ********************************
// ********************************


E_Condition DcmDicomDir::write(const E_TransferSyntax oxfer,
                               const E_EncodingType enctype,
                               const E_GrpLenEncoding glenc)
{
    if ( oxfer != DICOMDIR_DEFAULT_TRANSFERSYNTAX )
        cerr << "Error: DcmDicomDir::write(): wrong TransferSyntax used"
             << " - only LittleEndianExplicit allowed!"
             << endl;
    errorFlag = EC_Normal;
    E_TransferSyntax outxfer = DICOMDIR_DEFAULT_TRANSFERSYNTAX;
    /* find the path of the dicomdir to be created */
    char* sepPos = strrchr(dicomDirFileName, PATH_SEPARATOR);
    char* dicomdirPath = NULL;
    if (sepPos != NULL) {
        dicomdirPath = new char[sepPos - dicomDirFileName + 2];
        strncpy(dicomdirPath, dicomDirFileName, sepPos-dicomDirFileName);
    }
#if defined(HAVE_MKTEMP)
    char *newname = new char[ strlen( TEMPNAME_TEMPLATE ) + 1 ];
    strcpy( newname, TEMPNAME_TEMPLATE );
    mktemp( newname );
#else
    /* DANGER - just use a non-unique name as it is - DANGER */
    char *newname = new char[ strlen( TEMPNAME_TEMPLATE ) + 1 ];
    strcpy( newname, TEMPNAME_TEMPLATE );
#endif
    /* prepend dicomdirPath if possible */
    if (dicomdirPath != NULL) {
        char* oldname = newname;
        newname = new char[strlen(dicomdirPath) + 1 + strlen(oldname) + 1];
        sprintf(newname, "%s%c%s", dicomdirPath, PATH_SEPARATOR, oldname);
        delete oldname;
    }
    debug(1, ( "DcmDicomDir::write() use tempory filename \"%s\"", newname ));

    DcmDataset &dset = this->getDataset();       // existiert auf jeden Fall
    // existiert daher auch:
    DcmMetaInfo &metainfo = *(this->getDirFileFormat().getMetaInfo());
    DcmSequenceOfItems &localDirRecSeq = this->getDirRecSeq( dset );
    DcmTag unresSeqTag( DCM_DirectoryRecordSequence );
    DcmSequenceOfItems localUnresRecs( unresSeqTag );

    // fuege Media Stored SOP Class UID in MetaInfo ein
    insertMediaSOPUID( metainfo );

    this->getDirFileFormat().validateMetaInfo( outxfer );
    DcmFileStream outStream(newname, DCM_WriteMode);
    if (outStream.Fail()) {
        cerr << "ERROR: cannot create DICOMDIR temporary file: " 
             << newname << endl;
    }

    metainfo.transferInit();
    metainfo.write(outStream, META_HEADER_DEFAULT_TRANSFERSYNTAX, enctype);
    metainfo.transferEnd();

    Uint32 beginOfDataset = outStream.Tell();
    Uint32 bODset = beginOfDataset;

    // in schreibbares Format umwandeln
    errorFlag = convertTreeToLinear(bODset, outxfer, 
                                    enctype, glenc, localUnresRecs);

    dset.transferInit();
    // do not calculate GroupLength and Padding twice!
    dset.write(outStream, outxfer, enctype, EGL_noChange);
    dset.transferEnd();

    outStream.Close();

    char* backupname = NULL;
    if ( !mustCreateNewDir )
    {
#ifndef DICOMDIR_WITHOUT_BACKUP
        backupname = new char[ 1 + strlen( dicomDirFileName )
                                   + strlen( DICOMDIR_BACKUP_SUFFIX ) ];
        strcpy( backupname, dicomDirFileName );

#ifndef HAVE_LONG_FILE_NAMES
        char *suffix = strrchr( backupname, '.' );
        if ( suffix )
            *suffix = '\0';
#endif

        strcat( backupname, DICOMDIR_BACKUP_SUFFIX );
        unlink( backupname );
        if (errorFlag == EC_Normal) {
            if (rename(dicomDirFileName, backupname) != 0) {
                errorFlag = EC_InvalidStream;
            }
        }
#else
        if ( unlink( dicomDirFileName ) != 0 )
            errorFlag = EC_InvalidStream;
#endif
    }
    if (errorFlag == EC_Normal &&
        rename( newname, dicomDirFileName ) != 0)
        errorFlag = EC_InvalidStream;
    delete newname;
    modified = OFFalse;

    if (errorFlag == EC_Normal && backupname != NULL) {
        /* remove backup */
        unlink(backupname);
        delete backupname;
    }

    // entferne alle Records aus der Sequence localDirRecSeq
    while ( localDirRecSeq.card() > 0 )
        localDirRecSeq.remove((unsigned long)(0));

    // verschiebe Records, auf die kein Zeiger existiert, zurueck
    while ( localUnresRecs.card() > 0 )
    {
        DcmItem *unresRecord = 
            localUnresRecs.remove((unsigned long)(0));
        localDirRecSeq.insert( unresRecord );
    }
    return errorFlag;
}


// ********************************
// ********************************


E_Condition DcmDicomDir::countMRDRRefs( DcmDirectoryRecord *startRec,
                                        ItemOffset *refCounter,
                                        const unsigned long numCounters )
{
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
                        ++refCounter[ j ].fileOffset;       // Reference counter
                        break;
                    }
debug(3, ( "DcmDicomDir::countMRDRRefs() MRDR p=%p found, which is %ld times referenced and %ld times"
           " counted.", refMRDR, refMRDR->numberOfReferences, j ));

            }
            countMRDRRefs( subRecord, refCounter, numCounters );
        }
    }
    return l_error;
}


// ********************************


E_Condition DcmDicomDir::checkMRDRRefCounter( DcmDirectoryRecord *startRec,
                                              ItemOffset *refCounter,
                                              const unsigned long numCounters )
{
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
                        ++refCounter[ j ].fileOffset;       // Reference counter
                        break;
                    }
debug(3, ( "DcmDicomDir::checkMRDRRefCounter() MRDR p=%p found, which is %ld times referenced and %ld times"
           " counted.", refMRDR, refMRDR->numberOfReferences, j ));

            }
            E_Condition err1 = checkMRDRRefCounter( subRecord,
                                                    refCounter,
                                                    numCounters );
            if ( l_error == EC_Normal && err1 != EC_Normal )
                l_error = err1;         // der zuerst aufgetretene Fehler zaehlt
        }
    }
    return l_error;
}


// ********************************

/*
   Strategie fuer verify (mit autocorrect==OFTrue):
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

E_Condition DcmDicomDir::verify( OFBool autocorrect )
{
    errorFlag = EC_Normal;
    if ( autocorrect == OFTrue )
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
//    maxMRDRs = i;            // passe Tabellengroesse an reale Werte an

    // zaehle fuer jeden MRDR die Anzahl der auf ihn erfolgten Verweise:
    countMRDRRefs( &this->getRootRecord(), refCounter, maxMRDRs );

    // ueberpruefe die Korrektheit der gespeicherten Referenz-Zaehler:
    E_Condition err3 = EC_Normal;
    for (unsigned long k = 0; k < maxMRDRs; k++ )
    {
        DcmDirectoryRecord *refMRDR = (DcmDirectoryRecord*)refCounter[k].item;
        Uint32 refNum = refMRDR->lookForNumberOfReferences();   // friend
        if ( refCounter[k].fileOffset != refNum )
        {
debug(1, ( "DcmDicomDir::verify() Error: Refcounter of MRDR p=%p has incorrect value=%ld (must be"
           " %ld)", refMRDR, refNum, refCounter[k].fileOffset ));
Cdebug(1, refCounter[k].fileOffset==refMRDR->numberOfReferences,
          ( "but internal record class value numberOfReferences is correct" ));

            if ( autocorrect )             // korrigiere Referenzzaehler, friend
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
    E_Condition err2 = this->getRootRecord().verify( OFFalse );
    if (    errorFlag == EC_Normal
         && (    err1 != EC_Normal
              || err2 != EC_Normal
              || err3 != EC_Normal
            )
       )
        errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcdicdir.cc,v $
** Revision 1.21  1998-07-15 15:51:49  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.20  1998/06/29 12:17:59  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.19  1998/01/27 10:51:38  meichel
** Removed some unused variables, meaningless const modifiers
**   and unreached statements.
**
** Revision 1.18  1997/09/11 15:02:05  hewett
** Changed DcmDicomDir constructor to take const char* arguments.
**
** Revision 1.17  1997/07/21 08:03:27  andreas
** - DcmDirectoryRecord can be build with a referenced Name and a source
**   filename. These name now can differ (lower case - upper case
**   characters).
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.16  1997/07/03 15:09:55  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.15  1997/05/28 12:42:40  andreas
** - deleted unused variable lastDirIndex
**
** Revision 1.14  1997/05/16 08:31:27  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.13  1997/05/09 13:15:44  hewett
** Fixed bug related to renaming of temporary files accross file system
** boundaries (the rename() system call fails).  The temporary file used
** diring creation of a DICOMDIR is now created in the same file system
** directory as the DICOMDIR.
**
** Revision 1.12  1997/05/06 09:32:10  hewett
** Temporary DICOMDIR files are now located in the tmp directory using
** the tmpnam() function (if available).  Previously, temporary files
** were being created in the current working directory.
**
** Revision 1.11  1997/04/24 12:10:47  hewett
** Fixed DICOMDIR generation bug affecting inclusion of Group Length
** attributes (file offsets were not being computed correctly).
** Fixed DICOMDIR generation bug affecting ordering of
** patient/study/series/image records (item insertion into a sequence
** did produce the expected ordering).
** Fixed DICOMDIR generation bug affecting the use of Unknown VR
** attributes (the file offsets were not being computed correctly).
**
** Revision 1.10  1997/04/18 08:17:14  andreas
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
