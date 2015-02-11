#ifndef DCMTK_LOG4CPLUS_CONFIG_DEFINES_HXX
#define DCMTK_LOG4CPLUS_CONFIG_DEFINES_HXX

/* */
#ifdef HAVE_SYSLOG_H
#define DCMTK_LOG4CPLUS_HAVE_SYSLOG_H
#endif

/* */
#ifdef HAVE_ARPA_INET_H
#define DCMTK_LOG4CPLUS_HAVE_ARPA_INET_H
#endif

/* */
#ifdef HAVE_NETINET_IN_H
#define DCMTK_LOG4CPLUS_HAVE_NETINET_IN_H
#endif

/* */
#ifdef HAVE_NETINET_TCP_H
#define DCMTK_LOG4CPLUS_HAVE_NETINET_TCP_H
#endif

/* */
#ifdef HAVE_SYS_TIMEB_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_TIMEB_H
#endif

/* */
#ifdef HAVE_SYS_TIME_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_TIME_H
#endif

/* */
#ifdef HAVE_SYS_TYPES_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_TYPES_H
#endif

/* */
#ifdef HAVE_SYS_STAT_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_STAT_H
#endif

/* */
#ifdef HAVE_SYS_SYSCALL_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_SYSCALL_H
#endif

/* */
#ifdef HAVE_SYS_FILE_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_FILE_H
#endif

/* */
#ifdef HAVE_TIME_H
#define DCMTK_LOG4CPLUS_HAVE_TIME_H
#endif

/* */
#ifdef HAVE_SYS_SOCKET_H
#define DCMTK_LOG4CPLUS_HAVE_SYS_SOCKET_H
#endif

/* */
#ifdef HAVE_NETDB_H
#define DCMTK_LOG4CPLUS_HAVE_NETDB_H
#endif

/* */
#ifdef HAVE_UNISTD_H
#define DCMTK_LOG4CPLUS_HAVE_UNISTD_H
#endif

/* */
#ifdef HAVE_FCNTL_H
#define DCMTK_LOG4CPLUS_HAVE_FCNTL_H
#endif

/* */
#ifdef HAVE_STDARG_H
#define DCMTK_LOG4CPLUS_HAVE_STDARG_H
#endif

/* */
#ifdef HAVE_STDIO_H
#define DCMTK_LOG4CPLUS_HAVE_STDIO_H
#endif

/* */
#ifdef HAVE_STDLIB_H
#define DCMTK_LOG4CPLUS_HAVE_STDLIB_H
#endif

/* */
#ifdef HAVE_ERRNO_H
#define DCMTK_LOG4CPLUS_HAVE_ERRNO_H
#endif

/* */
#ifdef HAVE_WCHAR_H
#define DCMTK_LOG4CPLUS_HAVE_WCHAR_H
#endif

/* */
#ifdef HAVE_LIMITS_H
#define DCMTK_LOG4CPLUS_HAVE_LIMITS_H
#endif

/* */
#ifdef HAVE_FTIME
#define DCMTK_LOG4CPLUS_HAVE_FTIME
#endif

/* */
#ifdef HAVE_GETADDRINFO
#define DCMTK_LOG4CPLUS_HAVE_GETADDRINFO
#endif

/* */
#ifdef HAVE_GETHOSTBYNAME_R
#define DCMTK_LOG4CPLUS_HAVE_GETHOSTBYNAME_R
#endif

/* */
#ifdef HAVE_GETPID
#define DCMTK_LOG4CPLUS_HAVE_GETPID
#endif

/* */
#ifdef HAVE_PROTOTYPE_GETTIMEOFDAY
#define DCMTK_LOG4CPLUS_HAVE_GETTIMEOFDAY
#endif

/* Define to 1 if you have the `clock_gettime' function. */
#ifdef HAVE_PROTOTYPE_CLOCK_GETTIME
#define DCMTK_LOG4CPLUS_HAVE_CLOCK_GETTIME
#endif

