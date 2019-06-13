/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

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

#ifndef O_BINARY
#define O_BINARY 0                     /* only Windows has O_BINARY */
#endif


// ********************************


DcmDicomDir::DcmDicomDir()
  : errorFlag(EC_Normal),
    dicomDirFileName(),
    modified(OFFalse),
    mustCreateNewDir(OFFalse),
    DirFile(new DcmFileFormat()),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    dicomDirFileName.set(DEFAULT_DICOMDIR_NAME);

    OFCondition cond = DirFile->loadFile(dicomDirFileName);
    if (cond.bad())
    {
        delete DirFile; // clean up file format object
        DirFile = new DcmFileFormat();
        mustCreateNewDir = OFTrue;
    }

    createNewElements( "" );      // create missing data elements
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, OFFilename());
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
}


// ********************************


DcmDicomDir::DcmDicomDir(const OFFilename &fileName, const char *fileSetID)
  : errorFlag(EC_Normal),
    dicomDirFileName(),
    modified(OFFalse),
    mustCreateNewDir(OFFalse),
    DirFile(new DcmFileFormat()),
    RootRec(NULL),
    MRDRSeq(NULL)
{
    if ( fileName.isEmpty() )
        dicomDirFileName.set(DEFAULT_DICOMDIR_NAME);
    else
        dicomDirFileName = fileName;

    OFCondition cond = DirFile->loadFile(dicomDirFileName);
    if (cond.bad())
    {
      delete DirFile;   // clean up file format object
      DirFile = new DcmFileFormat();
      mustCreateNewDir = OFTrue;
    }

    createNewElements( fileSetID );   // create missing data elements
    RootRec = new DcmDirectoryRecord( ERT_root, NULL, OFFilename());
    DcmTag mrdrSeqTag( DCM_DirectoryRecordSequence );
    MRDRSeq = new DcmSequenceOfItems( mrdrSeqTag );

    errorFlag = convertLinearToTree();
}


// ********************************


/* This copy constructor implementation is untested
 */
DcmDicomDir::DcmDicomDir( const DcmDicomDir & old )
  : errorFlag(old.errorFlag),
    dicomDirFileName(old.dicomDirFileName),
    modified(old.modified),
    mustCreateNewDir(old.mustCreateNewDir),
    DirFile(new DcmFileFormat(*old.DirFile)),
    RootRec(new DcmDirectoryRecord(*old.RootRec)),
    MRDRSeq(new DcmSequenceOfItems(*old.MRDRSeq))
{
}


// ********************************


