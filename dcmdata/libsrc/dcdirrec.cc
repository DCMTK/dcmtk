/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmDirectoryRecord
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcspchrs.h"

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>       /* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif


// ********************************


static const char *DRTypeNames[] =
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
    "SR DOCUMENT", /* was "STRUCT REPORT" in old frozen draft */
    "PRESENTATION",
    "WAVEFORM",
    "RT DOSE",
    "RT STRUCTURE SET",
    "RT PLAN",
    "RT TREAT RECORD",
    "STORED PRINT",
    "KEY OBJECT DOC",
    "REGISTRATION",
    "FIDUCIAL",
    "RAW DATA",
    "SPECTROSCOPY",
    "ENCAP DOC",
    "VALUE MAP",
    "HANGING PROTOCOL",
    "STEREOMETRIC",
    "HL7 STRUC DOC",
    "PALETTE",
    "SURFACE",
    "MEASUREMENT",
    "IMPLANT",
    "IMPLANT GROUP",
    "IMPLANT ASSY",
    "PLAN",
    "SURFACE SCAN",
    "TRACT",
    "ASSESSMENT",
    "RADIOTHERAPY"
};

static const short DIM_OF_DRTypeNames = OFstatic_cast(short, (sizeof(DRTypeNames) / sizeof(DRTypeNames[0])));


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord()
  : DcmItem(DCM_ItemTag),
    recordsOriginFile(),
    lowerLevelList(new DcmSequenceOfItems(DCM_DirectoryRecordSequence)),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmTag &tag,
                                       const Uint32 len)
  : DcmItem(tag, len),
    recordsOriginFile(),
    lowerLevelList(new DcmSequenceOfItems(DCM_DirectoryRecordSequence)),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const E_DirRecType recordType,
                                       const char *referencedFileID,
                                       const OFFilename &sourceFilename,
                                       DcmFileFormat *fileFormat)
  : DcmItem(DCM_ItemTag),
    recordsOriginFile(),
    lowerLevelList(new DcmSequenceOfItems(DCM_DirectoryRecordSequence)),
    DirRecordType(recordType),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    setRecordsOriginFile(sourceFilename);

    if (DirRecordType != ERT_root)
        errorFlag = fillElementsAndReadSOP(referencedFileID, sourceFilename, fileFormat);
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const char *recordTypeName,
                                       const char *referencedFileID,
                                       const OFFilename &sourceFilename,
                                       DcmFileFormat *fileFormat)
  : DcmItem(DCM_ItemTag),
    recordsOriginFile(),
    lowerLevelList(new DcmSequenceOfItems(DCM_DirectoryRecordSequence)),
    DirRecordType(ERT_Private),
    referencedMRDR(NULL),
    numberOfReferences(0),
    offsetInFile(0)
{
    DirRecordType = recordNameToType(recordTypeName);
    setRecordsOriginFile(sourceFilename);

    if (DirRecordType != ERT_root)
        errorFlag = fillElementsAndReadSOP(referencedFileID, sourceFilename, fileFormat);
}


// ********************************


DcmDirectoryRecord::DcmDirectoryRecord(const DcmDirectoryRecord &old)
  : DcmItem(old),
    recordsOriginFile(old.recordsOriginFile),
    lowerLevelList(new DcmSequenceOfItems(*old.lowerLevelList)),
    DirRecordType(old.DirRecordType),
    referencedMRDR(old.referencedMRDR),  // copies a pointer!
    numberOfReferences(old.numberOfReferences),
    offsetInFile(old.offsetInFile)
{
}


// ********************************


DcmDirectoryRecord& DcmDirectoryRecord::operator=(const DcmDirectoryRecord& obj)
{
    if (this != &obj)
    {
        // copy parent's member variables
        DcmItem::operator=(obj);
        // copy DcmDirectoryRecords' member variables
        recordsOriginFile = obj.recordsOriginFile;
        lowerLevelList = new DcmSequenceOfItems(*obj.lowerLevelList);
        DirRecordType = obj.DirRecordType;
        referencedMRDR = obj.referencedMRDR;  // copies a pointer!
        numberOfReferences = obj.numberOfReferences;
        offsetInFile = obj.offsetInFile;
    }
    return *this;
}


// ********************************


OFCondition DcmDirectoryRecord::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmDirectoryRecord &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmDirectoryRecord::~DcmDirectoryRecord()
{
    delete lowerLevelList;
}


// ********************************


E_DirRecType DcmDirectoryRecord::recordNameToType(const char *recordTypeName)
{
    E_DirRecType recType = ERT_Private;
    if (recordTypeName != NULL)
    {
        short i = 0;
        while (i < DIM_OF_DRTypeNames && strcmp(DRTypeNames[i], recordTypeName) != 0)
            i++;

        if (i < DIM_OF_DRTypeNames && strcmp(DRTypeNames[i], recordTypeName) == 0)
            recType = OFstatic_cast(E_DirRecType, i);
        else if (strcmp(recordTypeName,"STRUCT REPORT") == 0)
            recType = ERT_SRDocument; // we recognize the old name as well
        DCMDATA_TRACE("DcmDirectoryRecord::recordNameToType() input char*=\"" << recordTypeName
            << "\" output enum=" << recType);
    }
    return recType;
}


// ********************************


