/*
** Configuration definations for the Metrowerks C++ Compiler environment
** on the Apple Macintosh. Created by hand.
** 
** Created: Andrew Hewett, 4.11.95
** Modified: Andrew Hewett, 27.01.97
*/

#ifndef CFMWERKS_H
#define CFMWERKS_H

/* only process if running a Metrowerks compiler */
#ifdef __MWERKS__

/* Compiling for Macintosh */
#ifndef macintosh
#define macintosh 1
#endif /* macintosh */

/* This set of defines assume that the GUSI socket library is available */
/* use the GUSI include file */
#define HAVE_GUSI_H 1 

/* Alternativly we can use a Windows Socket library */
/* 
** However, the Windows Socket library on the Mac distributed with CW 11 
** is very (very) unstable (last tried: 27.01.97)
*/
#undef HAVE_WINSOCK_H

//
// This set of #defines have been adapted for the Metrowerks C++ environment
// on Apple Machintosh from dcmtk/config/config.h.in
//

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
#undef _ALL_SOURCE
#endif

/* Define if you don't have vprintf but do have _doprnt.  */
#undef HAVE_DOPRNT

/* Define if you support file names longer than 14 characters.  */
#define HAVE_LONG_FILE_NAMES 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#undef HAVE_SYS_WAIT_H

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the wait3 system call.  */
#undef HAVE_WAIT3

/* Define as __inline if that's what the C compiler calls it.  */
#undef inline

/* Define to `int' if <sys/types.h> doesn't define.  */
#undef pid_t

/* Define if you need to in order for stat and other things to work.  */
#undef _POSIX_SOURCE

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#undef size_t

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your <sys/time.h> declares struct tm.  */
#undef TM_IN_SYS_TIME

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a double.  */
#define SIZEOF_DOUBLE 8

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* Define if you have the _findfirst function.  */
#undef HAVE__FINDFIRST 

/* Define if you have the accept function.  */
#define HAVE_ACCEPT 1

/* Define if you have the access function.  */
#ifdef HAVE_GUSI_H
#define HAVE_ACCESS 1
#else /* HAVE_GUSI_H */
/*
** Metrowerks CodeWarrior 11 no longer has an access() function
*/
#if __MWERKS__ >= 0x1100
#undef HAVE_ACCESS
#else /* __MWERKS__ >= 0x1100 */
/* at least CodeWarrior 6 and 10 have access() */
#define HAVE_ACCESS 1
#endif /* __MWERKS__ >= 0x1100 */
#endif /* HAVE_GUSI_H */

/* Define if you have the bind function.  */
#define HAVE_BIND 1

/* Define if you have the bzero function.  */
#undef HAVE_BZERO

/* Define if you have the connect function.  */
#define HAVE_CONNECT 1

/* Define if you have the cuserid function.  */
#undef HAVE_CUSERID

/* Define if you have the flock function.  */
#define HAVE_FLOCK 1

/* Define if you have the fork function.  */
#define HAVE_FORK 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getenv function.  */
#undef HAVE_GETENV

/* Define if you have the geteuid function.  */
#undef HAVE_GETEUID

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the gethostid function.  */
/* CW 6.0 and 10.0 do not have an implementation of gethostid().
*/
#undef HAVE_GETHOSTID

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME 1

/* Define if you have the getlogin function.  */
#undef HAVE_GETLOGIN

/* Define if you have the getpid function.  */
#undef HAVE_GETPID

/* Define if you have the getsockname function.  */
#define HAVE_GETSOCKNAME 1

/* Define if you have the getsockopt function.  */
#define HAVE_GETSOCKOPT 1

/* Define if you have the getuid function.  */
#undef HAVE_GETUID

/* Define if you have the getwd function.  */
#undef HAVE_GETWD

/* Define if you have the index function.  */
#undef HAVE_INDEX

/* Define if you have the itoa function.  */
#undef HAVE_ITOA

/* Define if you have the listen function.  */
#define HAVE_LISTEN 1

/* Define if you have the lockf function.  */
#undef HAVE_LOCKF

/* Define if you have the malloc_debug function.  */
#undef HAVE_MALLOC_DEBUG

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the mktemp function.  */
#undef HAVE_MKTEMP

/* Define if you have the rindex function.  */
#undef HAVE_RINDEX

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the semctl function.  */
#undef HAVE_SEMCTL

/* Define if you have the semget function.  */
#undef HAVE_SEMGET

