/*

** Configuration definations for the Microsoft Visual C++ 1.0 environment

** on the WINDOWS 3.1 PC. Created by hand.

** 

** Created: Andreas Barth, 06.12.95

** Modified:

*/



#ifndef MSCW31_H

#define MSCW31_H



/* Define if you have the vprintf function.  */

#define HAVE_VPRINTF 1



/* Define if you have the ANSI C header files.  */

#define STDC_HEADERS 1



/* The number of bytes in a char.  */

#define SIZEOF_CHAR 1



/* The number of bytes in a short.  */

#define SIZEOF_SHORT 2



/* The number of bytes in a int.  */

#define SIZEOF_INT 2



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



/* Define if you have the memcpy function.  */

#define HAVE_MEMCPY 1



/* Define if you have the mktemp function.  */

#define HAVE_MKTEMP 1

#define mktemp _mktemp 

#include <io.h> 



#define unlink _unlink

                       

/* Define if you have the <stdlib.h> header file.  */

#define HAVE_STDLIB_H 1



/* Define if you have a working getenv function */

/* The Macintosh has no concept of environment variables */

/* Metrowerks on Macintosh has genenv, but it always returns NULL */

/* #undef HAVE_GETENV */



/* Define if you have the iostream library (-liostream).  */

/* #undef HAVE_LIBIOSTREAM 1 */



/* Define if you have a <unix.h> include file */

/* #undef HAVE_UNIX_H */



/* User definable section */



/* Define the file system path separator */

#define PATH_SEPERATOR '\\'



/* Define the environment variable path separator */

/* #undef ENVIRONMENT_PATH_SEPARATOR ':' */


/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH	"\\dicom.dic"


/* Define if your implementation cannot pass command line arguments into main() */

#define HAVE_EMPTY_ARGC_ARGV 1



/* Compile in the debug code */

#define DEBUG 1



/* Define if your compile cannot user anonymous struct/class components */

#define NO_ANON_CLASS_COMP 1  



/* Compiling for WINDOWS 3.1 */

#define win31 1



#endif /* MSCW31_H */

