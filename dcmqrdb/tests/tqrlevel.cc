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
 *  Purpose: test program for handling of the QueryRetrieveLevel (0008,0052)
 *           in the index database handle (C-FIND / C-MOVE).
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"
#include "dcmtk/dcmqrdb/dcmqrdbs.h"
#include "dcmtk/dcmnet/dcmlayer.h"

/* Create a unique, empty storage directory for the index database handle,
 * named after the given temporary file. The caller must keep that file until
 * the directory is no longer used: the first name OFTempFile tries depends
 * on the current time only, so tests running in parallel (e.g. "ctest -j")
 * would get the same name and share the directory once the file is deleted.
 * Returns an empty string on failure.
 */
static OFString createStorageDir(const OFTempFile& temp)
{
    if (temp.getStatus().bad())
        return OFString();
    OFString dir = temp.getFilename();
    dir += "_qrdb";
    if (OFStandard::createDirectory(dir, dir).bad())
        return OFString();
    return dir;
}

/* Fill a generous chunk of the stack below the current frame with a non-zero
 * pattern. This models the "warm" stack of a real server that has already
 * handled prior requests: the never-NUL-terminated "level" buffer inside
 * startFindRequest() then reuses these non-zero bytes, so the buggy
 * upper-casing loop runs past the array end (caught by AddressSanitizer).
 * Without this priming, a clean process happens to leave those bytes zeroed
 * and the overflow stays dormant.
 */
static void dirtyStack()
{
    /* the "level" buffer lives roughly 11 KB into startFindRequest()'s frame */
    volatile unsigned char scratch[16384];
    for (size_t i = 0; i < sizeof(scratch); ++i)
        scratch[i] = OFstatic_cast(unsigned char, 0xAB);
}

/* Feed the given QueryRetrieveLevel value into startFindRequest() and make
 * sure the call returns in a defined manner (i.e. without reading or writing
 * past the internal fixed-size "level" buffer). This is the regression test
 * for the stack buffer overflow triggered by an oversized (0008,0052).
 */
static void runFind(const char *levelValue)
{
    dirtyStack();

    OFTempFile temp;
    OFString storageDir = createStorageDir(temp);
    OFCHECK(!storageDir.empty());
    if (storageDir.empty()) return;

    OFCondition cond;
    DcmQueryRetrieveIndexDatabaseHandle handle(storageDir.c_str(), -1, -1, cond);
    OFCHECK(cond.good());
    if (cond.bad()) return;

    DcmDataset ds;
    OFCHECK(ds.putAndInsertString(DCM_QueryRetrieveLevel, levelValue).good());

    DcmQueryRetrieveDatabaseStatus status;
    /* The actual point of the test: this must not overflow "level".
     * Under AddressSanitizer the former code triggers a stack-buffer-overflow
     * here; with the fix the call simply returns a defined condition.
     */
    handle.startFindRequest(UID_FINDStudyRootQueryRetrieveInformationModel, &ds, &status);

    /* clean up the index file in the temporary storage area (the now empty
     * directory is left to the system's temp-directory housekeeping)
     */
    OFString indexFile = storageDir;
    indexFile += PATH_SEPARATOR;
    indexFile += DBINDEXFILE;
    OFStandard::deleteFile(indexFile);
}

OFTEST(dcmqrdb_oversizedQueryRetrieveLevel)
{
    /* 60 bytes of 'a' -- longer than the 50-byte internal "level" buffer.
     * Before the fix this caused an out-of-bounds read+write while
     * upper-casing the (never NUL-terminated) buffer contents.
     */
    char oversized[61];
    memset(oversized, 'a', 60);
    oversized[60] = '\0';
    runFind(oversized);
}

OFTEST(dcmqrdb_lowercaseQueryRetrieveLevel)
{
    /* A regular, lower-case level must still be accepted: the handle
     * upper-cases it internally before comparing against "STUDY".
     */
    runFind("study");
}

#ifndef WITH_OPENSSL

// This dummy function creates a dependency on libdcmnet that is required when compiling
// on NetBSD with libwrap support enabled and OpenSSL support disabled. Otherwise there
// would be a linker error complaining about unresolved symbols allow_severity and deny_severity.

DcmTransportLayer *tscuscptls_dummyFunction()
{
  return new DcmTransportLayer();
}

#endif
