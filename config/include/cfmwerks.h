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

/* Define if you have a working getenv function */
/* The Macintosh has no concept of environment variables */
/* Metrowerks on Macintosh has genenv, but it always returns NULL */
/* #undef HAVE_GETENV */

/* Define if you have the iostream library (-liostream).  */
/* #undef HAVE_LIBIOSTREAM 1 */

/* Define if you have a <unix.h> include file */
#define HAVE_UNIX_H 1

/* User definable section */

/* Define the file system path separator */
#define PATH_SEPERATOR ':'

/* Define the environment variable path separator */
/* #undef ENVIRONMENT_PATH_SEPARATOR ':' */

/* Define if your implementation cannot pass command line arguments into main() */
#define HAVE_EMPTY_ARGC_ARGV 1

/* Compile in the debug code */
#define DEBUG 1

/* Compiling for Macintosh */
#define macintosh 1

#endif /* __MWERKS__ */

#endif /* CFMWERKS_H */
