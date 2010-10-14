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
 *  Purpose: class DcmDicomDir
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:07 $
 *  CVS/RCS Revision: $Revision: 1.61 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CERRNO
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#if defined(HAVE_MKTEMP) && !defined(HAVE_PROTOTYPE_MKTEMP)
extern "C" {
char * mktemp(char *);
}
#endif

// Solaris 2.5.1 has mkstemp() in libc.a but no prototype
#if defined(HAVE_MKSTEMP) && !defined(HAVE_PROTOTYPE_MKSTEMP)
extern "C" {
int mkstemp(char *);
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

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcmtk/dcmdata/dcistrmf.h"    /* for class DcmInputFileStream */
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */
#include "dcmtk/dcmdata/dcvrui.h"      /* for class DcmUniqueIdentifier */

// ********************************


DcmDicomDir::DcmDicomDir()
  : errorFlag(EC_Normal),
    dicomDirFileName(NULL),
    modified(OFFalse),
    mustCreateNewDir(OFFalse),
    DirFile(new DcmFileFormat()),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    dicomDirFileName = new char[ strlen( DEFAULT_DICOMDIR_NAME ) + 1 ];
    strcpy( dicomDirFileName, DEFAULT_DICOMDIR_NAME );

    OFCondition cond = DirFile->loadFile(dicomDirFileName);
    if (cond.bad())
    {
      delete DirFile; // clean up file format object
      DirFile = new DcmFileFormat();
      mustCreateNewDir = OFTrue;
    }

    createNewElements( "" );      // create missing data elements
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, NULL);
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
    DirFile(new DcmFileFormat()),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    if ( fileName == NULL || *fileName == '\0' )
        fileName = DEFAULT_DICOMDIR_NAME;
    dicomDirFileName = new char[ strlen( fileName ) + 1 ];
    strcpy( dicomDirFileName, fileName );

    OFCondition cond = DirFile->loadFile(dicomDirFileName);
    if (cond.bad())
    {
      delete DirFile;   // clean up file format object
      DirFile = new DcmFileFormat();
      mustCreateNewDir = OFTrue;
    }

    createNewElements( fileSetID );   // create missing data elements
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, NULL);
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
}


// ********************************


/* This copy constructor implementation is untested
 */
DcmDicomDir::DcmDicomDir( const DcmDicomDir & old )
  : errorFlag(old.errorFlag),
    dicomDirFileName(NULL),
    modified(old.modified),
    mustCreateNewDir(old.mustCreateNewDir),
    DirFile(new DcmFileFormat(*old.DirFile)),
    RootRec(new DcmDirectoryRecord(*old.RootRec)),
    MRDRSeq(new DcmSequenceOfItems(*old.MRDRSeq))
{
    dicomDirFileName = new char[ strlen( old.dicomDirFileName ) + 1 ];
    strcpy( dicomDirFileName, old.dicomDirFileName );
}


// ********************************


DcmDicomDir::~DcmDicomDir()
{
    if (modified)
        write();

    delete DirFile;
    delete[] dicomDirFileName;
    delete RootRec;
    delete MRDRSeq;
}


// ********************************

/*
   creates required data elements. Only called by the constructors.
*/

