#ifndef CFUNIX_H
#define CFUNIX_H

@TOP@

@BOTTOM@
/* User definable section */

/* Define the DCMTK default path */
#undef DCMTK_PREFIX

/* Define the default data dictionary path for the dcmdata library package */
#undef DCM_DICT_DEFAULT_PATH

/* Define path separator */
#define PATH_SEPARATOR '/'

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR ':'

/* Define if your system cannot pass command line arguments into main() (e.g. Macintosh) */
/* #define HAVE_EMPTY_ARGC_ARGV 1 */

/* Compile in the debug code */
/* #define DEBUG 1 */

/* Define if your compile cannot user anonymous struct/class components */
/* #define NO_ANON_CLASS_COMP 1 */

/* Define if your system has a prototype for bzero */
#undef HAVE_PROTOTYPE_BZERO

/* Define if your system has a prototype for accept */
#undef HAVE_PROTOTYPE_ACCEPT

/* Define if your system has a prototype for bind */
#undef HAVE_PROTOTYPE_BIND

/* Define if your system has a prototype for gethostbyname */
#undef HAVE_PROTOTYPE_GETHOSTBYNAME

/* Define if your system has a prototype for gethostid */
#undef HAVE_PROTOTYPE_GETHOSTID

/* Define if your system has a prototype for gethostname */
#undef HAVE_PROTOTYPE_GETHOSTNAME

/* Define if your system has a prototype for flock */
#undef HAVE_PROTOTYPE_FLOCK

/* Define if your system has a prototype for listen */
#undef HAVE_PROTOTYPE_LISTEN

/* Define if your system has a prototype for socket */
#undef HAVE_PROTOTYPE_SOCKET

/* Define if your system has a prototype for connect */
#undef HAVE_PROTOTYPE_CONNECT

/* Define if your system has a prototype for select */
#undef HAVE_PROTOTYPE_SELECT

/* Define if your system declares argument 2-4 of select()
   as int * instead of struct fd_set * */
#undef HAVE_INTP_SELECT

/* Define if your system declares argument 3 of accept()
   as int * instead of size_t * or socklen_t * */
#undef HAVE_INTP_ACCEPT

/* Define if your system declares argument 5 of getsockopt()
   as int * instead of size_t * or socklen_t * */
#undef HAVE_INTP_GETSOCKOPT

/* Define if your system supports readdir_r with the obsolete
   Posix 1.c draft 6 declaration (2 arguments) instead of the
   Posix 1.c declaration with 3 arguments. */
#undef HAVE_OLD_READDIR_R

/* Define if your system has a prototype for setsockopt */
#undef HAVE_PROTOTYPE_SETSOCKOPT

/* Define if your system has a prototype for getsockopt */
#undef HAVE_PROTOTYPE_GETSOCKOPT

/* Define if your system has a prototype for gettimeofday */
#undef HAVE_PROTOTYPE_GETTIMEOFDAY

/* Define if your system has a prototype for getsockname */
#undef HAVE_PROTOTYPE_GETSOCKNAME

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
#undef HAVE_PROTOTYPE__STRICMP

/* Define if your system has a prototype for struct utimbuf */
#undef HAVE_DECLARATION_STRUCT_UTIMBUF

/* Define if your system has a prototype for socklen_t */
#undef HAVE_DECLARATION_SOCKLEN_T

/* Define if your system has a prototype for std::_Ios_Openmode */
#undef HAVE_DECLARATION_STD___IOS_OPENMODE

/* Define if your system defines ios::nocreate in iostream.h */
#undef HAVE_IOS_NOCREATE

/* Define if your C++ compiler can work with class templates */
#undef HAVE_CLASS_TEMPLATE

/* Define if your C++ compiler can work with function templates */
#undef HAVE_FUNCTION_TEMPLATE

/* Define if your C++ compiler can work with static methods in class templates */
#undef HAVE_STATIC_TEMPLATE_METHOD

/* Define if your C++ compiler supports the explicit template specialization syntax */
#undef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION

/* Define if your system supports POSIX read/write locks */
#undef HAVE_PTHREAD_RWLOCK

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#undef ssize_t

/* Define if ANSI standard C++ includes are used */
#undef USE_STD_CXX_INCLUDES

/* Define if ANSI standard C++ includes use std namespace */
#undef HAVE_STD_NAMESPACE

/* Define if we are compiling for built-in private tag dictionary */
#undef WITH_PRIVATE_TAGS

/* Define if we are compiling with OpenSSL support */
#undef WITH_OPENSSL

/* Define if we are compiling with libtiff support */
#undef WITH_LIBTIFF

/* Define if we are compiling with libpng support */
#undef WITH_LIBPNG

/* Define if we are compiling with zlib support */
#undef WITH_ZLIB

/* Define if we are compiling with any type of Multi-thread support */
#undef WITH_THREADS

/* Define to the inline keyword supported by the C compiler, if any, or to the empty string */
#undef C_INLINE

/* Define if "const" is supported by the C compiler */
#undef HAVE_C_CONST

/* Define if char is unsigned on the C compiler */
#undef C_CHAR_UNSIGNED

/* Define if >> is unsigned on the C compiler */
#undef C_RIGHTSHIFT_UNSIGNED

/* Define the canonical host system type as a string constant */
#undef CANONICAL_HOST_TYPE

/* Define if signal handlers need ellipse (...) parameters */
#undef SIGNAL_HANDLER_WITH_ELLIPSE

/* Define if <math.h> fails if included extern "C" */
#undef INCLUDE_MATH_H_AS_CXX

#endif
