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
 *  Purpose: class DcmDirectoryRecord
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-02-23 15:11:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdirrec.cc,v $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream.h>
#include <ctype.h>

#include "dcdirrec.h"
#include "dctk.h"
#include "dcxfer.h"
#include "dcvr.h"
#include "dcvrus.h"
#include "dcdebug.h"

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>       /* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif


// ********************************


static const char* DRTypeNames[] =
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
    "VOI LUT",
    "SR DOCUMENT" /* was "STRUCT REPORT" in old frozen draft */
};

static short DIM_OF_DRTypeNames = (sizeof(DRTypeNames) / sizeof(DRTypeNames[0]));


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord()
  : DcmItem(ItemTag),
    recordsOriginFile(NULL),
    lowerLevelList(NULL),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmTag &tag,
                                       const Uint32 len)
  : DcmItem(tag, len),
    recordsOriginFile(NULL),
    lowerLevelList(NULL),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(
    const E_DirRecType recordType,
    const char * referencedFileID,
    const char * sourceFilename)
  : DcmItem(ItemTag),
    recordsOriginFile(NULL),
    lowerLevelList(NULL),
    DirRecordType(recordType),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    setRecordsOriginFile(sourceFilename);

    if ( DirRecordType != ERT_root )
        errorFlag = this -> fillElementsAndReadSOP(
            referencedFileID, sourceFilename);
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(
    const char * recordTypeName,
    const char * referencedFileID,
    const char * sourceFilename)
  : DcmItem(ItemTag),
    recordsOriginFile(NULL),
    lowerLevelList(NULL),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DcmTag sequTag( DCM_DirectoryRecordSequence );
    lowerLevelList = new DcmSequenceOfItems( sequTag );
    DirRecordType = this -> recordNameToType( recordTypeName );
    setRecordsOriginFile(sourceFilename);

    if ( DirRecordType != ERT_root )
        errorFlag = this -> fillElementsAndReadSOP(
            referencedFileID, sourceFilename);
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmDirectoryRecord &old)
  : DcmItem(old),
    recordsOriginFile(NULL),
    lowerLevelList(NULL),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DcmTag sequTag( DCM_DirectoryRecordSequence );
    if ( old.ident() == EVR_dirRecord )
    {
        DcmSequenceOfItems const *ll = old.lowerLevelList;
        lowerLevelList = new DcmSequenceOfItems( *ll ); // Copy-Constructor

        DirRecordType = old.DirRecordType;
        referencedMRDR = old.referencedMRDR;
        numberOfReferences = old.numberOfReferences;
        offsetInFile = old.offsetInFile;
        setRecordsOriginFile(old.recordsOriginFile);
    } else {
        lowerLevelList = new DcmSequenceOfItems( sequTag );

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
}


// ********************************


DcmDirectoryRecord::~DcmDirectoryRecord()
{
    delete lowerLevelList;
    delete[] recordsOriginFile;
}


// ********************************
// ********************************


E_DirRecType DcmDirectoryRecord::recordNameToType(const char * recordTypeName)
{
    E_DirRecType recType = ERT_Private;
    if (recordTypeName != (char*)NULL)
    {
        short i = 0;
        while (i < DIM_OF_DRTypeNames &&
               strcmp(DRTypeNames[i], recordTypeName) != 0)
            i++;

        if (i < DIM_OF_DRTypeNames && strcmp(DRTypeNames[i], recordTypeName) == 0) recType = (E_DirRecType)i;
        else if (strcmp(recordTypeName,"STRUCT REPORT")) recType = ERT_StructReport; // we recognise the old name as well
        debug(4, ("DcmDirectoryRecord::recordNameToType() input char*=\"%s\" output enum=%d", recordTypeName, recType));
    }
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
    while ( (c = *from++) != 0 ) {
        if ( c == '\\' ) {
            if ( lastchar != '\\' ) {      // eliminate double '\\'
                *to++ = PATH_SEPARATOR;
            }
        } else {
            *to++ = c;
        }
        lastchar = c;
    }
    *to = '\0';

    /* 
    ** Some OS's append a '.' to the filename of a ISO9660 filesystem.
    ** If the filename does not exist then try appending a '.'
    */
    FILE* f = NULL;
    if ((f = fopen(destName, "rb")) != NULL) {
        fclose(f);
    } else {
        char* newname = new char[strlen(destName) + 2];
        strcpy(newname, destName);
        strcat(newname, ".");
        if ((f = fopen(newname, "rb")) != NULL) {
            fclose(f);
            strcpy(destName, newname);
        } else {
            /* we cannot find the file.  let the caller deal with this */
        }
    }
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
                case ERT_StructReport:
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
    csP->putString( DRTypeNames[ newType ] );
    insert( csP, OFTrue );

    return l_error;
}