/* Define to 1 if you have the `nanosleep' function. */
#ifdef HAVE_NANOSLEEP
#define DCMTK_LOG4CPLUS_HAVE_NANOSLEEP
#endif

/* Define to 1 if you have the `clock_nanosleep' function. */
#ifdef HAVE_CLOCK_NANOSLEEP
#define DCMTK_LOG4CPLUS_HAVE_CLOCK_NANOSLEEP
#endif

/* */
#ifdef HAVE_GMTIME_R
#define DCMTK_LOG4CPLUS_HAVE_GMTIME_R
#endif

/* */
#ifdef HAVE_HTONL
#define DCMTK_LOG4CPLUS_HAVE_HTONL
#endif

/* */
#ifdef HAVE_HTONS
#define DCMTK_LOG4CPLUS_HAVE_HTONS
#endif

/* */
#ifdef HAVE_LOCALTIME_R
#define DCMTK_LOG4CPLUS_HAVE_LOCALTIME_R
#endif

/* */
#ifdef HAVE_LSTAT
#define DCMTK_LOG4CPLUS_HAVE_LSTAT
#endif

/* */
#ifdef HAVE_FCNTL
#define DCMTK_LOG4CPLUS_HAVE_FCNTL
#endif

/* */
#ifdef HAVE_LOCKF
#define DCMTK_LOG4CPLUS_HAVE_LOCKF
#endif

/* */
#ifdef HAVE_FLOCK
#define DCMTK_LOG4CPLUS_HAVE_FLOCK
#endif

/* */
#ifdef HAVE_NTOHL
#define DCMTK_LOG4CPLUS_HAVE_NTOHL
#endif

/* */
#ifdef HAVE_NTOHS
#define DCMTK_LOG4CPLUS_HAVE_NTOHS
#endif

/* */
#ifdef HAVE_STAT
#define DCMTK_LOG4CPLUS_HAVE_STAT
#endif

/* */
#ifdef WITH_THREADS
#define DCMTK_LOG4CPLUS_USE_PTHREADS
#else
/* Define if this is a single-threaded library. */
#define DCMTK_LOG4CPLUS_SINGLE_THREADED
#endif

/* Define for compilers/standard libraries that support more than just the "C"
   locale. */
#undef DCMTK_LOG4CPLUS_WORKING_LOCALE

/* Define for C99 compilers/standard libraries that support more than just the
   "C" locale. */
#undef DCMTK_LOG4CPLUS_WORKING_C_LOCALE

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
#define DCMTK_LOG4CPLUS_DEBUGGING
#endif

/* These are not needed since config.h uses DCMTK_DECL_EXPORT/IMPORT directly */
#if 0
/* Defined if the compiler understands __declspec(dllexport) or
   __attribute__((visibility("default"))) construct. */
#define DCMTK_LOG4CPLUS_DECLSPEC_EXPORT

/* Defined if the compiler understands __declspec(dllimport) or
   __attribute__((visibility("default"))) construct. */
#define DCMTK_LOG4CPLUS_DECLSPEC_IMPORT

/* Defined if the compiler understands
   __attribute__((visibility("hidden"))) construct. */
#define DCMTK_LOG4CPLUS_DECLSPEC_PRIVATE
#endif

/* This define is unused and thus its value does not matter */
#undef DCMTK_LOG4CPLUS_HAVE_TLS_SUPPORT

/* Configure should set this to thread_local, __declspec(thread) or __thread,
 * whatever works. However, let's just use the TLS functions directly instead. */
#undef DCMTK_LOG4CPLUS_THREAD_LOCAL_VAR

/* Defined if the host OS provides ENAMETOOLONG errno value. */
#ifdef HAVE_ENAMETOOLONG
#define DCMTK_LOG4CPLUS_HAVE_ENAMETOOLONG
#endif

