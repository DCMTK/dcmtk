/*
** Configuration definations for the Microsoft Visual C++ Compiler 
** with Intel i386/i486/Pentium processor environment.
** Created manually.
** 
*/

#ifndef CFWIN32_H
#define CFWIN32_H

/* only process if running in a Win32 environment */
#ifdef _WIN32

/* Compiling for windows */
#define windows 1

/* Define if you have the <windows.h> header file.  */
#define HAVE_WINDOWS_H 1

/* 
** All the following defines assume that the WinSock socket 
** library is available.
*/
#define HAVE_WINSOCK_H 1	/* use the WINSOCK.H include file */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if type char is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* #undef __CHAR_UNSIGNED__ */
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

/* The number of bytes in a void *.  */
#define SIZEOF_VOID_P 4

/* Define if you have the _findfirst function.  */
#define HAVE__FINDFIRST 1

/* Define if you have the accept function.  */
#define HAVE_ACCEPT 1

/* Define if you have the access function.  */
#define HAVE_ACCESS 1

/* Define if you have the bcmp function.  */
#undef HAVE_BCMP

/* Define if you have the bcopy function.  */
#undef HAVE_BCOPY

/* Define if you have the bind function.  */
#define HAVE_BIND 1

/* Define if you have the bzero function.  */
#undef HAVE_BZERO

/* Define if you have the connect function.  */
#define HAVE_CONNECT 1

/* Define if you have the cuserid function.  */
#undef HAVE_CUSERID

/* Define if you have the flock function.  */
#undef HAVE_FLOCK

/* Define if you have the fork function.  */
#undef HAVE_FORK 

/* Define if you have the getenv function.  */
#define HAVE_GETENV 1

/* Define if you have the geteuid function.  */
#undef HAVE_GETEUID

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the gethostid function.  */
#undef HAVE_GETHOSTID

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME 1

/* Define if you have the getlogin function.  */
#undef HAVE_GETLOGIN

/* Define if you have the getpid function.  */
#define HAVE_GETPID 1

/* Define if you have the getsockname function.  */
#define HAVE_GETSOCKNAME 1

/* Define if you have the getsockopt function.  */
#define HAVE_GETSOCKOPT 1

/* Define if you have the getuid function.  */
#undef HAVE_GETUID

/* Define if you have the index function.  */
#undef HAVE_INDEX

/* Define if you have the itoa function.  */
#define HAVE_ITOA 1

/* Define if you have the listen function.  */
#define HAVE_LISTEN 1

/* Define if you have the lockf function.  */
#undef HAVE_LOCKF

/* Define if you have the malloc_debug function.  */
#undef HAVE_MALLOC_DEBUG

/* Define if you have the memcmp function.  */
#define HAVE_MEMCMP 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the memset function.  */
#define HAVE_MEMSET 1

/* Define if you have the mktemp function.  */
#define HAVE_MKTEMP 1

/* Define if you have the rindex function.  */
#undef HAVE_RINDEX

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the setsockopt function.  */
#define HAVE_SETSOCKOPT 1

/* Define if you have the setuid function.  */
#undef HAVE_SETUID

/* Define if you have the sleep function.  */
#undef HAVE_SLEEP

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

/* Define if you have the strlcat function.  */
#undef HAVE_STRLCAT

/* Define if you have the strlcpy function.  */
#undef HAVE_STRLCPY

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the sysinfo function.  */
#undef HAVE_SYSINFO

/* Define if you have the tempnam function.  */
#undef HAVE_TEMPNAM 

/* Define if you have the tmpnam function.  */
#define HAVE_TMPNAM 1

/* Define if you have the uname function.  */
#undef HAVE_UNAME 

/* Define if you have the usleep function.  */
#undef HAVE_USLEEP

/* Define if you have the waitpid function.  */
#undef HAVE_WAITPID

/* Define if you have the <alloca.h> header file.  */
#undef HAVE_ALLOCA_H