/* Define if you have the semop function.  */
#undef HAVE_SEMOP

/* Define if you have the setsockopt function.  */
#define HAVE_SETSOCKOPT 1

/* Define if you have the setuid function.  */
#undef HAVE_SETUID

/* Define if you have the shmat function.  */
#undef HAVE_SHMAT

/* Define if you have the shmctl function.  */
#undef HAVE_SHMCTL

/* Define if you have the shmdt function.  */
#undef HAVE_SHMDT

/* Define if you have the shmget function.  */
#undef HAVE_SHMGET

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the stat function.  */
#define HAVE_STAT 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#undef HAVE_STRDUP

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtoul function.  */
#undef HAVE_STRTOUL

/* Define if you have the sysinfo function.  */
#undef HAVE_SYSINFO

/* Define if you have the tempnam function.  */
#define HAVE_TEMPNAM 1

/* Define if you have the uname function.  */
#define HAVE_UNAME 1

/* Define if you have the sleep function.  */
#undef HAVE_SLEEP

/* Define if you have the usleep function.  */
#define HAVE_USLEEP 1

/* Define if you have the waitpid function.  */
#undef HAVE_WAITPID

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <io.h> header file.  */
#undef HAVE_IO_H 

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <libc.h> header file.  */
#undef HAVE_LIBC_H

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <malloc.h> header file.  */
#undef HAVE_MALLOC_H

/* Define if you have the <ndir.h> header file.  */
#undef HAVE_NDIR_H

#ifdef HAVE_GUSI_H

/* Define if you have the <netdb.h> header file.  */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <stat.h> header file.  */
#undef HAVE_STAT_H /* GUSI define sys/stat.h with different values */

#elif HAVE_WINSOCK_H

/* Define if you have the <netdb.h> header file.  */
#undef HAVE_NETDB_H

/* Define if you have the <netinet/in.h> header file.  */
#undef HAVE_NETINET_IN_H

/* Define if you have the <stat.h> header file.  */
#define HAVE_STAT_H 1

#endif /* HAVE_WINSOCK_H */

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <sys/dir.h> header file.  */
#undef HAVE_SYS_DIR_H

#ifdef HAVE_GUSI_H

/* Define if you have the <sys/errno.h> header file.  */
#define HAVE_SYS_ERRNO_H 1

#elif HAVE_WINSOCK_H

/* Define if you have the <sys/errno.h> header file.  */
#undef HAVE_SYS_ERRNO_H

#endif /* HAVE_WINSOCK_H */

/* Define if you have the <sys/file.h> header file.  */
#undef HAVE_SYS_FILE_H

/* Define if you have the <sys/ipc.h> header file.  */
#undef HAVE_SYS_IPC_H

/* Define if you have the <sys/ndir.h> header file.  */
#undef HAVE_SYS_NDIR_H

/* Define if you have the <sys/param.h> header file.  */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/resource.h> header file.  */
#undef HAVE_SYS_RESOURCE_H

/* Define if you have the <sys/select.h> header file.  */
#undef HAVE_SYS_SELECT_H

/* Define if you have the <sys/sem.h> header file.  */
#undef HAVE_SYS_SEM_H

/* Define if you have the <sys/shm.h> header file.  */
#undef HAVE_SYS_SHM_H

#ifdef HAVE_GUSI_H

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

#elif HAVE_WINSOCK_H

/* Define if you have the <sys/socket.h> header file.  */
#undef HAVE_SYS_SOCKET_H

/* Define if you have the <sys/stat.h> header file.  */
#undef HAVE_SYS_STAT_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/types.h> header file.  */
#undef HAVE_SYS_TYPES_H

#endif /* HAVE_WINSOCK_H */

/* Define if you have the <sys/utsname.h> header file.  */
#undef HAVE_SYS_UTSNAME_H

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

#ifdef HAVE_GUSI_H

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <unix.h> header file.  */
#define HAVE_UNIX_H 1

#elif HAVE_WINSOCK_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H /* the CodeWarrior version conflicts with definitions in winsock.h */

/* Define if you have the <unix.h> header file.  */
#define HAVE_UNIX_H 1   /* the CodeWarrior version conflicts with definitions in winsock.h */

#endif /* HAVE_WINSOCK_H */

/* Define if you have the iostream library (-liostream).  */
#undef HAVE_LIBIOSTREAM

