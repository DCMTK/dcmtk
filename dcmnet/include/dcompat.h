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
** Module: dcompat.h
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
** Update Date:		$Date: 1996-03-26 18:38:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dcompat.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCOMPAT_H
#define DCOMPAT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifndef HAVE_FLOCK
/*
 * Simulate the flock function calls 
 * using the facilities of fcntl(2)
 */

#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    /* unlock */

int flock(int fd, int operation);

#endif /* !HAVE_FLOCK */

#ifndef HAVE_PROTOTYPE_BZERO
#ifndef bzero
#define bzero(p,len) memset((void*)(p), 0, (len));
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETHOSTNAME
#ifdef HAVE_GETHOSTNAME
/* gethostname is in the libraries but we have no prototype */
#ifdef __cplusplus
extern "C" {
#endif
int gethostname(char* name, int namelen);
#ifdef __cplusplus
};
#endif
#else
/* define gethostname ourselves */
int gethostname(char* name, int namelen);
#endif
#endif

#ifndef HAVE_GETPID
#ifndef getpid
#define getpid()	((int) 9000)
#endif
#endif

#ifndef HAVE_INDEX
#ifndef index
#define index(s, c)	strchr(s, c)
#endif
#endif
#ifndef HAVE_RINDEX
#ifndef rindex
#define rindex(s, c)	strrchr(s, c)
#endif
#endif

#ifndef HAVE_ACCESS

#ifndef R_OK
#define R_OK 0x01
#define W_OK 0x02
#define X_OK 0x04
#define F_OK 0x08
#endif

int access(const char* path, int amode);
#endif

#ifndef HAVE_STRERROR
char *strerror(int e);
#endif

#ifndef HAVE_TEMPNAM
char *tempnam(char *dir, char *pfx);
#endif

#ifndef HAVE_STRDUP
char *strdup(char* s);
#endif


#endif /* DCOMPAT_H */

/*
** CVS Log
** $Log: dcompat.h,v $
** Revision 1.1  1996-03-26 18:38:45  hewett
** Initial revision
**
**
*/
