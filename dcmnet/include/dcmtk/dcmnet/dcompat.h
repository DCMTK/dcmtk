/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
** Module: dcompat.h
**
** Purpose:
**      This is the place to declare compatability routines
**      which can be missing on some systems.
**
**      This include file is automatically included by dicom.h
**
** Module Prefix: none
**
*/

#ifndef DCOMPAT_H
#define DCOMPAT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofbmanip.h"    /* for bzero workaround */
#include "dcmtk/dcmnet/dndefine.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CTIME
#define INCLUDE_CSTRING
#define INCLUDE_CERRNO
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
/* Use the Grand Unified Sockets Interface (GUSI) on Macintosh */
#include <GUSI.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>  /* this includes either winsock.h or winsock2.h */
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>  /* include winsock.h directly i.e. on MacOS */
#ifdef macintosh
/*
** The WinSock header on Macintosh does not declare the WORD type nor the MAKEWORD
** macro need to initialize the WinSock library.
*/
typedef u_short WORD;
#define MAKEWORD(a,b) ((WORD) (((a)&0xff)<<8) | ((b)&0xff) )
#endif
#endif
#endif

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
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
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
END_EXTERN_C

#ifdef _WIN32
#include <process.h>
#include <io.h>
#ifndef __CYGWIN__
#include <sys/locking.h>
#endif
#endif

#ifndef EINTR
/* The WinSock header on Macintosh does not define an EINTR error code */
#ifdef HAVE_WINSOCK_H
#define EINTR WSAEINTR
#endif
#endif

#ifdef HAVE_PROTOTYPE_FLOCK
#define dcmtk_flock flock
#define dcmtk_plockerr perror
#endif

#ifndef HAVE_PROTOTYPE_FLOCK
#ifdef HAVE_FLOCK
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int flock(int fd, int operation);
END_EXTERN_C
#define dcmtk_flock flock
#define dcmtk_plockerr perror
#else
/*
 * Simulate the flock function calls
 * using the facilities of fcntl(2)
 */

#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    /* unlock */

DCMTK_DCMNET_EXPORT int dcmtk_flock(int fd, int operation);
DCMTK_DCMNET_EXPORT void dcmtk_plockerr(const char *s);

#endif /* !HAVE_FLOCK */
#endif

#ifndef HAVE_BZERO
#ifndef bzero
#define bzero(p,len) memset((void*)(p), 0, (len));
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETHOSTNAME
#ifdef HAVE_GETHOSTNAME
/* gethostname is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int gethostname(char* name, int namelen);
END_EXTERN_C
#else
/* define gethostname ourselves */
DCMTK_DCMNET_EXPORT int gethostname(char* name, int namelen);
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
DCMTK_DCMNET_EXPORT int getsockopt(int s, int level, int optname, char *optval, int *optlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SETSOCKOPT
#ifdef HAVE_SETSOCKOPT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int setsockopt(int s, int level, int optname, const char *optval, int optlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_LISTEN
#ifdef HAVE_LISTEN
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int listen(int s, int backlog);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SOCKET
#ifdef HAVE_SOCKET
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int socket(int domain, int type, int protocol);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_CONNECT
#ifdef HAVE_CONNECT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int connect(int s, struct sockaddr *name, int namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_SELECT
#ifdef HAVE_SELECT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int select(int nfds, fd_set *readfds, fd_set *writefds,
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
DCMTK_DCMNET_EXPORT int bind(int s, const struct sockaddr *name, int namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_ACCEPT
#ifdef HAVE_ACCEPT
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int accept(int s, struct sockaddr *addr, int *addrlen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_GETSOCKNAME
#ifdef HAVE_GETSOCKNAME
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int getsockname(int s, struct sockaddr *name, int *namelen);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_WAITPID
#ifdef HAVE_WAITPID
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int waitpid(pid_t pid, int *statusp, int options);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_PROTOTYPE_WAIT3
#ifdef HAVE_WAIT3
/* it is in the libraries but we have no prototype */
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int wait3(int *statusp, int options, struct rusage *rusage);
END_EXTERN_C
#else
/* don't know how to emulate */
#endif
#endif

#ifndef HAVE_ACCESS

#ifndef R_OK
#define R_OK 0x01
#define W_OK 0x02
#define X_OK 0x04
#define F_OK 0x08
#endif

DCMTK_DCMNET_EXPORT int access(const char* path, int amode);
#else /* HAVE_ACCESS */

#ifdef _WIN32
/* windows defines access but not the constants */
#ifndef R_OK
#define W_OK 02 /* Write permission */
#define R_OK 04 /* Read permission */
#define F_OK 00 /* Existance only */
#define X_OK 00 /* execute permission has no meaning under windows, treat as existance */
#endif /* R_OK */
#endif /* _WIN32 */

#endif /* HAVE_ACCESS */

#ifndef HAVE_STRERROR
DCMTK_DCMNET_EXPORT char *strerror(int e);
#endif

#ifndef HAVE_TEMPNAM
DCMTK_DCMNET_EXPORT char *tempnam(char *dir, char *pfx);
#endif

#ifdef _WIN32
#define NULL_DEVICE_NAME "nul"
#else
#define NULL_DEVICE_NAME "/dev/null"
#endif


#endif /* DCOMPAT_H */