/* Define if you have the nsl library (-lnsl).  */
#undef HAVE_LIBNSL

/* Define if you have the socket library (-lsocket).  */
#undef HAVE_LIBSOCKET
/* User definable section */

/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH "$SYSTEM$:$PREFS$:dicom.dic^dicom.dic"

/* Define path separator */
#define PATH_SEPARATOR ':'

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR '^'


/* Define if your system cannot pass command line arguments into main() (e.g. Macintosh) */
#define HAVE_EMPTY_ARGC_ARGV 1

/* Compile in the debug code */
#define DEBUG 1

/* Define if your compile cannot user anonymous struct/class components */
/* #define NO_ANON_CLASS_COMP 1 */ 

/* Define if your system has a prototype for bzero */
#undef HAVE_PROTOTYPE_BZERO

/* Define if your system has a prototype for accept */
#define HAVE_PROTOTYPE_ACCEPT 1

/* Define if your system has a prototype for bind */
#define HAVE_PROTOTYPE_BIND 1

/* Define if your system has a prototype for gethostbyname */
#define HAVE_PROTOTYPE_GETHOSTBYNAME 1

/* Define if your system has a prototype for gethostid */
/* CW 6.0 & 10.0 do not have an implementation of gethostid().
** However CW 10 defines a prototype !!
** I don't know how the versions of CW between 6 and 10 will react.
** The __MWERKS__ symbol has a value of 0x0710 for Version 7.1 and a value of 0x0001
** for versions earlier than 7.1
*/
#if __MWERKS__ >= 0x1000
#define HAVE_PROTOTYPE_GETHOSTID 1
#else /* __MWERKS__ >= 0x1000 */
#undef HAVE_PROTOTYPE_GETHOSTID
#endif /* __MWERKS__ >= 0x1000 */

/* Define if your system has a prototype for gethostname */
#define HAVE_PROTOTYPE_GETHOSTNAME 1

/* Define if your system has a prototype for flock */
#define HAVE_PROTOTYPE_FLOCK 1

/* Define if your system has a prototype for listen */
#define HAVE_PROTOTYPE_LISTEN 1

/* Define if your system has a prototype for socket */
#define HAVE_PROTOTYPE_SOCKET 1

/* Define if your system has a prototype for connect */
#define HAVE_PROTOTYPE_CONNECT 1

/* Define if your system has a prototype for select */
#define HAVE_PROTOTYPE_SELECT 1

/* Define if your system has a prototype for setsockopt */
#define HAVE_PROTOTYPE_SETSOCKOPT 1

/* Define if your system has a prototype for getsockopt */
#define HAVE_PROTOTYPE_GETSOCKOPT 1

/* Define if your system has a prototype for getsockname */
#define HAVE_PROTOTYPE_GETSOCKNAME 1

/* Define if your system has a prototype for wait3 */
#undef HAVE_PROTOTYPE_WAIT3

/* Define if your system has a prototype for waitpid */
#undef HAVE_PROTOTYPE_WAITPID

/* Define if your system has a prototype for usleep in libc.h */
#define HAVE_PROTOTYPE_USLEEP 1

/* Define if your system has a prototype for semctl in sys/sem.h */
#undef HAVE_PROTOTYPE_SEMCTL

/* Define if your system has a prototype for semget in sys/sem.h */
#undef HAVE_PROTOTYPE_SEMGET

/* Define if your system has a prototype for semop in sys/sem.h */
#undef HAVE_PROTOTYPE_SEMOP

/* Define if your system has a prototype for shmat in sys/shm.h */
#undef HAVE_PROTOTYPE_SHMAT

/* Define if your system has a prototype for shmdt in sys/shm.h */
#undef HAVE_PROTOTYPE_SHMDT

/* Define if your system has a prototype for shmctl in sys/shm.h */
#undef HAVE_PROTOTYPE_SHMCTL

/* Define if your system has a prototype for shmget in sys/shm.h */
#undef HAVE_PROTOTYPE_SHMGET

/* Define if your system has a prototype for mktemp */
#undef HAVE_PROTOTYPE_MKTEMP

/* Define if your system has a prototype for union semun */
#undef HAVE_DECLARATION_UNION_SEMUN

/* Define if your system has a prototype for struct sembuf */
#undef HAVE_DECLARATION_STRUCT_SEMBUF

//
//
//


#endif /* __MWERKS__ */

#endif /* CFMWERKS_H */