// ********************************


E_DirRecType DcmDirectoryRecord::lookForRecordType()
{
  E_DirRecType localType = ERT_Private;
  if ( !elementList->empty() )
    {
      DcmStack stack;
      if ( this->search( DCM_DirectoryRecordType,
                         stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
          if ( stack.top()->ident() == EVR_CS )
            {
              char * recName = NULL;
              DcmCodeString *recType = (DcmCodeString*)stack.top();
              recType->verify( OFTrue );            // erzwinge dealigning
              recType->getString(recName);
              localType = this->recordNameToType(recName);
debug(4, ( "DcmDirectoryRecord::lookForRecordType() RecordType-Element(0x%4.4hx,0x%4.4hx) Type=[%s]",
        recType->getGTag(), recType->getETag(), DRTypeNames[DirRecordType] ));

            }
        }
    }

  return localType;
}


// ********************************

static void 
hostToDicomFilename(char* fname)
{
    /*
    ** Massage filename into dicom format. 
    ** Elmiminate any invalid characters.
    ** Most commonly there is a '.' at the end of a filename.
    */
    int len = strlen(fname);
    int k = 0;
    char c = '\0';
    for (int i=0; i<len; i++) {
        c = fname[i];
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == PATH_SEPARATOR) {
            fname[k++] = '\\';
        } else if (isalpha(c) || isdigit(c) || (c == '_') || (c == '\\')) {
            /* filenames in DICOM must always be in uppercase */
            fname[k++] = toupper(c);
        }
    }
    fname[k] = '\0';
}


E_Condition 
DcmDirectoryRecord::setReferencedFileID( const char *referencedFileID )
{
    E_Condition l_error = EC_Normal;

    char* newFname = new char[strlen(referencedFileID) + 1];
    strcpy(newFname, referencedFileID);
    hostToDicomFilename(newFname);

    DcmTag refFileTag( DCM_ReferencedFileID );
    DcmCodeString *csP = new DcmCodeString( refFileTag );
    if ( referencedFileID != (char*)NULL )
        csP->putString( newFname );
    insert( csP, OFTrue );

    delete[] newFname;
    return l_error;
}


// ********************************


