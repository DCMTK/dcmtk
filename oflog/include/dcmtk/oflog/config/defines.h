#ifndef LOG4CPLUS_CONFIG_DEFINES_HXX
#define LOG4CPLUS_CONFIG_DEFINES_HXX

/* */
#ifdef HAVE_SYSLOG_H
#define LOG4CPLUS_HAVE_SYSLOG_H
#endif

/* */
#ifdef HAVE_NETINET_IN_H
#define LOG4CPLUS_HAVE_NETINET_IN_H
#endif

/* */
#ifdef HAVE_FTIME
#define LOG4CPLUS_HAVE_FTIME
#endif

/* */
#ifdef HAVE_PROTOTYPE_GETTIMEOFDAY
#define LOG4CPLUS_HAVE_GETTIMEOFDAY
#endif

/* */
#ifdef HAVE_GMTIME_R
#define LOG4CPLUS_HAVE_GMTIME_R
#endif

/* */
#ifdef HAVE_HTONL
#define LOG4CPLUS_HAVE_HTONL
#endif

/* */
#ifdef HAVE_HTONS
#define LOG4CPLUS_HAVE_HTONS
#endif

/* */
#ifdef HAVE_LOCALTIME_R
#define LOG4CPLUS_HAVE_LOCALTIME_R
#endif

/* */
#ifdef HAVE_LSTAT
#define LOG4CPLUS_HAVE_LSTAT
#endif

/* */
#ifdef HAVE_NTOHL
#define LOG4CPLUS_HAVE_NTOHL
#endif

/* */
#ifdef HAVE_NTOHS
#define LOG4CPLUS_HAVE_NTOHS
#endif

/* */
#ifdef HAVE_STAT
#define LOG4CPLUS_HAVE_STAT
#endif

/* */
#ifdef WITH_THREADS
#define LOG4CPLUS_USE_PTHREADS
#else
/* Define if this is a single-threaded library. */
#define LOG4CPLUS_SINGLE_THREADED
#endif

/* Define for compilers/standard libraries that support more than just the "C"
   locale. */
// No idea when to define this one
#undef LOG4CPLUS_WORKING_LOCALE

/* Define to int if undefined. */
#if 0
/* This block is handled by configure completely */
#undef socklen_t
#ifndef HAVE_DECLARATION_SOCKLEN_T
#define socklen_t int
#endif
#endif

/* Defined for --enable-debugging builds. */
#ifdef DEBUG
#define LOG4CPLUS_DEBUGGING
#endif

/* These two are not needed since we build static libraries */
/* Defined if the compiler understands __declspec(export) or __attribute__((export)) construct. */
#define LOG4CPLUS_DECLSPEC_EXPORT
/* Defined if the compiler understands __declspec(import) or __attribute__((import)) construct. */
#define LOG4CPLUS_DECLSPEC_IMPORT

/* Defined if the host OS provides ENAMETOOLONG errno value. */
#ifdef HAVE_ENAMETOOLONG
#define LOG4CPLUS_HAVE_ENAMETOOLONG
#endif

#endif // LOG4CPLUS_CONFIG_DEFINES_HXX
