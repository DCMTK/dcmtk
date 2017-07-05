# Set build configuration to use for configuration tests
IF(CMAKE_BUILD_TYPE)
  SET(CMAKE_TRY_COMPILE_CONFIGURATION "${CMAKE_BUILD_TYPE}")
ELSE(CMAKE_BUILD_TYPE)
  SET(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
ENDIF(CMAKE_BUILD_TYPE)

# Compiled-in dictionary support
IF(DCMTK_ENABLE_BUILTIN_DICTIONARY)
  SET(ENABLE_BUILTIN_DICTIONARY 1)
  MESSAGE(STATUS "Info: DCMTK will compile with built-in (compiled-in) dictionary")
  # No extra variable needed since its only evaluated in CMake files
ELSE(DCMTK_ENABLE_BUILTIN_DICTIONARY)
  SET(ENABLE_BUILTIN_DICTIONARY "")
  MESSAGE(STATUS "Info: DCMTK will compile without built-in (compiled-in) dictionary")
  # No extra variable needed since its only evaluated in CMake files
ENDIF(DCMTK_ENABLE_BUILTIN_DICTIONARY)

# External dictionary support
IF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
  SET(ENABLE_EXTERNAL_DICTIONARY 1)
  MESSAGE(STATUS "Info: DCMTK will try to load external dictionary from default path on startup")
ELSE(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
  SET(ENABLE_EXTERNAL_DICTIONARY "")
  MESSAGE(STATUS "Info: DCMTK will not try to load external dictionary from default path on startup")
ENDIF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)


# Private tags
IF(DCMTK_ENABLE_PRIVATE_TAGS)
  SET(ENABLE_PRIVATE_TAGS 1)
  MESSAGE(STATUS "Info: DCMTK's builtin private dictionary support will be enabled")
ELSE(DCMTK_ENABLE_PRIVATE_TAGS)
  SET(ENABLE_PRIVATE_TAGS "")
  MESSAGE(STATUS "Info: DCMTK's builtin private dictionary support will be disabled")
ENDIF(DCMTK_ENABLE_PRIVATE_TAGS)

# Thread support
IF(DCMTK_WITH_THREADS)
  SET(WITH_THREADS 1)
  MESSAGE(STATUS "Info: Thread support will be enabled")
ELSE(DCMTK_WITH_THREADS)
  SET(WITH_THREADS "")
  MESSAGE(STATUS "Info: Thread support will be disabled")
ENDIF(DCMTK_WITH_THREADS)

# Wide char file I/O support
IF(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  SET(WIDE_CHAR_FILE_IO_FUNCTIONS 1)
  MESSAGE(STATUS "Info: Wide char file I/O functions will be enabled")
ELSE(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  SET(WIDE_CHAR_FILE_IO_FUNCTIONS "")
  MESSAGE(STATUS "Info: Wide char file I/O functions will be disabled")
ENDIF(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)

# Wide char main function
IF(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
  SET(WIDE_CHAR_MAIN_FUNCTION 1)
  MESSAGE(STATUS "Info: Wide char main function for command line tools will be enabled")
ELSE(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
  SET(WIDE_CHAR_MAIN_FUNCTION "")
  MESSAGE(STATUS "Info: Wide char main function for command line tools will be disabled")
ENDIF(DCMTK_WIDE_CHAR_MAIN_FUNCTION)

IF(NOT DCMTK_ENABLE_CHARSET_CONVERSION)
  SET(DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING "Select character set conversion implementation.")
  IF(DCMTK_WITH_ICONV)
    IF(DCMTK_WITH_ICU)
      MESSAGE(WARNING "Warning: Both ICU and the libiconv are available, using libiconv. Modify DCMTK_ENABLE_CHARSET_CONVERSION for switching to ICU")
    ENDIF()
    SET(DCMTK_ENABLE_CHARSET_CONVERSION "libiconv" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  ELSEIF(DCMTK_WITH_ICU)
    SET(DCMTK_ENABLE_CHARSET_CONVERSION "ICU" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  ELSEIF(DCMTK_WITH_STDLIBC_ICONV)
    SET(DCMTK_ENABLE_CHARSET_CONVERSION "stdlibc (iconv)" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  ELSE()
    SET(DCMTK_ENABLE_CHARSET_CONVERSION "<disabled>" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  ENDIF()
ENDIF()

SET(DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES)
IF(DCMTK_WITH_ICONV)
  LIST(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "libiconv")
ENDIF()
IF(DCMTK_WITH_ICU)
  LIST(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "ICU")
ENDIF()
IF(DCMTK_WITH_STDLIBC_ICONV)
  LIST(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "stdlibc (iconv)")
ENDIF()
SET_PROPERTY(CACHE DCMTK_ENABLE_CHARSET_CONVERSION PROPERTY STRINGS ${DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES} "<disabled>")

IF(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "libiconv" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_ICONV")
  MESSAGE(STATUS "Info: Building DCMTK with character set conversion support using libiconv")
  SET(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_ICONV")
  SET(CHARSET_CONVERSION_LIBS ${LIBICONV_LIBS})
ELSEIF(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "ICU" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_ICU")
  MESSAGE(STATUS "Info: Building DCMTK with character set conversion support using the ICU")
  SET(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_ICU")
  SET(CHARSET_CONVERSION_LIBS ${ICU_LIBS})
ELSEIF(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "stdlibc (iconv)" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV")
  MESSAGE(STATUS "Info: Building DCMTK with character set conversion support using builtin iconv functions from the C standard library")
  SET(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV")
ELSE()
  MESSAGE(STATUS "Info: Building DCMTK without character set conversion support")
  SET(DCMTK_ENABLE_CHARSET_CONVERSION OFF)
ENDIF()

# Standard C++ headers (currently hard-coded)
#IF(VTK_USE_ANSI_STDLIB)
  SET(USE_STD_CXX_INCLUDES 1)
#ELSE(VTK_USE_ANSI_STDLIB)
#  SET(USE_STD_CXX_INCLUDES "")
#ENDIF(VTK_USE_ANSI_STDLIB)

# Configure file

# Windows being windows, it lies about its processor type to 32 bit binaries
SET(SYSTEM_PROCESSOR "$ENV{PROCESSOR_ARCHITEW6432}")
IF(NOT SYSTEM_PROCESSOR)
  SET(SYSTEM_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")
ENDIF(NOT SYSTEM_PROCESSOR)
# CMake doesn't provide a configure-style system type string
SET(CANONICAL_HOST_TYPE "${SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")
DCMTK_UNSET(SYSTEM_PROCESSOR)

# Configure dictionary path and install prefix
IF(WIN32 AND NOT CYGWIN)
  # Set DCMTK_PREFIX needed within some code. Be sure that all / are replaced by \\.
  SET(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  STRING(REGEX REPLACE "/" "\\\\\\\\" DCMTK_PREFIX "${DCMTK_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  SET(PATH_SEPARATOR "\\\\")
  SET(ENVIRONMENT_PATH_SEPARATOR ";")
  # Set dictionary path to the data dir inside install main dir (prefix)
  IF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    SET(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}\\\\${DCMTK_INSTALL_DATDIR}\\\\dicom.dic")
    # If private dictionary should be utilized, add it to default dictionary path.
    IF(ENABLE_PRIVATE_TAGS)
      SET(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH};${DCMTK_PREFIX}\\\\${DCMTK_INSTALL_DATDIR}\\\\private.dic")
    ENDIF(ENABLE_PRIVATE_TAGS)
     # Again, for Windows strip all / from path and replace it with \\.
    STRING(REGEX REPLACE "/" "\\\\\\\\" DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}")
  ELSE(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    SET(DCM_DICT_DEFAULT_PATH "")
  ENDIF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
  # Set default directory for configuration and support data.
  SET(DCMTK_DEFAULT_CONFIGURATION_DIR "")
  SET(DCMTK_DEFAULT_SUPPORT_DATA_DIR "")
ELSE(WIN32 AND NOT CYGWIN)
  # Set DCMTK_PREFIX needed within some code.
  SET(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  SET(PATH_SEPARATOR "/")
  SET(ENVIRONMENT_PATH_SEPARATOR ":")
  # Set dictionary path to the data dir inside install main dir (prefix).
  IF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    SET(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}/${DCMTK_INSTALL_DATDIR}/dicom.dic")
    # If private dictionary should be utilized, add it to default dictionary path.
    IF(ENABLE_PRIVATE_TAGS)
      SET(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}:${DCMTK_PREFIX}/${DCMTK_INSTALL_DATDIR}/private.dic")
    ENDIF(ENABLE_PRIVATE_TAGS)
  ELSE(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    SET(DCM_DICT_DEFAULT_PATH "")
  ENDIF(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
  # Set default directory for configuration and support data.
  SET(DCMTK_DEFAULT_CONFIGURATION_DIR "${DCMTK_PREFIX}/${DCMTK_INSTALL_ETCDIR}/")
  SET(DCMTK_DEFAULT_SUPPORT_DATA_DIR "${DCMTK_PREFIX}/${DCMTK_INSTALL_DATDIR}/")
ENDIF(WIN32 AND NOT CYGWIN)

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
INCLUDE("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkTryCompile.cmake")
INCLUDE("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkTryRun.cmake")
INCLUDE("${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake")
INCLUDE("${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake")
INCLUDE("${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake")
INCLUDE("${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake")
INCLUDE("${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake")
INCLUDE("${DCMTK_CMAKE_INCLUDE}CMake/CheckFunctionWithHeaderExists.cmake")
INCLUDE(CheckCXXSymbolExists OPTIONAL)
IF(NOT COMMAND CHECK_CXX_SYMBOL_EXISTS)
  # fallback implementation for old CMake Versions
  FUNCTION(CHECK_CXX_SYMBOL_EXISTS SYMBOL FILES VAR)
    SET(CODE)
    FOREACH(FILE ${FILES})
      SET(CODE "${CODE}#include <${FILE}>\n")
    ENDFOREACH()
    SET(CODE "${CODE}\nint main(int argc, char** argv)\n{\n  (void)argv;\n#ifndef ${SYMBOL}\n  return ((int*)(&${SYMBOL}))[argc];\n#else\n  (void)argc;\n  return 0;\n#endif\n}\n")
    DCMTK_TRY_COMPILE("${VAR}" "the compiler supports ${SYMBOL}" "${CODE}")
  ENDFUNCTION()
ENDIF(NOT COMMAND CHECK_CXX_SYMBOL_EXISTS)

FOREACH(FUNC "__FUNCTION__" "__PRETTY_FUNCTION__" "__func__")
  CHECK_SYMBOL_EXISTS("${FUNC}" "" "HAVE_${FUNC}_C_MACRO")
  # test if the C++ compiler also supports them (e.g. SunPro doesn't)
  CHECK_CXX_SYMBOL_EXISTS("${FUNC}" "" "HAVE_${FUNC}_CXX_MACRO")
  IF(HAVE_${FUNC}_C_MACRO AND HAVE_${FUNC}_CXX_MACRO)
    SET("HAVE_${FUNC}_MACRO" 1 CACHE INTERNAL "Have symbol ${FUNC}" FORCE)
  ELSE()
    SET("HAVE_${FUNC}_MACRO" CACHE INTERNAL "Have symbol ${FUNC}" FORCE)
  ENDIF()
ENDFOREACH()

# prepare include directories for 3rdparty libraries before performing
# header searches
IF(ZLIB_INCDIR)
  LIST(APPEND CMAKE_REQUIRED_INCLUDES "${ZLIB_INCDIR}")
ENDIF(ZLIB_INCDIR)

IF(ZLIB_INCLUDE_DIRS)
  LIST(APPEND CMAKE_REQUIRED_INCLUDES "${ZLIB_INCLUDE_DIRS}")
ENDIF(ZLIB_INCLUDE_DIRS)

IF(LIBPNG_INCDIR)
  LIST(APPEND CMAKE_REQUIRED_INCLUDES "${LIBPNG_INCDIR}")
ENDIF(LIBPNG_INCDIR)

IF(PNG_INCLUDE_DIR)
  LIST(APPEND CMAKE_REQUIRED_INCLUDES "${PNG_INCLUDE_DIR}")
ENDIF(PNG_INCLUDE_DIR)

IF(OPENSSL_INCDIR)
  LIST(APPEND CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCDIR}")
ENDIF(OPENSSL_INCDIR)

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
  CHECK_INCLUDE_FILE_CXX("grp.h" HAVE_GRP_H)
  CHECK_INCLUDE_FILE_CXX("malloc.h" HAVE_MALLOC_H)
  CHECK_INCLUDE_FILE_CXX("math.h" HAVE_MATH_H)
  CHECK_INCLUDE_FILE_CXX("cmath" HAVE_CMATH)
  CHECK_INCLUDE_FILE_CXX("ieeefp.h" HAVE_IEEEFP_H)
  CHECK_INCLUDE_FILE_CXX("inttypes.h" HAVE_INTTYPES_H)
  CHECK_INCLUDE_FILE_CXX("iomanip" HAVE_IOMANIP)
  CHECK_INCLUDE_FILE_CXX("iomanip.h" HAVE_IOMANIP_H)
  CHECK_INCLUDE_FILE_CXX("iostream" HAVE_IOSTREAM)
  CHECK_INCLUDE_FILE_CXX("iostream.h" HAVE_IOSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("io.h" HAVE_IO_H)
  CHECK_INCLUDE_FILE_CXX("iso646.h" HAVE_ISO646_H)
  CHECK_INCLUDE_FILE_CXX("png.h" HAVE_PNG_H)
  CHECK_INCLUDE_FILE_CXX("limits.h" HAVE_LIMITS_H)
  CHECK_INCLUDE_FILE_CXX("climits" HAVE_CLIMITS)
  CHECK_INCLUDE_FILE_CXX("locale.h" HAVE_LOCALE_H)
  CHECK_INCLUDE_FILE_CXX("ndir.h" HAVE_NDIR_H)
  CHECK_INCLUDE_FILE_CXX("netdb.h" HAVE_NETDB_H)
  CHECK_INCLUDE_FILE_CXX("new.h" HAVE_NEW_H)
  CHECK_INCLUDE_FILE_CXX("pwd.h" HAVE_PWD_H)
  CHECK_INCLUDE_FILE_CXX("semaphore.h" HAVE_SEMAPHORE_H)
  CHECK_INCLUDE_FILE_CXX("setjmp.h" HAVE_SETJMP_H)
  CHECK_INCLUDE_FILE_CXX("sstream" HAVE_SSTREAM)
  CHECK_INCLUDE_FILE_CXX("sstream.h" HAVE_SSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("stat.h" HAVE_STAT_H)
  CHECK_INCLUDE_FILE_CXX("stdbool.h" HAVE_STDBOOL_H)
  CHECK_INCLUDE_FILE_CXX("stddef.h" HAVE_STDDEF_H)
  CHECK_INCLUDE_FILE_CXX("cstddef" HAVE_CSTDDEF)
  CHECK_INCLUDE_FILE_CXX("stdint.h" HAVE_STDINT_H)
  CHECK_INCLUDE_FILE_CXX("cstdint" HAVE_CSTDINT)
  CHECK_INCLUDE_FILE_CXX("stdio.h" HAVE_STDIO_H)
  CHECK_INCLUDE_FILE_CXX("cstdio" HAVE_CSTDIO)
  CHECK_INCLUDE_FILE_CXX("strings.h" HAVE_STRINGS_H)
  CHECK_INCLUDE_FILE_CXX("string.h" HAVE_STRING_H)
  CHECK_INCLUDE_FILE_CXX("strstream.h" HAVE_STRSTREAM_H)
  CHECK_INCLUDE_FILE_CXX("strstream" HAVE_STRSTREAM)
  CHECK_INCLUDE_FILE_CXX("strstrea.h" HAVE_STRSTREA_H)
  CHECK_INCLUDE_FILE_CXX("synch.h" HAVE_SYNCH_H)
  CHECK_INCLUDE_FILE_CXX("syslog.h" HAVE_SYSLOG_H)
  CHECK_INCLUDE_FILE_CXX("sys/errno.h" HAVE_SYS_ERRNO_H)
  CHECK_INCLUDE_FILE_CXX("sys/dir.h" HAVE_SYS_DIR_H)
  CHECK_INCLUDE_FILE_CXX("sys/file.h" HAVE_SYS_FILE_H)
  CHECK_INCLUDE_FILE_CXX("sys/ndir.h" HAVE_SYS_NDIR_H)
  CHECK_INCLUDE_FILE_CXX("sys/param.h" HAVE_SYS_PARAM_H)
  CHECK_INCLUDE_FILE_CXX("sys/resource.h" HAVE_SYS_RESOURCE_H)
  CHECK_INCLUDE_FILE_CXX("sys/select.h" HAVE_SYS_SELECT_H)
  CHECK_INCLUDE_FILE_CXX("sys/syscall.h" HAVE_SYS_SYSCALL_H)
  CHECK_INCLUDE_FILE_CXX("sys/systeminfo.h" HAVE_SYS_SYSTEMINFO_H)
  CHECK_INCLUDE_FILE_CXX("sys/time.h" HAVE_SYS_TIME_H)
  CHECK_INCLUDE_FILE_CXX("sys/timeb.h" HAVE_SYS_TIMEB_H)
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
  CHECK_INCLUDE_FILE_CXX("wchar.h" HAVE_WCHAR_H)
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
  CHECK_INCLUDE_FILE_CXX("fenv.h" HAVE_FENV_H)

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

  IF(NOT HAVE_PNG_H)
    # <png.h> is unavailable, so test if we need to include it as <libpng/png.h>
    CHECK_INCLUDE_FILE_CXX("libpng/png.h" HAVE_LIBPNG_PNG_H)
  ELSE(NOT HAVE_PNG_H)
    # ensure including <png.h> is preferred
    DCMTK_UNSET_CACHE(HAVE_LIBPNG_PNG_H)
  ENDIF(NOT HAVE_PNG_H)

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
  CHECK_FUNCTION_EXISTS(_doprnt HAVE_DOPRNT)
  CHECK_FUNCTION_EXISTS(finite HAVE_FINITE)
  CHECK_FUNCTION_EXISTS(flock HAVE_FLOCK)
  CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
  CHECK_FUNCTION_EXISTS(fseeko HAVE_FSEEKO)
  CHECK_FUNCTION_EXISTS(ftime HAVE_FTIME)
  CHECK_FUNCTION_EXISTS(getaddrinfo HAVE_GETADDRINFO)
  CHECK_FUNCTION_EXISTS(getenv HAVE_GETENV)
  CHECK_FUNCTION_EXISTS(geteuid HAVE_GETEUID)
  CHECK_FUNCTION_EXISTS(getgrnam HAVE_GETGRNAM)
  CHECK_FUNCTION_EXISTS(gethostbyname HAVE_GETHOSTBYNAME)
  CHECK_FUNCTION_EXISTS(gethostbyname_r HAVE_GETHOSTBYNAME_R)
  CHECK_FUNCTION_EXISTS(gethostbyaddr_r HAVE_GETHOSTBYADDR_R)
  CHECK_FUNCTION_EXISTS(gethostname HAVE_GETHOSTNAME)
  CHECK_FUNCTION_EXISTS(gethostid HAVE_GETHOSTID)
  CHECK_FUNCTION_EXISTS(getlogin HAVE_GETLOGIN)
  CHECK_FUNCTION_EXISTS(getlogin_r HAVE_GETLOGIN_R)
  CHECK_FUNCTION_EXISTS(getpid HAVE_GETPID)
  CHECK_FUNCTION_EXISTS(getpwnam HAVE_GETPWNAM)
  CHECK_FUNCTION_EXISTS(getsockname HAVE_GETSOCKNAME)
  CHECK_FUNCTION_EXISTS(getsockopt HAVE_GETSOCKOPT)
  CHECK_FUNCTION_EXISTS(getuid HAVE_GETUID)
  CHECK_FUNCTION_EXISTS(gmtime_r HAVE_GMTIME_R)
  CHECK_FUNCTION_EXISTS(index HAVE_INDEX)
  CHECK_FUNCTION_EXISTS(isinf HAVE_ISINF)
  CHECK_FUNCTION_EXISTS(isnan HAVE_ISNAN)
  CHECK_FUNCTION_EXISTS(itoa HAVE_ITOA)
  CHECK_FUNCTION_EXISTS(listen HAVE_LISTEN)
  CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)
  CHECK_FUNCTION_EXISTS(lockf HAVE_LOCKF)
  CHECK_FUNCTION_EXISTS(lstat HAVE_LSTAT)
  CHECK_FUNCTION_EXISTS(malloc_debug HAVE_MALLOC_DEBUG)
  CHECK_FUNCTION_EXISTS(mbstowcs HAVE_MBSTOWCS)
  CHECK_FUNCTION_EXISTS(wcstombs HAVE_WCSTOMBS)
  CHECK_FUNCTION_EXISTS(memmove HAVE_MEMMOVE)
  CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
  CHECK_FUNCTION_EXISTS(mktemp HAVE_MKTEMP)
  CHECK_FUNCTION_EXISTS(readdir_r HAVE_READDIR_R)
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
  CHECK_FUNCTION_EXISTS(wait3 HAVE_WAIT3)
  CHECK_FUNCTION_EXISTS(waitpid HAVE_WAITPID)
  CHECK_FUNCTION_EXISTS(_findfirst HAVE__FINDFIRST)
  CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)

  SET(HEADERS)

  IF(HAVE_IO_H)
    SET(HEADERS ${HEADERS} io.h)
  ENDIF(HAVE_IO_H)

  IF(HAVE_IOSTREAM_H)
    SET(HEADERS ${HEADERS} iostream.h)
  ENDIF(HAVE_IOSTREAM_H)

  IF(HAVE_MATH_H)
    SET(HEADERS ${HEADERS} math.h)
  ENDIF(HAVE_MATH_H)

  IF(HAVE_CMATH)
    SET(HEADERS ${HEADERS} cmath)
  ENDIF(HAVE_CMATH)

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

  IF(HAVE_STDINT_H)
    SET(HEADERS ${HEADERS} stdint.h)
  ENDIF(HAVE_STDINT_H)

  IF(HAVE_STDDEF_H)
    SET(HEADERS ${HEADERS} stddef.h)
  ENDIF(HAVE_STDDEF_H)

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

  IF(HAVE_GRP_H)
    SET(HEADERS ${HEADERS} grp.h)
  ENDIF(HAVE_GRP_H)

  IF(HAVE_PWD_H)
    SET(HEADERS ${HEADERS} pwd.h)
  ENDIF(HAVE_PWD_H)

  IF(HAVE_SYS_SYSCALL_H)
    SET(HEADERS ${HEADERS} sys/syscall.h)
  ENDIF(HAVE_SYS_SYSCALL_H)

  IF(HAVE_WINSOCK_H)
    SET(HEADERS ${HEADERS} winsock.h)
    SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} iphlpapi ws2_32 netapi32 wsock32)
  ENDIF(HAVE_WINSOCK_H)

  IF(HAVE_FENV_H)
    SET(HEADERS ${HEADERS} fenv.h)
  ENDIF(HAVE_FENV_H)

  # std::vsnprintf and std::vsnprintf need the C++ version of the headers.
  # We just assume they exist when the C version was found
  SET(CXXHEADERS)

  IF(HAVE_CSTDIO)
    SET(CXXHEADERS ${CXXHEADERS} cstdio)
  ENDIF(HAVE_CSTDIO)

  IF(HAVE_CSTDARG)
    SET(CXXHEADERS ${CXXHEADERS} cstdarg)
  ENDIF(HAVE_CSTDARG)

  IF(HAVE_CSTDINT)
    SET(CXXHEADERS ${CXXHEADERS} cstdint)
  ENDIF(HAVE_CSTDINT)

  IF(HAVE_CSTDDEF)
    SET(CXXHEADERS ${CXXHEADERS} cstddef)
  ENDIF(HAVE_CSTDDEF)

  CHECK_FUNCTIONWITHHEADER_EXISTS(feenableexcept "${HEADERS}" HAVE_PROTOTYPE_FEENABLEEXCEPT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(isinf "${HEADERS}" HAVE_PROTOTYPE_ISINF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(isnan "${HEADERS}" HAVE_PROTOTYPE_ISNAN)
  CHECK_FUNCTIONWITHHEADER_EXISTS(finite "${HEADERS}" HAVE_PROTOTYPE_FINITE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::isinf "${HEADERS}" HAVE_PROTOTYPE_STD__ISINF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::isnan "${HEADERS}" HAVE_PROTOTYPE_STD__ISNAN)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::finite "${HEADERS}" HAVE_PROTOTYPE_STD__FINITE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(flock "${HEADERS}" HAVE_PROTOTYPE_FLOCK)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostbyname "${HEADERS}" HAVE_PROTOTYPE_GETHOSTBYNAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostbyname_r "${HEADERS}" HAVE_PROTOTYPE_GETHOSTBYNAME_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gethostbyaddr_r "${HEADERS}" HAVE_PROTOTYPE_GETHOSTBYADDR_R)
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
  CHECK_FUNCTIONWITHHEADER_EXISTS(_vsnprintf_s "${HEADERS}" HAVE__VSNPRINTF_S)
  CHECK_FUNCTIONWITHHEADER_EXISTS(vfprintf_s "${HEADERS}" HAVE_VFPRINTF_S)
  CHECK_FUNCTIONWITHHEADER_EXISTS(vprintf "${HEADERS}" HAVE_VPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(vsnprintf "${HEADERS}" HAVE_VSNPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(vsprintf_s "${HEADERS}" HAVE_VSPRINTF_S)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::vfprintf "${CXXHEADERS}" HAVE_PROTOTYPE_STD__VFPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(std::vsnprintf "${CXXHEADERS}" HAVE_PROTOTYPE_STD__VSNPRINTF)
  CHECK_FUNCTIONWITHHEADER_EXISTS(_stricmp "${HEADERS}" HAVE_PROTOTYPE__STRICMP)
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("socklen_t definition" "${HEADERS}" HAVE_DECLARATION_SOCKLEN_T)
  CHECK_FUNCTIONWITHHEADER_EXISTS(gettimeofday "${HEADERS}" HAVE_PROTOTYPE_GETTIMEOFDAY)
  CHECK_FUNCTIONWITHHEADER_EXISTS(connect "${HEADERS}" HAVE_PROTOTYPE_CONNECT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(mkstemp "${HEADERS}" HAVE_PROTOTYPE_MKSTEMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(mktemp "${HEADERS}" HAVE_PROTOTYPE_MKTEMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(memcmp "${HEADERS}" HAVE_MEMCMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(memcpy "${HEADERS}" HAVE_MEMCPY)
  CHECK_FUNCTIONWITHHEADER_EXISTS(memset "${HEADERS}" HAVE_MEMSET)
  CHECK_FUNCTIONWITHHEADER_EXISTS(select "${HEADERS}" HAVE_PROTOTYPE_SELECT)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strcasecmp "${HEADERS}" HAVE_PROTOTYPE_STRCASECMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strncasecmp "${HEADERS}" HAVE_PROTOTYPE_STRNCASECMP)
  CHECK_FUNCTIONWITHHEADER_EXISTS(strerror_r "${HEADERS}" HAVE_PROTOTYPE_STRERROR_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS(SYS_gettid "${HEADERS}" HAVE_SYS_GETTID)
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("std::ios_base::openmode definition" "${HEADERS}" HAVE_DECLARATION_STD__IOS_BASE__OPENMODE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(pthread_rwlock_init "${HEADERS}" HAVE_PTHREAD_RWLOCK)
  CHECK_FUNCTIONWITHHEADER_EXISTS("__sync_add_and_fetch((int*)0,0)" "${HEADERS}" HAVE_SYNC_ADD_AND_FETCH)
  CHECK_FUNCTIONWITHHEADER_EXISTS("__sync_sub_and_fetch((int*)0,0)" "${HEADERS}" HAVE_SYNC_SUB_AND_FETCH)
  CHECK_FUNCTIONWITHHEADER_EXISTS("InterlockedIncrement((long*)0)" "${HEADERS}" HAVE_INTERLOCKED_INCREMENT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("InterlockedDecrement((long*)0)" "${HEADERS}" HAVE_INTERLOCKED_DECREMENT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("_fpclassf(0.0f)" "${HEADERS}" HAVE_PROTOTYPE__FPCLASSF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("getgrnam_r((char*)0,(group*)0,(char*)0,0,(group**)0)" "${HEADERS}" HAVE_GETGRNAM_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS("getpwnam_r((char*)0,(passwd*)0,(char*)0,0,(passwd**)0)" "${HEADERS}" HAVE_GETPWNAM_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS(nanosleep "${HEADERS}" HAVE_PROTOTYPE_NANOSLEEP)
  CHECK_FUNCTIONWITHHEADER_EXISTS("&passwd::pw_gecos" "${HEADERS}" HAVE_PASSWD_GECOS)

  # TODO: Uncommented because it seems not to make sense. Remove if there are no complaints.
  #CHECK_LIBRARY_EXISTS(iostream "" "" HAVE_LIBIOSTREAM)

  # Check for some type definitions needed by JasPer and defines them if necessary
  # Even if not functions but types are looked for, the script works fine.
  # "definition" is an (exchangeable) identifier that is needed for successful compile test
  CHECK_FUNCTIONWITHHEADER_EXISTS("uchar definition" "${HEADERS}" HAVE_UCHAR_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ushort definition" "${HEADERS}" HAVE_USHORT_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("uint definition" "${HEADERS}" HAVE_UINT_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ulong definition" "${HEADERS}" HAVE_ULONG_TYPEDEF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("longlong definition" "${HEADERS}" HAVE_LONGLONG)
  CHECK_FUNCTIONWITHHEADER_EXISTS("ulonglong definition" "${HEADERS}" HAVE_ULONGLONG)
  CHECK_FUNCTIONWITHHEADER_EXISTS("long long definition" "${HEADERS}" HAVE_LONG_LONG)
  CHECK_FUNCTIONWITHHEADER_EXISTS("unsigned long long definition" "${HEADERS}" HAVE_UNSIGNED_LONG_LONG)
  CHECK_FUNCTIONWITHHEADER_EXISTS("int64_t definition" "${HEADERS}" HAVE_INT64_T)
  CHECK_FUNCTIONWITHHEADER_EXISTS("uint64_t definition" "${HEADERS}" HAVE_UINT64_T)

  # File access stuff
  CHECK_FUNCTIONWITHHEADER_EXISTS("fpos64_t definition" "${HEADERS}" HAVE_FPOS64_T)
  CHECK_FUNCTIONWITHHEADER_EXISTS("off64_t definition" "${HEADERS}" HAVE_OFF64_T)
  # Check if the POSIX functions are available (even on Windows). They are preferred
  # to the Microsoft specific functions on compilers like MinGW.
  CHECK_FUNCTIONWITHHEADER_EXISTS("popen" "${HEADERS}" HAVE_POPEN)
  CHECK_FUNCTIONWITHHEADER_EXISTS("pclose" "${HEADERS}" HAVE_PCLOSE)

  # Signal handling functions
  CHECK_FUNCTIONWITHHEADER_EXISTS("sigjmp_buf definition" "setjmp.h" HAVE_SIGJMP_BUF)

IF(DCMTK_WITH_OPENSSL)
  # Check if OpenSSL provides the SSL_CTX_get0_param function
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_get0_param" "openssl/ssl.h" HAVE_SSL_CTX_GET0_PARAM ${OPENSSL_LIBS})
  CHECK_FUNCTIONWITHHEADER_EXISTS("RAND_egd" "openssl/rand.h" HAVE_RAND_EGD ${OPENSSL_LIBS})
ENDIF(DCMTK_WITH_OPENSSL)

IF(HAVE_LOCKF AND ANDROID)
  # When Android introduced lockf, they forgot to put the constants like F_LOCK in the
  # appropriate headers, this tests if they are defined and disables lockf if they are not
  CHECK_FUNCTIONWITHHEADER_EXISTS("lockf(0, F_LOCK, 0)" "${HEADERS}" HAVE_LOCKF_CONSTANTS)
  IF(NOT HAVE_LOCKF_CONSTANTS)
    SET(HAVE_LOCKF FALSE CACHE INTERNAL "lockf implementation is broken")
  ENDIF()
ENDIF(HAVE_LOCKF AND ANDROID)

# Tests that require a try-compile

IF(HAVE_MATH_H)
  IF(HAVE_LIBC_H)
    # checks if <libc.h> and <math.h> cause a problem if libc.h is included extern "C"
    # and math.h is not. This is the case on QNX 6.2.x and 6.5.x.
    DCMTK_TRY_COMPILE(INCLUDE_LIBC_H_AS_EXTERN_C "<libc.h> can be included as extern \"C\""
    "extern \"C\" {
#include <libc.h>
}
#include <math.h>
int main()
{
    int i = 0;
    return 0;
}")
    IF(INCLUDE_LIBC_H_AS_EXTERN_C)
      SET(INCLUDE_LIBC_H_AS_CXX 0 CACHE INTERNAL "libc.h should be treated as a C++ header")
    ELSE()
      SET(INCLUDE_LIBC_H_AS_CXX 1 CACHE INTERNAL "libc.h should be treated as a C++ header")
    ENDIF()
  ENDIF(HAVE_LIBC_H)

  # checks if <math.h> must be included as a C++ include file (i.e. without extern "C").
  # Some sytems (Win32, HP/UX 10) use C++ language features in <math.h>.
  DCMTK_TRY_COMPILE(INCLUDE_MATH_H_AS_EXTERN_C "<math.h> can be included as extern \"C\""
  "extern \"C\" {
#include <math.h>
}
int main()
{
    int i = 0;
    return 0;
}")
  IF(INCLUDE_MATH_H_AS_EXTERN_C)
    SET(INCLUDE_MATH_H_AS_CXX 0 CACHE INTERNAL "math.h should be treated as a C++ header")
  ELSE()
    SET(INCLUDE_MATH_H_AS_CXX 1 CACHE INTERNAL "math.h should be treated as a C++ header")
  ENDIF()
ENDIF(HAVE_MATH_H)


IF(NOT DEFINED C_CHAR_UNSIGNED)
  MESSAGE(STATUS "Checking signedness of char")
  DCMTK_TRY_RUN(C_CHAR_SIGNED C_CHAR_SIGNED_COMPILED "${CMAKE_BINARY_DIR}/CMakeTmp/Char"
          "${DCMTK_SOURCE_DIR}/CMake/dcmtkTestCharSignedness.cc"
          COMPILE_OUTPUT_VARIABLE C_CHAR_SIGNED_COMPILE_OUTPUT)
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
ENDIF(NOT DEFINED C_CHAR_UNSIGNED)

DCMTK_TRY_COMPILE(HAVE_CXX_BOOL "C++ type bool exists"
    "// Minimal test for existence of 'bool' type.
void TestBool(bool) {}

int main()
{
  TestBool(false);
  TestBool(true);
  return 0;
}")

# Check for thread type
IF(HAVE_WINDOWS_H)
  SET(HAVE_INT_TYPE_PTHREAD_T 1)
ELSE(HAVE_WINDOWS_H)
  DCMTK_TRY_COMPILE(HAVE_INT_TYPE_PTHREAD_T "pthread_t is an integer type"
        "// test to see if pthread_t is a pointer type or not

#include <pthread.h>

int main ()
{
  pthread_t p;
  unsigned long l = p;
  return 0;
}")
  IF(NOT HAVE_INT_TYPE_PTHREAD_T)
    SET(HAVE_POINTER_TYPE_PTHREAD_T 1 CACHE INTERNAL "Set if pthread_t is a pointer type")
  ELSE(NOT HAVE_INT_TYPE_PTHREAD_T)
    SET(HAVE_POINTER_TYPE_PTHREAD_T 0 CACHE INTERNAL "Set if pthread_t is a pointer type")
  ENDIF(NOT HAVE_INT_TYPE_PTHREAD_T)
ENDIF(HAVE_WINDOWS_H)

# Check if typename works properly. Only MSC6 really fails here.
DCMTK_TRY_COMPILE(HAVE_TYPENAME "typename works correctly"
    "
template<typename type>
type identical(type arg)
{
    return arg;
}

template<typename type>
typename type::a foo(typename type::a arg)
{
    // This one is the usage of \"typename\" that we are checking for.
    // MSC6 refuses this one incorrectly.
    return identical<typename type::a>(arg);
}

class test {
public:
    typedef int a;
};

int main()
{
    return foo<test>(0);
}")

# Check if ENAMETOOLONG is defined.
DCMTK_TRY_COMPILE(HAVE_ENAMETOOLONG "ENAMETOOLONG is defined"
    "#include <errno.h>

int main()
{
    int value = ENAMETOOLONG;
    return 0;
}")

# Check if strerror_r returns a char* is defined.
DCMTK_TRY_COMPILE(HAVE_INT_STRERROR_R "strerror_r returns an int"
    "#include <string.h>

int main()
{
    char *buf = 0;
    int i = strerror_r(0, buf, 100);
    return i;
}")
IF(HAVE_INT_STRERROR_R)
  SET(HAVE_CHARP_STRERROR_R 0 CACHE INTERNAL "Set if strerror_r() returns a char*")
ELSE(HAVE_INT_STRERROR_R)
  SET(HAVE_CHARP_STRERROR_R 1 CACHE INTERNAL "Set if strerror_r() returns a char*")
ENDIF(HAVE_INT_STRERROR_R)

# Check if variable length arrays are supported.
DCMTK_TRY_COMPILE(HAVE_VLA "variable length arrays are supported"
    "int main()
{
    int n = 42;
    int foo[n];
    return 0;
}")

# Check if std::ios::nocreate exists
DCMTK_TRY_COMPILE(HAVE_IOS_NOCREATE "std::ios::nocreate exists"
    "#include <fstream>

using namespace std;

int main()
{
  std::ifstream file(\"name\", std::ios::nocreate);
  return 0;
}")

# Check for explicit large file support
DCMTK_TRY_COMPILE(_LARGEFILE64_SOURCE "explicit large file support is available"
    "#define _LARGEFILE64_SOURCE
#include <stdio.h>

using namespace std;

int main()
{
  FILE *f = fopen64(\"name\", \"r\");
  return 0;
}")

IF(WIN32)
  # If someone can tell me how to convince TRY_COMPILE to link against winsock,
  # we could use tests for these. Until then, here is what would be the result:
  SET(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  SET(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  SET(HAVE_INTP_SELECT 0 CACHE INTERNAL "Set if select() accepts an int* argument")
ELSE(WIN32)
  # Check if socket functions accept an int*
  DCMTK_TRY_COMPILE(HAVE_INTP_SOCKET, "socket functions accept an int* argument"
      "
#ifdef __cplusplus
extern \"C\" {
#endif
#ifdef _WIN32
/* Windows is pure evil */
#include <windows.h>
#else
#include <sys/socket.h>
#endif
#ifdef __cplusplus
}
#endif

int main()
{
    int i;
    struct sockaddr *addr = 0;
    int addrlen = 0;
    int optlen = 0;

    i = accept(1, addr, &addrlen);
    i = getsockopt(0, 0, 0, 0, &optlen);

    return 0;
}")
  IF(HAVE_INTP_SOCKET)
    SET(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
    SET(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  ELSE(HAVE_INTP_SOCKET)
    SET(HAVE_INTP_ACCEPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
    SET(HAVE_INTP_GETSOCKOPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
  ENDIF(HAVE_INTP_SOCKET)

  # Check if select() accepts an int*
  DCMTK_TRY_COMPILE(HAVE_INTP_SELECT "select() accepts an int* argument"
      "
#ifdef __cplusplus
extern \"C\" {
#endif
#ifdef _WIN32
/* Windows is pure evil */
#include <windows.h>
#else
#include <sys/socket.h>
#endif
#ifdef __cplusplus
}
#endif

int main()
{
    int i;
    int fds = 0;

    i = select(1, &fds, &fds, &fds, 0);

    return 0;
}
")
ENDIF(WIN32)

# Check for alignment query / specifier support
DCMTK_TRY_COMPILE(HAVE_GNU_ALIGNOF "__alignof__ is supported"
    "int main()
{
    char c[__alignof__(int)];
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_MS_ALIGNOF "__alignof is supported"
    "int main()
{
    char c[__alignof(int)];
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_ATTRIBUTE_ALIGNED "__attribute__((aligned)) is supported"
    "int main()
{
    __attribute__((aligned(4))) char c[16];
    return 0;
}")

DCMTK_TRY_COMPILE(ATTRIBUTE_ALIGNED_SUPPORTS_TEMPLATES "__attribute__((aligned)) supports templates"
    "template<typename T>
struct test { typedef T type __attribute__((aligned(4))); };
int main()
{
    test<char>::type i;
    return 0;
}")

DCMTK_TRY_COMPILE(ALIGNAS_SUPPORTS_TYPEDEFS "alignas supports typedefs"
    "int main()
{
    typedef char type[16] alignas(4);
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_DECLSPEC_ALIGN "__declspec(align) is supported"
    "int main()
{
    __declspec(align(4)) char c[16];
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_DEFAULT_CONSTRUCTOR_DETECTION_VIA_SFINAE "the compiler supports default constructor detection via SFINAE"
    "struct no_type {};
struct yes_type {double d;};
template<unsigned>
struct consume{};
template<typename X>
static yes_type sfinae(consume<sizeof *new X>*);
template<typename X>
static no_type sfinae(...);
struct test { test( int ); };
template<int I>
struct enable {};
template<>
struct enable<0> { enum { result = 0 }; };
int main()
{
    return enable<sizeof(sfinae<test>(0)) == sizeof(yes_type)>::result;
}")

DCMTK_TRY_COMPILE(HAVE_STD_NAMESPACE "ANSI standard C++ includes use std namespace"
    "#include <iostream>
int main()
{
    using namespace std;
    std::cout << endl;
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_STD__NOTHROW "the compiler supports std::nothrow"
    "#include <new>
int main()
{
    int* i = new (std::nothrow) int;
    return 0;
}")

DCMTK_TRY_COMPILE(HAVE_NOTHROW_DELETE "the compiler supports operator delete (std::nothrow)"
    "#include <new>
int main()
{
    int* i = 0;
    operator delete (i,std::nothrow);
    return 0;
}")

# Compile config/tests/arith.cc and generate config/arith.h
FUNCTION(INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES)
  SET(ARITH_H_FILE "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/arith.h")
  IF("${DCMTK_SOURCE_DIR}/config/tests/arith.cc" IS_NEWER_THAN "${ARITH_H_FILE}")
    IF(CMAKE_CROSSCOMPILING)
      IF(WIN32)
        UNIX_TO_WINE_PATH(ARITH_H_FILE "${ARITH_H_FILE}")
        STRING(REPLACE "\\" "\\\\" ARITH_H_FILE "${ARITH_H_FILE}")
      ELSEIF(ANDROID)
        SET(ARITH_H_DESTINATION "${ARITH_H_FILE}")
        SET(ARITH_H_FILE "${ANDROID_TEMPORARY_FILES_LOCATION}/arith.h")
      ENDIF()
    ENDIF(CMAKE_CROSSCOMPILING)
    DCMTK_TRY_RUN(
      RESULT COMPILED
      "${DCMTK_BINARY_DIR}/CMakeTmp/Arith"
      "${DCMTK_SOURCE_DIR}/config/tests/arith.cc"
      COMPILE_DEFINITIONS -I"${DCMTK_BINARY_DIR}/config/include" -I"${DCMTK_SOURCE_DIR}/ofstd/include" -I"${DCMTK_SOURCE_DIR}/ofstd/libsrc"
      RUN_OUTPUT_VARIABLE OUTPUT
      COMPILE_OUTPUT_VARIABLE CERR
      ARGS "\\\"${ARITH_H_FILE}\\\""
    )
    IF(COMPILED)
      IF(NOT RESULT)
        MESSAGE(STATUS "${OUTPUT}")
        IF(CMAKE_CROSSCOMPILING)
          IF(ANDROID)
            DCMTK_ANDROID_PULL(DCMTK_ANDROID_EMULATOR_INSTANCE "${ARITH_H_FILE}" DESTINATION "${ARITH_H_DESTINATION}")
          ENDIF()
        ENDIF(CMAKE_CROSSCOMPILING)
      ELSE(NOT RESULT)
        MESSAGE(FATAL_ERROR "${OUTPUT}")
      ENDIF(NOT RESULT)
    ELSE(COMPILED)
      MESSAGE(FATAL_ERROR "${CERR}")
    ENDIF(COMPILED)
  ENDIF() # file needs update
ENDFUNCTION(INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES)

FUNCTION(DCMTK_ADD_CXX11_FLAGS)
  STRING(FIND "${CMAKE_CXX_FLAGS}" "${DCMTK_CXX11_FLAGS}" INDEX)
  IF(INDEX EQUAL -1)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DCMTK_CXX11_FLAGS}" PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(DCMTK_ADD_CXX11_FLAGS)

FUNCTION(DCMTK_TEST_ENABLE_CXX11)
  SET(RESULT 0)
  SET(TEXT_RESULT "disabled")
  IF(DCMTK_ENABLE_CXX11)
    IF(DEFINED HAVE_CXX11_TEST_RESULT)
      IF(HAVE_CXX11_TEST_RESULT)
        DCMTK_ADD_CXX11_FLAGS()
        SET(RESULT 1)
        SET(TEXT_RESULT "enabled")
      ENDIF()
    ELSE()
      DCMTK_ADD_CXX11_FLAGS() # will automatically be removed by the function scope
      SET(MESSAGE "Checking whether the compiler supports C++11")
      MESSAGE(STATUS "${MESSAGE}")
      TRY_COMPILE(COMPILE_RESULT "${CMAKE_BINARY_DIR}" "${DCMTK_SOURCE_DIR}/config/tests/cxx11.cc")
      SET(HAVE_CXX11_TEST_RESULT "${COMPILE_RESULT}" CACHE INTERNAL "Caches the configuration test result for C++11 support.")
      IF(COMPILE_RESULT)
        SET(RESULT 1)
        SET(TEXT_RESULT "enabled")
        MESSAGE(STATUS "${MESSAGE} -- yes")
      ELSE()
        MESSAGE(STATUS "${MESSAGE} -- no")
      ENDIF()
    ENDIF()
  ENDIF()
  SET(HAVE_CXX11 "${RESULT}" CACHE INTERNAL "Set to 1 if the compiler supports C++11 and it should be enabled.")
  MESSAGE(STATUS "Info: C++11 features ${TEXT_RESULT}")
  IF(RESULT)
    # push C++11 CXX-flags to the parent scope
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
  ENDIF()
ENDFUNCTION(DCMTK_TEST_ENABLE_CXX11)

FUNCTION(DCMTK_TEST_ENABLE_STL_FEATURE NAME)
  STRING(TOUPPER "${NAME}" FEATURE)
  IF(ARGN)
    SET(SOURCEFILE "${ARGN}")
  ELSE()
    SET(SOURCEFILE "${NAME}")
  ENDIF()
  IF(DCMTK_ENABLE_STL_${FEATURE} STREQUAL "INFERRED")
    SET(DCMTK_ENABLE_STL_${FEATURE} ${DCMTK_ENABLE_STL})
  ENDIF()
  SET(RESULT 0)
  SET(TEXT_RESULT "disabled")
  IF(DCMTK_ENABLE_STL_${FEATURE} STREQUAL "ON")
    IF(DEFINED HAVE_STL_${FEATURE}_TEST_RESULT)
      IF(HAVE_STL_${FEATURE}_TEST_RESULT)
        SET(RESULT 1)
        SET(TEXT_RESULT "enabled")
      ENDIF()
    ELSE()
      SET(MESSAGE "Checking whether STL ${NAME} works correctly")
      MESSAGE(STATUS "${MESSAGE}")
      DCMTK_TRY_RUN(RUN_RESULT COMPILE_RESULT "${CMAKE_BINARY_DIR}" "${DCMTK_SOURCE_DIR}/config/tests/${SOURCEFILE}.cc")
      IF(COMPILE_RESULT AND RUN_RESULT EQUAL 0)
        SET(RESULT 1)
        SET(TEXT_RESULT "enabled")
        MESSAGE(STATUS "${MESSAGE} -- yes")
      ELSE()
        MESSAGE(STATUS "${MESSAGE} -- no")
      ENDIF()
      SET(HAVE_STL_${FEATURE}_TEST_RESULT ${RESULT} CACHE INTERNAL "Caches the configuration test result for STL ${NAME}")
    ENDIF()
  ENDIF()
  SET(HAVE_STL_${FEATURE} ${RESULT} CACHE INTERNAL "Set to 1 if the compiler/OS provides a working STL ${NAME} implementation.")
  MESSAGE(STATUS "Info: STL ${NAME} support ${TEXT_RESULT}")
ENDFUNCTION(DCMTK_TEST_ENABLE_STL_FEATURE)

DCMTK_TEST_ENABLE_CXX11()
DCMTK_TEST_ENABLE_STL_FEATURE("vector")
DCMTK_TEST_ENABLE_STL_FEATURE("algorithm" "algo")
DCMTK_TEST_ENABLE_STL_FEATURE("limits")
DCMTK_TEST_ENABLE_STL_FEATURE("list")
DCMTK_TEST_ENABLE_STL_FEATURE("map")
DCMTK_TEST_ENABLE_STL_FEATURE("memory")
DCMTK_TEST_ENABLE_STL_FEATURE("stack")
DCMTK_TEST_ENABLE_STL_FEATURE("string")
DCMTK_TEST_ENABLE_STL_FEATURE("type_traits" "ttraits")
DCMTK_TEST_ENABLE_STL_FEATURE("tuple")
DCMTK_TEST_ENABLE_STL_FEATURE("system_error" "syserr")

IF(CMAKE_CROSSCOMPILING)
  SET(DCMTK_CROSS_COMPILING ${CMAKE_CROSSCOMPILING})
ENDIF(CMAKE_CROSSCOMPILING)