/* Define if you have the <arpa/inet.h> header file.  */
#undef HAVE_ARPA_INET_H

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <dirent.h> header file.  */
#undef HAVE_DIRENT_H

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <float.h> header file.  */
#define HAVE_FLOAT_H 1

/* Define if you have the <fstream> header file.  */
#define HAVE_FSTREAM 1

/* Define if you have the <fstream.h> header file.  */
#define HAVE_FSTREAM_H 1

/* Define if you have the <io.h> header file.  */
#define HAVE_IO_H 1

/* Define if you have the <iomanip> header file.  */
#define HAVE_IOMANIP 1

/* Define if you have the <iomanip.h> header file.  */
#define HAVE_IOMANIP_H 1

/* Define if you have the <iostream> header file.  */
#define HAVE_IOSTREAM 1

/* Define if you have the <iostream.h> header file.  */
#define HAVE_IOSTREAM_H 1

/* Define if you have the <iso646.h> header file.  */
#define HAVE_ISO646_H 1

/* Define if you have the <libc.h> header file.  */
#undef HAVE_LIBC_H

/* Define if you have the <limits.h> header file.  */
#undef HAVE_LIMITS_H

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <math.h> header file.  */
#define HAVE_MATH_H 1

/* Define if you have the <ndir.h> header file.  */
#undef HAVE_NDIR_H

/* Define if you have the <netdb.h> header file.  */
#undef HAVE_NETDB_H

/* Define if you have the <netinet/in.h> header file.  */
#undef HAVE_NETINET_IN_H

/* Define if you have the <netinet/in_systm.h> header file.  */
#undef HAVE_NETINET_IN_SYSTM_H

/* Define if you have the <netinet/tcp.h> header file.  */
#undef HAVE_NETINET_TCP_H

/* Define if you have the <pthread.h> header file.  */
#undef HAVE_PTHREAD_H

/* Define if you have the <semaphore.h> header file.  */
#undef HAVE_SEMAPHORE_H

/* Define if you have the <setjmp.h> header file.  */
#define HAVE_SETJMP_H 1

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

/* Define if you have the <sstream> header file.  */
#define HAVE_SSTREAM 1

/* Define if you have the <sstream.h> header file.  */
#undef HAVE_SSTREAM_H

/* Define if you have the <stat.h> header file.  */
#undef HAVE_STAT_H 

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdio.h> header file.  */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#undef HAVE_STRINGS_H

/* Define if you have the <strstrea.h> header file.  */
#define HAVE_STRSTREA_H 1

/* Define if you have the <strstream> header file.  */
#define HAVE_STRSTREAM 1

/* Define if you have the <strstream.h> header file.  */
#undef HAVE_STRSTREAM_H

/* Define if you have the <synch.h> header file.  */
#undef HAVE_SYNCH_H

/* Define if you have the <sys/dir.h> header file.  */
#undef HAVE_SYS_DIR_H

/* Define if you have the <sys/errno.h> header file.  */
#undef HAVE_SYS_ERRNO_H

/* Define if you have the <sys/file.h> header file.  */
#undef HAVE_SYS_FILE_H

/* Define if you have the <sys/ndir.h> header file.  */
#undef HAVE_SYS_NDIR_H

/* Define if you have the <sys/param.h> header file.  */
#undef HAVE_SYS_PARAM_H

/* Define if you have the <sys/resource.h> header file.  */
#undef HAVE_SYS_RESOURCE_H

/* Define if you have the <sys/select.h> header file.  */
#undef HAVE_SYS_SELECT_H

/* Define if you have the <sys/socket.h> header file.  */
#undef HAVE_SYS_SOCKET_H

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/utime.h> header file.  */
#define HAVE_SYS_UTIME_H 1

/* Define if you have the <sys/utsname.h> header file.  */
#undef HAVE_SYS_UTSNAME_H

/* Define if you have the <thread.h> header file.  */
#undef HAVE_THREAD_H

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the <unix.h> header file.  */
#undef HAVE_UNIX_H

/* Define if you have the <utime.h> header file.  */
#undef HAVE_UTIME_H