DcmDicomDir::~DcmDicomDir()
{
    if (modified)
        write();

    delete DirFile;
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
                DCMDATA_TRACE("DcmDicomDir::lookForOffsetElem() Offset Element "
                    << offElem->getTag() << " offs=0x"
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
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
                                              const OFMap<Uint32, DcmDirectoryRecord *> &itOffsets,
                                              const DcmTagKey &offsetTag )
{
    OFCondition l_error = EC_Normal;
    if ( startPoint != NULL )
    {
        DcmStack stack;
        Uint32 offset;
        for (;;)
        {
            l_error = startPoint->nextObject(stack, OFTrue);
            if (l_error.bad())
                break;

            DcmObject *cur = stack.top();
            if (cur->ident() != EVR_up || cur->getTag() != offsetTag)
                continue;

            DcmUnsignedLongOffset *offElem = OFstatic_cast(DcmUnsignedLongOffset *, cur);
            l_error = offElem->getUint32(offset);

            /* an offset of 0 means that no directory record is referenced */
            if (l_error.good() && (offset > 0))
            {
                OFMap<Uint32, DcmDirectoryRecord *>::const_iterator it = itOffsets.find(offset);
                if (it != itOffsets.end())
                {
                    offElem->setNextRecord(it->second);
                } else {
                    DCMDATA_WARN("DcmDicomDir::resolveGivenOffsets() Cannot resolve offset " << offset);
                    /* FIXME: obviously, this error code is never returned but always ignored!? */
                    l_error = EC_InvalidOffset;
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
    DcmObject *obj = NULL;
    DcmDirectoryRecord *rec = NULL;
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );
    unsigned long maxitems = localDirRecSeq.card();
    OFMap<Uint32, DcmDirectoryRecord *> itOffsets;

    for (unsigned long i = 0; i < maxitems; i++ )
    {
        obj = localDirRecSeq.nextInContainer(obj);
        rec = OFstatic_cast(DcmDirectoryRecord *, obj);
        long filePos = rec->getFileOffset();
        itOffsets[ OFstatic_cast(Uint32, filePos) ] = rec;
        DCMDATA_DEBUG("DcmDicomDir::resolveAllOffsets() Item Offset [" << i << "] = 0x"
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << filePos);
    }
    resolveGivenOffsets( &dset, itOffsets,
        DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    resolveGivenOffsets( &dset, itOffsets,
        DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );

    resolveGivenOffsets( &localDirRecSeq, itOffsets,
        DCM_OffsetOfTheNextDirectoryRecord );
    resolveGivenOffsets( &localDirRecSeq, itOffsets,
        DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
    resolveGivenOffsets( &localDirRecSeq, itOffsets,
        DCM_RETIRED_MRDRDirectoryRecordOffset );

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
    else
    {
        while ( (startRec != NULL) && l_error.good() )
        {
            DcmDirectoryRecord *lowerRec = NULL;
            DcmDirectoryRecord *nextRec = NULL;

            // check whether directory record is really part of the given sequence:
            if (&fromDirSQ != startRec->getParent())
            {
                DCMDATA_ERROR("DcmDicomDir: Record with offset=" << startRec->getFileOffset()
                    << " is referenced more than once, ignoring later reference");
                l_error = EC_InvalidDICOMDIR;
                // exit the while loop
                break;
            }

            DcmUnsignedLongOffset *offElem;
            offElem = lookForOffsetElem( startRec, DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
            if ( offElem != NULL )
                lowerRec = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());
            offElem = lookForOffsetElem( startRec, DCM_OffsetOfTheNextDirectoryRecord );
            if ( offElem != NULL )
                nextRec = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());

            DCMDATA_TRACE("DcmDicomDir::moveRecordToTree() Record with"
                << " offset=" << startRec->getFileOffset()
                << " p=" << OFstatic_cast(void *, startRec)
                << " has lower=" << OFstatic_cast(void *, lowerRec)
                << " and next=" << OFstatic_cast(void *, nextRec) << " Record");

            linkMRDRtoRecord( startRec );

            // use protected method for insertion without type check:
            if ( toRecord->masterInsertSub( startRec ) == EC_Normal )
            {
                // only works since friend class
                DcmItem *dit = fromDirSQ.remove( startRec );
                if ( dit == NULL )
                {
                    DCMDATA_ERROR("DcmDicomDir: Record with offset=" << startRec->getFileOffset()
                        << " is part of unknown Sequence");
                }
            }
            else
            {
                DCMDATA_ERROR("DcmDicomDir::moveRecordToTree() Cannot insert DirRecord (=NULL?)");
            }

            // recursively call this method for next lower level:
            l_error = moveRecordToTree( lowerRec, fromDirSQ, startRec );

            // We handled this record, now move on to the next one on this level.
            // The next while-loop iteration does the equivalent of the following:
            // moveRecordToTree( nextRec, fromDirSQ, toRecord );
            startRec = nextRec;
        }
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
    // currently, always returns EC_Normal
    OFCondition l_error = resolveAllOffsets( dset );

    // search for first directory record:
    DcmDirectoryRecord *firstRootRecord = NULL;
    DcmUnsignedLongOffset *offElem = lookForOffsetElem( &dset, DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    if ( offElem != NULL )
        firstRootRecord = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());

    // create tree structure from flat record list:
    l_error = moveRecordToTree( firstRootRecord, localDirRecSeq, &getRootRecord() );

    if (l_error.good())
    {
        // move MRDRs from localDirRecSeq to global MRDRSeq:
        moveMRDRbetweenSQs( localDirRecSeq, getMRDRSequence() );

        // dissolve MRDR references for all remaining items
        for (unsigned long i = localDirRecSeq.card(); i > 0; i-- )
            linkMRDRtoRecord( OFstatic_cast(DcmDirectoryRecord *, localDirRecSeq.getItem(i-1)) );
    }
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
                                              const DcmTagKey &offsetTag )
{
    OFCondition l_error = EC_Normal;
    if ( startPoint != NULL )
    {
        DcmStack stack;
        for (;;)
        {
            l_error = startPoint->nextObject(stack, OFTrue);
            if (l_error.bad())
                break;

            DcmObject *cur = stack.top();
            if (cur->ident() != EVR_up || cur->getTag() != offsetTag)
                continue;

            DcmUnsignedLongOffset *offElem = OFstatic_cast(DcmUnsignedLongOffset *, cur);
            DcmObject *obj = offElem->getNextRecord();
            if (obj != NULL)
                offElem->putUint32(OFstatic_cast(DcmDirectoryRecord *, obj)->getFileOffset());
            else
                offElem->putUint32(0);
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
    DcmObject *obj = NULL;
    DcmDirectoryRecord *rec = NULL;
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq( dset );
    Uint32 offs_Item1 =  beginOfDataSet + lengthUntilSQ( dset, oxfer, enctype );
    unsigned long num = localDirRecSeq.card();

    Uint32 item_pos = offs_Item1;
    for (unsigned long i = 0; i < num; i++ )
    {
        obj = localDirRecSeq.nextInContainer(obj);
        rec = OFstatic_cast(DcmDirectoryRecord *, obj);
        rec->setFileOffset( item_pos );
        item_pos = lengthOfRecord( rec, oxfer, enctype ) + item_pos;
    }

    /* calling convertGivenPointer() requires that the above for-loop has been run through */
    OFCondition e1 = convertGivenPointer( &dset, DCM_OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity );
    OFCondition e2 = convertGivenPointer( &dset, DCM_OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity );

    OFCondition e3 = convertGivenPointer( &localDirRecSeq, DCM_OffsetOfTheNextDirectoryRecord );
    OFCondition e4 = convertGivenPointer( &localDirRecSeq, DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
    OFCondition e5 = convertGivenPointer( &localDirRecSeq, DCM_RETIRED_MRDRDirectoryRecordOffset );

    if ( e1 == EC_InvalidVR || e2 == EC_InvalidVR || e3 == EC_InvalidVR || e4 == EC_InvalidVR || e5 == EC_InvalidVR )
        l_error = EC_InvalidVR;

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
                DCMDATA_TRACE("DcmDicomDir::copyRecordPtrToSQ() Next Offset Element "
                    << uloP->getTag() << " offs=0x" << STD_NAMESPACE hex
                    << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << l_uint
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
                DCMDATA_TRACE("DcmDicomDir::copyRecordPtrToSQ() Lower Offset Element "
                    << uloP->getTag() << " offs=0x" << STD_NAMESPACE hex
                    << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << l_uint
                    << " p=" << OFstatic_cast(void *, uloP)
                    << " lower=" << OFstatic_cast(void *, *firstRec));
#endif

                /* insert at beginning */
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


void DcmDicomDir::print(STD_NAMESPACE ostream &out,
                        const size_t flags,
                        const int level,
                        const char *pixelFileName,
                        size_t *pixelCounter)
{
    int i;
    for ( i=0; i<level; i++)
        out << "  ";
    out << "# Dicom Directory" << OFendl;

    for ( i=0; i<level; i++)
        out << "  ";
    out << "# Meta-Info and General Directory Information" << OFendl;
    getDirFileFormat().print(out, flags, 0, pixelFileName, pixelCounter);

    out << OFendl;
    for ( i=0; i<level; i++)
        out << "  ";
    out << "# Item Hierarchy (Root Record not shown)" << OFendl;
    getRootRecord().lowerLevelList->print(out, flags, 1, pixelFileName, pixelCounter);  // friend class

    out << OFendl;
    for ( i=0; i<level; i++)
        out << "  ";
    out << "# Used Multi Referenced Directory Records" << OFendl;
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


const OFFilename &DcmDicomDir::getDirFileName()
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
            newMRDR = new DcmDirectoryRecord( ERT_Mrdr, filename, OFFilename());
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
    if (oxfer != DICOMDIR_DEFAULT_TRANSFERSYNTAX)
    {
        DCMDATA_ERROR("DcmDicomDir::write() Wrong TransferSyntax used, only LittleEndianExplicit allowed");
    }
    errorFlag = EC_Normal;
    E_TransferSyntax outxfer = DICOMDIR_DEFAULT_TRANSFERSYNTAX;

    // create a temporary file based on the DICOMDIR filename
    OFFilename tempFilename;
    OFStandard::appendFilenameExtension(tempFilename, dicomDirFileName, DICOMDIR_TEMP_SUFFIX);

    DcmOutputFileStream *outStream = new DcmOutputFileStream(tempFilename);
    if (! outStream->good())
    {
        DCMDATA_ERROR("DcmDicomDir: Cannot create DICOMDIR temporary file: " << tempFilename);
        errorFlag = outStream->status();
        delete outStream;
        return errorFlag;
    }

    DcmDataset &dset = getDataset(); // guaranteed to exist
    DcmMetaInfo &metainfo = *(getDirFileFormat().getMetaInfo());
    DcmSequenceOfItems &localDirRecSeq = getDirRecSeq(dset);
    DcmTag unresSeqTag(DCM_DirectoryRecordSequence);
    DcmSequenceOfItems localUnresRecs(unresSeqTag);

    // insert Media Stored SOP Class UID
    insertMediaSOPUID(metainfo);

    getDirFileFormat().validateMetaInfo(outxfer);

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

    OFFilename backupFilename;
    if (!mustCreateNewDir)
    {
#ifndef DICOMDIR_WITHOUT_BACKUP
        // create a temporary backup of the existing DICOMDIR
        OFStandard::appendFilenameExtension(backupFilename, dicomDirFileName, DICOMDIR_BACKUP_SUFFIX);
        OFStandard::deleteFile(backupFilename);
        if (errorFlag == EC_Normal)
        {
            if (!OFStandard::renameFile(dicomDirFileName, backupFilename))
            {
                OFString buffer = OFStandard::getLastSystemErrorCode().message();
                errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, buffer.c_str());
            }
        }
#else
        if (!OFStandard::deleteFile(dicomDirFileName))
        {
            OFString buffer = OFStandard::getLastSystemErrorCode().message();
            errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, buffer.c_str());
        }
#endif
    }

    if (errorFlag == EC_Normal && !OFStandard::renameFile(tempFilename, dicomDirFileName))
    {
        OFString buffer = OFStandard::getLastSystemErrorCode().message();
        errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, buffer.c_str());
    }

    modified = OFFalse;

    if (errorFlag == EC_Normal) {
        // remove temporary backup (if any)
        OFStandard::deleteFile(backupFilename);
    }

    // remove all records from sequence localDirRecSeq
    while (localDirRecSeq.card() > 0)
        localDirRecSeq.remove(OFstatic_cast(unsigned long, 0));

    // move records to which no pointer exists back
    while (localUnresRecs.card() > 0)
    {
        DcmItem *unresRecord =
            localUnresRecs.remove(OFstatic_cast(unsigned long, 0));
        localDirRecSeq.insert(unresRecord);
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
                {
                    if ( refMRDR == refCounter[ j ].item )
                    {
                        ++refCounter[ j ].fileOffset;       // Reference counter
                        break;
                    }
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
                {
                    if ( refMRDR == refCounter[ j ].item )
                    {
                        ++refCounter[ j ].fileOffset;       // reference counter
                        break;
                    }
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