OFCondition DcmDicomDir::createNewElements( const char* fileSetID )
{
    OFCondition l_error = EC_Normal;
    DcmUnsignedLongOffset *uloP;
    DcmUnsignedShort *usP;
    DcmCodeString *csP;
    DcmDataset &dset = getDataset();    // guaranteed to exist

    DcmTag fileIDTag( DCM_FileSetID );
    csP = new DcmCodeString( fileIDTag );                  // (0004,1130)
    if ( fileSetID != NULL && *fileSetID != '\0' )
        csP->putString( fileSetID );
    if ( dset.insert( csP, OFFalse ) != EC_Normal )
        delete csP;

    // not created or inserted:                            // (0004,1141)
                                                           // (0004,1142)

    DcmTag firstRecTag( DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    uloP = new DcmUnsignedLongOffset( firstRecTag );       // (0004,1200)
    uloP->putUint32(Uint32(0));
    if ( dset.insert( uloP, OFFalse ) != EC_Normal )
        delete uloP;

    DcmTag lastRecTag( DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );
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
    if ( DirFile == NULL )
        DirFile = new DcmFileFormat();
    DcmDataset *localDataset = DirFile->getDataset();

    if ( localDataset == NULL )
    {
        errorFlag = EC_CorruptedData;
        DCMDATA_ERROR("DcmDicomDir::getDataset() Missing Dataset in DICOMDIR File. Must create new DICOMDIR file.");
        if ( DirFile != NULL )
            delete DirFile;
        DirFile = new DcmFileFormat();
        localDataset = DirFile->getDataset();
    }
    return *localDataset;     // must exist, otherwise error in DcmFileFormat
}


// ********************************


DcmSequenceOfItems& DcmDicomDir::getDirRecSeq( DcmDataset &dset )
{
    DcmSequenceOfItems *localDirRecSeq = NULL;
    DcmStack stack;
    if ( dset.search( DCM_DirectoryRecordSequence, stack, ESM_fromHere, OFFalse ) == EC_Normal )
    {
        if ( stack.top()->ident() == EVR_SQ )
            localDirRecSeq = OFstatic_cast(DcmSequenceOfItems *, stack.top());
    }

    if ( localDirRecSeq == NULL )
    {
        errorFlag = EC_CorruptedData;
        if ( !mustCreateNewDir )
        {
            DCMDATA_WARN("DcmDicomDir::getDirRecSeq() Missing Directory Record Sequence. Must create new one.");
        }
        DcmTag dirSeqTag( DCM_DirectoryRecordSequence );    // (0004,1220)
        localDirRecSeq = new DcmSequenceOfItems( dirSeqTag );
        dset.insert( localDirRecSeq, OFTrue );
    }
    return *localDirRecSeq;   // must exist, otherwise memory exhausted
}


// ********************************


DcmUnsignedLongOffset* DcmDicomDir::lookForOffsetElem( DcmObject *obj,
                                                       const DcmTagKey &offsetTag )
{
    DcmUnsignedLongOffset *offElem = NULL;
    if ( obj != NULL )
    {
        DcmStack stack;
        if ( obj->search( offsetTag, stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_up )
            {
                offElem = OFstatic_cast(DcmUnsignedLongOffset *, stack.top());
#ifdef DEBUG
                Uint32 l_uint = 0;
                offElem->getUint32(l_uint);
                DCMDATA_TRACE("DcmDicomDir::lookForOffsetElem() Offset Element ("
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                    << STD_NAMESPACE setw(4) << offElem->getGTag() << ","
                    << STD_NAMESPACE setw(4) << offElem->getETag() << ") offs=0x"
                    << STD_NAMESPACE setw(8) << l_uint
                    << " p=" << OFstatic_cast(void *, offElem)
                    << " l=" << offElem->getNextRecord());
#endif
            }
        }
    }
    return offElem;
}


// ********************************


OFCondition DcmDicomDir::resolveGivenOffsets( DcmObject *startPoint,
                                              ItemOffset *itOffsets,
                                              const unsigned long numOffsets,
                                              const DcmTagKey &offsetTag )
{
  OFCondition l_error = EC_Normal;
  if ( startPoint != NULL )
  {
      DcmStack stack;
      Uint32 offset;
      while ( startPoint->search( offsetTag, stack, ESM_afterStackTop, OFTrue ) == EC_Normal )
      {
          if ( stack.top()->ident() != EVR_up )
              continue;
          DcmUnsignedLongOffset *offElem = OFstatic_cast(DcmUnsignedLongOffset *, stack.top());
          for (unsigned long i = 0; i < numOffsets; i++ )
          {
              l_error = offElem->getUint32(offset);
              if (offset == itOffsets[ i ].fileOffset )
              {
                  DCMDATA_DEBUG("DcmDicomDir::resolveGivenOffset() Offset Element ("
                      << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                      << STD_NAMESPACE setw(4) << offElem->getGTag() << ","
                      << STD_NAMESPACE setw(4) << offElem->getETag() << ") with offset 0x"
                      << STD_NAMESPACE setw(8) << offset << " found");

                  offElem->setNextRecord( itOffsets[ i ].item );
                  break;
              }
          }
      }
  }

  return l_error;
}


// ********************************


OFCondition DcmDicomDir::resolveAllOffsets( DcmDataset &dset )   // inout
{
    OFCondition l_error = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );
    unsigned long maxitems = localDirRecSeq.card();
    ItemOffset *itOffsets = new ItemOffset[ maxitems + 1 ];

    for (unsigned long i = 0; i < maxitems; i++ )
    {
        DcmDirectoryRecord *rec;
        rec = OFstatic_cast(DcmDirectoryRecord *, localDirRecSeq.getItem( i ));
        long filePos = rec->getFileOffset();
        itOffsets[ i ].item = rec;
        itOffsets[ i ].fileOffset = OFstatic_cast(Uint32, filePos);
        DCMDATA_DEBUG("DcmDicomDir::resolveAllOffsets() Item Offset [" << i << "] = 0x"
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << filePos);
    }
    resolveGivenOffsets( &dset, itOffsets, maxitems,
        DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    resolveGivenOffsets( &dset, itOffsets, maxitems,
        DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );

    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
        DCM_OffsetOfTheNextDirectoryRecord );
    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
        DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
    resolveGivenOffsets( &localDirRecSeq, itOffsets, maxitems,
        DCM_RETIRED_MRDRDirectoryRecordOffset );

    delete[] itOffsets;

    return l_error;
}


// ********************************


OFCondition DcmDicomDir::linkMRDRtoRecord( DcmDirectoryRecord *dRec )
{
    OFCondition l_error = EC_Normal;

    if ( dRec != NULL )
    {
        // enter referenced MRDR into protected attribute:
        // only works since DcmDicomDir is a friend class of DcmDirectoryRecord
        dRec->referencedMRDR = dRec->lookForReferencedMRDR();
    }

    return l_error;
}


// ********************************


