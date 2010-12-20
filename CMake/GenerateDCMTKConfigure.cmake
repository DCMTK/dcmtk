# Private tags
IF(DCMTK_WITH_PRIVATE_TAGS)
  SET(WITH_PRIVATE_TAGS 1)
  MESSAGE(STATUS "Info: DCMTK's builtin private dictionary support will be enabled")
ELSE(DCMTK_WITH_PRIVATE_TAGS)
  SET(WITH_PRIVATE_TAGS "")
  MESSAGE(STATUS "Info: DCMTK's builtin private dictionary support will be disabled")
ENDIF(DCMTK_WITH_PRIVATE_TAGS)

# Thread support
IF(DCMTK_WITH_THREADS)
  SET(WITH_THREADS 1)
  MESSAGE(STATUS "Info: Thread support will be enabled")
ELSE(DCMTK_WITH_THREADS)
  SET(WITH_THREADS "")
  MESSAGE(STATUS "Info: Thread support will be disabled")
ENDIF(DCMTK_WITH_THREADS)

# Standard C++ headers (currently hard-coded)
#IF(VTK_USE_ANSI_STDLIB)
  SET(USE_STD_CXX_INCLUDES 1)
#ELSE(VTK_USE_ANSI_STDLIB)
#  SET(USE_STD_CXX_INCLUDES "")
#ENDIF(VTK_USE_ANSI_STDLIB)

# Configure file

# CMake doesn't provide a configure-style system type string
SET(CANONICAL_HOST_TYPE "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")

# Configure dictionary path and install prefix
IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
  # Set DCMTK_PREFIX needed within some code. Be sure that all / are replaced by \\.
  SET(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  STRING(REGEX REPLACE "/" "\\\\\\\\" DCMTK_PREFIX "${DCMTK_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  SET(PATH_SEPARATOR "\\\\")
  SET(ENVIRONMENT_PATH_SEPARATOR ";")
  # Set dictionary path to the data dir inside install main dir (prefix)
  SET(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}${INSTALL_DATDIR}${PATH_SEPARATOR}dicom.dic")
  # If private dictionary should be utilized, add it to default dictionary path.
  IF(WITH_PRIVATE_TAGS)
    SET(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}${ENVIRONMENT_PATH_SEPARATOR}${DCMTK_PREFIX}${INSTALL_DATDIR}${PATH_SEPARATOR}private.dic")
  ENDIF(WITH_PRIVATE_TAGS)
  # Again, for Windows strip all / from path and replace it with \\.
  STRING(REGEX REPLACE "/" "\\\\\\\\" DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}")
  # Set default directory for configuration and support data.
  SET(DCMTK_DEFAULT_CONFIGURATION_DIR "")
  SET(DCMTK_DEFAULT_SUPPORT_DATA_DIR "")
ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
  # Set DCMTK_PREFIX needed within some code.
  SET(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  SET(PATH_SEPARATOR "/")
  SET(ENVIRONMENT_PATH_SEPARATOR ":")
  # Set dictionary path to the data dir inside install main dir (prefix).
  SET(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}${INSTALL_DATDIR}${PATH_SEPARATOR}dicom.dic")
  # If private dictionary should be utilized, add it to default dictionary path.
  IF(WITH_PRIVATE_TAGS)
    SET(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}${ENVIRONMENT_PATH_SEPARATOR}${DCMTK_PREFIX}${INSTALL_DATDIR}${PATH_SEPARATOR}private.dic")
  ENDIF(WITH_PRIVATE_TAGS)
  # Set default directory for configuration and support data.
  SET(DCMTK_DEFAULT_CONFIGURATION_DIR "${DCMTK_PREFIX}${INSTALL_ETCDIR}${PATH_SEPARATOR}")
  SET(DCMTK_DEFAULT_SUPPORT_DATA_DIR "${DCMTK_PREFIX}${INSTALL_DATDIR}${PATH_SEPARATOR}")
ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)

# Check the sizes of various types
INCLUDE (CheckTypeSize)
CHECK_TYPE_SIZE("char" SIZEOF_CHAR)
CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("float" SIZEOF_FLOAT)
CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P)

# Check for include files, libraries, and functions
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake)
INCLUDE(${DCMTK_SOURCE_DIR}/CMake/CheckFunctionWithHeaderExists.cmake)

# For Windows, hardcode these values to avoid long search times
IF(WIN32 AND NOT CYGWIN)
  CHECK_INCLUDE_FILE_CXX("windows.h" HAVE_WINDOWS_H)
  CHECK_INCLUDE_FILE_CXX("winsock.h" HAVE_WINSOCK_H)
