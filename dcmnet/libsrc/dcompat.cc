/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**
**  This software and supporting documentation were
**  developed by
**
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**
**  for CEN/TC251/WG4 as a contribution to the Radiological
**  Society of North America (RSNA) 1993 Digital Imaging and
**  Communications in Medicine (DICOM) Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE
**  IS WITH THE USER.
**
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice.
**
*/

/*
**
** Author: Andrew Hewett                Created: 11-08-93
**
** Module: dcompat.cc
**
** Purpose:
**      This is the place to declare compatability routines
**      which can be missing on some systems.
**
** Base Reference System is SUNOS 4.1.3
**
** This include file is automatically included by dicom.h
**
** Module Prefix: none
**
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dcompat.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/diutil.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_STAT_H
#include <stat.h>
#endif
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>  /* for access() on Win32 */
#endif
END_EXTERN_C

/*
 * On DEC alpha the linker moans if a library is empty.
 * So define a dummy variable.
 */
char dcompat_functionDefinedOnlyToStopLinkerMoaning;


#ifndef HAVE_FLOCK
#ifdef macintosh

// MacOS seems not to support file locking

int dcmtk_flock(int fd, int operation)
{
  DCMNET_WARN("Unsupported flock(fd[" << fd << "],operation[0x"
    << hex << operation << "])");
  return 0;
}

void dcmtk_plockerr(const char *s)
{
  DCMNET_WARN(s << ": flock not implemented");
}

#else /* macintosh */
#ifdef _WIN32

#ifndef USE__LOCKING

/* Note: this emulation of flock() for Win32 uses the function _get_osfhandle()
 * which takes a Unix-style file descriptor and derives the corresponding
 * Win32 API file handle (HANDLE). This function may not be available on all
 * compilers for Win32, sorry.
 * Since _locking() does not implement shared/exclusive locks, the only alternative
 * would be to re-write the complete file access to Win32 API functions. Have fun!
 */

int dcmtk_flock(int fd, int operation)
{
#ifdef __CYGWIN__
  HANDLE handle=(void *)get_osfhandle(fd);
#else
  HANDLE handle=(void *)_get_osfhandle(fd);
#endif
  OVERLAPPED overl;
  OFBitmanipTemplate<char>::zeroMem((char *)&overl, sizeof(overl));

  if (operation==LOCK_SH)
  {
    if (GetVersion() < 0x80000000) // Windows NT
    {
      // LockFileEx is only supported on Windows NT
      if (LockFileEx(handle,0,0,(DWORD)-1,(DWORD)-1,&overl) !=0) return 0; else return -1;
    } else {
      // using LockFile on Win32s and Win95. LOCKS ARE ALWAYS EXCLUSIVE!
      // make sure that we lock less than 2GB, to avoid errors on FAT16
      if (LockFile(handle, 0,0,(DWORD)0x7FFFFFFF, 0) !=0) return 0; else return -1;
    }
  }
  else if (operation==LOCK_EX)
  {
    if (GetVersion() < 0x80000000) // Windows NT
    {
      // LockFileEx is only supported on Windows NT
      if (LockFileEx(handle,LOCKFILE_EXCLUSIVE_LOCK,0,(DWORD)-1,(DWORD)-1,&overl) !=0) return 0; else return -1;
    } else {
      // using LockFile on Win32s and Win95.
      // make sure that we lock less than 2GB, to avoid errors on FAT16
      if (LockFile(handle, 0,0,(DWORD)0x7FFFFFFF, 0) !=0) return 0; else return -1;
    }
  }
  else if (operation==LOCK_UN)
  {
    if (GetVersion() < 0x80000000) // Windows NT
    {
      // UnlockFileEx is only supported on Windows NT
      if (UnlockFileEx(handle,0,(DWORD)-1,(DWORD)-1,&overl) !=0) return 0; else return -1;
    } else {
      // using UnlockFile on Win32s and Win95.
      if (UnlockFile(handle, 0, 0,(DWORD)0x7FFFFFFF, 0) !=0) return 0; else return -1;
    }
  }
  else return -1; /* unknown lock operation */
}

void dcmtk_plockerr(const char *s)
{
  LPVOID lpMsgBuf=NULL;

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf, 0, NULL);

  if (lpMsgBuf && s)
      DCMNET_ERROR(s << ": " << (const char*)lpMsgBuf);
  LocalFree(lpMsgBuf);
}

#else /* USE__LOCKING */

/* Note: this alternative emulation of flock() for Win32 uses _locking().
 * This version should only be used on compilers where _get_osfhandle()
 * is not available since it does not implement shared locks.
 */