OFCondition DcmDicomDir::moveRecordToTree( DcmDirectoryRecord *startRec,
                                           DcmSequenceOfItems &fromDirSQ,
                                           DcmDirectoryRecord *toRecord )
{
    OFCondition l_error = EC_Normal;

    if (toRecord  == NULL)
        l_error = EC_IllegalCall;
    else if ( startRec != NULL )
    {
        DcmDirectoryRecord *lowerRec = NULL;
        DcmDirectoryRecord *nextRec = NULL;

        DcmUnsignedLongOffset *offElem;
        offElem = lookForOffsetElem( startRec, DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
        if ( offElem != NULL )
            lowerRec = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());
        offElem = lookForOffsetElem( startRec, DCM_OffsetOfTheNextDirectoryRecord );
        if ( offElem != NULL )
            nextRec = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());

        DCMDATA_DEBUG("DcmDicomDir::moveRecordToTree() Record ("
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
            << STD_NAMESPACE setw(4) << startRec->getGTag() << ","
            << STD_NAMESPACE setw(4) << startRec->getGTag()
            << ") p=" << OFstatic_cast(void *, startRec)
            << " has lower=" << OFstatic_cast(void *, lowerRec)
            << " and next=" << OFstatic_cast(void *, nextRec) << " Record");

        linkMRDRtoRecord( startRec );

        // use protected method for insertion without type check:
        if ( toRecord->masterInsertSub( startRec ) == EC_Normal )
        {                                         // only works since friend class
             DcmItem *dit = fromDirSQ.remove( startRec );
             if ( dit == NULL )
             {
                 DCMDATA_ERROR("DcmDicomDir::moveRecordToTree() DirRecord is part of unknown Sequence");
             }
        }
        else
        {
            DCMDATA_ERROR("DcmDicomDir::moveRecordToTree() Cannot insert DirRecord (=NULL?)");
        }
        moveRecordToTree( lowerRec, fromDirSQ, startRec );
        moveRecordToTree( nextRec, fromDirSQ, toRecord );
    }

    return l_error;
}


// ********************************


OFCondition DcmDicomDir::moveMRDRbetweenSQs( DcmSequenceOfItems &fromSQ,
                                             DcmSequenceOfItems &toSQ )
{
    OFCondition l_error = EC_Normal;

    unsigned long num = fromSQ.card();
    for (unsigned long i = 0, j = 0; i < num; i++)
    {
        DcmDirectoryRecord *dRec;
        dRec = OFstatic_cast(DcmDirectoryRecord *, fromSQ.getItem( j ));
        if (dRec != NULL && dRec->getRecordType() == ERT_Mrdr)
        {
            toSQ.insert( dRec );
            fromSQ.remove( j );
        } else
            j++;
    }

    return l_error;
}


// ********************************


OFCondition DcmDicomDir::convertLinearToTree()
{
    DcmDataset &dset = getDataset();    // guaranteed to exist
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );
    OFCondition l_error = resolveAllOffsets( dset );

    // search for first directory record:
    DcmDirectoryRecord *firstRootRecord = NULL;
    DcmUnsignedLongOffset *offElem = lookForOffsetElem( &dset, DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    if ( offElem != NULL )
        firstRootRecord = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());

    // create tree structure from flat record list:
    moveRecordToTree( firstRootRecord, localDirRecSeq, &getRootRecord() );

    // move MRDRs from localDirRecSeq to global MRDRSeq:
    moveMRDRbetweenSQs( localDirRecSeq, getMRDRSequence() );

    // dissolve MRDR references for all remaining items
    for (unsigned long i = localDirRecSeq.card(); i > 0; i-- )
        linkMRDRtoRecord( OFstatic_cast(DcmDirectoryRecord *, localDirRecSeq.getItem(i-1)) );

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
            DCMDATA_WARN("DcmDicomDir::lengthUntilSQ() Sub element \"" << subvr.getVRName() << "\" has undefined Length");
        }

        if ( dO->getVR() == EVR_SQ  && enctype == EET_UndefinedLength )
            templen += 8;           // for ItemDelimitationItem

    }
    DCMDATA_TRACE("DcmDicomDir::lengthUntilSQ() Length of Dataset until SQ=" << templen);

    return templen;
}


// ********************************


Uint32 DcmDicomDir::lengthOfRecord( DcmItem *item,
                                    E_TransferSyntax oxfer,
                                    E_EncodingType enctype )
{
    Uint32 templen = 0;
    if ( item != NULL )
    {
        templen = item->getLength( oxfer, enctype );

        templen += 8;               // for Tag and Length

        if ( enctype == EET_UndefinedLength )
            templen += 8;           // for ItemDelimitationItem
    }
    return templen;
}


// ********************************


