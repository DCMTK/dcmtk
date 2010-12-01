/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:35 $
** CVS/RCS Revision:    $Revision: 1.35 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
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

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <winbase.h>
#endif

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


#ifndef HAVE_TEMPNAM
/*
 * These functions are not present on NeXTs but are used by the
 * DB module.
 */

char *
tempnam(char *dir, char *pfx)
{
#define TMPDIR_1	"/usr/tmp"
#define TMPDIR_2	"/tmp"
#define AMODES		(R_OK | W_OK | X_OK)
    char *tmpdir = NULL;
    char *env = NULL;
    char prefix[6];
    char *name = NULL;
    static unsigned short mix = 0;

    /* check environment variable first */
    if (((env = getenv("TMPDIR")) != NULL) && access(env, AMODES) == 0) {
	tmpdir = env;
    } else if (dir != NULL && access(dir, AMODES) == 0) {
	tmpdir = dir;
    } else if (access(TMPDIR_1, AMODES) == 0) {
	tmpdir = TMPDIR_1;
    } else if (access(TMPDIR_2, AMODES) == 0) {
	 tmpdir = TMPDIR_2;
    }

    if (tmpdir == NULL) {
	return NULL; 	/* no suitable directory found */
    }


    name = (char*)malloc(strlen(tmpdir) + 1 + MIN(strlen(pfx), 5) + 15);
    if (name == NULL) {
	return NULL;	/* malloc failure */
    }

    /* SUNOS Compatability: take the first 5 characters of prefix (pfx) */
    bzero(prefix, sizeof(prefix));
    strncpy(prefix, pfx, 5);
    /*
     * Find a suitable name.
     * Use at most 14 characters for filename component of
     * path.  The last 5 characters use the process id, the middle
     * 4 some hex number.
     * note will recycle after about 65536 times
     */

    mix++;	/* will recycle */

    sprintf(name, "%s%c%s%04x%05d", tmpdir, PATH_SEPARATOR, prefix,
	(unsigned int)mix, (int)OFStandard::getProcessID());

    return name;
}

#endif /* ! HAVE_TEMPNAM */


/*
** CVS Log
** $Log: dcompat.cc,v $
** Revision 1.35  2010-12-01 08:26:35  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.34  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.33  2010-06-02 14:52:03  joergr
** Replaced calls to strerror() by new helper function OFStandard::strerror()
** which results in using the thread safe version of strerror() if available.
** Added #warning statement for systems where HAVE_STRERROR is not defined.
**
** Revision 1.32  2010-01-20 13:49:47  uli
** Added OFStandard::getProcessID().
**
** Revision 1.31  2009-12-02 13:16:16  uli
** Corrected build failures on windows.
**
** Revision 1.30  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.29  2006-08-15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.28  2005/12/08 15:44:36  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.27  2004/08/03 11:42:47  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.26  2002/11/27 13:04:38  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.25  2001/06/05 10:05:46  joergr
** Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H to reflect
** the fact that the latest Cygwin/gcc version does not define _WIN32 any more.
**
** Revision 1.24  2000/03/03 14:11:20  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.23  2000/02/23 15:12:29  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.22  2000/02/02 15:17:36  meichel
** Replaced some #if statements by more robust #ifdef
**
** Revision 1.21  2000/02/01 10:24:07  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.20  1999/11/12 16:51:05  meichel
** Corrected file locking code that did not work correctly under Win95/98.
**
** Revision 1.19  1999/05/04 12:18:26  joergr
** Minor changes to support Cygwin B20.1 (check __CYGWIN__ to distinguish from
** MSVC which also defines _WIN32).
**
** Revision 1.18  1999/04/30 16:36:30  meichel
** Renamed all flock calls to dcmtk_flock to avoid name clash between flock()
** emulation based on fcntl() and a constructor for struct flock.
**
** Revision 1.17  1999/04/22 13:35:29  meichel
** Corrected Win32 API version of flock emulation
**
** Revision 1.16  1999/04/21 13:02:58  meichel
** Now always including <windows.h> instead of <winsock.h> on Win32 platforms.
**   This makes sure that <winsock2.h> is used if available.
**
** Revision 1.15  1999/04/19 08:43:08  meichel
** Implemented locking for Win95/98 and Win32s using
**   LockFile() instead of LockFileEx() which is only supported on NT.
**
** Revision 1.14  1999/01/20 19:12:35  meichel
** Some code purifications in Win32 variant of flock() emulation.
**
** Revision 1.13  1999/01/12 17:09:02  vorwerk
** differences of lock from windows and flock corrected.
**
** Revision 1.11  1999/01/11 13:06:13  vorwerk
** Shared and exclusive locking mechanism for Windows with MS Visual C++ added.
**
** Revision 1.10  1999/01/06 16:32:33  vorwerk
** exclusive lockmechanism for windows added
**
** Revision 1.9  1997/09/18 14:41:26  meichel
** Some systems, e.g. NeXTStep, need the third argument
**   for fcntl calls to be casted to int. Other systems,
**   e.g. OSF1-Alpha, won't accept this because int and struct flock *
**   have different sizes. The workaround used here is to use a typecast to int
**   if sizeof(void *) == sizeof(int) and leave it away otherwise.
**
** Revision 1.8  1997/08/06 12:22:18  andreas
** - Change definition of path to database index now using consistently
**   the defines PATH_SEPARATOR and DBINDEXFILE
**
** Revision 1.7  1997/02/06 12:17:11  hewett
** Updated for Macintosh CodeWarrior 11.  Corrected for incompatibilities
** in the timeval structure between unix.h and winsock.h
**
** Revision 1.6  1996/09/27 14:03:48  hewett
** Added simple version of access(...) for Win32.  This needs improvement.
**
** Revision 1.5  1996/09/27 09:18:02  hewett
** Changed flock_t to struct flock (for IBM AIX C Set++).
**
** Revision 1.4  1996/09/27 08:37:15  hewett
** Preliminary Win32 support.  File-locking is disabled.
**
** Revision 1.3  1996/06/20 07:35:48  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.2  1996/04/25 16:11:12  hewett
** Added parameter casts to char* for bzero calls.  Replaced some declarations
** of DIC_UL with unsigned long (reduces mismatch problems with 32 & 64 bit
** architectures).  Added some protection to inclusion of sys/socket.h (due
** to MIPS/Ultrix).
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
