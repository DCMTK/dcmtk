/*
 *
 *  Copyright (C) 2011-2016, OFFIS e.V.
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

#ifndef OFTEMPF_H
#define OFTEMPF_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftypes.h"     /* for OFBool */

BEGIN_EXTERN_C
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for O_RDWR */
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* needed on Solaris for O_RDWR */
#endif
END_EXTERN_C

/** this class manages the lifetime of a temporary file. The file will be
 *  created when needed and automatically be deleted by the destructor.
 */
class DCMTK_OFSTD_EXPORT OFTempFile
{
public:
    /** default constructor, create a temporary file with a random name.
     *  The file will be automatically deleted.
     *  @param flags additional flags to pass to open() as its third argument.
     *  @param dir the directory in which the file should be created.
     *         The default is a directory suitable for temporary files.
     *  @param prefix this string is prepended to the name of the temporary file.
     *  @param postfix this string is appended to the name of the temporary file.
     *  @return EC_Normal if a file could be created.
     *  @see getStatus() to know if a file was successfully created
     */
    OFTempFile(unsigned int flags = O_RDWR, const OFString& dir = "",
               const OFString& prefix = "", const OFString& postfix = ".tmp");

    /// destructor. Deletes the temporary file, if one was created
    ~OFTempFile();

    /// gets the error status of this class. If this is bad, no file was created
    OFCondition getStatus() const { return m_status; }

    /** gets the filename of the temporary file. If no temporary file was
     *  created, this will return an empty string.
     *  @return filename of the temporary file
     */
    const char *getFilename() const { return m_fileName.c_str(); }

    /** gets the file descriptor of the temporary file. If no temporary file was
     *  created, this will return -1. The returned fd is still owned by
     *  this class and will be closed later.
     *  @return file descriptor for the temporary file
     */
    int getFileHandle() const { return m_fileHandle; }

    /** steals the created temporary file from this class. This means that the
     *  file won't be deleted. However, by default the file handle is closed.
     *  @param dontCloseHandle if true, the file handle is not closed.
     */
    void stealFile(OFBool dontCloseHandle = OFFalse);

    /** Create a new temporary file.
     *  @param fileName after this call, this will contain the name of the file
     *         that was created
     *  @param fd_out if not NULL, this will be set to a file descriptor for this
     *         file
     *  @param flags additional flags to pass to open() as its third argument
     *  @param dir the directory in which the file should be created.
     *         The default is a directory suitable for temporary files.
     *  @param prefix this string is prepended to the name of the temporary file.
     *  @param postfix this string is appended to the name of the temporary file.
     *  @return EC_Normal if a file could be created.
     */
    static OFCondition createFile(OFString& fileName, int* fd_out = NULL,
            unsigned int flags = O_RDWR, const OFString& dir = "",
            const OFString& prefix = "", const OFString& postfix = ".tmp");

    /** Return the system-specific path for storing temporary files.
     *  @param sPath will be set to the path on return.
     */
    static void getTempPath(OFString& sPath);

private:
    /// private undefined copy constructor
    OFTempFile(const OFTempFile&);

    /// private undefined assignment operator
    OFTempFile& operator=(const OFTempFile&);

    /// Status for this class, if EC_Normal then we are managing a temporary file
    OFCondition m_status;

    /// Path to the temporary file that we are managing, empty string if none
    OFString m_fileName;

    /// File handle for our file, -1 if we don't have any file
    int m_fileHandle;
};

#endif