/* Define if you have the <wctype.h> header file.  */
#define HAVE_WCTYPE_H 1

/* Define if you have the iostream library (-liostream).  */
#undef HAVE_LIBIOSTREAM

/* Define if you have the nsl library (-lnsl).  */
#undef HAVE_LIBNSL

/* Define if you have the socket library (-lsocket).  */
#undef HAVE_LIBSOCKET
/* User definable section */

/* Define the DCMTK default path */
#define DCMTK_PREFIX "C:\\usr\\local\\dicom"

/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH "dicom.dic;\\dicom.dic;" DCMTK_PREFIX "\\lib\\dicom.dic"

/* Define path separator */
#define PATH_SEPARATOR '\\'

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR ';'

/* Define if your system cannot pass command line arguments into main() (e.g. Macintosh) */
#undef HAVE_EMPTY_ARGC_ARGV

/* Compile in the debug code */
/* #define DEBUG 1 */ 

/* Define if your compile cannot user anonymous struct/class components */
#define NO_ANON_CLASS_COMP 1

/* Define if your system has a prototype for bzero */
#undef HAVE_PROTOTYPE_BZERO

/* Define if your system has a prototype for accept */
#define HAVE_PROTOTYPE_ACCEPT 1

/* Define if your system has a prototype for bind */
#define HAVE_PROTOTYPE_BIND 1

/* Define if your system has a prototype for gethostbyname */
#define HAVE_PROTOTYPE_GETHOSTBYNAME 1

/* Define if your system has a prototype for gethostid */
#undef HAVE_PROTOTYPE_GETHOSTID

/* Define if your system has a prototype for gethostname */
#define HAVE_PROTOTYPE_GETHOSTNAME 1

/* Define if your system has a prototype for flock */
#undef HAVE_PROTOTYPE_FLOCK

/* Define if your system has a prototype for listen */
#define HAVE_PROTOTYPE_LISTEN 1

/* Define if your system has a prototype for socket */
#define HAVE_PROTOTYPE_SOCKET 1

/* Define if your system has a prototype for connect */
#define HAVE_PROTOTYPE_CONNECT 1

/* Define if your system has a prototype for select */
#define HAVE_PROTOTYPE_SELECT 1

/* Define if your system declares argument 2-4 of select() 
   as int * instead of struct fd_set * */
#undef HAVE_INTP_SELECT

/* Define if your system declares argument 3 of accept() 
   as int * instead of size_t * or socklen_t * */
#define HAVE_INTP_ACCEPT 1

/* Define if your system declares argument 5 of getsockopt()
   as int * instead of size_t * or socklen_t * */
#define HAVE_INTP_GETSOCKOPT 1

/* Define if your system supports readdir_r with the obsolete
   Posix 1.c draft 6 declaration (2 arguments) instead of the
   Posix 1.c declaration with 3 arguments. */
#undef HAVE_OLD_READDIR_R

/* Define if your system has a prototype for setsockopt */
#define HAVE_PROTOTYPE_SETSOCKOPT 1

/* Define if your system has a prototype for getsockopt */
#define HAVE_PROTOTYPE_GETSOCKOPT 1

/* Define if your system has a prototype for gettimeofday */
#undef HAVE_PROTOTYPE_GETTIMEOFDAY

/* Define if your system has a prototype for getsockname */
#define HAVE_PROTOTYPE_GETSOCKNAME 1

/* Define if your system has a prototype for wait3 */
#undef HAVE_PROTOTYPE_WAIT3

/* Define if your system has a prototype for waitpid */
#undef HAVE_PROTOTYPE_WAITPID

/* Define if your system has a prototype for usleep in libc.h */
#undef HAVE_PROTOTYPE_USLEEP

/* Define if your system has a prototype for mktemp */
#undef HAVE_PROTOTYPE_MKTEMP

/* Define if your system has a prototype for strcasecmp in string.h */
#undef HAVE_PROTOTYPE_STRCASECMP

