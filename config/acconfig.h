#ifndef CONFIG_H
#define CONFIG_H

@TOP@

@BOTTOM@
/* User definable section */

/* Define path separator */
#define PATH_SEPARATOR '/'

/* Define the environment variable path separator */
#define ENVIRONMENT_PATH_SEPARATOR ':'

/* Define the default data dictionary path for the dcmdata library package */
#define DCM_DICT_DEFAULT_PATH	"/usr/local/dicom/lib/dicom.dic"

/* Define if your system cannot pass command line arguments into main() (e.g. Macintosh) */
/* #define HAVE_EMPTY_ARGC_ARGV 1 */

/* Compile in the debug code */
/* #define DEBUG 1 */ 

/* Define if your compile cannot user anonymous struct/class components */
/* #define NO_ANON_CLASS_COMP 1 */ 

/* Define if your system has a prototype for accept */
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

/* Define if your system has a prototype for getsockopt */
#undef HAVE_PROTOTYPE_GETSOCKOPT

/* Define if your system has a prototype for listen */
#undef HAVE_PROTOTYPE_LISTEN

/* Define if your system has a prototype for socket */
#undef HAVE_PROTOTYPE_SOCKET

/* Define if your system has a prototype for connect */
#undef HAVE_PROTOTYPE_CONNECT

/* Define if your system has a prototype for setsockopt */
#undef HAVE_PROTOTYPE_SETSOCKOPT

/* Define if your system has a prototype for wait3 */
#undef HAVE_PROTOTYPE_WAIT3

/* Define if your system has a prototype for usleep in libc.h */
#undef HAVE_PROTOTYPE_USLEEP

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

#endif