char *DcmDirectoryRecord::buildFileName(const char *origName,
                                        char *destName,
                                        size_t len) const
{
    const char *from = origName;
    char *to = destName;
    char c;
    char lastchar = '\0';
    while ((c = *from++) != 0)
    {
        if (c == '\\')
        {
            if (lastchar != '\\')      // eliminate double '\\'
                *to++ = PATH_SEPARATOR;
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
    if ((f = fopen(destName, "rb")) != NULL)
    {
        fclose(f);
    } else {
        size_t buflen = strlen(destName) + 2;
        char* newname = new char[buflen];
        OFStandard::strlcpy(newname, destName, buflen);
        OFStandard::strlcat(newname, ".", buflen);
        if ((f = fopen(newname, "rb")) != NULL)
        {
            fclose(f);
            OFStandard::strlcpy(destName, newname, len);
        } else {
            /* we cannot find the file. let the caller deal with this */
        }
        delete[] newname;
    }
    return destName;
}


// ********************************


OFCondition DcmDirectoryRecord::checkHierarchy(const E_DirRecType upperRecord,
                                               const E_DirRecType lowerRecord)
{
    OFCondition l_error = EC_IllegalCall;
    switch (upperRecord)
    {
        case ERT_root:
            switch (lowerRecord)
            {
                case ERT_Patient:
                case ERT_PrintQueue:
                case ERT_Topic:
                case ERT_HangingProtocol:
                case ERT_Palette:
                case ERT_Implant:
                case ERT_ImplantGroup:
                case ERT_ImplantAssy:
                case ERT_Private:
                    l_error = EC_Normal;
                    break;
                default:
                    l_error = EC_IllegalCall;
                    break;
            }
            break;
        case ERT_FilmBox:  // retired
            switch (lowerRecord)
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
        case ERT_FilmSession:  // retired
            switch (lowerRecord)
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
            switch (lowerRecord)
            {
                case ERT_Study:
                case ERT_HL7StrucDoc:  // retired
                case ERT_Private:
                    l_error = EC_Normal;
                    break;
                default:
                    l_error = EC_IllegalCall;
                    break;
            }
            break;
        case ERT_PrintQueue:  // retired
            switch (lowerRecord)
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
        case ERT_Results:  // retired
            switch (lowerRecord)
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
            switch (lowerRecord)
            {
                case ERT_Curve:
                case ERT_Image:
                case ERT_ModalityLut:
                case ERT_Overlay:
                case ERT_VoiLut:
                case ERT_SRDocument:
                case ERT_Presentation:
                case ERT_Waveform:
                case ERT_RTDose:
                case ERT_RTStructureSet:
                case ERT_RTPlan:
                case ERT_RTTreatRecord:
                case ERT_StoredPrint:
                case ERT_KeyObjectDoc:
                case ERT_Registration:
                case ERT_Fiducial:
                case ERT_RawData:
                case ERT_Spectroscopy:
                case ERT_EncapDoc:
                case ERT_ValueMap:
                case ERT_Stereometric:
                case ERT_Surface:
                case ERT_Measurement:
                case ERT_Plan:
                case ERT_SurfaceScan:
                case ERT_Tract:
                case ERT_Assessment:
                case ERT_Radiotherapy:
                case ERT_Private:
                    l_error = EC_Normal;
                    break;
                default:
                    l_error = EC_IllegalCall;
                    break;
            }
            break;
        case ERT_Study:
            switch (lowerRecord)
            {
                case ERT_FilmSession:
                case ERT_Results:
                case ERT_Series:
                case ERT_StudyComponent:
                case ERT_Visit:
                case ERT_Private:
                    l_error = EC_Normal;
                    break;
                default:
                    l_error = EC_IllegalCall;
                    break;
            }
            break;
        case ERT_Topic:  // retired
            switch (lowerRecord)
            {
                case ERT_Curve:
                case ERT_FilmSession:
                case ERT_Image:
                case ERT_ModalityLut:
                case ERT_Overlay:
                case ERT_Series:
                case ERT_Study:
                case ERT_VoiLut:
                case ERT_SRDocument:
                case ERT_Presentation:
                case ERT_Waveform:
                case ERT_RTDose:
                case ERT_RTStructureSet:
                case ERT_RTPlan:
                case ERT_RTTreatRecord:
                case ERT_StoredPrint:
                case ERT_KeyObjectDoc:
                case ERT_Registration:
                case ERT_Fiducial:
                case ERT_RawData:
                case ERT_Spectroscopy:
                case ERT_Private:
                    l_error = EC_Normal;
                    break;
                default:
                    l_error = EC_IllegalCall;
                    break;
            }
            break;
        case ERT_Mrdr:  // retired
            l_error = EC_IllegalCall;
            break;
        case ERT_Curve:
        case ERT_Image:
        case ERT_ImageBox:
        case ERT_Interpretation:
        case ERT_ModalityLut:
        case ERT_Overlay:
        case ERT_StudyComponent:
        case ERT_Visit:
        case ERT_VoiLut:
        case ERT_SRDocument:
        case ERT_Presentation:
        case ERT_Waveform:
        case ERT_RTDose:
        case ERT_RTStructureSet:
        case ERT_RTPlan:
        case ERT_RTTreatRecord:
        case ERT_StoredPrint:
        case ERT_KeyObjectDoc:
        case ERT_Registration:
        case ERT_Fiducial:
        case ERT_RawData:
        case ERT_Spectroscopy:
        case ERT_EncapDoc:
        case ERT_ValueMap:
        case ERT_HangingProtocol:
        case ERT_Stereometric:
        case ERT_HL7StrucDoc:  // retired
        case ERT_Palette:
        case ERT_Surface:
        case ERT_Measurement:
        case ERT_Implant:
        case ERT_ImplantGroup:
        case ERT_ImplantAssy:
        case ERT_Plan:
        case ERT_SurfaceScan:
        case ERT_Tract:
        case ERT_Assessment:
        case ERT_Radiotherapy:
        case ERT_Private:
            switch (lowerRecord)
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


OFCondition DcmDirectoryRecord::setRecordType(E_DirRecType newType)
{
    OFCondition l_error = EC_Normal;

    DcmTag dirRecTag(DCM_DirectoryRecordType);
    DcmCodeString *csP = new DcmCodeString(dirRecTag);
    csP->putString(DRTypeNames[newType]);
    insert(csP, OFTrue);

    return l_error;
}


// ********************************



E_DirRecType DcmDirectoryRecord::lookForRecordType()
{
    E_DirRecType localType = ERT_Private;
    if (!elementList->empty())
    {
        DcmStack stack;
        if (search(DCM_DirectoryRecordType, stack, ESM_fromHere, OFFalse).good())
        {
            if (stack.top()->ident() == EVR_CS)
            {
                char *recName = NULL;
                DcmCodeString *recType = OFstatic_cast(DcmCodeString *, stack.top());
                recType->verify(OFTrue);            // force dealignment
                recType->getString(recName);
                localType = recordNameToType(recName);

                DCMDATA_TRACE("DcmDirectoryRecord::lookForRecordType() RecordType Element "
                    << recType->getTag() << " Type = " << DRTypeNames[DirRecordType]);
            }
        }
    }
    return localType;
}


// ********************************

static void hostToDicomFilename(char *fname)
{
    /*
    ** Massage filename into DICOM format.
    ** Eliminate any invalid characters.
    ** Most commonly there is a '.' at the end of a filename.
    */
    size_t len = strlen(fname);
    int k = 0;
    unsigned char c = '\0';
    for (size_t i = 0; i < len; ++i)
    {
        c = OFstatic_cast(unsigned char, fname[i]);
        /* the PATH_SEPARATOR depends on the OS (see <osconfig.h>) */
        if (c == PATH_SEPARATOR)
        {
            fname[k++] = '\\';
        } else if (isalpha(c) || isdigit(c) || (c == '_') || (c == '\\')) {
            /* filenames in DICOM must always be in uppercase */
            fname[k++] = OFstatic_cast(char, toupper(c));
        }
    }
    fname[k] = '\0';
}


OFCondition DcmDirectoryRecord::setReferencedFileID(const char *referencedFileID)
{
    OFCondition l_error = EC_Normal;

    size_t bufsize = strlen(referencedFileID) + 1;
    char* newFname = new char[bufsize];
    OFStandard::strlcpy(newFname, referencedFileID, bufsize);
    hostToDicomFilename(newFname);

    DcmTag refFileTag(DCM_ReferencedFileID);
    DcmCodeString *csP = new DcmCodeString(refFileTag);
    if (referencedFileID != NULL)
        csP->putString(newFname);
    insert(csP, OFTrue);

    delete[] newFname;
    return l_error;
}


// ********************************


const char *DcmDirectoryRecord::lookForReferencedFileID()
{
    char *localFile = NULL;
    if (!elementList->empty())
    {
        DcmStack stack;
        if (search(DCM_ReferencedFileID, stack, ESM_fromHere, OFFalse).good())
        {
            if (stack.top()->ident() == EVR_CS)
            {
                DcmCodeString *refFile = OFstatic_cast(DcmCodeString *, stack.top());
                refFile->verify(OFTrue);          // force dealignment
                refFile->getString(localFile);
                if (localFile != NULL && *localFile == '\0')
                    localFile = NULL;
            }
        }
    }
    DCMDATA_TRACE("DcmDirectoryRecord::lookForReferencedFileID() ReferencedFileID = "
        << ((localFile) ? localFile : ""));

    return localFile;
}


// ********************************


DcmDirectoryRecord *DcmDirectoryRecord::lookForReferencedMRDR()
{
    DcmDirectoryRecord *localMRDR = NULL;
    if (!elementList->empty())
    {
        DcmStack stack;
        if (search(DCM_RETIRED_MRDRDirectoryRecordOffset, stack, ESM_fromHere, OFFalse).good())
        {
            if (stack.top()->ident() == EVR_up)
            {
                DcmUnsignedLongOffset *offElem;
                offElem = OFstatic_cast(DcmUnsignedLongOffset *, stack.top());
                localMRDR = OFstatic_cast(DcmDirectoryRecord *, offElem->getNextRecord());
#ifdef DEBUG
                Uint32 l_uint = 0;
                offElem->getUint32(l_uint);
                DCMDATA_TRACE("DcmDirectoryRecord::lookForReferencedMRDR() MRDR Offset Element "
                    << offElem->getTag() << " offs=0x"
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                    << STD_NAMESPACE setw(8) << l_uint
                    << " p=" << OFstatic_cast(void *, offElem)
                    << " n=" << OFstatic_cast(void *, localMRDR));
#endif
            }
        }
    }
    if (localMRDR == NULL)
        DCMDATA_TRACE("DcmDirectoryRecord::lookForReferencedMRDR() no ReferencedMRDR found");

    return localMRDR;
}


// ********************************


const char *DcmDirectoryRecord::getReferencedFileName()
{
    const char *localFile = NULL;
    if (referencedMRDR != NULL)
        localFile = referencedMRDR->lookForReferencedFileID();
    else
        localFile = lookForReferencedFileID();
    return localFile;
}


// ********************************


OFCondition DcmDirectoryRecord::setRecordInUseFlag(const Uint16 newFlag)
{
    OFCondition l_error = EC_Normal;

    DcmTag recInUseTag(DCM_RecordInUseFlag);
    DcmUnsignedShort *usP = new DcmUnsignedShort(recInUseTag);
    usP->putUint16(newFlag);
    insert(usP, OFTrue);

    return l_error;
}


// ********************************


Uint16 DcmDirectoryRecord::lookForRecordInUseFlag()
{
    Uint16 localFlag = Uint16(0xffff);     // default value: record is in use
    if (!elementList->empty())
    {
        DcmStack stack;
        if (search(DCM_RecordInUseFlag, stack, ESM_fromHere, OFFalse).good())
        {
            if (stack.top()->ident() == EVR_US)
                errorFlag = OFstatic_cast(DcmUnsignedShort *, stack.top())->getUint16(localFlag);
        }
    }
    return localFlag;
}


// ********************************


Uint32 DcmDirectoryRecord::getFileOffset() const
{
    return offsetInFile;
}


// ********************************


Uint32 DcmDirectoryRecord::setFileOffset(Uint32 position)
{
    offsetInFile = position;
    return position;
}


// ********************************


OFCondition DcmDirectoryRecord::setNumberOfReferences(Uint32 newRefNum)
{
    OFCondition l_error = EC_Normal;
    if (DirRecordType == ERT_Mrdr)
    {
        // insert new value
        DcmTag numRefTag(DCM_RETIRED_NumberOfReferences);
        DcmUnsignedLong *newUL = new DcmUnsignedLong(numRefTag);
        newUL->putUint32(newRefNum);
        insert(newUL, OFTrue);
    } else {
        errorFlag = EC_IllegalCall;
        DCMDATA_ERROR("illegal usage of DcmDirectoryRecord::setNumberOfReferences() - RecordType must be MRDR");
    }
    return l_error;
}


// ********************************


Uint32 DcmDirectoryRecord::lookForNumberOfReferences()
{
    Uint32 localRefNum = 0L;
    if (!elementList->empty())
    {
        DcmStack stack;
        if (search(DCM_RETIRED_NumberOfReferences, stack, ESM_fromHere, OFFalse).good())
        {
            if (stack.top()->ident() == EVR_UL)
                errorFlag = OFstatic_cast(DcmUnsignedLong *, stack.top())->getUint32(localRefNum);
        }
    }
    return localRefNum;
}


// ********************************


Uint32 DcmDirectoryRecord::increaseRefNum()
{
    if (DirRecordType == ERT_Mrdr)
    {
        if (numberOfReferences == 0L)
            setRecordInUseFlag(0xffff);         // activate record
        numberOfReferences++;
        errorFlag = setNumberOfReferences(numberOfReferences);
    } else {
        errorFlag = EC_IllegalCall;
        DCMDATA_ERROR("illegal usage of DcmDirectoryRecord::increaseRefNum() - RecordType must be MRDR");
    }
    return numberOfReferences;
}


// ********************************


Uint32 DcmDirectoryRecord::decreaseRefNum()
{
    if (DirRecordType == ERT_Mrdr)
    {
        if (numberOfReferences > 0)
        {
            numberOfReferences--;
            if (numberOfReferences == 0L)
                setRecordInUseFlag(0x0000);     // deactivate record
            errorFlag = setNumberOfReferences(numberOfReferences);
        } else {
            errorFlag = EC_IllegalCall;
            DCMDATA_WARN("DcmDirectoryRecord::decreaseRefNum() attempt to decrease value lower than zero");
        }
    } else {
        errorFlag = EC_IllegalCall;
        DCMDATA_ERROR("illegal usage of DcmDirectoryRecord::decreaseRefNum() - RecordType must be MRDR");
    }
    return numberOfReferences;
}



// ********************************

// --- IGNORE THE FOLLOWING GERMAN COMMENT ---
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

OFCondition DcmDirectoryRecord::fillElementsAndReadSOP(const char *referencedFileID,
                                                       const OFFilename &sourceFileName,
                                                       DcmFileFormat *fileFormat)
{
    OFCondition l_error = EC_Normal;
    OFFilename fileName;
    DcmFileFormat *refFile = NULL;
    /* This variable is only set if we created our own DcmFileFormat instance */
    DcmFileFormat *ownFile = NULL;

    if (fileFormat != NULL && sourceFileName.isEmpty())
        return EC_IllegalCall;

    OFBool directFromFile = OFFalse;
    OFBool indirectViaMRDR = OFFalse;
    if (referencedFileID != NULL && *referencedFileID != '\0')
        directFromFile = OFTrue;
    else if (DirRecordType != ERT_Mrdr && referencedMRDR != NULL)     // illegal for MRDR
    {
        indirectViaMRDR = OFTrue;
        referencedFileID = referencedMRDR->lookForReferencedFileID();
    }

    if (referencedFileID != NULL && *referencedFileID != '\0')
    {
        if (sourceFileName.isEmpty())
        {
            /* create a new source filename */
            size_t bufsize = strlen(referencedFileID) + 2;
            char *newname = new char[bufsize];
            buildFileName(referencedFileID, newname, bufsize);
            fileName.set(newname);
            delete[] newname;
        } else {
            /* just copy the source filename */
            fileName = sourceFileName;
        }

        if (DirRecordType != ERT_Mrdr)
        {
            if (fileFormat)
            {
                DCMDATA_TRACE("DcmDirectoryRecord::fillElementsAndReadSOP(): Using existing file format for \"" << fileName << "\".");
                refFile = fileFormat;
            } else {
                DCMDATA_TRACE("DcmDirectoryRecord::fillElementsAndReadSOP(): Load file \"" << fileName << "\" because our caller didn't do so.");
                ownFile = new DcmFileFormat();
                l_error = ownFile->loadFile(fileName);
                refFile = ownFile;
                if (l_error.bad())
                {
                  DCMDATA_ERROR("DcmDirectoryRecord::fillElementsAndReadSOP(): DicomFile \""
                      << fileName << "\" not found");
                  directFromFile = OFFalse;
                  indirectViaMRDR = OFFalse;
                }
            }
        }
    } else {
        directFromFile = OFFalse;
        indirectViaMRDR = OFFalse;
    }

    DcmStack stack;
    DcmUnsignedLongOffset *uloP;
    DcmUniqueIdentifier *uiP;

    DcmTag nextOffTag(DCM_OffsetOfTheNextDirectoryRecord); // (0004,1400)
    uloP = new DcmUnsignedLongOffset(nextOffTag);
    uloP->putUint32(Uint32(0));
    if (insert(uloP, OFFalse).bad())
        delete uloP;

    setRecordInUseFlag(0xffff);                           // (0004,1410)

    DcmTag lowerOffTag(DCM_OffsetOfReferencedLowerLevelDirectoryEntity);
    uloP = new DcmUnsignedLongOffset(lowerOffTag);        // (0004,1420)
    uloP->putUint32(Uint32(0));
    if (insert(uloP, OFFalse).bad())
        delete uloP;

    setRecordType(DirRecordType);                         // (0004,1430)

    DcmTag privRecTag(DCM_PrivateRecordUID);              // (0004,1432)
    if (DirRecordType == ERT_Private)
    {
        uiP = new DcmUniqueIdentifier(privRecTag);
        if (insert(uiP, OFFalse).bad())
            delete uiP;
    } else
        delete remove(privRecTag);

    if (directFromFile)                                   // (0004,1500)
        setReferencedFileID(referencedFileID);
    else
    {
        DcmTag refFileTag(DCM_ReferencedFileID);
        delete remove(refFileTag);
    }

    DcmTag mrdrOffTag(DCM_RETIRED_MRDRDirectoryRecordOffset);     // (0004,1504)
    if (indirectViaMRDR)
    {
        // create pointer attribute to MRDR
        DcmUnsignedLongOffset *uloP2 = new DcmUnsignedLongOffset(mrdrOffTag);
        uloP2->putUint32(Uint32(0));
        uloP2->setNextRecord(referencedMRDR);
        insert(uloP2, OFTrue);
    } else
        delete remove(mrdrOffTag);

    DcmTag refSOPClassTag(DCM_ReferencedSOPClassUIDInFile);
    DcmTag refSOPInstTag(DCM_ReferencedSOPInstanceUIDInFile);
    DcmTag refFileXferTag(DCM_ReferencedTransferSyntaxUIDInFile);

    if (DirRecordType != ERT_Mrdr && (directFromFile || indirectViaMRDR))
    {
        if (refFile == NULL)
        {
            DCMDATA_ERROR("Internal ERROR in DcmDirectoryRecord::fillElementsAndReadSOP()");
        }
        uiP = new DcmUniqueIdentifier(refSOPClassTag);    // (0004,1510)
        if (refFile->search(DCM_SOPClassUID, stack).good())
        {
            char *uid = NULL;
            OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getString(uid);
            uiP->putString(uid);
        } else {
            DCMDATA_ERROR("DcmDirectoryRecord::fillElementsAndReadSOP(): "
                << "can't find SOPClassUID in Dataset: " << fileName);
            l_error = EC_CorruptedData;
        }
        insert(uiP, OFTrue);

        uiP = new DcmUniqueIdentifier(refSOPInstTag);     // (0004,1511)
        if (refFile->search(DCM_SOPInstanceUID, stack).good() || refFile->search(DCM_MediaStorageSOPInstanceUID, stack).good())
        {
            char *uid = NULL;
            OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getString(uid);
            uiP->putString(uid);
        } else {
            DCMDATA_ERROR("DcmDirectoryRecord::fillElementsAndReadSOP(): "
                << "can't find SOPInstanceUID neither in Dataset or MetaInfo of file: " << fileName);
            l_error = EC_CorruptedData;
        }
        insert(uiP, OFTrue);

        uiP = new DcmUniqueIdentifier(refFileXferTag);     // (0004,1512)
        if (refFile->search(DCM_TransferSyntaxUID, stack).good())
        {
            char *uid = NULL;
            OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getString(uid);
            uiP->putString(uid);
        } else {
            DCMDATA_ERROR("DcmDirectoryRecord::fillElementsAndReadSOP(): "
                << "can't find TransferSyntaxUID in MetaInfo of file: " << fileName);
            l_error = EC_CorruptedData;
        }
        insert(uiP, OFTrue);
    }
    else  // not only in this case: if (DirRecordType == ERT_Mrdr)
    {
        // remove SOP UIDs from Directory Record
        delete remove(refSOPClassTag);
        delete remove(refSOPInstTag);
        delete remove(refFileXferTag);
    }

    delete ownFile;

    return l_error;
}


// ********************************


OFCondition DcmDirectoryRecord::masterInsertSub(DcmDirectoryRecord *dirRec,
                                                const unsigned long where)
{
    // insert without type check
    errorFlag = lowerLevelList->insert(dirRec, where);
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::purgeReferencedFile()
{
    OFCondition l_error = EC_Normal;
    if (DirRecordType != ERT_root)
    {
        char *localFileName = NULL;

        // remove filename from directory record
        const char *fileName = lookForReferencedFileID();
        if (fileName != NULL)
        {
            size_t buflen = strlen(fileName) + 2;
            localFileName = new char[buflen];
            buildFileName(fileName, localFileName, buflen);
            setReferencedFileID(NULL);
        }

        DCMDATA_DEBUG("DcmDirectoryRecord::purgeReferencedFile() trying to purge file "
            << localFileName << " from file system");

        if (localFileName != NULL)
        {                                 // filename exists
            if (unlink(localFileName) != 0)
            {
                OFString buffer = OFStandard::getLastSystemErrorCode().message();
                errorFlag = makeOFCondition(OFM_dcmdata, 19, OF_error, buffer.c_str());
            }
            delete[] localFileName;
        } else {                          // no referenced file exists
            // do nothing
        }
    } else
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


OFCondition DcmDirectoryRecord::convertCharacterSet(const OFString &fromCharset,
                                                    const OFString &toCharset,
                                                    const size_t flags,
                                                    const OFBool updateCharset)
{
    // call the method of the base class; this method is only needed to avoid a compiler warning
    return DcmItem::convertCharacterSet(fromCharset, toCharset, flags, updateCharset);
}


OFCondition DcmDirectoryRecord::convertCharacterSet(const OFString &toCharset,
                                                    const size_t flags,
                                                    const OFBool ignoreCharset)
{
    // call the method of the base class; this method is only needed to avoid a compiler warning
    return DcmItem::convertCharacterSet(toCharset, flags, ignoreCharset);
}


OFCondition DcmDirectoryRecord::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    DCMDATA_DEBUG("DcmDirectoryRecord::convertCharacterSet() processing directory record with offset "
        << getFileOffset());
    // handle special case of DICOMDIR: the Specific Character Set (0008,0005)
    // can be specified individually for each directory record (i.e. item)
    OFCondition status = EC_Normal;
    OFString fromCharset;
    const OFString toCharset = converter.getDestinationCharacterSet();
    // determine value of Specific Character Set (0008,0005) if present in this item
    if (findAndGetOFStringArray(DCM_SpecificCharacterSet, fromCharset, OFFalse /*searchIntoSub*/).good() &&
        (fromCharset != converter.getSourceCharacterSet()))
    {
        DcmSpecificCharacterSet newConverter;
        // create a new character set converter
        DCMDATA_DEBUG("DcmDirectoryRecord::convertCharacterSet() creating a new character set converter for '"
            << fromCharset << "'" << (fromCharset.empty() ? " (ASCII)" : "") << " to '"
            << toCharset << "'" << (toCharset.empty() ? " (ASCII)" : ""));
        // select source and destination character set, use same transliteration mode
        status = newConverter.selectCharacterSet(fromCharset, toCharset);
        if (status.good())
        {
            const unsigned cflags = converter.getConversionFlags();
            if (cflags > 0)
                status = newConverter.setConversionFlags(cflags);
            if (status.good())
            {
                // convert all affected element values in the item with the new converter
                status = DcmItem::convertCharacterSet(newConverter);
                // update the Specific Character Set (0008,0005) element
                updateSpecificCharacterSet(status, newConverter);
            }
        }
    } else {
        // no Specific Character Set attribute or the same character set,
        // so proceed with the given converter
        status = DcmItem::convertCharacterSet(converter);
    }
    return status;
}


// ********************************


void DcmDirectoryRecord::print(STD_NAMESPACE ostream&out,
                               const size_t flags,
                               const int level,
                               const char *pixelFileName,
                               size_t *pixelCounter)
{
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* print record line */
        OFOStringStream oss;
        oss << "\"Directory Record\" (offset=$"
            << getFileOffset() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print item content */
        if (!elementList->empty())
        {
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (elementList->seek(ELP_next));
        }
        if (lowerLevelList->card() > 0)
            lowerLevelList->print(out, flags, level + 1);
   } else {
        /* print record start line */
        OFOStringStream oss;
        oss << "\"Directory Record\" " << DRTypeNames[DirRecordType]
            << " #=" << card() << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print record comment line */
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_INFO;
        printNestingLevel(out, flags, level);
        out << "#  offset=$" << getFileOffset();
        if (referencedMRDR != NULL)
            out << "  refMRDR=$" << referencedMRDR->getFileOffset();
        if (DirRecordType == ERT_Mrdr)
            out << "  refCount=" << numberOfReferences;
        const char *refFile = getReferencedFileName();
        if (refFile != NULL)
            out << "  refFileID=\"" << refFile << "\"";
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
        out << OFendl;
        /* print item content */
        if (!elementList->empty())
        {
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (elementList->seek(ELP_next));
        }
        if (lowerLevelList->card() > 0)
            lowerLevelList->print(out, flags, level + 1);
        /* print record end line */
        DcmTag delimItemTag(DCM_ItemDelimitationItemTag);
        if (getLengthField() == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "\"ItemDelimitationItem\"", &delimItemTag);
        else
            printInfoLine(out, flags, level, "\"ItemDelimitationItem for re-encoding\"", &delimItemTag);
    }
}


// ********************************


OFCondition DcmDirectoryRecord::writeXML(STD_NAMESPACE ostream &out,
                                         const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* in Native DICOM Model, there is no concept of a DICOMDIR */
        l_error = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertToXML, OF_error,
            "Cannot convert Directory Record to Native DICOM Model");
    } else {
        /* XML start tag for "item" */
        out << "<item";
        /* cardinality (number of attributes) = 1..n */
        out << " card=\"" << card() << "\"";
        /* value length in bytes = 0..max (if not undefined) */
        if (getLengthField() != DCM_UndefinedLength)
            out << " len=\"" << getLengthField() << "\"";
        /* byte offset of the record */
        out << " offset=\"" << getFileOffset() << "\"";
        out << ">" << OFendl;
        /* write item content */
        if (!elementList->empty())
        {
            /* write content of all children */
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                l_error = dO->writeXML(out, flags);
            } while (l_error.good() && elementList->seek(ELP_next));
        }
        if (l_error.good())
        {
            if (lowerLevelList->card() > 0)
                lowerLevelList->writeXML(out, flags);
            /* XML end tag for "item" */
            out << "</item>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmDirectoryRecord::read(DcmInputStream &inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (getTransferState() != ERW_ready)
        {
            DcmXfer xferSyn(xfer);
            errorFlag = DcmItem::read(inStream, xfer, glenc, maxReadLength);
            /*
            ** Remember the actual file offset for this Directory Record.
            ** Compute by subtracting the Item header (tag & length fields)
            ** from the start position of data within the Item (fStartPosition).
            ** fStartPosition is set in DcmItem::read(...)
            ** offsetInFile is used in the print(...) method.
            */
            offsetInFile = OFstatic_cast(Uint32, fStartPosition) - xferSyn.sizeofTagHeader(getTag().getEVR());
        }

        if (getTransferState() == ERW_ready && DirRecordType == ERT_Private)     // minimizes multiple evaluation
        {
            DirRecordType = lookForRecordType();
            if (DirRecordType == ERT_Mrdr)
                numberOfReferences = lookForNumberOfReferences();
        }
    }
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::verify(const OFBool autocorrect)
{
    OFCondition err1 = EC_Normal;
    OFCondition err2 = EC_Normal;
    errorFlag = EC_Normal;
    if (autocorrect == OFTrue && DirRecordType != ERT_root)
        errorFlag = fillElementsAndReadSOP(getReferencedFileName(), "");

    err1 = DcmItem::verify(autocorrect);
    err2 = lowerLevelList->verify(autocorrect);
    if (errorFlag.good() && (err1.bad() || err2.bad()))
        errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::search(const DcmTagKey &tag,
                                       DcmStack &resultStack,
                                       E_SearchMode mode,
                                       OFBool searchIntoSub)
{
    OFCondition l_error = DcmItem::search(tag, resultStack, mode, searchIntoSub);
    if (l_error.bad())
    {
        if (mode != ESM_afterStackTop || resultStack.top() == this)
            resultStack.push(lowerLevelList);
        l_error = lowerLevelList->search(tag, resultStack, mode, searchIntoSub);
        if (l_error.bad())
            resultStack.pop();
    }
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


OFCondition DcmDirectoryRecord::assignToSOPFile(const char *referencedFileID,
                                                const OFFilename &sourceFileName)
{
    errorFlag = EC_Normal;

    if (DirRecordType != ERT_root)
    {
        DCMDATA_DEBUG("DcmDirectoryRecord::assignToSOPFile() old Referenced File ID was "
            << getReferencedFileName());
        DCMDATA_DEBUG("new Referenced File ID is " << referencedFileID);

        // update against the old reference counter
        if (referencedMRDR != NULL)
            referencedMRDR->decreaseRefNum();
        referencedMRDR = NULL;

        errorFlag = fillElementsAndReadSOP(referencedFileID, sourceFileName);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::assignToMRDR(DcmDirectoryRecord *mrdr)
{
    errorFlag = EC_Normal;
    if (DirRecordType != ERT_root
        && mrdr != NULL                        // new MRDR available
        && mrdr != referencedMRDR              // old MRDR != new MRDR
      )
    {
        DCMDATA_DEBUG("DcmDirectoryRecord::assignToMRDR() old Referenced File ID was "
            << getReferencedFileName());
        DCMDATA_DEBUG("new Referenced File ID is " << mrdr->lookForReferencedFileID());

        // set internal pointer to mrdr and update against the old value
        if (referencedMRDR != NULL)
            referencedMRDR->decreaseRefNum();
        referencedMRDR = mrdr;

        // increase reference counter in referencedMRDR by one
        referencedMRDR->increaseRefNum();

        // set length of Referenced File ID to zero and fill data elements
        errorFlag = fillElementsAndReadSOP(NULL, "");
    } else
        errorFlag = EC_IllegalCall;

    return errorFlag;
}


// ********************************


unsigned long DcmDirectoryRecord::cardSub() const
{
    return lowerLevelList->card();
}


// ********************************


OFCondition DcmDirectoryRecord::insertSub(DcmDirectoryRecord *dirRec,
                                          unsigned long where,
                                          OFBool before)
{
    if (dirRec != NULL)
    {
        if (checkHierarchy(DirRecordType, dirRec->DirRecordType).good())
            errorFlag = lowerLevelList->insert(dirRec, where, before);
        else
        {
            errorFlag = EC_IllegalCall;
            DCMDATA_DEBUG("DcmDirectoryRecord::insertSub() dcdirrec: ("
                << DRTypeNames[getRecordType()] << " -> "
                << DRTypeNames[dirRec->getRecordType()] << ") hierarchy not allowed");
        }
    }
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::insertSubAtCurrentPos(DcmDirectoryRecord *dirRec,
                                                      OFBool before)
{
    if (dirRec != NULL)
    {
        if (checkHierarchy(DirRecordType, dirRec->DirRecordType).good())
            errorFlag = lowerLevelList->insertAtCurrentPos(dirRec, before);
        else {
            errorFlag = EC_IllegalCall;
            DCMDATA_DEBUG("DcmDirectoryRecord::insertSubAtCurrentPos() dcdirrec: ("
                << DRTypeNames[getRecordType()] << " -> " << DRTypeNames[dirRec->getRecordType()]
                << ") hierarchy not allowed");
        }
    }
    return errorFlag;
}


// ********************************


DcmDirectoryRecord *DcmDirectoryRecord::getSub(const unsigned long num)
{
    DcmDirectoryRecord *retRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->getItem(num));
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


DcmDirectoryRecord *DcmDirectoryRecord::nextSub(const DcmDirectoryRecord *dirRec)
{
    DcmDirectoryRecord *retRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->nextInContainer(dirRec));
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::removeSub(const unsigned long num)
{
    DcmDirectoryRecord *retRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->remove(num));
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


DcmDirectoryRecord* DcmDirectoryRecord::removeSub(DcmDirectoryRecord *dirRec)
{
    DcmDirectoryRecord *retRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->remove(dirRec));
    errorFlag = lowerLevelList->error();
    return retRec;
}


// ********************************


OFCondition DcmDirectoryRecord::deleteSubAndPurgeFile(const unsigned long num)
{
    DcmDirectoryRecord *subDirRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->remove(num));
    errorFlag = lowerLevelList->error();
    if (subDirRec != NULL)
    {
        DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

        if (localSubRefMRDR != NULL)
        {                                   // file is referenced (indirect)
            localSubRefMRDR->decreaseRefNum();
        } else                              // remove file directly
            errorFlag = subDirRec->purgeReferencedFile();

        DCMDATA_DEBUG("DcmDirectoryRecord::deleteSubAndPurgeFile() now purging lower records:");

        while (subDirRec->cardSub() > 0)    // remove all sub sub records
            subDirRec->deleteSubAndPurgeFile(OFstatic_cast(unsigned long, 0));
        delete subDirRec;                   // remove sub directory record
    }
    return errorFlag;
}


// ********************************


OFCondition DcmDirectoryRecord::deleteSubAndPurgeFile(DcmDirectoryRecord *dirRec)
{
    DcmDirectoryRecord *subDirRec = OFstatic_cast(DcmDirectoryRecord *, lowerLevelList->remove(dirRec));
    errorFlag = lowerLevelList->error();
    if (subDirRec != NULL)
    {
        DcmDirectoryRecord *localSubRefMRDR = subDirRec->getReferencedMRDR();

        if (localSubRefMRDR != NULL)
        {                                   // file is referenced (indirect)
            localSubRefMRDR->decreaseRefNum();
        } else                              // remove file directly
            errorFlag = subDirRec->purgeReferencedFile();

        DCMDATA_DEBUG("DcmDirectoryRecord::deleteSubAndPurgeFile() now purging lower records:");

        while (subDirRec->cardSub() > 0)    // remove all sub sub records
            subDirRec->deleteSubAndPurgeFile(OFstatic_cast(unsigned long, 0));
        delete subDirRec;                   // remove sub directory record
    }
    return errorFlag;
}


OFCondition DcmDirectoryRecord::clearSub()
{
    errorFlag = lowerLevelList->clear();
    return errorFlag;
}


void DcmDirectoryRecord::setRecordsOriginFile(const OFFilename &fname)
{
    recordsOriginFile = fname;
}

const OFFilename &DcmDirectoryRecord::getRecordsOriginFile()
{
    return recordsOriginFile;
}