OFCondition DcmDicomDir::convertGivenPointer( DcmObject *startPoint,
                                              ItemOffset *itOffsets,
                                              const unsigned long numOffsets,
                                              const DcmTagKey &offsetTag )
{
    OFCondition l_error = EC_Normal;
    if ( startPoint != NULL )
    {
        DcmStack stack;
        while ( startPoint->search( offsetTag, stack, ESM_afterStackTop, OFTrue ) == EC_Normal )
        {
            if ( stack.top()->ident() != EVR_up )
                continue;
            DcmUnsignedLongOffset *offElem = OFstatic_cast(DcmUnsignedLongOffset *, stack.top());

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


OFCondition DcmDicomDir::convertAllPointer( DcmDataset &dset,          // inout
                                            Uint32 beginOfDataSet,     // in
                                            E_TransferSyntax oxfer,    // in
                                            E_EncodingType enctype )   // in
{
    OFCondition l_error = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );

    Uint32 offs_Item1 =  beginOfDataSet + lengthUntilSQ( dset, oxfer, enctype );
    unsigned long num = localDirRecSeq.card();
    ItemOffset *itOffsets = new ItemOffset[ num ];

    Uint32 item_pos = offs_Item1;
    for (unsigned long i = 0; i < num; i++ )
    {
        DcmDirectoryRecord *rec;
        rec = OFstatic_cast(DcmDirectoryRecord *, localDirRecSeq.getItem( i ));
        rec->setFileOffset( item_pos );
        itOffsets[ i ].item = rec;
        itOffsets[ i ].fileOffset = item_pos;
        item_pos = lengthOfRecord( rec, oxfer, enctype ) + item_pos;
    }

    OFCondition e1 = convertGivenPointer( &dset, itOffsets, num, DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    OFCondition e2 = convertGivenPointer( &dset, itOffsets, num, DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );

    OFCondition e3 = convertGivenPointer( &localDirRecSeq, itOffsets, num, DCM_OffsetOfTheNextDirectoryRecord );
    OFCondition e4 = convertGivenPointer( &localDirRecSeq, itOffsets, num, DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
    OFCondition e5 = convertGivenPointer( &localDirRecSeq, itOffsets, num, DCM_RETIRED_MRDRDirectoryRecordOffset );

    if ( e1 == EC_InvalidVR || e2 == EC_InvalidVR || e3 == EC_InvalidVR || e4 == EC_InvalidVR || e5 == EC_InvalidVR )
        l_error = EC_InvalidVR;
    delete[] itOffsets;

    return l_error;
}


// ********************************


OFCondition DcmDicomDir::copyRecordPtrToSQ( DcmDirectoryRecord *record,
                                            DcmSequenceOfItems &toDirSQ,
                                            DcmDirectoryRecord **firstRec,
                                            DcmDirectoryRecord **lastRec )
{
    DcmDirectoryRecord *nextRec = NULL;
    DcmDirectoryRecord *lastReturnItem = NULL;

    if ( record != NULL )
    {
        unsigned long lastIndex = record->cardSub();
        for (unsigned long i = lastIndex; i > 0; i-- )
        {
            DCMDATA_DEBUG("DcmDicomDir::copyRecordPtrToSQ() Testing sub record no. " << i << " of " << lastIndex);

            DcmDirectoryRecord *subRecord = record->getSub( i-1 );

            if ( subRecord != NULL )
            {
                DcmUnsignedLongOffset *uloP;
                if ( i == lastIndex )
                    lastReturnItem = subRecord;         // memorize last item
                                                        // adjust nextPointer
                DcmTag nextRecTag( DCM_OffsetOfTheNextDirectoryRecord );
                uloP = new DcmUnsignedLongOffset( nextRecTag );
                uloP->putUint32(Uint32(0));
                uloP->setNextRecord( nextRec );
                subRecord->insert( uloP, OFTrue );
#ifdef DEBUG
                Uint32 l_uint = 0;
                uloP->getUint32(l_uint);
                DCMDATA_TRACE("DcmDicomDir::copyRecordPtrToSQ() Next Offset Element ("
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                    << STD_NAMESPACE setw(4) << uloP->getGTag() << ","
                    << STD_NAMESPACE setw(4) << uloP->getETag() << ") offs=0x"
                    << STD_NAMESPACE setw(8) << l_uint
                    << " p=" << OFstatic_cast(void *, uloP)
                    << " next=" << OFstatic_cast(void *, nextRec));
#endif
                copyRecordPtrToSQ( subRecord, toDirSQ, firstRec, lastRec );

                // adjust lowerPointer
                DcmTag lowerRefTag( DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
                uloP = new DcmUnsignedLongOffset( lowerRefTag );
                uloP->putUint32(Uint32(0));
                uloP->setNextRecord( *firstRec );
                subRecord->insert( uloP, OFTrue );
#ifdef DEBUG
                uloP->getUint32(l_uint);
                DCMDATA_TRACE("DcmDicomDir::copyRecordPtrToSQ() Lower Offset Element ("
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                    << STD_NAMESPACE setw(4) << uloP->getGTag() << ","
                    << STD_NAMESPACE setw(4) << uloP->getETag() << ") offs=0x"
                    << STD_NAMESPACE setw(8) << l_uint
                    << " p=" << OFstatic_cast(void *, uloP)
                    << " lower=" << OFstatic_cast(void *, *firstRec));
#endif

                /* insert at begining */
                toDirSQ.prepend( subRecord );

                nextRec = subRecord;
            }
        }  // for ( i ...
    }
    if ( lastRec != NULL )
        *lastRec = lastReturnItem;
    // points to first record of subordinate level
    if ( firstRec != NULL )
        *firstRec = nextRec;
    return EC_Normal;
}


// ********************************


OFCondition DcmDicomDir::convertTreeToLinear(Uint32 beginOfDataSet,
                                             E_TransferSyntax oxfer,
                                             E_EncodingType enctype,
                                             E_GrpLenEncoding glenc,
                                             DcmSequenceOfItems &unresRecs )
{
    OFCondition l_error = EC_Normal;
    DcmDataset &dset = getDataset();    // guaranteed to exist
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );

    // copy items to which no pointer exists to a temporary list
    unsigned long numUnresItems = localDirRecSeq.card();
    for (unsigned long i = numUnresItems; i > 0; i-- )
    {
        DCMDATA_DEBUG("DcmDicomDir::convertTreeToLinear() Copy pointer of unresolved Record no. "
            << i << " of " << numUnresItems << " to unresRecsSeq:");
        unresRecs.insert( localDirRecSeq.getItem(i-1), 0 );
    }

    // convert items back into the root directory entity:
    DcmDirectoryRecord *firstRootRecord[1], *lastRootRecord[1];
    copyRecordPtrToSQ( &getRootRecord(), localDirRecSeq, firstRootRecord, lastRootRecord );

    // set pointer to first directory record:
    DcmUnsignedLongOffset *offElem = lookForOffsetElem( &dset, DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    if ( offElem != NULL )
        offElem->setNextRecord( *firstRootRecord );

    // set pointer to last directory record:
    offElem = lookForOffsetElem( &dset, DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );
    if ( offElem != NULL )
        offElem->setNextRecord( *lastRootRecord );

    // copy MRDRs in localDirRecSeq
    unsigned long numMRDRItems = getMRDRSequence().card();
    for (unsigned long j = numMRDRItems; j > 0; j-- )
    {
        DCMDATA_DEBUG("DcmDicomDir::convertTreeToLinear() Copy pointer of MRDR no. " << j << " of "
            << numUnresItems << " to localDirRecSeq:");
        localDirRecSeq.insert( getMRDRSequence().getItem(j-1), 0 );
    }

    // compute group lengths before computing byte offsets
    dset.computeGroupLengthAndPadding(glenc, EPD_noChange, oxfer, enctype);

    // convert maximum twice
    if ( convertAllPointer( dset, beginOfDataSet, oxfer, enctype ) == EC_InvalidVR )
        if ( convertAllPointer( dset, beginOfDataSet, oxfer, enctype ) == EC_InvalidVR )
        {
            DCMDATA_ERROR("DcmDicomDir: There are some incorrect Offsets in file " << dicomDirFileName);
            l_error = EC_CorruptedData;
        }
    return l_error;
}


// ********************************


OFCondition DcmDicomDir::insertMediaSOPUID( DcmMetaInfo &metaInfo )  // inout
{
    OFCondition l_error = EC_Normal;
    DcmTag medSOPClassTag( DCM_MediaStorageSOPClassUID );
    DcmUniqueIdentifier *mediaStorageSOPClassUID = new DcmUniqueIdentifier( medSOPClassTag );
    const char* valueUID = UID_MediaStorageDirectoryStorage;
    mediaStorageSOPClassUID->putString( valueUID );
    metaInfo.insert( mediaStorageSOPClassUID, OFTrue );
    return l_error;
}


// ********************************


void DcmDicomDir::print(STD_NAMESPACE ostream&out,
                        const size_t flags,
                        const int level,
                        const char *pixelFileName,
                        size_t *pixelCounter)
{
    int i;
    for ( i=0; i<level; i++)
        out << "    ";
    out << "# Dicom DIR" << OFendl;

    for ( i=0; i<level; i++)
        out << "    ";
    out << "# Meta-Info and General Directory Information" << OFendl;
    getDirFileFormat().print(out, flags, 0, pixelFileName, pixelCounter);

    out << OFendl;
    for ( i=0; i<level; i++)
        out << "    ";
    out << "# Item Hierarchy (root Record not shown)" << OFendl;
    getRootRecord().lowerLevelList->print(out, flags, 1, pixelFileName, pixelCounter);  // friend class

    out << OFendl;
    for ( i=0; i<level; i++)
        out << "    ";
    out << "# used Multi Referenced Directory Records" << OFendl;
    getMRDRSequence().print(out, flags, 1, pixelFileName, pixelCounter);
}


// ********************************


OFCondition DcmDicomDir::error()
{
    return errorFlag;
}


// ********************************


DcmFileFormat& DcmDicomDir::getDirFileFormat()
{
    return *DirFile;
}


// ********************************


const char* DcmDicomDir::getDirFileName()
{
    return dicomDirFileName;
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
                                                   const char *filename )
{
    DcmDirectoryRecord* retRec = NULL;
    if ( filename != NULL && *filename != '\0' && startRec != NULL)
    {
        unsigned long lastIndex = startRec->cardSub();
        for (unsigned long i = 0; i < lastIndex; i++ )
        {
            DcmDirectoryRecord *subRecord = startRec->getSub( i );
            const char* subName = subRecord->lookForReferencedFileID(); // friend

            if ( subName != NULL && !strcmp( filename, subName ) )
            {
                DCMDATA_DEBUG("DcmDicomDir::recurseMatchFile() Record p=" << OFstatic_cast(void *, subRecord)
                    << " with matching filename [" << subName << "] found");
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
                                                  const char *filename )
{
    DcmDirectoryRecord* retRec = NULL;
    if ( filename != NULL && *filename != '\0' )
    {
        unsigned long lastIndex = recSeq.card();
        for (unsigned long i = 0; i < lastIndex; i++ )
        {
            DcmDirectoryRecord *record;
            record = OFstatic_cast(DcmDirectoryRecord *, recSeq.getItem( i ));
            const char* subName = record->lookForReferencedFileID(); // friend

            if ( subName != NULL && !strcmp( filename, subName ) )
            {
                DCMDATA_DEBUG("DcmDicomDir::searchMatchFile() Record p=" << OFstatic_cast(void *, record)
                    << " with matching filename [" << subName << "] found");
                retRec = record;
                break;
            }
        }
    }

    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchFilename( const char *filename )
{
    DcmDirectoryRecord* retRec = NULL;
    if ( filename != NULL && *filename != '\0' )
    {
        retRec = recurseMatchFile( &getRootRecord(), filename );
        if ( retRec == NULL )
        {
            retRec = searchMatchFile( getMRDRSequence(), filename );
            if ( retRec == NULL )
            {
                DcmDataset &dset = getDataset();
                retRec = searchMatchFile( getDirRecSeq(dset), filename );
            }
        }
    }
    if (retRec == NULL)
    {
        DCMDATA_DEBUG("DcmDicomDir::matchFilename() No Record with matching filename ["
            << filename << "] found");
    }
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDicomDir::matchOrCreateMRDR( const char *filename )
{
    DcmDirectoryRecord* newMRDR = NULL;
    DcmDirectoryRecord* matchRec = matchFilename( filename );
    if ( matchRec != NULL )
    {
        if ( matchRec->getRecordType() == ERT_Mrdr )
            newMRDR = matchRec;
        else if ( matchRec->getRecordType() != ERT_root )
        {
            newMRDR = new DcmDirectoryRecord( ERT_Mrdr, filename, NULL);
            if ( matchRec->assignToMRDR( newMRDR ) != EC_IllegalCall )
                getMRDRSequence().insert( newMRDR );
            else
            {
                delete newMRDR;
                newMRDR = NULL;
                DCMDATA_ERROR("DcmDicomDir: Internal ERROR: Can't Create MRDR");
            }
            if (newMRDR != NULL)
            {
                DCMDATA_DEBUG("DcmDicomDir::matchOrCreateMRDR() New MRDR p=" << OFstatic_cast(void *, newMRDR)
                    << " with matching filename [" << filename << "] created, original Record p="
                    << OFstatic_cast(void *, matchRec) << " with same filename modified");
            }
            modified = OFTrue;
        }
    }
    if (newMRDR == NULL)
        DCMDATA_WARN("DcmDicomDir::matchOrCreateMRDR() No MRDR with matching filename [" << filename << "] found");

    return newMRDR;
}


// ********************************
// ********************************


OFCondition DcmDicomDir::write(const E_TransferSyntax oxfer,
                               const E_EncodingType enctype,
                               const E_GrpLenEncoding glenc)
{
    if ( oxfer != DICOMDIR_DEFAULT_TRANSFERSYNTAX )
    {
        DCMDATA_ERROR("DcmDicomDir::write() Wrong TransferSyntax used, only LittleEndianExplicit allowed");
    }
    errorFlag = EC_Normal;
    E_TransferSyntax outxfer = DICOMDIR_DEFAULT_TRANSFERSYNTAX;

    /* find the path of the dicomdir to be created */
    OFString tempfilename = dicomDirFileName;
    size_t pathsepposition = tempfilename.rfind(PATH_SEPARATOR);
    if (pathsepposition == OFString_npos)
      tempfilename.erase();
      else tempfilename.erase(pathsepposition +1);

    // create template for temporary file
    tempfilename += TEMPNAME_TEMPLATE;

    // copy template into non-const buffer
    char *tempfile = new char[tempfilename.size() + 1];
    OFStandard::strlcpy(tempfile, tempfilename.c_str(), tempfilename.size() + 1);

#ifdef HAVE_MKSTEMP
    int tempfilefd = mkstemp(tempfile);
    if (tempfilefd < 0)
    {
        char buf[256];
        DCMDATA_ERROR("DcmDicomDir: Cannot create DICOMDIR temporary file: " << tempfile);
        delete[] tempfile;
        const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
        if (text == NULL) text = "(unknown error code)";
        errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
        return errorFlag;
    }

    FILE *f = fdopen(tempfilefd, "wb");
    if (f == NULL)
    {
        char buf[256];
        DCMDATA_ERROR("DcmDicomDir: Cannot create DICOMDIR temporary file: " << tempfile);
        delete[] tempfile;
        const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
        if (text == NULL) text = "(unknown error code)";
        errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
        return errorFlag;
    }

    DcmOutputFileStream *outStream = new DcmOutputFileStream(f);
#else /* ! HAVE_MKSTEMP */

#ifdef HAVE_MKTEMP
    mktemp( tempfile );
#endif
    DcmOutputFileStream *outStream = new DcmOutputFileStream(tempfile);

#endif /* HAVE_MKSTEMP */

    if (! outStream->good())
    {
        DCMDATA_ERROR("DcmDicomDir: Cannot create DICOMDIR temporary file: " << tempfile);
        errorFlag = outStream->status();
        delete[] tempfile;
        delete outStream;
        return errorFlag;
    }

    DcmDataset &dset = getDataset(); // guaranteed to exist
    DcmMetaInfo &metainfo = *(getDirFileFormat().getMetaInfo());
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );
    DcmTag unresSeqTag( DCM_DirectoryRecordSequence );
    DcmSequenceOfItems localUnresRecs( unresSeqTag );

    // insert Media Stored SOP Class UID
    insertMediaSOPUID( metainfo );

    getDirFileFormat().validateMetaInfo( outxfer );

    {
        // it is important that the cache object is destroyed before the file is renamed!
        // Therefore, the variable declaration is "encapsulated" in curly brackets.
        DcmWriteCache wcache;

        metainfo.transferInit();
        metainfo.write(*outStream, META_HEADER_DEFAULT_TRANSFERSYNTAX, enctype, &wcache);
        metainfo.transferEnd();

        Uint32 beginOfDataset = OFstatic_cast(Uint32, outStream->tell());

        // convert to writable format
        errorFlag = convertTreeToLinear(beginOfDataset, outxfer, enctype, glenc, localUnresRecs);

        dset.transferInit();
        // do not calculate GroupLength and Padding twice!
        dset.write(*outStream, outxfer, enctype, &wcache, EGL_noChange);
        dset.transferEnd();
    }

    // outStream is closed here
    delete outStream;

    char* backupname = NULL;
    if ( !mustCreateNewDir )
    {
#ifndef DICOMDIR_WITHOUT_BACKUP
        backupname = new char[ 1 + strlen( dicomDirFileName ) + strlen( DICOMDIR_BACKUP_SUFFIX ) ];
        strcpy( backupname, dicomDirFileName );

#ifndef HAVE_LONG_FILE_NAMES
        char *suffix = strrchr( backupname, '.' );
        if ( suffix )
            *suffix = '\0';
#endif

        strcat( backupname, DICOMDIR_BACKUP_SUFFIX );
        unlink( backupname );
        if (errorFlag == EC_Normal)
        {
            if (rename(dicomDirFileName, backupname) != 0)
            {
              char buf[256];
              const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
              if (text == NULL) text = "(unknown error code)";
              errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
            }
        }
#else
        if ( unlink( dicomDirFileName ) != 0 )
        {
          char buf[256];
          const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
          if (text == NULL) text = "(unknown error code)";
          errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
        }
#endif
    }

    if (errorFlag == EC_Normal && rename( tempfile, dicomDirFileName ) != 0)
    {
      char buf[256];
      const char *text = OFStandard::strerror(errno, buf, sizeof(buf));
      if (text == NULL) text = "(unknown error code)";
      errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, text);
    }

    delete[] tempfile;
    modified = OFFalse;

    if (errorFlag == EC_Normal && backupname != NULL) {
        /* remove backup */
        unlink(backupname);
        delete[] backupname;
    }

    // remove all records from sequence localDirRecSeq
    while ( localDirRecSeq.card() > 0 )
        localDirRecSeq.remove(OFstatic_cast(unsigned long, 0));

    // move records to which no pointer exists back
    while ( localUnresRecs.card() > 0 )
    {
        DcmItem *unresRecord =
            localUnresRecs.remove(OFstatic_cast(unsigned long, 0));
        localDirRecSeq.insert( unresRecord );
    }
    return errorFlag;
}


// ********************************
// ********************************


OFCondition DcmDicomDir::countMRDRRefs( DcmDirectoryRecord *startRec,
                                        ItemOffset *refCounter,
                                        const unsigned long numCounters )
{
    OFCondition l_error = EC_Normal;
    if ( refCounter == NULL )
        l_error = EC_IllegalCall;
    else if ( startRec != NULL )
    {
        unsigned long lastIndex = startRec->cardSub();
        for (unsigned long i = 0; i < lastIndex; i++ )
        {
            DcmDirectoryRecord *subRecord = startRec->getSub( i );
            DcmDirectoryRecord *refMRDR = subRecord->lookForReferencedMRDR();
                                                            // friend class
            if ( refMRDR != NULL )
            {
                unsigned long j;
                for ( j = 0; j < numCounters; j++ )
                    if ( refMRDR == refCounter[ j ].item )
                    {
                        ++refCounter[ j ].fileOffset;       // Reference counter
                        break;
                    }
                DCMDATA_DEBUG("DcmDicomDir::countMRDRRefs() MRDR p=" << OFstatic_cast(void *, refMRDR)
                    << " found, which is " << refMRDR->numberOfReferences << " times referenced and "
                    << j << " times counted");
            }
            countMRDRRefs( subRecord, refCounter, numCounters );
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmDicomDir::checkMRDRRefCounter( DcmDirectoryRecord *startRec,
                                              ItemOffset *refCounter,
                                              const unsigned long numCounters )
{
    OFCondition l_error = EC_Normal;
    if ( refCounter == NULL )
        l_error = EC_IllegalCall;
    else if ( startRec != NULL )
    {
        unsigned long lastIndex = startRec->cardSub();
        for (unsigned long i = 0; i < lastIndex; i++ )
        {
            DcmDirectoryRecord *subRecord = startRec->getSub( i );
            DcmDirectoryRecord *refMRDR = subRecord->lookForReferencedMRDR();
                                                            // friend class
            if ( refMRDR != NULL )
            {
                unsigned long j;
                for ( j = 0; j < numCounters; j++ )
                    if ( refMRDR == refCounter[ j ].item )
                    {
                        ++refCounter[ j ].fileOffset;       // reference counter
                        break;
                    }
                DCMDATA_DEBUG("DcmDicomDir::checkMRDRRefCounter() MRDR p=" << OFstatic_cast(void *, refMRDR)
                    << " found, which is " << refMRDR->numberOfReferences << " times referenced and "
                    << j << " times counted");
            }
            OFCondition err1 = checkMRDRRefCounter( subRecord, refCounter, numCounters );
            if ( l_error == EC_Normal && err1 != EC_Normal )
                l_error = err1;         // the first error counts
        }
    }
    return l_error;
}


// ********************************

/* GERMAN COMMENT - PLEASE IGNORE

   Strategie fuer verify (mit autocorrect==OFTrue):
   - lege Tabelle an mit Zeigern auf MRDRs und Referenzzaehlern mit der Groesse
     getDirRecSeq( getDataset() ).card() + getMRDRSequence().card()
   - durchlaufe den Record-Baum und erhoehe bei jedem Auftreten eines
     MRDR-Verweises, den entsprechenden Zaehler in obiger Tabelle
   - setze in allen MRDRs, auf die laut Tabelle kein Verweis existiert, das
     activation flag auf INAKTIV
PENDING:
   - ueberpruefe fuer alle inaktiven MRDRs, ob deren referenzierte Dateien
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

OFCondition DcmDicomDir::verify( OFBool autocorrect )
{
    errorFlag = EC_Normal;
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq(getDataset());
    unsigned long maxMRDRs = localDirRecSeq.card() + getMRDRSequence().card();
    ItemOffset *refCounter = new ItemOffset[ maxMRDRs ];

    // create MRDR table for MRDRs from MRDRSeq and from DirRecSeq:
    unsigned long i;
    for ( i = 0; i < getMRDRSequence().card(); i++ )
    {
        DcmDirectoryRecord *rec;
        rec = OFstatic_cast(DcmDirectoryRecord *, getMRDRSequence().getItem( i ));
        refCounter[i].item = rec;
        refCounter[i].fileOffset = 0L;
    }
    for (unsigned long j = 0; j < localDirRecSeq.card() && i < maxMRDRs; j++ )
    {
        DcmDirectoryRecord *rec;
        rec = OFstatic_cast(DcmDirectoryRecord *, localDirRecSeq.getItem( j ));
        if ( rec->getRecordType() == ERT_Mrdr )
        {
            refCounter[i].item = rec;
            refCounter[i].fileOffset = 0L;
            i++;
        }
    }
//    maxMRDRs = i;            // adjust table size to real value

    // count number of references for each MRDR
    countMRDRRefs( &getRootRecord(), refCounter, maxMRDRs );

    // check stored reference counters for correctness
    OFCondition err3 = EC_Normal;
    for (unsigned long k = 0; k < maxMRDRs; k++ )
    {
        DcmDirectoryRecord *refMRDR = OFstatic_cast(DcmDirectoryRecord *, refCounter[k].item);
        Uint32 refNum = refMRDR->lookForNumberOfReferences();   // friend
        if ( refCounter[k].fileOffset != refNum )
        {
            DCMDATA_ERROR("DcmDicomDir::verify() Reference counter of MRDR p=" << OFstatic_cast(void *, refMRDR)
                << " has incorrect value=" << refNum << " (must be " << refCounter[k].fileOffset << ")");
            if (refCounter[k].fileOffset==refMRDR->numberOfReferences)
                DCMDATA_ERROR("but internal record class value numberOfReferences is correct");

            if ( autocorrect )             // correct reference counter, friend
                refMRDR->setNumberOfReferences( refCounter[k].fileOffset );
            else
                err3 = EC_CorruptedData;
        }
        // set inactivation flag for MRDRs without reference
        if ( autocorrect && refCounter[k].fileOffset == 0L )
            refMRDR->setRecordInUseFlag( 0xffff );
    }

    delete[] refCounter;
    OFCondition err1 = getDirFileFormat().verify( autocorrect );
    OFCondition err2 = getRootRecord().verify( OFFalse );   // no automatic correction
    if ( errorFlag == EC_Normal && ( err1 != EC_Normal || err2 != EC_Normal || err3 != EC_Normal ) )
        errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcdicdir.cc,v $
** Revision 1.61  2010-10-14 13:14:07  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.60  2010-08-18 15:13:23  joergr
** Added const specifier to char pointers where appropriate. Thanks to forum
** user "takeos" for the report.
**
** Revision 1.59  2010-06-03 10:28:40  joergr
** Replaced calls to strerror() by new helper function OFStandard::strerror()
** which results in using the thread safe version of strerror() if available.
**
** Revision 1.58  2010-03-01 09:08:45  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.57  2009-12-04 17:14:34  joergr
** Slightly modified some log messages.
**
** Revision 1.56  2009-11-13 13:11:20  joergr
** Fixed minor issues in log output.
**
** Revision 1.55  2009-11-04 09:58:09  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.54  2009-09-28 13:30:59  joergr
** Moved general purpose definition file from module dcmdata to ofstd, and
** added new defines in order to make the usage easier.
**
** Revision 1.53  2009-02-25 13:10:56  joergr
** Fixed file locking issue that prevented DICOMDIR files with large element
** values (e.g. pixel data inside an IconImageSequence) from being renamed.
**
** Revision 1.52  2009-02-04 18:01:23  joergr
** Fixed various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.51  2008-04-30 12:38:42  meichel
** Fixed compile errors due to changes in attribute tag names
**
** Revision 1.50  2007/11/29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.49  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.48  2005/12/08 15:41:02  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.47  2005/11/28 15:53:13  meichel
** Renamed macros in dcdebug.h
**
** Revision 1.46  2005/11/07 16:59:26  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.45  2004/09/24 08:45:55  joergr
** Replaced "delete" statement by "delete[]" (object created with "new[]").
**
** Revision 1.44  2004/08/03 11:41:09  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.43  2004/03/16 13:44:03  joergr
** Renamed UID_BasicDirectoryStorageSOPClass to UID_MediaStorageDirectoryStorage.
**
** Revision 1.42  2004/02/04 16:25:41  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
** Translated remaining German comments.
**
** Revision 1.41  2004/01/21 10:39:10  meichel
** Added special handling for platforms where mkstemp() exists but no
**   prototype is defined.
**
** Revision 1.40  2003/11/07 13:51:39  meichel
** Now using mkstemp instead of mktemp if available
**
** Revision 1.39  2002/12/06 13:10:46  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.38  2002/11/27 12:06:43  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.37  2002/08/27 16:55:44  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.36  2002/08/21 10:14:20  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.35  2002/04/16 13:43:15  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.34  2001/11/01 16:14:55  meichel
** Fixed bug in dcmdata affecting the creation of a temporary file for a
**   DICOMDIR when an explicit path is given.
**
** Revision 1.33  2001/09/25 17:19:47  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.32  2001/06/01 15:49:00  meichel
** Updated copyright header
**
** Revision 1.31  2000/04/14 15:55:03  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.30  2000/03/08 16:26:31  meichel
** Updated copyright header.
**
** Revision 1.29  2000/03/06 18:12:08  joergr
** Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
**
** Revision 1.28  2000/03/03 14:05:31  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.27  2000/02/23 15:11:48  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.26  2000/02/10 10:52:17  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.25  2000/02/02 14:32:48  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.24  2000/02/01 10:12:04  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.23  1999/03/31 09:25:20  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.22  1999/03/22 09:58:55  meichel
** Reworked data dictionary based on the 1998 DICOM edition and the latest
**   supplement versions. Corrected dcmtk applications for minor changes
**   in attribute name constants.
**
** Revision 1.21  1998/07/15 15:51:49  joergr
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
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
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
*/
