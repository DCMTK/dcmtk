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
** This include file is automatically included by dicom.h 
**
** Module Prefix: none 
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-05-03 10:30:41 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dcompat.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCOMPAT_H
#define DCOMPAT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif


#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C };
#else
#define BEGIN_EXTERN_C 
#define END_EXTERN_C
#endif

#ifndef HAVE_PROTOTYPE_FLOCK
#ifdef HAVE_FLOCK
BEGIN_EXTERN_C
int flock(int fd, int operation);
END_EXTERN_C
#else
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
#endif

#ifndef HAVE_PROTOTYPE_BZERO
#ifdef HAVE_BZERO
BEGIN_EXTERN_C
void bzero(char* s, int len);
END_EXTERN_C
#else
#ifndef bzero
#define bzero(p,len) memset((void*)(p), 0, (len));
#endif
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETHOSTNAME
#ifdef HAVE_GETHOSTNAME
/* gethostname is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int gethostname(char* name, int namelen);
END_EXTERN_C
#else
/* define gethostname ourselves */
int gethostname(char* name, int namelen);
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETHOSTBYNAME
#ifdef HAVE_GETHOSTBYNAME
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
struct hostent *gethostbyname(const char* name);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETSOCKOPT
#ifdef HAVE_GETSOCKOPT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int getsockopt(int s, int level, int optname, char *optval, int *optlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SETSOCKOPT
#ifdef HAVE_SETSOCKOPT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int setsockopt(int s, int level, int optname, const char *optval, int optlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_LISTEN
#ifdef HAVE_LISTEN
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int listen(int s, int backlog);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SOCKET
#ifdef HAVE_SOCKET
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int socket(int domain, int type, int protocol);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_CONNECT
#ifdef HAVE_CONNECT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int connect(int s, struct sockaddr *name, int namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SELECT
#ifdef HAVE_SELECT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int select(int nfds, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_BIND
#ifdef HAVE_BIND
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int bind(int s, const struct sockaddr *name, int namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_ACCEPT
#ifdef HAVE_ACCEPT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int accept(int s, struct sockaddr *addr, int *addrlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETSOCKNAME
#ifdef HAVE_GETSOCKNAME
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int getsockname(int s, struct sockaddr *name, int *namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_WAITPID
#ifdef HAVE_WAITPID
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int waitpid(pid_t pid, int *statusp, int options);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_WAIT3
#ifdef HAVE_WAIT3
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
int wait3(int *statusp, int options, struct rusage *rusage);
END_EXTERN_C
#else
/* don't know how to emulate */
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

#endif /* DCOMPAT_H */

/*
** CVS Log
** $Log: dcompat.h,v $
** Revision 1.3  1996-05-03 10:30:41  hewett
** Added some common include files.
**
** Revision 1.2  1996/04/25 16:05:44  hewett
** Added prototypes for some network functions if not available on system.
**
** Revision 1.1.1.1  1996/03/26 18:38:45  hewett
** Initial Release.
**
**
*/
