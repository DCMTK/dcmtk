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
** Author:Andrew Hewett		Created: 11-08-93 
** 	   Oldenburg University
**	   Germany
**
** Module: dcompat.cc
** 
** Purpose:
** This is the place to declare compatability routines
** which can be missing on some systems.  
**
** Base Reference System is SUNOS 4.1.3
**
** This include file is automatically included by dicom.h 
**
** Module Prefix: none 
** 
**
** Last Update:		$Author: vorwerk $
** Update Date:		$Date: 1999-01-11 14:44:59 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dcompat.cc,v $
** CVS/RCS Revision:	$Revision: 1.12 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcompat.h"
#include "dicom.h"

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_UNIX_H
#if defined(macintosh) && defined (HAVE_WINSOCK_H)
/* unix.h defines timeval incompatible with winsock.h */
#define timeval _UNWANTED_timeval
#endif
#include <unix.h>	/* for unlink() under Metrowerks C++ (Macintosh) */
#undef timeval
#endif
#include <errno.h>
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
#if HAVE_IO_H
#define access my_access	// Workaround to make Visual C++ Compiler happy!
#include <io.h>
#undef access
#endif

END_EXTERN_C
#if defined (windows)
/* Structure that controls the locking mechanism of Windows NT */
BEGIN_EXTERN_C
OVERLAPPED overl;
END_EXTERN_C

#include "windows.h"
#include "winbase.h"
#endif
/*
 * On DEC alpha the linker moans if a library is empty.
 * So define a dummy variable.
 */
char dcompat_functionDefinedOnlyToStopLinkerMoaning;



#ifndef HAVE_FLOCK
#if defined(macintosh)
int flock(int fd, int operation)
{
    fprintf(stderr, "dcmnet\libsrc\dcompat(mac): WARNING ! \n");
	fprintf(stderr, "Unsupported flock(fd[%d],operation[0x%x]\n");
        fd, operation);
    return 0;
}
#else
#if defined(windows)

/*
* MS Visual C++ only
*/

int flock(int fd, int operation)
{
			if (operation==LOCK_SH){
				int endpos;
		        endpos = lseek(fd, 0, SEEK_END);
				printf("shared Lock \n");
				HANDLE handle;
				handle=(void *)_get_osfhandle(fd);
				if (LockFileEx(handle,0,0,endpos,0,&overl)!=0) {
					printf("file locked \n");
					return 0;}

				else return -1;
			}
				
			
    if ((operation==LOCK_EX))
	{
		printf("exclusive Lock \n");
		int endpos;
		endpos = lseek(fd, 0, SEEK_END);
				HANDLE handle;
				long a;
				a=_get_osfhandle(fd);
				handle=(int *)a;
				if (LockFileEx(handle,LOCKFILE_EXCLUSIVE_LOCK,0,endpos,0,&overl)!=0) {
					printf("file locked \n");
					return 0;}

				else return -1;
	
	}
	if (operation==LOCK_UN)
	{
		int endpos;
		endpos = lseek(fd, 0, SEEK_END);
		fprintf(stderr,"Groesse %d \n",endpos);
		HANDLE handle;
				long a;
				a=_get_osfhandle(fd);
				handle=(int *)a;
				if (UnlockFileEx(handle,0,endpos,0,&overl)!=0) {
					printf("file unlocked \n");
					return 0;}
		else
		{
			return -1;
		}
	}
	return -1;
}
#else
/*
 * Simulate the flock function calls (e.g. Solaris 2 does not have them)
 * using the facilities of fcntl(2)
 */

int flock(int fd, int operation)
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
#endif

#endif /* ! HAVE_FLOCK */
#endif

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

#if defined(macintosh) || defined(windows)
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
        sprintf(string,"Error number: %d", errornum);
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
	(unsigned int)mix, getpid());

    return name;    
} 

#endif /* ! HAVE_TEMPNAM */


/*
** CVS Log
** $Log: dcompat.cc,v $
** Revision 1.12  1999-01-11 14:44:59  vorwerk
** lock variable moved into Windows section.
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