/* Define if your system has a prototype for strncasecmp in string.h */
#undef HAVE_PROTOTYPE_STRNCASECMP

/* Define if your system has a prototype for _stricmp in string.h */
#define HAVE_PROTOTYPE__STRICMP 1

/* Define if your system has a prototype for struct utimbuf */
#define HAVE_DECLARATION_STRUCT_UTIMBUF 1

/* Define if your system has a prototype for socklen_t */
#undef HAVE_DECLARATION_SOCKLEN_T

/* Define if your system has a prototype for std::_Ios_Openmode */
#undef HAVE_DECLARATION_STD___IOS_OPENMODE

/* Define if your system defines ios::nocreate in iostream.h */
/* see definition for HAVE_IOS_NOCREATE below */

/* Define if your C++ compiler can work with class templates */
#define HAVE_CLASS_TEMPLATE 1
 
/* Define if your C++ compiler can work with function templates */
#define HAVE_FUNCTION_TEMPLATE 1
 
/* Define if your C++ compiler can work with static methods in class templates */
#define HAVE_STATIC_TEMPLATE_METHOD 1

/* Define if your C++ compiler supports the explicit template specialization syntax */
#define HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION 1

/* Define if your system supports POSIX read/write locks */
#undef HAVE_PTHREAD_RWLOCK

/* Define to `long' if <sys/types.h> doesn't define.  */
#define ssize_t long

/* by default, enable ANSI standard C++ includes on Visual C++ 6 and newer */
#ifdef _MSC_VER
#if _MSC_VER >= 1200
/* _MSC_VER == 1100 on Microsoft Visual C++ 5.0 */
/* _MSC_VER == 1200 on Microsoft Visual C++ 6.0 */
/* _MSC_VER == 1300 on Microsoft Visual C++ 7.0 (.NET) */
#define USE_STD_CXX_INCLUDES
#endif
#endif

/* Define if ANSI standard C++ includes are used */
/* must be explicitly defined in Makefile/Project File */

#ifdef USE_STD_CXX_INCLUDES

/* Define if your system defines ios::nocreate in iostream.h */
#undef HAVE_IOS_NOCREATE 

/* Define if ANSI standard C++ includes use std namespace */
#define HAVE_STD_NAMESPACE 1

/* Define if it is not possible to assign stream objects */
#define NO_IOS_BASE_ASSIGN 1

#else

/* Define if your system defines ios::nocreate in iostream.h */
#define HAVE_IOS_NOCREATE 1

/* Define if ANSI standard C++ includes use std namespace */
#undef HAVE_STD_NAMESPACE 

/* Define if it is not possible to assign stream objects */
#undef NO_IOS_BASE_ASSIGN 

#endif

/* Define if we are compiling for built-in private tag dictionary */
/* #undef WITH_PRIVATE_TAGS */

/* Define if we are compiling with OpenSSL support */
/* #undef WITH_OPENSSL */

/* Define if we are compiling with libtiff support */
#define WITH_LIBTIFF

/* Define if we are compiling with zlib support */
#define WITH_ZLIB

/* Define if we are compiling with any type of Multi-thread support */
#define WITH_THREADS

/* Define to the inline keyword supported by the C compiler, if any, or to the empty string */
#define C_INLINE __inline

/* Define if "const" is supported by the C compiler */
#define HAVE_C_CONST 1

/* Define if char is unsigned on the C compiler */
/* #undef C_CHAR_UNSIGNED */

/* Define if >> is unsigned on the C compiler */
/* #undef C_RIGHTSHIFT_UNSIGNED */

/* Define the canonical host system type as a string constant */
#define CANONICAL_HOST_TYPE "i386-pc-win32"

/* Define if signal handlers need ellipse (...) parameters */
/* #undef SIGNAL_HANDLER_WITH_ELLIPSE */

/* Define if <math.h> fails if included extern "C" */
#define INCLUDE_MATH_H_AS_CXX 1

#endif /* _WIN32 */

#endif /* CFWIN32_H */
