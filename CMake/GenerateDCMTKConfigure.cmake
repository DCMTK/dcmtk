# Set build configuration to use for configuration tests
if(CMAKE_BUILD_TYPE)
  set(CMAKE_TRY_COMPILE_CONFIGURATION "${CMAKE_BUILD_TYPE}")
else()
  set(CMAKE_TRY_COMPILE_CONFIGURATION "Release")
endif()

# Compiled-in dictionary support
if(DCMTK_ENABLE_BUILTIN_DICTIONARY)
  set(ENABLE_BUILTIN_DICTIONARY 1)
  message(STATUS "Info: DCMTK will compile with built-in (compiled-in) dictionary")
  # No extra variable needed since its only evaluated in CMake files
else()
  set(ENABLE_BUILTIN_DICTIONARY "")
  message(STATUS "Info: DCMTK will compile without built-in (compiled-in) dictionary")
  # No extra variable needed since its only evaluated in CMake files
endif()

# External dictionary support
if(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
  set(ENABLE_EXTERNAL_DICTIONARY 1)
  message(STATUS "Info: DCMTK will try to load external dictionary from default path on startup")
else()
  set(ENABLE_EXTERNAL_DICTIONARY "")
  message(STATUS "Info: DCMTK will not try to load external dictionary from default path on startup")
endif()


# Private tags
if(DCMTK_ENABLE_PRIVATE_TAGS)
  set(ENABLE_PRIVATE_TAGS 1)
  message(STATUS "Info: DCMTK's builtin private dictionary support will be enabled")
else()
  set(ENABLE_PRIVATE_TAGS "")
  message(STATUS "Info: DCMTK's builtin private dictionary support will be disabled")
endif()

# Thread support
if(DCMTK_WITH_THREADS)
  set(WITH_THREADS 1)
  message(STATUS "Info: Thread support will be enabled")
else()
  set(WITH_THREADS "")
  message(STATUS "Info: Thread support will be disabled")
endif()

# Wide char file I/O support
if(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  set(WIDE_CHAR_FILE_IO_FUNCTIONS 1)
  message(STATUS "Info: Wide char file I/O functions will be enabled")
else()
  set(WIDE_CHAR_FILE_IO_FUNCTIONS "")
  message(STATUS "Info: Wide char file I/O functions will be disabled")
endif()

# Wide char main function
if(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
  set(WIDE_CHAR_MAIN_FUNCTION 1)
  message(STATUS "Info: Wide char main function for command line tools will be enabled")
else()
  set(WIDE_CHAR_MAIN_FUNCTION "")
  message(STATUS "Info: Wide char main function for command line tools will be disabled")
endif()

if(NOT DCMTK_ENABLE_CHARSET_CONVERSION)
  set(DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING "Select character set conversion implementation.")
  if(DCMTK_WITH_ICONV)
    if(DCMTK_WITH_ICU)
      message(WARNING "Warning: Both ICU and the libiconv are available, using libiconv. Modify DCMTK_ENABLE_CHARSET_CONVERSION for switching to ICU")
    endif()
    set(DCMTK_ENABLE_CHARSET_CONVERSION "libiconv" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  elseif(DCMTK_WITH_ICU)
    set(DCMTK_ENABLE_CHARSET_CONVERSION "ICU" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  elseif(DCMTK_WITH_STDLIBC_ICONV)
    set(DCMTK_ENABLE_CHARSET_CONVERSION "stdlibc (iconv)" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  else()
    set(DCMTK_ENABLE_CHARSET_CONVERSION "<disabled>" CACHE STRING "${DCMTK_ENABLE_CHARSET_CONVERSION_DOCSTRING}")
  endif()
endif()

set(DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES)
if(DCMTK_WITH_ICONV)
  list(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "libiconv")
endif()
if(DCMTK_WITH_ICU)
  list(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "ICU")
endif()
if(DCMTK_WITH_STDLIBC_ICONV)
  list(APPEND DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES "stdlibc (iconv)")
endif()
set_property(CACHE DCMTK_ENABLE_CHARSET_CONVERSION PROPERTY STRINGS ${DCMTK_ENABLE_CHARSET_CONVERSION_ALTERNATIVES} "<disabled>")

if(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "libiconv" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_ICONV")
  message(STATUS "Info: Building DCMTK with character set conversion support using libiconv")
  set(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_ICONV")
  set(CHARSET_CONVERSION_LIBS ${LIBICONV_LIBS})
elseif(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "ICU" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_ICU")
  message(STATUS "Info: Building DCMTK with character set conversion support using the ICU")
  set(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_ICU")
  set(CHARSET_CONVERSION_LIBS ${ICU_LIBS})
elseif(DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "stdlibc (iconv)" OR DCMTK_ENABLE_CHARSET_CONVERSION STREQUAL "DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV")
  message(STATUS "Info: Building DCMTK with character set conversion support using builtin iconv functions from the C standard library")
  set(DCMTK_ENABLE_CHARSET_CONVERSION "DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV")
else()
  message(STATUS "Info: Building DCMTK without character set conversion support")
  set(DCMTK_ENABLE_CHARSET_CONVERSION OFF)
endif()

# Standard C++ headers (currently hard-coded)
#if(VTK_USE_ANSI_STDLIB)
  set(USE_STD_CXX_INCLUDES 1)
#else()
#  set(USE_STD_CXX_INCLUDES "")
#endif()

# Configure file

# Windows being windows, it lies about its processor type to 32 bit binaries
set(SYSTEM_PROCESSOR "$ENV{PROCESSOR_ARCHITEW6432}")
if(NOT SYSTEM_PROCESSOR)
  set(SYSTEM_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")
endif()
# CMake doesn't provide a configure-style system type string
set(CANONICAL_HOST_TYPE "${SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")
DCMTK_UNSET(SYSTEM_PROCESSOR)

# Configure dictionary path and install prefix
if(WIN32 AND NOT CYGWIN)
  # Set DCMTK_PREFIX needed within some code. Be sure that all / are replaced by \\.
  set(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  string(REGEX REPLACE "/" "\\\\\\\\" DCMTK_PREFIX "${DCMTK_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  set(PATH_SEPARATOR "\\\\")
  set(ENVIRONMENT_PATH_SEPARATOR ";")
  # Set dictionary path to the data dir inside install main dir (prefix)
  if(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    set(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}\\\\${CMAKE_INSTALL_DATADIR}\\\\dcmtk\\\\dicom.dic")
    # If private dictionary should be utilized, add it to default dictionary path.
    if(ENABLE_PRIVATE_TAGS)
      set(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH};${DCMTK_PREFIX}\\\\${CMAKE_INSTALL_DATADIR}\\\\dcmtk\\\\private.dic")
    endif()
     # Again, for Windows strip all / from path and replace it with \\.
    string(REGEX REPLACE "/" "\\\\\\\\" DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}")
  else()
    set(DCM_DICT_DEFAULT_PATH "")
  endif()
  # Set default directory for configuration and support data.
  set(DCMTK_DEFAULT_CONFIGURATION_DIR "")
  set(DCMTK_DEFAULT_SUPPORT_DATA_DIR "")
else()
  # Set DCMTK_PREFIX needed within some code.
  set(DCMTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
  # Set path and multiple path separator being used in dictionary code etc.
  set(PATH_SEPARATOR "/")
  set(ENVIRONMENT_PATH_SEPARATOR ":")
  # Set dictionary path to the data dir inside install main dir (prefix).
  if(DCMTK_ENABLE_EXTERNAL_DICTIONARY)
    set(DCM_DICT_DEFAULT_PATH "${DCMTK_PREFIX}/${CMAKE_INSTALL_DATADIR}/dcmtk/dicom.dic")
    # If private dictionary should be utilized, add it to default dictionary path.
    if(ENABLE_PRIVATE_TAGS)
      set(DCM_DICT_DEFAULT_PATH "${DCM_DICT_DEFAULT_PATH}:${DCMTK_PREFIX}/${CMAKE_INSTALL_DATADIR}/dcmtk/private.dic")
    endif()
  else()
    set(DCM_DICT_DEFAULT_PATH "")
  endif()
  # Set default directory for configuration and support data.
  set(DCMTK_DEFAULT_CONFIGURATION_DIR "${DCMTK_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/dcmtk/")
  set(DCMTK_DEFAULT_SUPPORT_DATA_DIR "${DCMTK_PREFIX}/${CMAKE_INSTALL_DATADIR}/dcmtk/")
endif()

# Check the sizes of various types
include (CheckTypeSize)
CHECK_TYPE_SIZE("char" SIZEOF_CHAR)
CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("float" SIZEOF_FLOAT)
CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P)

# Check for include files, libraries, and functions
include("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkTryCompile.cmake")
include("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkTryRun.cmake")
include("${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake")
include("${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake")
include("${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake")
include("${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake")
include("${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake")
include("${DCMTK_CMAKE_INCLUDE}CMake/CheckFunctionWithHeaderExists.cmake")
include(CheckCXXSymbolExists OPTIONAL)
if(NOT COMMAND CHECK_CXX_SYMBOL_EXISTS)
  # fallback implementation for old CMake Versions
  function(CHECK_CXX_SYMBOL_EXISTS SYMBOL FILES VAR)
    set(CODE)
    foreach(FILE ${FILES})
      set(CODE "${CODE}#include <${FILE}>\n")
    endforeach()
    set(CODE "${CODE}\nint main(int argc, char** argv)\n{\n  (void)argv;\n#ifndef ${SYMBOL}\n  return ((int*)(&${SYMBOL}))[argc];\n#else\n  (void)argc;\n  return 0;\n#endif\n}\n")
    DCMTK_TRY_COMPILE("${VAR}" "the compiler supports ${SYMBOL}" "${CODE}")
  endfunction()
endif()

foreach(FUNC "__FUNCTION__" "__PRETTY_FUNCTION__" "__func__")
  CHECK_SYMBOL_EXISTS("${FUNC}" "" "HAVE_${FUNC}_C_MACRO")
  # test if the C++ compiler also supports them (e.g. SunPro doesn't)
  CHECK_CXX_SYMBOL_EXISTS("${FUNC}" "" "HAVE_${FUNC}_CXX_MACRO")
  if(HAVE_${FUNC}_C_MACRO AND HAVE_${FUNC}_CXX_MACRO)
    set("HAVE_${FUNC}_MACRO" 1 CACHE INTERNAL "Have symbol ${FUNC}" FORCE)
  else()
    set("HAVE_${FUNC}_MACRO" CACHE INTERNAL "Have symbol ${FUNC}" FORCE)
  endif()
endforeach()

# prepare include directories for 3rdparty libraries before performing
# header searches
if(ZLIB_INCDIR)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${ZLIB_INCDIR}")
endif()

if(ZLIB_INCLUDE_DIRS)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${ZLIB_INCLUDE_DIRS}")
endif()

if(LIBPNG_INCDIR)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${LIBPNG_INCDIR}")
endif()

if(PNG_INCLUDE_DIR)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${PNG_INCLUDE_DIR}")
endif()

if(OPENSSL_INCDIR)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCDIR}")
endif()

if(OPENSSL_INCLUDE_DIR)
  list(APPEND CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCLUDE_DIR}")
endif()

# For Windows, hardcode these values to avoid long search times
if(WIN32 AND NOT CYGWIN)
  CHECK_INCLUDE_FILE_CXX("windows.h" HAVE_WINDOWS_H)
  CHECK_INCLUDE_FILE_CXX("winsock.h" HAVE_WINSOCK_H)
endif()

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
  CHECK_INCLUDE_FILE_CXX("streambuf.h" HAVE_STREAMBUF_H)
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
  CHECK_INCLUDE_FILE_CXX("process.h" HAVE_PROCESS_H)
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

if(NOT APPLE)
  # poll on macOS is unreliable, it first did not exist, then was broken until
  # fixed in 10.9 only to break again in 10.12.
  CHECK_INCLUDE_FILE_CXX("poll.h" DCMTK_HAVE_POLL)
  if(DCMTK_HAVE_POLL)
    add_definitions(-DDCMTK_HAVE_POLL=1)
  endif()
endif()

  # This mimics the autoconf test. There are systems out there
  # (e.g. FreeBSD and NeXT) where tcp.h can't be compiled on its own.
  set(TCP_H_DEPS "")
  if(HAVE_SYS_TYPES_H)
    # This one is needed to make FreeBSD happy
    set(TCP_H_DEPS "sys/types.h")
  endif()
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/in_systm.h" HAVE_NETINET_IN_SYSTM_H)
  if(HAVE_NETINET_IN_SYSTM_H)
    set(TCP_H_DEPS "${TCP_H_DEPS};netinet/in_systm.h")
  endif()
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/in.h" HAVE_NETINET_IN_H)
  if(HAVE_NETINET_IN_H)
    set(TCP_H_DEPS "${TCP_H_DEPS};netinet/in.h")
  endif()
  CHECK_INCLUDE_FILES("${TCP_H_DEPS};netinet/tcp.h" HAVE_NETINET_TCP_H)

  if(NOT HAVE_PNG_H)
    # <png.h> is unavailable, so test if we need to include it as <libpng/png.h>
    CHECK_INCLUDE_FILE_CXX("libpng/png.h" HAVE_LIBPNG_PNG_H)
  else()
    # ensure including <png.h> is preferred
    DCMTK_UNSET_CACHE(HAVE_LIBPNG_PNG_H)
  endif()

  # There is no CMake macro to take care of these yet

  if(WIN32 AND NOT CYGWIN AND NOT MINGW)
    set(HAVE_NO_TYPEDEF_SSIZE_T TRUE)
    set(HAVE_NO_TYPEDEF_PID_T TRUE)
  else()
    set(HAVE_NO_TYPEDEF_PID_T FALSE)
    set(HAVE_NO_TYPEDEF_SIZE_T FALSE)
    if(NOT ${HAVE_SYS_TYPES_H})
      set(HAVE_NO_TYPEDEF_SSIZE_T TRUE)
    endif()
  endif()

  set(HEADERS)

  if(HAVE_IO_H)
    set(HEADERS ${HEADERS} io.h)
  endif()

  if(HAVE_IOSTREAM_H)
    set(HEADERS ${HEADERS} iostream.h)
  endif()

  if(HAVE_MATH_H)
    set(HEADERS ${HEADERS} math.h)
  endif()

  if(HAVE_LIBC_H)
    set(HEADERS ${HEADERS} libc.h)
  endif()

  if(HAVE_THREAD_H)
    set(HEADERS ${HEADERS} thread.h)
  endif()

  if(HAVE_PROCESS_H)
    set(HEADERS ${HEADERS} process.h)
  endif()

  if(HAVE_PTHREAD_H)
    set(HEADERS ${HEADERS} pthread.h)
  endif()

  if(HAVE_UNISTD_H)
    set(HEADERS ${HEADERS} unistd.h)
  endif()

  if(HAVE_STDLIB_H)
    set(HEADERS ${HEADERS} stdlib.h)
  endif()

  if(HAVE_STDINT_H)
    set(HEADERS ${HEADERS} stdint.h)
  endif()

  if(HAVE_STDDEF_H)
    set(HEADERS ${HEADERS} stddef.h)
  endif()

  if(HAVE_NETDB_H)
    set(HEADERS ${HEADERS} netdb.h)
  endif()

  if(HAVE_SYS_FILE_H)
    set(HEADERS ${HEADERS} sys/file.h)
  endif()

  if(HAVE_STRING_H)
    set(HEADERS ${HEADERS} string.h)
  endif()

  if(HAVE_STRINGS_H)
    set(HEADERS ${HEADERS} strings.h)
  endif()

  if(HAVE_SYS_WAIT_H)
    set(HEADERS ${HEADERS} sys/wait.h)
  endif()

  if(HAVE_SYS_TIME_H)
    set(HEADERS ${HEADERS} sys/time.h)
  endif()

  if(HAVE_SYS_RESOURCE_H)
    set(HEADERS ${HEADERS} sys/resource.h)
  endif()

  if(HAVE_SYS_TYPES_H)
    set(HEADERS ${HEADERS} sys/types.h)
  endif()

  if(HAVE_SYS_SOCKET_H)
    set(HEADERS ${HEADERS} sys/socket.h)
  endif()

  if(HAVE_SYS_STAT_H)
    set(HEADERS ${HEADERS} sys/stat.h)
  endif()

  if(HAVE_SYS_TIMEB_H)
    set(HEADERS ${HEADERS} sys/timeb.h)
  endif()

  if(HAVE_STDARG_H)
    set(HEADERS ${HEADERS} stdarg.h)
  endif()

  if(HAVE_STDIO_H)
    set(HEADERS ${HEADERS} stdio.h)
  endif()

  if(HAVE_SYS_SELECT_H)
    set(HEADERS ${HEADERS} sys/select.h)
  endif()

  if(HAVE_WINDOWS_H)
    # also add winsock2.h and ws2tcpip.h that are available since Windows NT 4.0
    set(HEADERS ${HEADERS} winsock2.h ws2tcpip.h windows.h)
  endif()

  if(HAVE_GRP_H)
    set(HEADERS ${HEADERS} grp.h)
  endif()

  if(HAVE_PWD_H)
    set(HEADERS ${HEADERS} pwd.h)
  endif()

  if(HAVE_DIRENT_H)
    set(HEADERS ${HEADERS} dirent.h)
  endif()

  if(HAVE_SYS_SYSCALL_H)
    set(HEADERS ${HEADERS} sys/syscall.h)
  endif()

  if(HAVE_WINSOCK_H)
    set(HEADERS ${HEADERS} winsock.h)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} iphlpapi ws2_32 netapi32 wsock32)
  endif()

  if(HAVE_FENV_H)
    set(HEADERS ${HEADERS} fenv.h)
  endif()

  # std::vsnprintf and std::vsnprintf need the C++ version of the headers.
  # We just assume they exist when the C version was found
  set(CXXHEADERS)

  if(HAVE_CMATH)
    set(CXXHEADERS ${CXXHEADERS} cmath)
  endif()

  if(HAVE_CSTDIO)
    set(CXXHEADERS ${CXXHEADERS} cstdio)
  endif()

  if(HAVE_CSTDARG)
    set(CXXHEADERS ${CXXHEADERS} cstdarg)
  endif()

  if(HAVE_CSTDINT)
    set(CXXHEADERS ${CXXHEADERS} cstdint)
  endif()

  if(HAVE_CSTDDEF)
    set(CXXHEADERS ${CXXHEADERS} cstddef)
  endif()

  if(WIN32)
    # CHECK_FUNCTION_EXISTS does not work correctly on Windows (due to symbol name mangling)
    # use CHECK_SYMBOL_EXISTS instead
    macro(CHECK_FUNCTION_EXISTS FUNCTION VAR)
      check_symbol_exists("${FUNCTION}" "${HEADERS}" "${VAR}")
    endmacro()
  endif()

  CHECK_FUNCTION_EXISTS(connect HAVE_CONNECT)
  CHECK_FUNCTION_EXISTS(accept HAVE_ACCEPT)
  CHECK_FUNCTION_EXISTS(access HAVE_ACCESS)
  CHECK_FUNCTION_EXISTS(atoll HAVE_ATOLL)
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
  CHECK_FUNCTION_EXISTS(getrusage HAVE_GETRUSAGE)
  CHECK_FUNCTION_EXISTS(getsockname HAVE_GETSOCKNAME)
  CHECK_FUNCTION_EXISTS(getsockopt HAVE_GETSOCKOPT)
  CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
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

  CHECK_FUNCTIONWITHHEADER_EXISTS(feenableexcept "${HEADERS}" HAVE_PROTOTYPE_FEENABLEEXCEPT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("isinf(0.)" "${HEADERS}" HAVE_PROTOTYPE_ISINF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("isnan(0.)" "${HEADERS}" HAVE_PROTOTYPE_ISNAN)
  CHECK_FUNCTIONWITHHEADER_EXISTS(finite "${HEADERS}" HAVE_PROTOTYPE_FINITE)
  CHECK_FUNCTIONWITHHEADER_EXISTS("std::isinf(0.)" "${CXXHEADERS}" HAVE_PROTOTYPE_STD__ISINF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("std::isnan(0.)" "${CXXHEADERS}" HAVE_PROTOTYPE_STD__ISNAN)
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
  CHECK_FUNCTIONWITHHEADER_EXISTS("std::ios_base::openmode definition" "${CXXHEADERS}" HAVE_DECLARATION_STD__IOS_BASE__OPENMODE)
  CHECK_FUNCTIONWITHHEADER_EXISTS(pthread_rwlock_init "${HEADERS}" HAVE_PTHREAD_RWLOCK)
  CHECK_FUNCTIONWITHHEADER_EXISTS("__sync_add_and_fetch((int*)0,0)" "${HEADERS}" HAVE_SYNC_ADD_AND_FETCH)
  CHECK_FUNCTIONWITHHEADER_EXISTS("__sync_sub_and_fetch((int*)0,0)" "${HEADERS}" HAVE_SYNC_SUB_AND_FETCH)
  CHECK_FUNCTIONWITHHEADER_EXISTS("InterlockedIncrement((long*)0)" "${HEADERS}" HAVE_INTERLOCKED_INCREMENT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("InterlockedDecrement((long*)0)" "${HEADERS}" HAVE_INTERLOCKED_DECREMENT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("_fpclassf(0.0f)" "${HEADERS}" HAVE_PROTOTYPE__FPCLASSF)
  CHECK_FUNCTIONWITHHEADER_EXISTS("getgrnam_r((char*)0,(group*)0,(char*)0,0,(group**)0)" "${HEADERS}" HAVE_GETGRNAM_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS("getpwnam_r((char*)0,(passwd*)0,(char*)0,0,(passwd**)0)" "${HEADERS}" HAVE_GETPWNAM_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS("readdir_r((DIR*)0,(dirent*)0,(dirent**)0)" "${HEADERS}" HAVE_READDIR_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS("readdir_r((DIR*)0,(dirent*)0)" "${HEADERS}" HAVE_OLD_READDIR_R)
  CHECK_FUNCTIONWITHHEADER_EXISTS(nanosleep "${HEADERS}" HAVE_PROTOTYPE_NANOSLEEP)
  CHECK_FUNCTIONWITHHEADER_EXISTS("&passwd::pw_gecos" "${HEADERS}" HAVE_PASSWD_GECOS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TryAcquireSRWLockShared((PSRWLOCK)0)" "${HEADERS}" HAVE_PROTOTYPE_TRYACQUIRESRWLOCKSHARED)

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

if(DCMTK_WITH_OPENSSL)
  # Check if OpenSSL provides the SSL_CTX_get0_param function
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_get0_param" "openssl/ssl.h" HAVE_SSL_CTX_GET0_PARAM ${OPENSSL_LIBS})
  CHECK_FUNCTIONWITHHEADER_EXISTS("RAND_egd" "openssl/rand.h" HAVE_RAND_EGD ${OPENSSL_LIBS})
endif()

if(HAVE_LOCKF AND ANDROID)
  # When Android introduced lockf, they forgot to put the constants like F_LOCK in the
  # appropriate headers, this tests if they are defined and disables lockf if they are not
  CHECK_FUNCTIONWITHHEADER_EXISTS("lockf(0, F_LOCK, 0)" "${HEADERS}" HAVE_LOCKF_CONSTANTS)
  if(NOT HAVE_LOCKF_CONSTANTS)
    set(HAVE_LOCKF FALSE CACHE INTERNAL "lockf implementation is broken")
  endif()
endif()

# Tests that require a try-compile

if(HAVE_MATH_H)
  if(HAVE_LIBC_H)
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
    if(INCLUDE_LIBC_H_AS_EXTERN_C)
      set(INCLUDE_LIBC_H_AS_CXX 0 CACHE INTERNAL "libc.h should be treated as a C++ header")
    else()
      set(INCLUDE_LIBC_H_AS_CXX 1 CACHE INTERNAL "libc.h should be treated as a C++ header")
    endif()
  endif()

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
  if(INCLUDE_MATH_H_AS_EXTERN_C)
    set(INCLUDE_MATH_H_AS_CXX 0 CACHE INTERNAL "math.h should be treated as a C++ header")
  else()
    set(INCLUDE_MATH_H_AS_CXX 1 CACHE INTERNAL "math.h should be treated as a C++ header")
  endif()
endif()


if(NOT DEFINED C_CHAR_UNSIGNED)
  message(STATUS "Checking signedness of char")
  DCMTK_TRY_RUN(C_CHAR_SIGNED C_CHAR_SIGNED_COMPILED "${CMAKE_BINARY_DIR}/CMakeTmp/Char"
          "${DCMTK_SOURCE_DIR}/CMake/dcmtkTestCharSignedness.cc"
          COMPILE_OUTPUT_VARIABLE C_CHAR_SIGNED_COMPILE_OUTPUT)
  if(C_CHAR_SIGNED_COMPILED)
    if(C_CHAR_SIGNED)
      message(STATUS "Checking signedness of char -- signed")
      set(C_CHAR_UNSIGNED 0 CACHE INTERNAL "Whether char is unsigned.")
    else()
      message(STATUS "Checking signedness of char -- unsigned")
      set(C_CHAR_UNSIGNED 1 CACHE INTERNAL "Whether char is unsigned.")
    endif()
  else()
    message(STATUS "Checking signedness of char -- failed")
  endif()
endif()

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
if(HAVE_WINDOWS_H)
  set(HAVE_INT_TYPE_PTHREAD_T 1)
else()
  DCMTK_TRY_COMPILE(HAVE_INT_TYPE_PTHREAD_T "pthread_t is an integer type"
        "// test to see if pthread_t is a pointer type or not

#include <pthread.h>

int main ()
{
  pthread_t p;
  unsigned long l = p;
  return 0;
}")
  if(NOT HAVE_INT_TYPE_PTHREAD_T)
    set(HAVE_POINTER_TYPE_PTHREAD_T 1 CACHE INTERNAL "Set if pthread_t is a pointer type")
  else()
    set(HAVE_POINTER_TYPE_PTHREAD_T 0 CACHE INTERNAL "Set if pthread_t is a pointer type")
  endif()
endif()

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
if(HAVE_INT_STRERROR_R)
  set(HAVE_CHARP_STRERROR_R 0 CACHE INTERNAL "Set if strerror_r() returns a char*")
else()
  set(HAVE_CHARP_STRERROR_R 1 CACHE INTERNAL "Set if strerror_r() returns a char*")
endif()

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

# do try compile to detect lfs and flags
function(DCMTK_LFS_TRY_COMPILE VAR FILE FLAGS DEFINITIONS)
  if(FLAGS OR DEFINITIONS)
    set(OPTIONS " with arguments: \"")
  else()
    set(OPTIONS)
  endif()
  if(DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS OR FLAGS)
    set(CMAKE_FLAGS CMAKE_FLAGS ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS})
    if(FLAGS)
      list(APPEND CMAKE_FLAGS "-DCMAKE_C_FLAGS:STRING=${FLAGS}")
      set(OPTIONS "${OPTIONS}${FLAGS}")
    endif()
  else()
    set(CMAKE_FLAGS)
  endif()
  if(DEFINITIONS)
    set(COMPILE_DEFINITIONS COMPILE_DEFINITIONS ${DEFINITIONS})
    if(FLAGS)
      set(OPTIONS "${OPTIONS} ")
    endif()
    set(OPTIONS "${OPTIONS}${DEFINITIONS}")
  else()
    set(COMPILE_DEFINITIONS)
  endif()
  if(FLAGS OR DEFINITIONS)
    set(OPTIONS "${OPTIONS}\"")
  endif()
  set(SOURCEFILE "${DCMTK_SOURCE_DIR}/config/tests/${FILE}")
  try_compile(RESULT
        "${CMAKE_BINARY_DIR}"
        "${SOURCEFILE}"
        ${CMAKE_FLAGS}
        ${COMPILE_DEFINITIONS}
        OUTPUT_VARIABLE OUTPUT
  )
  set("${VAR}" "${RESULT}" PARENT_SCOPE)
  if(RESULT)
    set(LOGFILE "CMakeOutput.log")
    set(LOG "succeded")
  else()
    set(LOGFILE "CMakeError.log")
    set(LOG "failed")
  endif()
  file(TO_NATIVE_PATH "${SOURCEFILE}" SOURCEFILE)
  file(APPEND "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${LOGFILE}"
    "compiling \"${SOURCEFILE}\"${OPTIONS} ${LOG} with the following output:\n"
    "${OUTPUT}\n"
  )
endfunction()

function(DCMTK_CHECK_ENABLE_LFS)
  # normalize arguments
  if(DCMTK_ENABLE_LFS)
    # for cases insensitive comparison
    string(TOLOWER "${DCMTK_ENABLE_LFS}" DCMTK_ENABLE_LFS)
  endif()
  if(NOT DCMTK_ENABLE_LFS OR DCMTK_ENABLE_LFS MATCHES "^(on|true|yes|1)$")
    set(DCMTK_ENABLE_LFS "auto")
  elseif(DCMTK_ENABLE_LFS MATCHES "^(no|false|0)$")
    set(DCMTK_ENABLE_LFS "off")
  endif()
  # determin whether lfs64 is available in case it wasn't detected yet it may be used
  if(NOT DEFINED DCMTK_LFS64_AVAILABLE AND DCMTK_ENABLE_LFS MATCHES "^(lfs64|auto)$")
    set(DCMTK_LFS64_DEFINITIONS)
    set(MESSAGE_RESULT "no")
    set(MESSAGE "Checking whether explicit large file support (LFS64) is available")
    message(STATUS "${MESSAGE}")
    DCMTK_LFS_TRY_COMPILE(RESULT "lfs64.cc" "" "")
    if(NOT RESULT)
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs64.cc" "" "-D_LARGEFILE64_SOURCE")
      if(RESULT)
        set(DCMTK_LFS64_DEFINITIONS "-D_LARGEFILE64_SOURCE")
        set(MESSAGE_RESULT "yes, with ${DCMTK_LFS64_DEFINITIONS}")
      endif()
    else()
      set(MESSAGE_RESULT "yes")
    endif()
    if(RESULT)
      set(DCMTK_ENABLE_LFS "lfs64")
      set(DCMTK_LFS64_DEFINITIONS "${DCMTK_LFS64_DEFINITIONS}" CACHE INTERNAL "which compiler definitions to set for enabling LFS64 support")
    endif()
    set(DCMTK_LFS64_AVAILABLE "${RESULT}" CACHE INTERNAL "whether LFS64 is available or not" FORCE)
    message(STATUS "${MESSAGE} -- ${MESSAGE_RESULT}")
  endif()
  # determin whether lfs is available in case it wasn't detected yet it may be used
  if(NOT DEFINED DCMTK_LFS_AVAILABLE AND DCMTK_ENABLE_LFS MATCHES "^(lfs|auto)$")
    set(DCMTK_LFS_FLAGS)
    set(DCMTK_LFS_DEFINITIONS)
    set(MESSAGE_RESULT "no")
    set(MESSAGE "Checking whether large file support (LFS) is available")
    message(STATUS "${MESSAGE}")
    # determine size of fpos_t (for the strange LFS implementation on Windows)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    if(HAVE_STDIO_H)
      # prefer stdio.h so that is not in namespace std
      set(CMAKE_EXTRA_INCLUDE_FILES "stdio.h")
    elseif(HAVE_CSTDIO)
      # use cstdio as the fallback
      set(CMAKE_EXTRA_INCLUDE_FILES "cstdio")
    endif()
    CHECK_TYPE_SIZE("fpos_t" SIZEOF_FPOS_T)
    # assume sizeof off_t to be correct, will be removed if below tests fail
    set(SIZEOF_OFF_T 8)
    # try compile different combinations of compiler flags and definitions
    DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "" "")
    if(NOT RESULT)
      set(DCMTK_LFS_FLAGS "-n32")
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "-n32" "")
    endif()
    if(NOT RESULT)
      set(DCMTK_LFS_FLAGS "")
      set(DCMTK_LFS_DEFINITIONS "-D_FILE_OFFSET_BITS=64")
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "" "-D_FILE_OFFSET_BITS=64")
    endif()
    if(NOT RESULT)
      set(DCMTK_LFS_FLAGS "-n32")
      set(DCMTK_LFS_DEFINITIONS "-D_FILE_OFFSET_BITS=64")
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "-n32" "-D_FILE_OFFSET_BITS=64")
    endif()
    if(NOT RESULT)
      set(DCMTK_LFS_FLAGS "")
      set(DCMTK_LFS_DEFINITIONS "-D_LARGE_FILES=1")
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "" "-D_LARGE_FILES=1")
    endif()
    if(NOT RESULT)
      set(DCMTK_LFS_FLAGS "-n32")
      set(DCMTK_LFS_DEFINITIONS "-D_LARGE_FILES=1")
      DCMTK_LFS_TRY_COMPILE(RESULT "lfs.c" "-n32" "-D_LARGE_FILES=1")
    endif()
    if(NOT RESULT)
      # remove flags and reset SIZEOF_OFF_T to indeterminate
      set(DCMTK_LFS_FLAGS)
      set(DCMTK_LFS_DEFINITIONS)
      set(SIZEOF_OFF_T)
      # detect strange LFS implementation that (at least) Windows provides
      # strange since sizeof(fpos_t) == 8 but sizeof(off_t) == 4!
      if(SIZEOF_FPOS_T EQUAL 8)
        set(RESULT TRUE)
      endif()
    endif()
    # format a nice result message
    if(RESULT)
      set(DCMTK_ENABLE_LFS "lfs")
      set(DCMTK_LFS_FLAGS "${DCMTK_LFS_FLAGS}" CACHE INTERNAL "which compiler flags to set for enabling LFS support")
      set(DCMTK_LFS_DEFINITIONS "${DCMTK_LFS_DEFINITIONS}" CACHE INTERNAL "which compiler definitions to set for enabling LFS support")
      set(MESSAGE_RESULT "yes")
      if(NOT DCMTK_LFS_FLAGS STREQUAL "" OR NOT DCMTK_LFS_DEFINITIONS STREQUAL "")
        set(MESSAGE_RESULT "${MESSAGE_RESULT}, with")
        if(NOT DCMTK_LFS_FLAGS STREQUAL "")
          set(MESSAGE_RESULT "${MESSAGE_RESULT} ${DCMTK_LFS_FLAGS}")
        endif()
        if(NOT DCMTK_LFS_DEFINITIONS STREQUAL "")
          set(MESSAGE_RESULT "${MESSAGE_RESULT} ${DCMTK_LFS_DEFINITIONS}")
        endif()
      endif()
    endif()
    set(DCMTK_LFS_AVAILABLE "${RESULT}" CACHE INTERNAL "whether LFS is available or not" FORCE)
    if(DEFINED SIZEOF_OFF_T)
      set(SIZEOF_OFF_T "${SIZEOF_OFF_T}" CACHE INTERNAL "")
    endif()
    message(STATUS "${MESSAGE} -- ${MESSAGE_RESULT}")
  endif()
  # auto-select LFS implementation in case this is not the first run and the above tests did not select it
  if(DCMTK_ENABLE_LFS STREQUAL "auto")
    if(DCMTK_LFS64_AVAILABLE)
      set(DCMTK_ENABLE_LFS "lfs64")
    elseif(DCMTK_LFS_AVAILABLE)
      set(DCMTK_ENABLE_LFS "lfs")
    else()
      set(DCMTK_ENABLE_LFS "off")
    endif()
  elseif(NOT DCMTK_ENABLE_LFS MATCHES "^(lfs|lfs64|off)$")
    # print a warning in case the given argument was not understood
    message(WARNING "unknown argument \"${DCMTK_ENABLE_LFS}\" for DCMTK_ENABLE_LFS, setting it to \"off\"")
    set(DCMTK_ENABLE_LFS "off")
  elseif(DCMTK_ENABLE_LFS STREQUAL "lfs64" AND NOT DCMTK_LFS64_AVAILABLE)
    # test if the explicitly chosen implementation is really available
    message(WARNING "LFS64 was enabled but LFS64 support is not available, focing DCMTK_ENABLE_LFS to \"off\"")
    set(DCMTK_ENABLE_LFS "off")
  elseif(DCMTK_ENABLE_LFS STREQUAL "lfs" AND NOT DCMTK_LFS_AVAILABLE)
    # test if the explicitly chosen implementation is really available
    message(WARNING "LFS was enabled but LFS support is not available, focing DCMTK_ENABLE_LFS to \"off\"")
    set(DCMTK_ENABLE_LFS "off")
  endif()
  # create a list of available LFS types for the CMake GUI
  set(AVAILABLE_LFS_TYPES)
  if(NOT DEFINED DCMTK_LFS64_AVAILABLE OR DCMTK_LFS64_AVAILABLE)
    list(APPEND AVAILABLE_LFS_TYPES "lfs64")
  endif()
  if(NOT DEFINED DCMTK_LFS_AVAILABLE OR DCMTK_LFS_AVAILABLE)
    list(APPEND AVAILABLE_LFS_TYPES "lfs")
  endif()
  # store the chosen value to the cache (potentially normalizing the given argument)
  set(DCMTK_ENABLE_LFS "${DCMTK_ENABLE_LFS}" CACHE STRING "whether to use lfs/lfs64 or not" FORCE)
  set_property(CACHE DCMTK_ENABLE_LFS  PROPERTY STRINGS "auto" ${AVAILABLE_LFS_TYPES} "off")
  # set values for osconfig.h and add compiler flags and definitions (if necessary)
  if(DCMTK_ENABLE_LFS STREQUAL "lfs64")
    # set the value for generating osconfig.h
    set(DCMTK_LFS_MODE "DCMTK_LFS64" CACHE INTERNAL "" FORCE)
    if(NOT DCMTK_LFS64_DEFINITIONS STREQUAL "")
      add_definitions(${DCMTK_LFS64_DEFINITIONS})
    endif()
    message(STATUS "Info: Building DCMTK with explicit large file support (LFS64)")
  elseif(DCMTK_ENABLE_LFS STREQUAL "lfs")
    # set the value for generating osconfig.h
    set(DCMTK_LFS_MODE "DCMTK_LFS" CACHE INTERNAL "" FORCE)
    if(NOT DCMTK_LFS_FLAGS STREQUAL "")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DCMTK_LFS_FLAGS}")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DCMTK_LFS_FLAGS}")
    endif()
    if(NOT DCMTK_LFS_DEFINITIONS STREQUAL "")
      add_definitions(${DCMTK_LFS_DEFINITIONS})
    endif()
    message(STATUS "Info: Building DCMTK with large file support (LFS)")
  else()
    set(DCMTK_ENABLE_LFS)
    message(STATUS "Info: Building DCMTK without large file support, files >4GB may be inaccesible!")
  endif()
endfunction()

DCMTK_CHECK_ENABLE_LFS()

if(WIN32)
  # If someone can tell me how to convince TRY_COMPILE to link against winsock,
  # we could use tests for these. Until then, here is what would be the result:
  set(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  set(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  set(HAVE_INTP_SELECT 0 CACHE INTERNAL "Set if select() accepts an int* argument")
else()
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
  if(HAVE_INTP_SOCKET)
    set(HAVE_INTP_ACCEPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
    set(HAVE_INTP_GETSOCKOPT 1 CACHE INTERNAL "Set if socket functions accept an int* argument")
  else()
    set(HAVE_INTP_ACCEPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
    set(HAVE_INTP_GETSOCKOPT 0 CACHE INTERNAL "Set if socket functions accept an int* argument")
  endif()

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
endif()

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

DCMTK_TRY_COMPILE(HAVE_STATIC_ASSERT "the compiler supports static_assert"
    "#include <cassert>
int main()
{
    static_assert(true, \"good\");
    return 0;
}")

function(ANALYZE_ICONV_FLAGS)
    if(DCMTK_WITH_ICONV OR DCMTK_WITH_STDLIBC_ICONV)
        set(TEXT "Detecting fixed iconv conversion flags")
        message(STATUS "${TEXT}")
        set(EXTRA_ARGS)
        if(NOT DCMTK_WITH_STDLIBC_ICONV)
            list(APPEND EXTRA_ARGS
                CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${LIBICONV_INCDIR}"
                LINK_LIBRARIES ${LIBICONV_LIBS}
            )
        endif()
        if(LIBICONV_SECOND_ARGUMENT_CONST)
            list(APPEND EXTRA_ARGS
                COMPILE_DEFINITIONS "-DLIBICONV_SECOND_ARGUMENT_CONST=${LIBICONV_SECOND_ARGUMENT_CONST}"
            )
        endif()
        DCMTK_TRY_RUN(RUN_RESULT COMPILE_RESULT
            "${CMAKE_BINARY_DIR}/CMakeTmp/Iconv"
            "${DCMTK_SOURCE_DIR}/config/tests/iconv.cc"
            ${EXTRA_ARGS}
            COMPILE_OUTPUT_VARIABLE CERR
            RUN_OUTPUT_VARIABLE OUTPUT
        )
        if(COMPILE_RESULT)
            set(DCMTK_ICONV_FLAGS_ANALYZED TRUE CACHE INTERNAL "")
            if(RUN_RESULT EQUAL 0)
                message(STATUS "${TEXT} - ${OUTPUT}")
                set(DCMTK_FIXED_ICONV_CONVERSION_FLAGS "${OUTPUT}" CACHE INTERNAL "")
            else()
                message(STATUS "${TEXT} - unknown")
            endif()
        else()
            message(FATAL_ERROR "${CERR}")
        endif()
    endif()
endfunction()

if(NOT DCMTK_ICONV_FLAGS_ANALYZED)
    analyze_iconv_flags()
endif()

function(ANALYZE_STDLIBC_ICONV_DEFAULT_ENCODING)
    if(DCMTK_WITH_STDLIBC_ICONV)
        set(TEXT "Checking whether iconv_open() accepts \"\" as an argument")
        message(STATUS "${TEXT}")
        set(EXTRA_ARGS)
        DCMTK_TRY_RUN(RUN_RESULT COMPILE_RESULT
            "${CMAKE_BINARY_DIR}/CMakeTmp/lciconv"
            "${DCMTK_SOURCE_DIR}/config/tests/lciconv.cc"
            COMPILE_OUTPUT_VARIABLE CERR
        )
        if(COMPILE_RESULT)
            if(RUN_RESULT EQUAL 0)
                message(STATUS "${TEXT} - yes")
                set(DCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING 1 CACHE INTERNAL "")
            else()
                message(STATUS "${TEXT} - no")
                set(DCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING CACHE INTERNAL "")
            endif()
        else()
            message(FATAL_ERROR "${CERR}")
        endif()
    endif()
endfunction()

if(NOT DEFINED DCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING)
    analyze_stdlibc_iconv_default_encoding()
endif()

if(HAVE_PASSWD_GECOS AND NOT DEFINED PASSWD_GECOS_IS_DEFINE_TO_PASSWD)
    DCMTK_TRY_COMPILE(PASSWD_GECOS_IS_DEFINE_TO_PASSWD "pw_gecos is #defined to pw_passwd"
        "#include <pwd.h>
int main()
{
    struct S { int pw_passwd; };
    &S::pw_gecos;
    return 0;
}")
    if(PASSWD_GECOS_IS_DEFINE_TO_PASSWD)
        set(HAVE_PASSWD_GECOS 0 CACHE INTERNAL "Have symbol &passwd::pw_gecos")
    endif()
endif()

# Compile config/tests/arith.cc and generate config/arith.h
function(INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES)
  set(ARITH_H_FILE "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/arith.h")
  if("${DCMTK_SOURCE_DIR}/config/tests/arith.cc" IS_NEWER_THAN "${ARITH_H_FILE}")
    if(CMAKE_CROSSCOMPILING)
      if(WIN32)
        UNIX_TO_WINE_PATH(ARITH_H_FILE "${ARITH_H_FILE}")
        string(REPLACE "\\" "\\\\" ARITH_H_FILE "${ARITH_H_FILE}")
      elseif(ANDROID)
        set(ARITH_H_DESTINATION "${ARITH_H_FILE}")
        set(ARITH_H_FILE "${ANDROID_TEMPORARY_FILES_LOCATION}/arith.h")
      endif()
    endif()
    DCMTK_TRY_RUN(
      RESULT COMPILED
      "${DCMTK_BINARY_DIR}/CMakeTmp/Arith"
      "${DCMTK_SOURCE_DIR}/config/tests/arith.cc"
      COMPILE_DEFINITIONS -I"${DCMTK_BINARY_DIR}/config/include" -I"${DCMTK_SOURCE_DIR}/ofstd/include" -I"${DCMTK_SOURCE_DIR}/ofstd/libsrc"
      RUN_OUTPUT_VARIABLE OUTPUT
      COMPILE_OUTPUT_VARIABLE CERR
      ARGS "\\\"${ARITH_H_FILE}\\\""
    )
    if(COMPILED)
      if(NOT RESULT)
        message(STATUS "${OUTPUT}")
        if(CMAKE_CROSSCOMPILING)
          if(ANDROID)
            DCMTK_ANDROID_PULL(DCMTK_ANDROID_EMULATOR_INSTANCE "${ARITH_H_FILE}" DESTINATION "${ARITH_H_DESTINATION}")
          endif()
        endif()
      else()
        message(FATAL_ERROR "${OUTPUT}")
      endif()
    else()
      message(FATAL_ERROR "${CERR}")
    endif()
  endif() # file needs update
endfunction()

function(DCMTK_CHECK_CXX_STANDARD STANDARD)
  set(RESULT 0)
  if(DEFINED HAVE_CXX${STANDARD}_TEST_RESULT)
    if(HAVE_CXX${STANDARD}_TEST_RESULT)
      set(RESULT 1)
    endif()
  else()
    set(MESSAGE "Checking whether the compiler supports C++${STANDARD}")
    message(STATUS "${MESSAGE}")
    try_compile(COMPILE_RESULT "${CMAKE_BINARY_DIR}" "${DCMTK_SOURCE_DIR}/config/tests/cxx${STANDARD}.cc")
    set(HAVE_CXX${STANDARD}_TEST_RESULT "${COMPILE_RESULT}" CACHE INTERNAL "Caches the configuration test result for C++${STANDARD} support.")
    if(COMPILE_RESULT)
      set(RESULT 1)
      message(STATUS "${MESSAGE} -- yes")
    else()
      message(STATUS "${MESSAGE} -- no")
    endif()
  endif()
  set("ENABLE_CXX${STANDARD}" "${RESULT}" PARENT_SCOPE)
endfunction()

function(DCMTK_TEST_ENABLE_CXX11)
  get_property(MODERN_CXX_STANDARDS GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARDS)
  foreach(STANDARD ${MODERN_CXX_STANDARDS})
    set(ENABLE_CXX${STANDARD} 0)
  endforeach()
  get_property(DCMTK_CMAKE_HAS_CXX_STANDARD GLOBAL PROPERTY DCMTK_CMAKE_HAS_CXX_STANDARD)
  if(DCMTK_CMAKE_HAS_CXX_STANDARD)
    get_property(MODERN_CXX_STANDARD GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARD)
    if(MODERN_CXX_STANDARD AND DCMTK_ENABLE_CXX11)
      dcmtk_upper_bound(MODERN_CXX_STANDARDS "${CMAKE_CXX_STANDARD}" N)
      math(EXPR N "${N}-1")
      foreach(I RANGE ${N})
        list(GET MODERN_CXX_STANDARDS ${I} STANDARD)
        dcmtk_check_cxx_standard("${STANDARD}")
        if(NOT ENABLE_CXX${STANDARD})
          break()
        endif()
      endforeach()
    endif()
  elseif(DCMTK_ENABLE_CXX11 AND NOT DCMTK_ENABLE_CXX11 STREQUAL "INFERRED")
    # set C++11 compiler flags for the test, will automatically be removed by the function scope
    string(FIND "${CMAKE_CXX_FLAGS}" "${DCMTK_CXX11_FLAGS}" INDEX)
    if(INDEX EQUAL -1)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DCMTK_CXX11_FLAGS}" PARENT_SCOPE)
    endif()
    dcmtk_check_cxx_standard(11)
    if(ENABLE_CXX11)
      # push C++11 CXX-flags to the parent scope
      set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
    endif()
  endif()
  foreach(STANDARD ${MODERN_CXX_STANDARDS})
    set(HAVE_CXX${STANDARD} "${ENABLE_CXX${STANDARD}}" CACHE INTERNAL "Set to 1 if the compiler supports C++${STANDARD} and it should be enabled.")
    if(HAVE_CXX${STANDARD})
      message(STATUS "Info: C++${STANDARD} features enabled")
    else()
      message(STATUS "Info: C++${STANDARD} features disabled")
    endif()
  endforeach()
endfunction()

function(DCMTK_TEST_ENABLE_STL_FEATURE NAME)
  string(TOUPPER "${NAME}" FEATURE)
  if(ARGN)
    set(SOURCEFILE "${ARGN}")
  else()
    set(SOURCEFILE "${NAME}")
  endif()
  if(DCMTK_ENABLE_STL_${FEATURE} STREQUAL "INFERRED")
    set(DCMTK_ENABLE_STL_${FEATURE} ${DCMTK_ENABLE_STL})
  endif()
  set(RESULT 0)
  set(TEXT_RESULT "disabled")
  if(DCMTK_ENABLE_STL_${FEATURE} STREQUAL "ON")
    if(DEFINED HAVE_STL_${FEATURE}_TEST_RESULT)
      if(HAVE_STL_${FEATURE}_TEST_RESULT)
        set(RESULT 1)
        set(TEXT_RESULT "enabled")
      endif()
    else()
      set(MESSAGE "Checking whether STL ${NAME} works correctly")
      message(STATUS "${MESSAGE}")
      DCMTK_TRY_RUN(RUN_RESULT COMPILE_RESULT "${CMAKE_BINARY_DIR}" "${DCMTK_SOURCE_DIR}/config/tests/${SOURCEFILE}.cc")
      if(COMPILE_RESULT AND RUN_RESULT EQUAL 0)
        set(RESULT 1)
        set(TEXT_RESULT "enabled")
        message(STATUS "${MESSAGE} -- yes")
      else()
        message(STATUS "${MESSAGE} -- no")
      endif()
      set(HAVE_STL_${FEATURE}_TEST_RESULT ${RESULT} CACHE INTERNAL "Caches the configuration test result for STL ${NAME}")
    endif()
  endif()
  set(HAVE_STL_${FEATURE} ${RESULT} CACHE INTERNAL "Set to 1 if the compiler/OS provides a working STL ${NAME} implementation.")
  message(STATUS "Info: STL ${NAME} support ${TEXT_RESULT}")
endfunction()

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

if(CMAKE_CROSSCOMPILING)
  set(DCMTK_CROSS_COMPILING ${CMAKE_CROSSCOMPILING})
endif()
