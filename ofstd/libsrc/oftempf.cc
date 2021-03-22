/*
 *
 *  Copyright (C) 2011-2021, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Defines a class which manages a temporary file
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/offname.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include <ctime>


BEGIN_EXTERN_C
#ifdef HAVE_IO_H
#include <io.h>          /* for open() and close() on Win32 */
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>     /* for GetTempPath() */
#endif


/* Maximum number of attempts to open a temporary file. Please not that
 * OFFilenameCreator already retries in a loop, so this can be quite low.
 */
#define MAX_ATTEMPTS 5

OFTempFile::OFTempFile(unsigned int flags, const OFString& dir, const OFString& prefix, const OFString& postfix)
: m_status(EC_Normal /* no default constructor */)
, m_fileName()
, m_fileHandle(-1)
{
    m_status = createFile(m_fileName, &m_fileHandle, flags, dir, prefix, postfix);
}

OFTempFile::~OFTempFile()
{
    if (m_fileHandle >= 0)
        close(m_fileHandle);
    m_fileHandle = -1;
    if (!m_fileName.empty())
        unlink(m_fileName.c_str());
    m_fileName.clear();
}

void OFTempFile::stealFile(OFBool dontCloseHandle)
{
    if (!dontCloseHandle)
        close(m_fileHandle);
    m_fileName.clear();
    m_fileHandle = -1;
}

OFCondition OFTempFile::createFile(OFString& fileName, int* fd_out, unsigned int flags,
        const OFString& dir, const OFString& prefix, const OFString& postfix)
{
    OFFilenameCreator creator;
    unsigned int seed = OFstatic_cast(unsigned int, time(NULL));
    unsigned int attempt;
    OFCondition status = EC_Normal;
    OFString tempDir;
    const char *pdir;
    int fd = -1;

    if (dir.empty())
    {
        getTempPath(tempDir);
        pdir = tempDir.c_str();
    }
    else
    {
        pdir = dir.c_str();
    }

    for (attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
    {
        OFBool ok = creator.makeFilename(seed, pdir, prefix.c_str(), postfix.c_str(), fileName);
        if (ok)
        {
            // Please note that O_CREAT|O_EXCL makes sure that the file didn't
            // exist yet. This might seem unneeded since OFFileNameCreator
            // already checked, but someone might have created the file since
            // that check.
            fd = open(fileName.c_str(), flags|O_CREAT|O_EXCL, 0600);
            if (fd >= 0)
                break;
        }
    }

    if (fd < 0)
    {
        fileName.clear();
        fd = -1;
        status = EC_CouldNotCreateTemporaryFile;
    }
    if (fd_out)
        *fd_out = fd;
    else
        close(fd);

    return status;
}

void OFTempFile::getTempPath(OFString& sPath)
{
    // We could also try getenv("TMPDIR"), if getenv() is available.
#ifdef _WIN32
#define BUFFER_SIZE 1024
    char buffer[BUFFER_SIZE];
    GetTempPathA(BUFFER_SIZE, buffer);
    sPath = buffer;
#elif defined(__ANDROID__)
    sPath = ANDROID_TEMPORARY_FILES_LOCATION;
#else
    sPath = "/tmp";
#endif
}
