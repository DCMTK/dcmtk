/*
** Configuration definations for the Metrowerks C++ Compiler environment
** on the Apple Macintosh. Created by hand.
** 
** Created: Andrew Hewett, 4.11.95
** Modified:
*/

#ifndef CFMWERKS_H
#define CFMWERKS_H

/* only process if running a Metrowerks compiler */
#ifdef __MWERKS__

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* The number of bytes in a int.  */
/* only true if the 4 byte integer compile option selected */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a double.  */
/* only true if the 8 byte double compile option selected */
#define SIZEOF_DOUBLE 8

/* Define if you have the gethostid function.  */
/* #undef HAVE_GETHOSTID */

/* Define if you have the gethostname function.  */
/* #undef HAVE_GETHOSTNAME */

/* Define if you have the itoa function.  */
#define HAVE_ITOA 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the memcpy function */
#define HAVE_MEMCPY 1

/* Define if you have the strerror function */
#define HAVE_STRERROR 1

/* Define if you have the getpid function */
/* Metrowerks on Mac defines a getpid() in <unistd.h> which
** simply returns a constant value
*/
/* if GUSI is being used then this does not work */
/* #define HAVE_GETPID 1 */

/* Define if you have a working getenv function */
/* The Macintosh has no concept of environment variables */
/* Metrowerks on Macintosh has genenv, but it always returns NULL */
/* #undef HAVE_GETENV */

/* Define if you have the iostream library (-liostream).  */
/* #undef HAVE_LIBIOSTREAM 1 */

/* Define if you have a <unix.h> include file */
#define HAVE_UNIX_H 1

/* Define if you gave a <fcntl.h> include file */
#define HAVE_FCNTL_H 1

/* Define if you gave a <stat.h> include file */
#define HAVE_STAT_H 1

/* Define if you gave a <stdlib.h> include file */
#define HAVE_STDLIB_H 1

/* Define if you gave a <stdarg.h> include file */
#define HAVE_STDARG_H 1

/* Define if you gave a <stddef.h> include file */
#define HAVE_STDDEF_H 1

/* This set of include files are available in the GUSI socket library */
#define HAVE_GUSI_H 1	/* use the GUSI include file */
#define HAVE_FLOCK 1	/* GUSI defines a version */
#define HAVE_ACCESS 1	/* GUSI defines a version */

/* User definable section */

/* Define the file system path separator */
#define PATH_SEPARATOR ':'

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR '^'

/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH	"dicom.dic"


/* Define if your implementation cannot pass command line arguments into main() */
#define HAVE_EMPTY_ARGC_ARGV 1

/* Compile in the debug code */
#define DEBUG 1

/* Define if your compile cannot user anonymous struct/class components */
/* #define NO_ANON_CLASS_COMP 1 */ 

/* Compiling for Macintosh */
#define macintosh 1

#endif /* __MWERKS__ */

#endif /* CFMWERKS_H */
