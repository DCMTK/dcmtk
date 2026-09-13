/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Michael Onken
 *
 *  Purpose: test program for the quota mechanism of the index database
 *           handle (deletion of the oldest study).
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmnet/dcmlayer.h"

/* Create a unique, empty storage directory for the index database handle.
 * Returns an empty string on failure.
 */
static OFString createStorageDir()
{
    OFTempFile temp;
    if (temp.getStatus().bad())
        return OFString();
    OFString dir = temp.getFilename();
    dir += "_qrquota";
    if (OFStandard::createDirectory(dir, dir).bad())
        return OFString();
    return dir;
}

/* Write a minimal Secondary Capture object with the given UIDs into the
 * storage area and register it in the index database.
 */
static OFBool storeInstance(
    DcmQueryRetrieveIndexDatabaseHandle& handle,
    const OFString& studyUID,
    const OFString& instanceUID,
    OFString& fileName)
{
    char buffer[MAXPATHLEN+1];
    if (handle.makeNewStoreFileName(UID_SecondaryCaptureImageStorage,
        instanceUID.c_str(), buffer, sizeof(buffer)).bad()) return OFFalse;
    fileName = buffer;

    DcmFileFormat fileformat;
    DcmDataset *dset = fileformat.getDataset();
    dset->putAndInsertOFStringArray(DCM_PatientName, "QUOTA^TEST");
    dset->putAndInsertOFStringArray(DCM_PatientID, "QUOTA");
    dset->putAndInsertOFStringArray(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
    dset->putAndInsertOFStringArray(DCM_SOPInstanceUID, instanceUID);
    dset->putAndInsertOFStringArray(DCM_StudyInstanceUID, studyUID);
    dset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, studyUID + ".1");
    dset->putAndInsertOFStringArray(DCM_Modality, "OT");
    if (fileformat.saveFile(fileName, EXS_LittleEndianExplicit).bad()) return OFFalse;

    DcmQueryRetrieveDatabaseStatus status;
    return handle.storeRequest(UID_SecondaryCaptureImageStorage,
        instanceUID.c_str(), fileName.c_str(), &status).good();
}

OFTEST(dcmqrdb_quotaDeletesOnlySelectedStudy)
{
    OFString storageDir = createStorageDir();
    OFCHECK(!storageDir.empty());
    if (storageDir.empty()) return;

    OFCondition cond;
    /* room for two studies only, so storing a third one evicts the oldest */
    DcmQueryRetrieveIndexDatabaseHandle handle(storageDir.c_str(), 2, 1048576, cond);
    OFCHECK(cond.good());
    if (cond.bad()) return;

    /* The Study Instance UID of the second study starts with the Study
     * Instance UID of the first one. This relation occurs naturally whenever
     * the last UID component is an unpadded counter (".1" and ".10").
     */
    const OFString studyA = "1.2.276.0.7230010.3.1.2.1787205428.1";
    const OFString studyB = "1.2.276.0.7230010.3.1.2.1787205428.10";
    const OFString studyC = "1.2.276.0.7230010.3.1.2.1787205428.2";

    OFString fileA, fileB, fileC;
    OFCHECK(storeInstance(handle, studyA, studyA + ".1.1", fileA));
    OFCHECK(storeInstance(handle, studyB, studyB + ".1.1", fileB));
    OFCHECK(OFStandard::fileExists(fileA));
    OFCHECK(OFStandard::fileExists(fileB));

    /* No descriptor entry is left for the third study, so the oldest study
     * (study A) is deleted to make room for it. Study B must survive: before
     * the fix it was removed as well, because its UID has the UID of study A
     * as a prefix.
     */
    OFCHECK(storeInstance(handle, studyC, studyC + ".1.1", fileC));
    OFCHECK(!OFStandard::fileExists(fileA));
    OFCHECK(OFStandard::fileExists(fileB));
    OFCHECK(OFStandard::fileExists(fileC));

    /* clean up */
    OFStandard::deleteFile(fileB);
    OFStandard::deleteFile(fileC);
    OFString indexFile = storageDir;
    indexFile += PATH_SEPARATOR;
    indexFile += DBINDEXFILE;
    OFStandard::deleteFile(indexFile);
}

#ifndef WITH_OPENSSL

// This dummy function creates a dependency on libdcmnet that is required when compiling
// on NetBSD with libwrap support enabled and OpenSSL support disabled. Otherwise there
// would be a linker error complaining about unresolved symbols allow_severity and deny_severity.

DcmTransportLayer *tquota_dummyFunction()
{
  return new DcmTransportLayer();
}

#endif
