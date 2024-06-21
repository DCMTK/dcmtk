/*
 *
 *  Copyright (C) 1994-2024, OFFIS e.V.
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
**      This is the place to declare compatibility routines
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
#include "dcmtk/dcmnet/dndefine.h"
#include <cerrno>

#ifdef HAVE_WINDOWS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  /* for Windows defines */
#elif defined(HAVE_WINSOCK_H)
#include <winsock.h>  /* include winsock.h directly i.e. on MacOS */
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
#endif

#ifndef HAVE_PROTOTYPE_FLOCK
#ifdef HAVE_FLOCK
BEGIN_EXTERN_C
DCMTK_DCMNET_EXPORT int flock(int fd, int operation);
END_EXTERN_C
#define dcmtk_flock flock
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

#endif /* !HAVE_FLOCK */
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
#define F_OK 00 /* Existence only */
#define X_OK 00 /* Execute permission has no meaning under Windows, treat as existence */
#endif /* R_OK */
#endif /* _WIN32 */

#endif /* HAVE_ACCESS */

#ifdef _WIN32
#define NULL_DEVICE_NAME "nul"
#else
#define NULL_DEVICE_NAME "/dev/null"
#endif

DCMTK_DCMNET_EXPORT void dcmtk_plockerr(const char *s);

#endif /* DCOMPAT_H */