/* Defined if the compiler provides __sync_add_and_fetch(). */
#ifdef HAVE_SYNC_ADD_AND_FETCH
#define DCMTK_LOG4CPLUS_HAVE___SYNC_ADD_AND_FETCH
#endif

/* Defined if the compiler provides __sync_sub_and_fetch(). */
#ifdef HAVE_SYNC_SUB_AND_FETCH
#define DCMTK_LOG4CPLUS_HAVE___SYNC_SUB_AND_FETCH
#endif

/* Defined if the compiler provides C++11 <atomic> header and increment,
   decrement operations. */
#ifdef HAVE_CXX11_ATOMICS
#define DCMTK_LOG4CPLUS_HAVE_CXX11_ATOMICS
#endif

/* */
#ifdef HAVE_C99_VARIADIC_MACROS
#define DCMTK_LOG4CPLUS_HAVE_C99_VARIADIC_MACROS
#endif

/* */
#ifdef HAVE_GNU_VARIADIC_MACROS
#define DCMTK_LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS
#endif

/* */
#ifdef HAVE_VSNPRINTF
#define DCMTK_LOG4CPLUS_HAVE_VSNPRINTF
#endif

/* */
#ifdef HAVE_PROTOTYPE_STD__VSNPRINTF
#define DCMTK_LOG4CPLUS_HAVE__VSNPRINTF
#endif

/* Define to 1 if you have the `vfprintf_s' function. */
#ifdef HAVE_VFPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE_VFPRINTF_S
#endif

/* Define to 1 if you have the `vfwprintf_s' function. */
#ifdef HAVE_VFWPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE_VFWPRINTF_S
#endif

/* Define to 1 if you have the `vsprintf_s' function. */
#ifdef HAVE_VSPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE_VSPRINTF_S
#endif

/* Define to 1 if you have the `vswprintf_s' function. */
#ifdef HAVE_VSWPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE_VSWPRINTF_S
#endif

/* Define to 1 if you have the `_vsnprintf_s' function. */
#ifdef HAVE__VSNPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE__VSNPRINTF_S
#endif

/* Define to 1 if you have the `_vsnwprintf_s' function. */
#ifdef HAVE__VSNWPRINTF_S
#define DCMTK_LOG4CPLUS_HAVE__VSNWPRINTF_S
#endif

/* Defined if the compiler supports __FUNCTION__ macro. */
#ifdef HAVE___FUNCTION___MACRO
#define DCMTK_LOG4CPLUS_HAVE_FUNCTION_MACRO
#endif

/* Defined if the compiler supports __PRETTY_FUNCTION__ macro. */
#ifdef HAVE___PRETTY_FUNCTION___MACRO
#define DCMTK_LOG4CPLUS_HAVE_PRETTY_FUNCTION_MACRO
#endif

/* Defined if the compiler supports __func__ symbol. */
#ifdef HAVE___func___MACRO
#define DCMTK_LOG4CPLUS_HAVE_FUNC_SYMBOL
#endif

/* Define to 1 if you have the `mbstowcs' function. */
#ifdef HAVE_MBSTOWCS
#define DCMTK_LOG4CPLUS_HAVE_MBSTOWCS
#endif

/* Define to 1 if you have the `wcstombs' function. */
#ifdef HAVE_WCSTOMBS
#define DCMTK_LOG4CPLUS_HAVE_WCSTOMBS
#endif

/* Define to 1 if you have Linux style syscall(SYS_gettid). */
#if defined(HAVE_SYS_SYSCALL_H) && defined(HAVE_SYS_GETTID)
#define DCMTK_LOG4CPLUS_HAVE_GETTID
#endif

/* Define when iconv() is available. */
#ifdef WITH_LIBICONV
#define DCMTK_LOG4CPLUS_WITH_ICONV
#define DCMTK_LOG4CPLUS_HAVE_ICONV_H
#endif

#endif // DCMTK_LOG4CPLUS_CONFIG_DEFINES_HXX