const char* DcmDirectoryRecord::lookForReferencedFileID()
{
    char *localFile = (char*)NULL;
    if ( !elementList->empty() )
    {
        DcmStack stack;
        if ( this->search( DCM_ReferencedFileID,
                           stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_CS )
            {
                DcmCodeString *refFile = (DcmCodeString*)stack.top();
                refFile->verify( OFTrue );          // erzwinge dealigning
                refFile->getString(localFile);
                if ( localFile != (char*)NULL && *localFile == '\0' )
                    localFile = (char*)NULL;
            }
        }
    }
debug(4, ( "DcmDirectoryRecord::lookForReferencedFileID() ReferencedFileID = [%s]", ((localFile)?(localFile):("")) ));
    return localFile;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::lookForReferencedMRDR()
{
    DcmDirectoryRecord *localMRDR = (DcmDirectoryRecord*)NULL;
    if ( !elementList->empty() )
    {
        DcmStack stack;
        if ( this->search( DCM_MRDRDirectoryRecordOffset,
                           stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_up )
            {
                DcmUnsignedLongOffset *offElem;
                offElem = (DcmUnsignedLongOffset*)stack.top();
                localMRDR = (DcmDirectoryRecord*)offElem->getNextRecord();
#ifdef DEBUG
Uint32 l_uint = 0;
offElem->getUint32(l_uint);
debug(4, ( "DcmDirectoryRecord::lookForReferencedMRDR() MRDR Offset-Element(0x%4.4hx,0x%4.4hx) offs=0x%8.8lx p=%p n=%p",
           offElem->getGTag(), offElem->getETag(),
           l_uint, offElem, localMRDR ));
#endif

            }
        }
    }
Cdebug(4, localMRDR==(DcmDirectoryRecord*)NULL, ("DcmDirectoryRecord::lookForReferencedMRDR() no ReferencedMRDR found" ));

    return localMRDR;
}


// ********************************


const char* DcmDirectoryRecord::getReferencedFileName()
{
    const char *localFile = (char*)NULL;
    if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
        localFile = referencedMRDR->lookForReferencedFileID();
    else
        localFile = this->lookForReferencedFileID();
    return localFile;
}


// ********************************


E_Condition DcmDirectoryRecord::setRecordInUseFlag(const Uint16 newFlag )
{
    E_Condition l_error = EC_Normal;

    DcmTag recInUseTag( DCM_RecordInUseFlag );
    DcmUnsignedShort *usP = new DcmUnsignedShort( recInUseTag );
    usP->putUint16( newFlag );
    insert( usP, OFTrue );

    return l_error;
}


// ********************************


Uint16 DcmDirectoryRecord::lookForRecordInUseFlag()
{
    Uint16 localFlag = Uint16(0xffff);     // default value: Record is in use
    if ( !elementList->empty() )
    {
        DcmStack stack;
        if ( this->search( DCM_RecordInUseFlag,
                           stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_US )
                errorFlag = ((DcmUnsignedShort*)stack.top())->
                        getUint16(localFlag);
        }
    }
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
        // neuen Wert eintragen
        DcmTag numRefTag( DCM_NumberOfReferences );
        DcmUnsignedLong *newUL = new DcmUnsignedLong( numRefTag );
        newUL->putUint32( newRefNum );
        insert( newUL, OFTrue );
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
    Uint32 localRefNum = 0L;
    if ( !elementList->empty() )
    {
        DcmStack stack;
        if ( this->search( DCM_NumberOfReferences,
                           stack, ESM_fromHere, OFFalse ) == EC_Normal )
        {
            if ( stack.top()->ident() == EVR_UL )
                errorFlag = ((DcmUnsignedLong*)stack.top())->
                        getUint32(localRefNum);
        }
    }
    return localRefNum;
}


// ********************************


Uint32 DcmDirectoryRecord::increaseRefNum()
{
    if ( DirRecordType == ERT_Mrdr )
    {
        if ( numberOfReferences == 0L )
            this->setRecordInUseFlag( 0xffff );         // aktiviere Record
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
                this->setRecordInUseFlag( 0x0000 );     // deaktiviere Record
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

//
// Es werden alle Datenelemente gemaess Tabelle B.3.2.2-1 DICOM part 10
// erzeugt.
// Wird ein gueltiger Dateiname (referencedFileID) fuer eine SOP-Instanz
// uebergeben, so werden alle benoetigten keys aus dieser Datei ausgelesen:
//   Das Datenelement MRDRDirectoryRecordOffset entfaellt.
//   directFromFile == OFTrue
//
// Ist referencedFileID ein NULL-Zeiger, so wird versucht, die keys indirekt
// ueber den referenzierten MRDR zu besorgen.
//   Das Datenelement ReferencedFileID bekommt die Laenge 0.
//   indirectViaMRDR == OFTrue
//
// Existiert kein Verweis auf einen MRDR, so wird davon ausgegangen, das auch
// keine SOP-Instanz mit dem aktuellen Directory Record verknuepft werden soll:
//   Die Datenelemente ReferencedFileID, MRDRDirectoryRecordOffset,
//   ReferencedSOPClassUIDinFile und ReferencedSOPInstanceUIDinFile entfallen.
//   !directFromFile && !indirectViaMRDR
//

E_Condition DcmDirectoryRecord::fillElementsAndReadSOP(
    const char * referencedFileID,
    const char * sourceFileName)
{
    E_Condition l_error = EC_Normal;
    char *fileName = NULL;
    DcmFileStream * fileStream = NULL;
    DcmFileFormat *refFile = (DcmFileFormat*)NULL;

    OFBool directFromFile = OFFalse;
    OFBool indirectViaMRDR = OFFalse;
    if (referencedFileID != (char*)NULL && *referencedFileID != '\0' )
        directFromFile = OFTrue;
    else if (DirRecordType != ERT_Mrdr &&       // fuer MRDR verboten
             referencedMRDR != NULL)
    {
        indirectViaMRDR = OFTrue;
        referencedFileID = referencedMRDR->lookForReferencedFileID();
    }

    if ( referencedFileID != (char*)NULL && *referencedFileID != '\0' )
    {
        if (!sourceFileName)
        {
            fileName = new char[ strlen( referencedFileID ) + 2 ];
            buildFileName( referencedFileID, fileName );
        }
        else
        {
            fileName = new char[strlen(sourceFileName)+1];
            strcpy(fileName, sourceFileName);
        }

        fileStream = new DcmFileStream(fileName, DCM_ReadMode);
        if (!fileStream || fileStream->GetError() != EC_Normal )
        {
            cerr << "Error: DcmDirectoryRecord::readSOPandFileElements():"
                " DicomFile \"" << fileName << "\" not found." << endl;
            l_error = EC_InvalidStream;
            directFromFile = OFFalse;
            indirectViaMRDR = OFFalse;
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
        directFromFile = OFFalse;
        indirectViaMRDR = OFFalse;
    }
    DcmStack stack;
    DcmUnsignedLongOffset *uloP;
    DcmUniqueIdentifier *uiP;

    DcmTag nextOffTag( DCM_OffsetOfTheNextDirectoryRecord ); // (0004,1400)
    uloP = new DcmUnsignedLongOffset( nextOffTag );
    uloP->putUint32(Uint32(0));
    if ( insert( uloP, OFFalse ) != EC_Normal )
        delete uloP;

    this->setRecordInUseFlag( 0xffff );                     // (0004,1410)

    DcmTag lowerOffTag( DCM_OffsetOfReferencedLowerLevelDirectoryEntity );
    uloP = new DcmUnsignedLongOffset( lowerOffTag );        // (0004,1420)
    uloP->putUint32(Uint32(0));
    if ( insert( uloP, OFFalse ) != EC_Normal )
        delete uloP;

    this->setRecordType( DirRecordType );                   // (0004,1430)

    DcmTag privRecTag( DCM_PrivateRecordUID );              // (0004,1432)
    if ( DirRecordType == ERT_Private )
    {
        uiP = new DcmUniqueIdentifier( privRecTag );
        if ( insert( uiP, OFFalse ) != EC_Normal )
            delete uiP;
    }
    else
        delete this->remove( privRecTag );

    if ( directFromFile )                                   // (0004,1500)
        this->setReferencedFileID( referencedFileID );
    else
    {
        DcmTag refFileTag( DCM_ReferencedFileID );
        delete this->remove( refFileTag );
    }

    DcmTag mrdrOffTag( DCM_MRDRDirectoryRecordOffset );         // (0004,1504)
    if ( indirectViaMRDR )
    {
        // erzeuge Zeiger-Attribut auf MRDR
        DcmUnsignedLongOffset *uloP2 = new DcmUnsignedLongOffset( mrdrOffTag );
        uloP2->putUint32(Uint32(0));
        uloP2->setNextRecord( referencedMRDR );
        insert( uloP2, OFTrue );
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
            char *uid = NULL;
            ((DcmUniqueIdentifier*)stack.top())->getString(uid);
            uiP->putString( uid );
        }
        else
        {
            cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP(): "
                "I can't find DCM_SOPClassUID in Dataset ["
                 << fileName << "] !" << endl;
            l_error = EC_CorruptedData;
        }
        insert( uiP, OFTrue );

        uiP = new DcmUniqueIdentifier( refSOPInstTag );     // (0004,1511)
        if (    refFile->search( DCM_SOPInstanceUID, stack )
                == EC_Normal
                || refFile->search( DCM_MediaStorageSOPInstanceUID, stack )
                == EC_Normal
            )
        {
            char *uid = NULL;
            ((DcmUniqueIdentifier*)stack.top())->getString(uid);
            uiP->putString( uid );
        }
        else
        {
            cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP():"
                " I can't find DCM_SOPInstanceUID neither in Dataset"
                " or MetaInfo of file ["
                 << fileName << "] !" << endl;
            l_error = EC_CorruptedData;
        }
        insert( uiP, OFTrue );

        uiP = new DcmUniqueIdentifier( refFileXferTag );     // (0004,1512)
        if ( refFile->search( DCM_TransferSyntaxUID, stack )
             == EC_Normal )
        {
            char *uid = NULL;
            ((DcmUniqueIdentifier*)stack.top())->getString(uid);
            uiP->putString( uid );
        }
        else
        {
            cerr << "Error: DcmDirectoryRecord::fillElementsAndReadSOP():"
                " I can't find DCM_TransferSyntaxUID in MetaInfo of file ["
                 << fileName << "] !" << endl;
            l_error = EC_CorruptedData;
        }
        insert( uiP, OFTrue );
    }
    else  // nicht nur dann: if ( DirRecordType == ERT_Mrdr )
    {
        // entferne SOP-UIDs aus Directory Record
        delete this->remove( refSOPClassTag );
        delete this->remove( refSOPInstTag );
        delete this->remove( refFileXferTag );
    }

    if ( refFile != (DcmFileFormat*)NULL )
        delete refFile;
    if (fileStream != NULL)
        delete fileStream;
    if ( fileName != (char*)NULL)
        delete[] fileName;

    return l_error;
}


// ********************************


E_Condition DcmDirectoryRecord::masterInsertSub(DcmDirectoryRecord* dirRec,
                                                                                                const unsigned long where)
{
    // Einfuegen ohne Typ-Ueberpruefung
    errorFlag = lowerLevelList->insert( dirRec, where );
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::purgeReferencedFile()
{
    E_Condition l_error = EC_Normal;
    if ( DirRecordType != ERT_root )
    {
        char *localFileName = (char*)NULL;

        // loesche Dateinamen aus directory record
        const char *fileName = this->lookForReferencedFileID();
        if ( fileName != (char*)NULL )
        {
            localFileName = new char[ strlen( fileName ) + 2 ];
            buildFileName( fileName, localFileName );
            this->setReferencedFileID( (char*)NULL );
        }
debug(2, ( "DcmDirectoryRecord::purgeReferencedFile() trying to purge file %s from file system", localFileName ));

        if ( localFileName != (char*)NULL )
        {                               // Dateiname vorhanden
            if ( unlink( localFileName ) != 0 )
            {
                l_error = EC_InvalidStream;
                cerr << "Error: DcmDirectoryRecord::purgeReferencedFile()"
                        " cannot purge file ["
                     << localFileName << "] from file system." << endl;
            }
            delete[] localFileName;
        }
        else                            // keine referenzierte Datei vorhanden
            ;  // do nothing
    }
    else
        l_error = EC_IllegalCall;

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


void DcmDirectoryRecord::print(ostream & out, const OFBool showFullData,
                               const int level, const char *pixelFileName,
		                       size_t *pixelCounter)
{
    int i;
    const char *type = DRTypeNames[ DirRecordType ];
    char *info = new char[ strlen(type) + 50 ];

    sprintf( info, "\"Directory Record\" %s #=%ld ", type, (long)card() );
    printInfoLine(out, showFullData, level, info );
    delete[] info;
    const char* refFile = this->getReferencedFileName();
    for ( i=1; i<level; i++)
        out << "    ";
    out << "#    offset= $" << this->getFileOffset();
    if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
        out << "   referenced MRDR=$"
             << referencedMRDR->getFileOffset();
    if ( DirRecordType == ERT_Mrdr )
        out << "   No of references="
             << numberOfReferences;
    if ( refFile != (char*)NULL )
        out << "   refFileID=\"" << refFile << "\"";
    out << endl;
    if ( !elementList->empty() )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            dO->print(out, showFullData, level + 1, pixelFileName, pixelCounter);
        } while ( elementList->seek( ELP_next ) );
    }
    if ( lowerLevelList->card() > 0 )
        lowerLevelList->print(out, showFullData, level + 1);
    DcmTag delimItemTag( DCM_ItemDelimitationItem );
    if ( Length == DCM_UndefinedLength )
        printInfoLine(out, showFullData, level, delimItemTag,
                      0, "\"ItemDelimitationItem\"" );
    else
        printInfoLine(out, showFullData, level, delimItemTag,
                      0, "\"ItemDelimitationItem for re-encoding\"" );
}


// ********************************


E_Condition DcmDirectoryRecord::read(DcmStream & inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else {

        if(fTransferState != ERW_ready) {
            DcmXfer xferSyn(xfer);
            errorFlag = DcmItem::read(inStream, xfer, glenc, maxReadLength);
            /*
            ** Remember the actual file offset for this Directory Record.  
            ** Compute by subtracting the Item header (tag & length fields)
            ** from the start position of data within the Item (fStartPosition).
            ** fStartPosition is set in DcmItem::read(...)
            ** offsetInFile is used in the print(...) method.
            */
            offsetInFile = fStartPosition - xferSyn.sizeofTagHeader(Tag.getEVR());
        }
        
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


E_Condition DcmDirectoryRecord::verify(const OFBool autocorrect )
{
    E_Condition err1, err2;
    errorFlag = EC_Normal;
    if ( autocorrect == OFTrue && DirRecordType != ERT_root )
        errorFlag = fillElementsAndReadSOP(this->getReferencedFileName(), NULL);

    err1 = DcmItem::verify( autocorrect );
    err2 = lowerLevelList->verify( autocorrect );
    if (    errorFlag == EC_Normal
         && (    err1 != EC_Normal
              || err2 != EC_Normal
            )
       )
        errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::search( const DcmTagKey &tag,
                                        DcmStack &resultStack,
                                        E_SearchMode mode,
                                        OFBool searchIntoSub )
{
    E_Condition l_error = DcmItem::search( tag,
                                           resultStack,
                                           mode,
                                           searchIntoSub );
    if ( l_error != EC_Normal )
    {
debug(5, ( "DcmDirectoryRecord::search() no element found: err(%d, %s). now searching in lowerLevelList",
           l_error, dcmErrorConditionToString(l_error) ));

        if ( mode != ESM_afterStackTop || resultStack.top() == this )
            resultStack.push( lowerLevelList );
        l_error = lowerLevelList->search( tag,
                                          resultStack,
                                          mode,
                                          searchIntoSub );
        if ( l_error != EC_Normal )
            resultStack.pop();
    }
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


E_Condition DcmDirectoryRecord::assignToSOPFile(
    const char * referencedFileID,
    const char * sourceFileName)
{
    errorFlag = EC_Normal;

    if ( DirRecordType != ERT_root )
    {
debug(2, ( "DcmDirectoryRecord::assignToSOPFile() old Referenced File ID was %s", this->getReferencedFileName() ));
debug(2, ( "new Referenced File ID is  %s", referencedFileID ));

        // aktualisiere geg. den alten Referenz-Zaehler
        if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
            referencedMRDR->decreaseRefNum();
        referencedMRDR = (DcmDirectoryRecord*)NULL;

        errorFlag = this->fillElementsAndReadSOP(
            referencedFileID, sourceFileName);
    }
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::assignToMRDR( DcmDirectoryRecord *mrdr )
{
    errorFlag = EC_Normal;
    if (   DirRecordType != ERT_root
        && mrdr != (DcmDirectoryRecord*)NULL   // neuer MRDR vorhanden
        && mrdr != referencedMRDR              // alter MRDR != neuer MRDR
       )
    {
debug(2, ( "DcmDirectoryRecord::assignToMRDR() old Referenced File ID was %s", this->getReferencedFileName() ));
debug(2, ( "new Referenced File ID is  %s", mrdr->lookForReferencedFileID() ));

        // setze internen Zeiger auf mrdr und aktualisiere geg. den alten Wert
        if ( referencedMRDR != (DcmDirectoryRecord*)NULL )
            referencedMRDR->decreaseRefNum();
        referencedMRDR = mrdr;

        // erhoehe Referenz-Zaehler in referencedMRDR um einen
        referencedMRDR->increaseRefNum();

        // setze Laenge der Referenced File ID auf Null und fuelle Datenelemente
        errorFlag = this->fillElementsAndReadSOP(NULL, NULL);
    }
    else
        errorFlag = EC_IllegalCall;

    return errorFlag;
}


// ********************************

unsigned long DcmDirectoryRecord::cardSub()
{
    return lowerLevelList->card();
}


// ********************************


E_Condition DcmDirectoryRecord::insertSub( DcmDirectoryRecord* dirRec,
                                           unsigned long where,
                                           OFBool before )
{
    if ( dirRec != (DcmDirectoryRecord*)NULL )
    {
        if (    checkHierarchy( DirRecordType, dirRec->DirRecordType )
             == EC_Normal )
            errorFlag = lowerLevelList->insert( dirRec, where, before );
        else
        {
            errorFlag = EC_IllegalCall;
debug(1, ( "DcmDirectoryRecord::insertSub() dcdirrec: ( %s -> %s ) hierarchy not allowed.",
           DRTypeNames[ this->getRecordType() ],
           DRTypeNames[ dirRec->getRecordType() ] ));

        }
    }

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
    DcmDirectoryRecord *subDirRec = (DcmDirectoryRecord*)
                                                lowerLevelList->remove( num );
    errorFlag = lowerLevelList->error();
    if ( subDirRec != (DcmDirectoryRecord*)NULL )
    {
        DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

        if ( localSubRefMRDR != (DcmDirectoryRecord*)NULL )
        {                                   // Datei wird indirekt referenziert
            localSubRefMRDR->decreaseRefNum();
        }
        else                                // Datei direkt entfernen
            errorFlag = subDirRec->purgeReferencedFile();
debug(2, ( "DcmDirectoryRecord::deleteSubAndPurgeFile() now purging lower records:" ));

        while ( subDirRec->cardSub() > 0 )  // alle sub sub records loeschen
            subDirRec->deleteSubAndPurgeFile((unsigned long)(0));
        delete subDirRec;                   // loesche sub directory record
    }
    return errorFlag;
}


// ********************************


E_Condition DcmDirectoryRecord::deleteSubAndPurgeFile( DcmDirectoryRecord* dirRec )
{
    DcmDirectoryRecord *subDirRec = (DcmDirectoryRecord*)
                                              lowerLevelList->remove( dirRec );
    errorFlag = lowerLevelList->error();
    if ( subDirRec != (DcmDirectoryRecord*)NULL )
    {
        DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

        if ( localSubRefMRDR != (DcmDirectoryRecord*)NULL )
        {                                   // Datei wird indirekt referenziert
            localSubRefMRDR->decreaseRefNum();
        }
        else                                // Datei direkt entfernen
            errorFlag = subDirRec->purgeReferencedFile();
debug(2, ( "DcmDirectoryRecord::deleteSubAndPurgeFile() now purging lower records:" ));

        while ( subDirRec->cardSub() > 0 )  // alle sub sub records loeschen
            subDirRec->deleteSubAndPurgeFile((unsigned long)(0));
        delete subDirRec;                   // loesche sub directory record
    }

    return errorFlag;
}


E_Condition DcmDirectoryRecord::clearSub()
{
    errorFlag = lowerLevelList->clear();
    return errorFlag;
}

void DcmDirectoryRecord::setRecordsOriginFile(const char* fname)
{
    if (recordsOriginFile != NULL) {
        delete[] recordsOriginFile;
    }
    if (fname != NULL) {
        recordsOriginFile = new char[strlen(fname) + 1];
        strcpy(recordsOriginFile, fname);
    } else {
        recordsOriginFile = NULL;
    }
}

const char* 
DcmDirectoryRecord::getRecordsOriginFile()
{
    return recordsOriginFile;
}

/*
 * CVS/RCS Log:
 * $Log: dcdirrec.cc,v $
 * Revision 1.31  2000-02-23 15:11:50  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.30  2000/02/10 10:52:18  joergr
 * Added new feature to dcmdump (enhanced print method of dcmdata): write
 * pixel data/item value fields to raw files.
 *
 * Revision 1.29  2000/02/03 11:49:07  meichel
 * Updated dcmgpdir to new directory record structure in letter ballot text
 *   of Structured Report.
 *
 * Revision 1.28  2000/02/02 14:32:50  joergr
 * Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
 *
 * Revision 1.27  2000/02/01 10:12:05  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.26  1999/03/31 09:25:25  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
