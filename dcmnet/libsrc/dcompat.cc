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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-04-25 16:11:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/dcompat.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_UNIX_H
#include <unix.h>
#endif
#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
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

#include "dcompat.h"
#include "dicom.h"

/*
 * On DEC alpha the linker moans if a library is empty.
 * So define a dummy variable.
 */
char dcompat_functionDefinedOnlyToStopLinkerMoaning;



#ifndef HAVE_FLOCK

#ifdef macintosh
int flock(int fd, int operation)
{
    fprintf(stderr, "WARNING: Unsupported flock(fd[%d],operation[0x%x]\n",
        fd, operation);
    return 0;
}
#else
/*
 * Simulate the flock function calls (e.g. Solaris 2 does not have them)
 * using the facilities of fcntl(2)
 */

int flock(int fd, int operation)
{
    flock_t fl;
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
    
    result = fcntl(fd, cmd, &fl);
    return result;
}
#endif

#endif /* ! HAVE_FLOCK */


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

#ifdef macintosh
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

    sprintf(name, "%s/%s%04x%05d", tmpdir, prefix, 
	(unsigned int)mix, getpid());

    return name;    
} 

#endif /* ! HAVE_TEMPNAM */


/*
** CVS Log
** $Log: dcompat.cc,v $
** Revision 1.2  1996-04-25 16:11:12  hewett
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