int dcmtk_flock(int fd, int operation)
{
    long originalPosition = tell(fd);
    long fileSize = lseek(fd, 0L, SEEK_END);
    if (fileSize < 0) return fileSize;
    long maxSize = 0x7fffffff;

    /* seek to beginning of file */
    long pos = lseek(fd, 0L, SEEK_SET);
    if (pos < 0) return pos;
    int mode = 0;

    /* we only have exclusive lock using the windows _locking function */
    if (operation & LOCK_SH) mode = _LK_LOCK; /* shared lock */
    if (operation & LOCK_EX) mode = _LK_LOCK; /* exclusive lock */
    if (operation & LOCK_UN) mode = _LK_UNLCK; /* unlock */
    if (operation & LOCK_NB) mode = _LK_NBLCK; /* non-blocking */
    int status = _locking(fd, mode, maxSize);

    /* reset file point back to original position */
    pos = lseek(fd, originalPosition, SEEK_SET);
    if (pos < 0) return pos;
    return status;
}

void dcmtk_plockerr(const char *s)
{
  char buf[256];
  DCMNET_ERROR(s << ": " << OFStandard::strerror(errno, buf, sizeof(buf)));
}

#endif /* USE__LOCKING */
#else /* _WIN32 */

/*
 * Simulate the flock function calls (e.g. Solaris 2 does not have them)
 * using the facilities of fcntl(2)
 */

int dcmtk_flock(int fd, int operation)
{
    struct flock fl;
    int result;
    int cmd;

    fl.l_whence = 0;
    fl.l_start = 0;	/* from beginning of file */
    fl.l_len = 0;	/* until end of file */

    if (operation & LOCK_SH) {
	/* shared lock */
    	fl.l_type = F_RDLCK;
    }
    if (operation & LOCK_EX) {
        /* exclusive lock */
	fl.l_type = F_WRLCK;
    }
    if (operation & LOCK_UN) {
	/* unlock */
        fl.l_type = F_UNLCK;
    }
    if (operation & LOCK_NB) {
        /* non-blocking */
	cmd = F_SETLK;
    } else {
    	/* blocking */
	cmd = F_SETLKW;
    }

#if SIZEOF_VOID_P == SIZEOF_INT
  /* some systems, e.g. NeXTStep, need the third argument
   * for fcntl calls to be casted to int. Other systems,
   * e.g. OSF1-Alpha, won't accept this because int and struct flock *
   * have different sizes. The workaround used here is to use a typecast to int
   * if sizeof(void *) == sizeof(int) and leave it away otherwise.
   */
    result = fcntl(fd, cmd, (int)(&fl));
#else
    result = fcntl(fd, cmd, &fl);
#endif

    return result;
}

void dcmtk_plockerr(const char *s)
{
  char buf[256];
  DCMNET_ERROR(s << ": " << OFStandard::strerror(errno, buf, sizeof(buf)));
}

#endif /* _WIN32 */
#endif /* macintosh */
#endif /* HAVE_FLOCK */

#ifndef HAVE_GETHOSTNAME
/*
** Use the SYSV uname function (if we have it)
*/
#ifdef HAVE_UNAME
int gethostname(char* name, int namelen);
{
    struct utsname uts;
    int rc;

    bzero(&uts, sizeof(uts));
    rc = utsname(&uts);
    if (rc >= 0) {
	strncpy(name, uts.nodename, namelen);
	rc = 0;
    }
    return rc;
}
#endif /* HAVE_UNAME */
#endif /* ! HAVE_GETHOSTNAME */

#ifndef HAVE_ACCESS

/*
** The access function is OS dependent.
*/

#if defined(macintosh) || defined(_WIN32)
int access(const char* path, int /* amode */)
{
    int rc;
    struct stat buf;

    rc = stat(path, &buf);

    /* WARNING
    ** on the macintosh if a file is there we can do anything with it except
    ** if it is locked or on a read only filesystem.  Trying to find out about
    ** that is too complicated at the moment.
    */
    return rc;
}
#endif

#endif

#if 0 // never called, replaced by OFStandard::strerror()
#ifndef HAVE_STRERROR

#warning Your system does not seem to have the strerror() function

/*
 * strerror does not appear to be available on SunOs 4.1.3
 */
char *strerror(int errornum)
{
    static char string[256];
    char *s = NULL;
    /*
     * These are not in the system include files,
     * declare them here.
     */
    extern int sys_nerr;
    extern char *sys_errlist[];

    string[0] = '\0';
    if (errornum < 0 || errornum >= sys_nerr) {
        sprintf(string, "Error number: %d", errornum);
	s = string;
    } else {
        s = sys_errlist[errornum];
    }
    return s;
}

#endif /* ! HAVE_STRERROR */
#endif