ENDIF(WIN32 AND NOT CYGWIN)

  CHECK_INCLUDE_FILE_CXX("errno.h" HAVE_ERRNO_H)
  CHECK_INCLUDE_FILE_CXX("dirent.h" HAVE_DIRENT_H)
  CHECK_INCLUDE_FILE_CXX("fcntl.h" HAVE_FCNTL_H)
  CHECK_INCLUDE_FILE_CXX("fstream" HAVE_FSTREAM)
  CHECK_INCLUDE_FILE_CXX("fstream.h" HAVE_FSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("fnmatch.h" HAVE_FNMATCH_H)
  CHECK_INCLUDE_FILE_CXX("float.h" HAVE_FLOAT_H)
  CHECK_INCLUDE_FILE_CXX("malloc.h" HAVE_MALLOC_H)
  CHECK_INCLUDE_FILE_CXX("math.h" HAVE_MATH_H)
  CHECK_INCLUDE_FILE_CXX("ieeefp.h" HAVE_IEEEFP_H)
  CHECK_INCLUDE_FILE_CXX("inttypes.h" HAVE_INTTYPES_H)
  CHECK_INCLUDE_FILE_CXX("iomanip" HAVE_IOMANIP)
  CHECK_INCLUDE_FILE_CXX("iomanip.h" HAVE_IOMANIP_H)
  CHECK_INCLUDE_FILE_CXX("iostream" HAVE_IOSTREAM)
  CHECK_INCLUDE_FILE_CXX("iostream.h" HAVE_IOSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("io.h" HAVE_IO_H)
  CHECK_INCLUDE_FILE_CXX("iso646.h" HAVE_ISO646_H)
  CHECK_INCLUDE_FILE_CXX("png.h" HAVE_LIBPNG_PNG_H)
  CHECK_INCLUDE_FILE_CXX("limits.h" HAVE_LIMITS_H)
  CHECK_INCLUDE_FILE_CXX("locale.h" HAVE_LOCALE_H)
  CHECK_INCLUDE_FILE_CXX("ndir.h" HAVE_NDIR_H)
  CHECK_INCLUDE_FILE_CXX("netdb.h" HAVE_NETDB_H)
  CHECK_INCLUDE_FILE_CXX("new.h" HAVE_NEW_H)
  CHECK_INCLUDE_FILE_CXX("semaphore.h" HAVE_SEMAPHORE_H)
  CHECK_INCLUDE_FILE_CXX("setjmp.h" HAVE_SETJMP_H)
  CHECK_INCLUDE_FILE_CXX("sstream" HAVE_SSTREAM)
  CHECK_INCLUDE_FILE_CXX("sstream.h" HAVE_SSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("stat.h" HAVE_STAT_H)
  CHECK_INCLUDE_FILE_CXX("stddef.h" HAVE_STDDEF_H)
  CHECK_INCLUDE_FILE_CXX("stdint.h" HAVE_STDINT_H)
  CHECK_INCLUDE_FILE_CXX("stdio.h" HAVE_STDIO_H)
  CHECK_INCLUDE_FILE_CXX("cstdio" HAVE_CSTDIO)
  CHECK_INCLUDE_FILE_CXX("strings.h" HAVE_STRINGS_H)
  CHECK_INCLUDE_FILE_CXX("string.h" HAVE_STRING_H)
  CHECK_INCLUDE_FILE_CXX("strstream.h" HAVE_STRSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("strstream" HAVE_STRSTREAM)
  CHECK_INCLUDE_FILE_CXX("strstrea.h" HAVE_STRSTREA_H)
  CHECK_INCLUDE_FILE_CXX("synch.h" HAVE_SYNCH_H)
  CHECK_INCLUDE_FILE_CXX("sys/errno.h" HAVE_SYS_ERRNO_H)
  CHECK_INCLUDE_FILE_CXX("sys/dir.h" HAVE_SYS_DIR_H)
  CHECK_INCLUDE_FILE_CXX("sys/file.h" HAVE_SYS_FILE_H)
  CHECK_INCLUDE_FILE_CXX("sys/ndir.h" HAVE_SYS_NDIR_H)
  CHECK_INCLUDE_FILE_CXX("sys/param.h" HAVE_SYS_PARAM_H)
  CHECK_INCLUDE_FILE_CXX("sys/resource.h" HAVE_SYS_RESOURCE_H)
  CHECK_INCLUDE_FILE_CXX("sys/select.h" HAVE_SYS_SELECT_H)
  CHECK_INCLUDE_FILE_CXX("sys/time.h" HAVE_SYS_TIME_H)
  CHECK_INCLUDE_FILE_CXX("sys/types.h" HAVE_SYS_TYPES_H)
  CHECK_INCLUDE_FILE_CXX("sys/utime.h" HAVE_SYS_UTIME_H)
  CHECK_INCLUDE_FILE_CXX("sys/utsname.h" HAVE_SYS_UTSNAME_H)
  CHECK_INCLUDE_FILE_CXX("sys/wait.h" HAVE_SYS_WAIT_H)
  CHECK_INCLUDE_FILE_CXX("sys/socket.h" HAVE_SYS_SOCKET_H)
  CHECK_INCLUDE_FILE_CXX("thread.h" HAVE_THREAD_H)
  CHECK_INCLUDE_FILE_CXX("sys/stat.h" HAVE_SYS_STAT_H)
  CHECK_INCLUDE_FILE_CXX("time.h" HAVE_TIME_H)
  CHECK_INCLUDE_FILE_CXX("unistd.h" HAVE_UNISTD_H)
  CHECK_INCLUDE_FILE_CXX("unix.h" HAVE_UNIX_H)
  CHECK_INCLUDE_FILE_CXX("utime.h" HAVE_UTIME_H)
  CHECK_INCLUDE_FILE_CXX("wctype.h" HAVE_WCTYPE_H)
  CHECK_INCLUDE_FILE_CXX("alloca.h" HAVE_ALLOCA_H)
  CHECK_INCLUDE_FILE_CXX("arpa/inet.h" HAVE_ARPA_INET_H)
  CHECK_INCLUDE_FILE_CXX("ctype.h" HAVE_CTYPE_H)
  CHECK_INCLUDE_FILE_CXX("memory.h" HAVE_MEMORY_H)
  CHECK_INCLUDE_FILE_CXX("pthread.h" HAVE_PTHREAD_H)
  CHECK_INCLUDE_FILE_CXX("libc.h" HAVE_LIBC_H)
  CHECK_INCLUDE_FILE_CXX("stdlib.h" HAVE_STDLIB_H)
  CHECK_INCLUDE_FILE_CXX("stdarg.h" HAVE_STDARG_H)
  CHECK_INCLUDE_FILE_CXX("cstdarg" HAVE_CSTDARG)
  CHECK_INCLUDE_FILE_CXX("signal.h" HAVE_SIGNAL_H)

  # This mimics the autoconf test. There are systems out there
  # (e.g. FreeBSD and NeXT) where tcp.h can't be compiled on its own.
  SET(TCP_H_DEPS "")
  IF(HAVE_SYS_TYPES_H)
    # This one is needed to make FreeBSD happy
    SET(TCP_H_DEPS "sys/types.h")
  ENDIF(HAVE_SYS_TYPES_H)
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/in_systm.h" HAVE_NETINET_IN_SYSTM_H)
  IF(HAVE_NETINET_IN_SYSTM_H)
    SET(TCP_H_DEPS "${TCP_H_DEPS};netinet/in_systm.h")
  ENDIF(HAVE_NETINET_IN_SYSTM_H)
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/in.h" HAVE_NETINET_IN_H)
  IF(HAVE_NETINET_IN_H)
    SET(TCP_H_DEPS "${TCP_H_DEPS};netinet/in.h")
  ENDIF(HAVE_NETINET_IN_H)
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/tcp.h" HAVE_NETINET_TCP_H)

  # There is no CMake macro to take care of these yet

  IF(WIN32 AND NOT CYGWIN AND NOT MINGW)
    SET(HAVE_NO_TYPEDEF_SSIZE_T TRUE)
    SET(HAVE_NO_TYPEDEF_PID_T TRUE)
  ELSE(WIN32 AND NOT CYGWIN AND NOT MINGW)
    SET(HAVE_NO_TYPEDEF_PID_T FALSE)
    SET(HAVE_NO_TYPEDEF_SIZE_T FALSE)
    IF(NOT ${HAVE_SYS_TYPES_H})
      SET(HAVE_NO_TYPEDEF_SSIZE_T TRUE)
    ENDIF(NOT ${HAVE_SYS_TYPES_H})
  ENDIF(WIN32 AND NOT CYGWIN AND NOT MINGW)

  CHECK_FUNCTION_EXISTS(connect HAVE_CONNECT)
  CHECK_FUNCTION_EXISTS(accept HAVE_ACCEPT)
  CHECK_FUNCTION_EXISTS(access HAVE_ACCESS)
  CHECK_FUNCTION_EXISTS(bcmp HAVE_BCMP)
  CHECK_FUNCTION_EXISTS(bcopy HAVE_BCOPY)
  CHECK_FUNCTION_EXISTS(bind HAVE_BIND)
  CHECK_FUNCTION_EXISTS(bzero HAVE_BZERO)
  CHECK_FUNCTION_EXISTS(cuserid HAVE_CUSERID)
  CHECK_FUNCTION_EXISTS(vprintf HAVE_VPRINTF)
  CHECK_FUNCTION_EXISTS(finite HAVE_FINITE)
  CHECK_FUNCTION_EXISTS(_doprnt HAVE_DOPRNT)
  CHECK_FUNCTION_EXISTS(flock HAVE_FLOCK)
  CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
  CHECK_FUNCTION_EXISTS(getenv HAVE_GETENV)
  CHECK_FUNCTION_EXISTS(geteuid HAVE_GETEUID)
  CHECK_FUNCTION_EXISTS(gethostbyname HAVE_GETHOSTBYNAME)
  CHECK_FUNCTION_EXISTS(gethostname HAVE_GETHOSTNAME)
  CHECK_FUNCTION_EXISTS(gethostid HAVE_GETHOSTID)
  CHECK_FUNCTION_EXISTS(getlogin HAVE_GETLOGIN)
  CHECK_FUNCTION_EXISTS(getpid HAVE_GETPID)
  CHECK_FUNCTION_EXISTS(getsockname HAVE_GETSOCKNAME)
  CHECK_FUNCTION_EXISTS(getsockopt HAVE_GETSOCKOPT)
  CHECK_FUNCTION_EXISTS(getuid HAVE_GETUID)
  CHECK_FUNCTION_EXISTS(index HAVE_INDEX)
  CHECK_FUNCTION_EXISTS(isinf HAVE_ISINF)
  CHECK_FUNCTION_EXISTS(isnan HAVE_ISNAN)
  CHECK_FUNCTION_EXISTS(itoa HAVE_ITOA)
  CHECK_FUNCTION_EXISTS(listen HAVE_LISTEN)
  CHECK_FUNCTION_EXISTS(lockf HAVE_LOCKF)
  CHECK_FUNCTION_EXISTS(malloc_debug HAVE_MALLOC_DEBUG)
  CHECK_FUNCTION_EXISTS(memcmp HAVE_MEMCMP)
  CHECK_FUNCTION_EXISTS(memcpy HAVE_MEMCPY)
  CHECK_FUNCTION_EXISTS(memmove HAVE_MEMMOVE)
  CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
  CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
  CHECK_FUNCTION_EXISTS(mktemp HAVE_MKTEMP)
  CHECK_FUNCTION_EXISTS(rindex HAVE_RINDEX)
  CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
  CHECK_FUNCTION_EXISTS(setsockopt HAVE_SETSOCKOPT)
  CHECK_FUNCTION_EXISTS(setuid HAVE_SETUID)
  CHECK_FUNCTION_EXISTS(sleep HAVE_SLEEP)
  CHECK_FUNCTION_EXISTS(socket HAVE_SOCKET)
  CHECK_FUNCTION_EXISTS(stat HAVE_STAT)
  CHECK_FUNCTION_EXISTS(strchr HAVE_STRCHR)
  CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
  CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)
  CHECK_FUNCTION_EXISTS(strlcat HAVE_STRLCAT)
  CHECK_FUNCTION_EXISTS(strlcpy HAVE_STRLCPY)
  CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
  CHECK_FUNCTION_EXISTS(strtoul HAVE_STRTOUL)
  CHECK_FUNCTION_EXISTS(sysinfo HAVE_SYSINFO)
  CHECK_FUNCTION_EXISTS(tempnam HAVE_TEMPNAM)
  CHECK_FUNCTION_EXISTS(tmpnam HAVE_TMPNAM)
  CHECK_FUNCTION_EXISTS(uname HAVE_UNAME)
  CHECK_FUNCTION_EXISTS(usleep HAVE_USLEEP)
  CHECK_FUNCTION_EXISTS(waitpid HAVE_WAITPID)
  CHECK_FUNCTION_EXISTS(_findfirst HAVE__FINDFIRST)

  SET(HEADERS)

  IF(HAVE_IOSTREAM_H)
    SET(HEADERS ${HEADERS} iostream.h)
  ENDIF(HAVE_IOSTREAM_H)

  IF(HAVE_MATH_H)
    SET(HEADERS ${HEADERS} math.h)
  ENDIF(HAVE_MATH_H)

  IF(HAVE_LIBC_H)
    SET(HEADERS ${HEADERS} libc.h)
  ENDIF(HAVE_LIBC_H)

  IF(HAVE_PTHREAD_H)
    SET(HEADERS ${HEADERS} pthread.h)
  ENDIF(HAVE_PTHREAD_H)

  IF(HAVE_UNISTD_H)
    SET(HEADERS ${HEADERS} unistd.h)
  ENDIF(HAVE_UNISTD_H)

  IF(HAVE_STDLIB_H)
    SET(HEADERS ${HEADERS} stdlib.h)
  ENDIF(HAVE_STDLIB_H)

  IF(HAVE_NETDB_H)
    SET(HEADERS ${HEADERS} netdb.h)
  ENDIF(HAVE_NETDB_H)

  IF(HAVE_SYS_FILE_H)
    SET(HEADERS ${HEADERS} sys/file.h)
  ENDIF(HAVE_SYS_FILE_H)

  IF(HAVE_STRING_H)
    SET(HEADERS ${HEADERS} string.h)
  ENDIF(HAVE_STRING_H)

  IF(HAVE_STRINGS_H)
    SET(HEADERS ${HEADERS} strings.h)
  ENDIF(HAVE_STRINGS_H)

  IF(HAVE_SYS_WAIT_H)
    SET(HEADERS ${HEADERS} sys/wait.h)
  ENDIF(HAVE_SYS_WAIT_H)

  IF(HAVE_SYS_TIME_H)
    SET(HEADERS ${HEADERS} sys/time.h)
  ENDIF(HAVE_SYS_TIME_H)

  IF(HAVE_SYS_RESOURCE_H)
    SET(HEADERS ${HEADERS} sys/resource.h)
  ENDIF(HAVE_SYS_RESOURCE_H)

  IF(HAVE_SYS_TYPES_H)
    SET(HEADERS ${HEADERS} sys/types.h)
  ENDIF(HAVE_SYS_TYPES_H)

  IF(HAVE_SYS_SOCKET_H)
    SET(HEADERS ${HEADERS} sys/socket.h)
  ENDIF(HAVE_SYS_SOCKET_H)

  IF(HAVE_STDARG_H)
    SET(HEADERS ${HEADERS} stdarg.h)
  ENDIF(HAVE_STDARG_H)

  IF(HAVE_STDIO_H)
    SET(HEADERS ${HEADERS} stdio.h)
  ENDIF(HAVE_STDIO_H)

  IF(HAVE_SYS_SELECT_H)
    SET(HEADERS ${HEADERS} sys/select.h)
  ENDIF(HAVE_SYS_SELECT_H)

  IF(HAVE_WINDOWS_H)
    SET(HEADERS ${HEADERS} windows.h)
  ENDIF(HAVE_WINDOWS_H)

  IF(HAVE_WINSOCK_H)
    SET(HEADERS ${HEADERS} winsock.h)
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ws2_32 netapi32 wsock32)
  ENDIF(HAVE_WINSOCK_H)

  # std::vsnprintf and std::vsnprintf need the C++ version of the headers.
  # We just assume they exist when the C version was found
  SET(CXXHEADERS)

  IF(HAVE_CSTDIO)
    SET(CXXHEADERS ${CXXHEADERS} cstdio)
  ENDIF(HAVE_CSTDIO)

  IF(HAVE_CSTDARG)
    SET(CXXHEADERS ${CXXHEADERS} cstdarg)
  ENDIF(HAVE_CSTDARG)

  CHECK_FUNCTIONWITHHEADER_EXISTS(isinf "${HEADERS}" HAVE_PROTOTYPE_ISINF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(isnan "${HEADERS}" HAVE_PROTOTYPE_ISNAN)
  CHECK_FUNCTIONWITHHEADER_EXISTS(finite "${HEADERS}" HAVE_PROTOTYPE_FINITE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(flock "${HEADERS}" HAVE_PROTOTYPE_FLOCK)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostbyname "${HEADERS}" HAVE_PROTOTYPE_GETHOSTBYNAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostid "${HEADERS}" HAVE_PROTOTYPE_GETHOSTID)
  CHECK_FUNCTIONWITHHEADER_EXISTS(bzero "${HEADERS}" HAVE_PROTOTYPE_BZERO)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostname "${HEADERS}" HAVE_PROTOTYPE_GETHOSTNAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS(waitpid "${HEADERS}" HAVE_PROTOTYPE_WAITPID)
  CHECK_FUNCTIONWITHHEADER_EXISTS(wait3 "${HEADERS}" HAVE_PROTOTYPE_WAIT3)
  CHECK_FUNCTIONWITHHEADER_EXISTS(usleep "${HEADERS}" HAVE_PROTOTYPE_USLEEP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(accept "${HEADERS}" HAVE_PROTOTYPE_ACCEPT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(bind "${HEADERS}" HAVE_PROTOTYPE_BIND)
  CHECK_FUNCTIONWITHHEADER_EXISTS(getsockname "${HEADERS}" HAVE_PROTOTYPE_GETSOCKNAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS(getsockopt "${HEADERS}" HAVE_PROTOTYPE_GETSOCKOPT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(getsockname "${HEADERS}" HAVE_PROTOTYPE_GETSOCKNAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS(setsockopt "${HEADERS}" HAVE_PROTOTYPE_SETSOCKOPT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(socket "${HEADERS}" HAVE_PROTOTYPE_SOCKET)
  CHECK_FUNCTIONWITHHEADER_EXISTS(listen "${HEADERS}" HAVE_PROTOTYPE_LISTEN)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::vfprintf "${CXXHEADERS}" HAVE_PROTOTYPE_STD__VFPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::vsnprintf "${CXXHEADERS}" HAVE_PROTOTYPE_STD__VSNPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(_stricmp "${HEADERS}" HAVE_PROTOTYPE__STRICMP)
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("socklen_t definition" "${HEADERS}" HAVE_DECLARATION_SOCKLEN_T)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gettimeofday "${HEADERS}" HAVE_PROTOTYPE_GETTIMEOFDAY)
  CHECK_FUNCTIONWITHHEADER_EXISTS(connect "${HEADERS}" HAVE_PROTOTYPE_CONNECT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(mkstemp "${HEADERS}" HAVE_PROTOTYPE_MKSTEMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(mktemp "${HEADERS}" HAVE_PROTOTYPE_MKTEMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(select "${HEADERS}" HAVE_PROTOTYPE_SELECT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strcasecmp "${HEADERS}" HAVE_PROTOTYPE_STRCASECMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strncasecmp "${HEADERS}" HAVE_PROTOTYPE_STRNCASECMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strerror_r "${HEADERS}" HAVE_PROTOTYPE_STRERROR_R)
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("std::ios_base::openmode definition" "${HEADERS}" HAVE_DECLARATION_STD__IOS_BASE__OPENMODE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(pthread_rwlock_init "${HEADERS}" HAVE_PTHREAD_RWLOCK)

  # TODO: Uncommented because it seems not to make sense. Remove if there are no complaints.
  #CHECK_LIBRARY_EXISTS(iostream "" "" HAVE_LIBIOSTREAM)
  #CHECK_LIBRARY_EXISTS(nsl "" "" HAVE_LIBNSL)
  #CHECK_LIBRARY_EXISTS(socket "" "" HAVE_LIBSOCKET)

  # Check for some type definitions needed by JasPer and defines them if necessary
  # Even if not functions but types are looked for, the script works fine.
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("uchar definition" "${HEADERS}" HAVE_UCHAR_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ushort definition" "${HEADERS}" HAVE_USHORT_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("uint definition" "${HEADERS}" HAVE_UINT_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ulong definition" "${HEADERS}" HAVE_ULONG_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("longlong definition" "${HEADERS}" HAVE_LONGLONG)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ulonglong definition" "${HEADERS}" HAVE_ULONGLONG)

# Tests that require a try-compile

# Check for HAVE_CXX_BOOL
IF("HAVE_CXX_BOOL" MATCHES "^HAVE_CXX_BOOL$")
  MESSAGE(STATUS "Checking support for C++ type bool")
  TRY_COMPILE(HAVE_CXX_BOOL
              ${CMAKE_BINARY_DIR}/CMakeTmp/Bool
              ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestBoolType.cc
              OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_CXX_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- yes")
    SET(HAVE_CXX_BOOL 1 CACHE INTERNAL "Support for C++ type bool")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if the C++ compiler supports type bool "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_CXX_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- no")
    SET(HAVE_CXX_BOOL 0 CACHE INTERNAL "Support for C++ type bool")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if the C++ compiler supports type bool "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_CXX_BOOL)
ENDIF("HAVE_CXX_BOOL" MATCHES "^HAVE_CXX_BOOL$")

IF("C_CHAR_UNSIGNED" MATCHES "^C_CHAR_UNSIGNED$")
  MESSAGE(STATUS "Checking signedness of char")
  TRY_RUN(C_CHAR_SIGNED C_CHAR_SIGNED_COMPILED ${CMAKE_BINARY_DIR}/CMakeTmp/Char
          ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestCharSignedness.cc)
  IF(C_CHAR_SIGNED_COMPILED)
    IF(C_CHAR_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- signed")
      SET(C_CHAR_UNSIGNED 0 CACHE INTERNAL "Whether char is unsigned.")
    ELSE(C_CHAR_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- unsigned")
      SET(C_CHAR_UNSIGNED 1 CACHE INTERNAL "Whether char is unsigned.")
    ENDIF(C_CHAR_SIGNED)
  ELSE(C_CHAR_SIGNED_COMPILED)
    MESSAGE(STATUS "Checking signedness of char -- failed")
  ENDIF(C_CHAR_SIGNED_COMPILED)
ENDIF("C_CHAR_UNSIGNED" MATCHES "^C_CHAR_UNSIGNED$")

# Check for thread type
IF("HAVE_POINTER_TYPE_PTHREAD_T" MATCHES "^HAVE_POINTER_TYPE_PTHREAD_T$")
  MESSAGE(STATUS "Checking whether pthread_t is a pointer type")
  IF (HAVE_WINDOWS_H)
    SET(HAVE_INT_TYPE_PTHREAD_T 1)
  ELSE (HAVE_WINDOWS_H)
    TRY_COMPILE(HAVE_INT_TYPE_PTHREAD_T
      ${CMAKE_BINARY_DIR}/CMakeTmp/PThreadType
      ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestPThreadType.cc
      OUTPUT_VARIABLE OUTPUT)
  ENDIF(HAVE_WINDOWS_H)
  IF(NOT HAVE_INT_TYPE_PTHREAD_T)
    MESSAGE(STATUS "Checking whether pthread_t is a pointer type -- yes")
    SET(HAVE_POINTER_TYPE_PTHREAD_T 1 CACHE INTERNAL "Set if pthread_t is a pointer type")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if the C++ compiler supports type bool "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(NOT HAVE_INT_TYPE_PTHREAD_T)
    MESSAGE(STATUS "Checking whether pthread_t is a pointer type -- no")
    SET(HAVE_POINTER_TYPE_PTHREAD_T 0 CACHE INTERNAL "Set if pthread_t is a pointer type")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if pthread_t is a pointer type "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(NOT HAVE_INT_TYPE_PTHREAD_T)
ENDIF("HAVE_POINTER_TYPE_PTHREAD_T" MATCHES "^HAVE_POINTER_TYPE_PTHREAD_T$")

# Check if typename works properly. Only MSC6 really fails here.
IF("HAVE_TYPENAME" MATCHES "^HAVE_TYPENAME$")
  MESSAGE(STATUS "Checking whether typename works correctly")
  TRY_COMPILE(HAVE_TYPENAME
    ${CMAKE_BINARY_DIR}/CMakeTmp/Typename
    ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestTypename.cc
    OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_TYPENAME)
    MESSAGE(STATUS "Checking whether typename works correctly -- yes")
    SET(HAVE_TYPENAME 1 CACHE INTERNAL "Set if typename works correctly")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if the C++ compiler supports typename "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_TYPENAME)
    MESSAGE(STATUS "Checking whether typename works correctly -- no")
    SET(HAVE_TYPENAME 0 CACHE INTERNAL "Set if typename works correctly")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if the C++ compiler supports typename "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_TYPENAME)
ENDIF("HAVE_TYPENAME" MATCHES "^HAVE_TYPENAME$")

# Check if ENAMETOOLONG is defined.
IF("HAVE_ENAMETOOLONG" MATCHES "^HAVE_ENAMETOOLONG$")
  MESSAGE(STATUS "Checking whether ENAMETOOLONG is defined")
  TRY_COMPILE(HAVE_ENAMETOOLONG
    ${CMAKE_BINARY_DIR}/CMakeTmp/NameTooLong
    ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestNameTooLong.cc
    OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_ENAMETOOLONG)
    MESSAGE(STATUS "Checking whether ENAMETOOLONG is defined -- yes")
    SET(HAVE_ENAMETOOLONG 1 CACHE INTERNAL "Set if ENAMETOOLONG is defined")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if ENAMETOOLONG is defined "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_ENAMETOOLONG)
    MESSAGE(STATUS "Checking whether ENAMETOOLONG is defined -- no")
    SET(HAVE_ENAMETOOLONG 0 CACHE INTERNAL "Set if ENAMETOOLONG is defined")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if ENAMETOOLONG is defined "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_ENAMETOOLONG)
ENDIF("HAVE_ENAMETOOLONG" MATCHES "^HAVE_ENAMETOOLONG$")

# Check if strerror_r returns a char* is defined.
IF("HAVE_CHARP_STRERROR_R" MATCHES "^HAVE_CHARP_STRERROR_R$")
  MESSAGE(STATUS "Checking whether strerror_r returns an int")
  TRY_COMPILE(HAVE_CHARP_STRERROR_R
    ${CMAKE_BINARY_DIR}/CMakeTmp/CharPStrerror
    ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestCharPStrerror.cc
    OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_CHARP_STRERROR_R)
    MESSAGE(STATUS "Checking whether strerror_r returns an int -- yes")
    SET(HAVE_CHARP_STRERROR_R 0 CACHE INTERNAL "Set if strerror_r returns a char*")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if strerror_r returns an int "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_CHARP_STRERROR_R)
    MESSAGE(STATUS "Checking whether strerror_r returns an int -- no")
    SET(HAVE_CHARP_STRERROR_R 1 CACHE INTERNAL "Set if strerror_r returns a char*")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if strerror_r returns an int "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_CHARP_STRERROR_R)
ENDIF("HAVE_CHARP_STRERROR_R" MATCHES "^HAVE_CHARP_STRERROR_R$")

# Check if variable length arrays are supported.
IF("HAVE_VLA" MATCHES "^HAVE_VLA$")
  MESSAGE(STATUS "Checking whether variable length arrays are supported")
  TRY_COMPILE(HAVE_VLA
    ${CMAKE_BINARY_DIR}/CMakeTmp/VariableLengthArray
    ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestVariableLengthArrays.cc
    OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_VLA)
    MESSAGE(STATUS "Checking whether variable length arrays are supported -- yes")
    SET(HAVE_VLA 1 CACHE INTERNAL "Set if variable length arrays are supported")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if variable length arrays are supported "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_VLA)
    MESSAGE(STATUS "Checking whether variable length arrays are supported -- no")
    SET(HAVE_VLA 0 CACHE INTERNAL "Set if variable length arrays are supported")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if variable length arrays are supported "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_VLA)
ENDIF("HAVE_VLA" MATCHES "^HAVE_VLA$")

# Check if std::ios::nocreate exists
IF("HAVE_IOS_NOCREATE" MATCHES "^HAVE_IOS_NOCREATE$")
  MESSAGE(STATUS "Checking whether std::ios::nocreate exists")
  TRY_COMPILE(HAVE_IOS_NOCREATE
    ${CMAKE_BINARY_DIR}/CMakeTmp/IosNocreate
    ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestStdIosNocreate.cc
    OUTPUT_VARIABLE OUTPUT)
  IF(HAVE_IOS_NOCREATE)
    MESSAGE(STATUS "Checking whether std::ios::nocreate exists -- yes")
    SET(HAVE_IOS_NOCREATE 1 CACHE INTERNAL "Set if std::ios::nocreate is supported")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
      "Determining if std::ios::nocreate is supported "
      "passed with the following output:\n"
      "${OUTPUT}\n")
  ELSE(HAVE_IOS_NOCREATE)
    MESSAGE(STATUS "Checking whether std::ios::nocreate exists -- no")
    SET(HAVE_IOS_NOCREATE 0 CACHE INTERNAL "Set if std::ios::nocreate is supported")
    FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if std::ios::nocreate is supported "
      "failed with the following output:\n"
      "${OUTPUT}\n")
  ENDIF(HAVE_IOS_NOCREATE)
ENDIF("HAVE_IOS_NOCREATE" MATCHES "^HAVE_IOS_NOCREATE$")

IF(WIN32)
  # If someone can tell me how to convince TRY_COMPILE to link against winsock,
  # we could use tests for these. Until then, here is what would be the result:
  SET(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  SET(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  SET(HAVE_INTP_SELECT 0 CACHE INTERNAL "Set if select() accepts an int* argument")
ELSE(WIN32)
  # Check if socket functions accept an int*
  IF("HAVE_INTP_SOCKET" MATCHES "^HAVE_INTP_SOCKET$")
    MESSAGE(STATUS "Checking whether socket functions accept an int* argument")
    TRY_COMPILE(HAVE_INTP_SOCKET
      ${CMAKE_BINARY_DIR}/CMakeTmp/testSocketIntP
      ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestSocketIntP.cc
      OUTPUT_VARIABLE OUTPUT)
    IF(HAVE_INTP_SOCKET)
      MESSAGE(STATUS "Checking whether socket functions accept an int* argument -- yes")
      SET(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
      SET(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Determining if socket functions accept an int* argument "
        "passed with the following output:\n"
        "${OUTPUT}\n")
    ELSE(HAVE_INTP_SOCKET)
      MESSAGE(STATUS "Checking whether socket functions accept an int* argument -- no")
      SET(HAVE_INTP_ACCEPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
      SET(HAVE_INTP_GETSOCKOPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Determining if socket functions accept an int* argument "
        "failed with the following output:\n"
        "${OUTPUT}\n")
    ENDIF(HAVE_INTP_SOCKET)
  ENDIF("HAVE_INTP_SOCKET" MATCHES "^HAVE_INTP_SOCKET$")

  # Check if select() accepts an int*
  IF("HAVE_INTP_SELECT" MATCHES "^HAVE_INTP_SELECT$")
    MESSAGE(STATUS "Checking whether select() accepts an int* argument")
    TRY_COMPILE(HAVE_INTP_SELECT
      ${CMAKE_BINARY_DIR}/CMakeTmp/testSelectIntP
      ${DCMTK_SOURCE_DIR}/CMake/dcmtkTestSelectIntP.cc
      OUTPUT_VARIABLE OUTPUT)
    IF(HAVE_INTP_SELECT)
      MESSAGE(STATUS "Checking whether select() accepts an int* argument -- yes")
      SET(HAVE_INTP_SELECT 1 CACHE INTERNAL "Set if select() accepts an int* argument")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Determining if select() accepts an int* argument "
        "passed with the following output:\n"
        "${OUTPUT}\n")
    ELSE(HAVE_INTP_SELECT)
      MESSAGE(STATUS "Checking whether select() accepts an int* argument -- no")
      SET(HAVE_INTP_SELECT 0 CACHE INTERNAL "Set if select() accepts an int* argument")
      FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Determining if select() accepts an int* argument "
        "failed with the following output:\n"
        "${OUTPUT}\n")
    ENDIF(HAVE_INTP_SELECT)
  ENDIF("HAVE_INTP_SELECT" MATCHES "^HAVE_INTP_SELECT$")
ENDIF(WIN32)
