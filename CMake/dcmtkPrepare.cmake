# Set up the build environment.
# This should be run before the individual modules are created.

# This file should only run once
IF(DEFINED DCMTK_CONFIGURATION_DONE)
  RETURN()
ENDIF(DEFINED DCMTK_CONFIGURATION_DONE)
SET(DCMTK_CONFIGURATION_DONE true)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.11)
  SET(CMAKE_BACKWARDS_COMPATIBILITY 2.8.11 CACHE STRING "Latest version of CMake when this project was released." FORCE)
ENDIF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.11)

# Basic version information
# (Starting with version 3.5.5, an odd number at the last position indicates
#  a development snapshot and an even number indicates an official release.)
SET(DCMTK_MAJOR_VERSION 3)
SET(DCMTK_MINOR_VERSION 6)
SET(DCMTK_BUILD_VERSION 1)
# The ABI is not guaranteed to be stable between different snapshots/releases,
# so this particular version number is increased for each snapshot or release.
SET(DCMTK_ABI_VERSION 3)

# Package "release" settings (some are currently unused and, therefore, disabled)
SET(DCMTK_PACKAGE_NAME "dcmtk")
SET(DCMTK_PACKAGE_DATE "DEV")
SET(DCMTK_PACKAGE_VERSION "${DCMTK_MAJOR_VERSION}.${DCMTK_MINOR_VERSION}.${DCMTK_BUILD_VERSION}")
SET(DCMTK_PACKAGE_VERSION_NUMBER ${DCMTK_MAJOR_VERSION}${DCMTK_MINOR_VERSION}${DCMTK_BUILD_VERSION})
SET(DCMTK_PACKAGE_VERSION_SUFFIX "")
#SET(DCMTK_PACKAGE_TARNAME "dcmtk-${DCMTK_PACKAGE_VERSION}")
#SET(DCMTK_PACKAGE_STRING "dcmtk ${DCMTK_PACKAGE_VERSION}")
#SET(DCMTK_PACKAGE_BUGREPORT "dicom-bugs@offis.de")

# Shared library version information
SET(DCMTK_LIBRARY_PROPERTIES VERSION "${DCMTK_PACKAGE_VERSION}" SOVERSION "${DCMTK_ABI_VERSION}")

# General build options and settings
OPTION(BUILD_APPS "Build command line applications and test programs." ON)
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF)
OPTION(BUILD_SINGLE_SHARED_LIBRARY "Build a single DCMTK library." OFF)
MARK_AS_ADVANCED(BUILD_SINGLE_SHARED_LIBRARY)
SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Library postfix for debug builds. Usually left blank.")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/${DCMTK_CMAKE_INCLUDE}/CMake/")
IF(BUILD_SINGLE_SHARED_LIBRARY)
  # When we are building a single shared lib, we are building shared libs :-)
  SET(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
ENDIF(BUILD_SINGLE_SHARED_LIBRARY)

# DCMTK build options
OPTION(DCMTK_WITH_TIFF "Configure DCMTK with support for TIFF." ON)
OPTION(DCMTK_WITH_PNG "Configure DCMTK with support for PNG." ON)
OPTION(DCMTK_WITH_XML "Configure DCMTK with support for XML." ON)
OPTION(DCMTK_WITH_ZLIB "Configure DCMTK with support for ZLIB." ON)
OPTION(DCMTK_WITH_OPENSSL "Configure DCMTK with support for OPENSSL." ON)
OPTION(DCMTK_WITH_SNDFILE "Configure DCMTK with support for SNDFILE." ON)
OPTION(DCMTK_WITH_ICONV "Configure DCMTK with support for ICONV." ON)
IF(NOT WIN32)
  OPTION(DCMTK_WITH_WRAP "Configure DCMTK with support for WRAP." ON)
ENDIF(NOT WIN32)
OPTION(DCMTK_WITH_PRIVATE_TAGS "Configure DCMTK with support for DICOM private tags coming with DCMTK." OFF)
OPTION(DCMTK_WITH_THREADS "Configure DCMTK with support for multi-threading." ON)
OPTION(DCMTK_WITH_DOXYGEN "Build API documentation with DOXYGEN." ON)
OPTION(DCMTK_GENERATE_DOXYGEN_TAGFILE "Generate a tag file with DOXYGEN." OFF)
OPTION(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS "Build with wide char file I/O functions." OFF)
OPTION(DCMTK_WIDE_CHAR_MAIN_FUNCTION "Build command line tools with wide char main function." OFF)

# Mark various settings as "advanced"
MARK_AS_ADVANCED(CMAKE_DEBUG_POSTFIX)
MARK_AS_ADVANCED(FORCE EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)
MARK_AS_ADVANCED(SNDFILE_DIR DCMTK_WITH_SNDFILE) # not yet needed in public DCMTK
MARK_AS_ADVANCED(DCMTK_GENERATE_DOXYGEN_TAGFILE)
IF(NOT WIN32)
  # support for wide char file I/O functions is currently Windows-specific
  MARK_AS_ADVANCED(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  # support for wide char main function is Windows-specific
  MARK_AS_ADVANCED(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
ENDIF(NOT WIN32)

ENABLE_TESTING()

#-----------------------------------------------------------------------------
# Installation sub-directories
#-----------------------------------------------------------------------------

SET(DCMTK_INSTALL_BINDIR "bin" CACHE STRING "Installation sub-directory for binary executables.")
SET(DCMTK_INSTALL_INCDIR "include" CACHE STRING "Installation sub-directory for header files.")
SET(DCMTK_INSTALL_LIBDIR "lib" CACHE STRING "Installation sub-directory for object code libraries.")
SET(DCMTK_INSTALL_ETCDIR "etc/dcmtk" CACHE STRING "Installation sub-directory for configuration files.")
SET(DCMTK_INSTALL_DATDIR "share/dcmtk" CACHE STRING "Installation sub-directory for sample files and the like.")
SET(DCMTK_INSTALL_DOCDIR "share/doc/dcmtk" CACHE STRING "Installation sub-directory for general documentation.")
SET(DCMTK_INSTALL_HTMDIR "share/doc/dcmtk/html" CACHE STRING "Installation sub-directory for HTML documentation.")
SET(DCMTK_INSTALL_MANDIR "share/man" CACHE STRING "Installation sub-directory for man pages.")

MARK_AS_ADVANCED(DCMTK_INSTALL_BINDIR DCMTK_INSTALL_INCDIR DCMTK_INSTALL_LIBDIR DCMTK_INSTALL_ETCDIR
                 DCMTK_INSTALL_DATDIR DCMTK_INSTALL_DOCDIR DCMTK_INSTALL_HTMDIR DCMTK_INSTALL_MANDIR)

#-----------------------------------------------------------------------------
# Platform-independent settings
#-----------------------------------------------------------------------------

IF(COMMAND CMAKE_POLICY)
  # Enable new CMake behaviour when dealing with libraries whose full path is
  # given to the linker. This is necessary to avoid warnings in CMake versions
  # greater than 2.6.
  # See http://www.cmake.org/cmake/help/cmake-2.6.html#policy:CMP0003
  CMAKE_POLICY(SET CMP0003 NEW)

  # Works around warnings about escaped quotes in ADD_DEFINITIONS statements
  CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(COMMAND CMAKE_POLICY)

# pass optional build date to compiler
#ADD_DEFINITIONS("-DDCMTK_BUILD_DATE=\\\"YYYY-MM-DD\\\"")

# make OFString(NULL) safe by default
ADD_DEFINITIONS("-DUSE_NULL_SAFE_OFSTRING")

# tell the DCMTK that we are building the DCMTK
ADD_DEFINITIONS("-DDCMTK_BUILD_IN_PROGRESS")

# build output files in these directories
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#-----------------------------------------------------------------------------
# Platform-specific settings
#-----------------------------------------------------------------------------

# set project wide flags for compiler and linker

IF(WIN32)
  OPTION(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS "Overwrite compiler flags with DCMTK's WIN32 package default values." ON)
ELSE(WIN32)
  SET(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS OFF)
ENDIF(WIN32)

IF(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS AND NOT BUILD_SHARED_LIBS)

  # settings for Microsoft Visual Studio
  IF(CMAKE_GENERATOR MATCHES "Visual Studio .*")
    # get Visual Studio Version
    STRING(REGEX REPLACE "Visual Studio ([0-9]+).*" "\\1" VS_VERSION "${CMAKE_GENERATOR}")
    # these settings never change even for C or C++
    SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
    SET(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /MT /O2")
    SET(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /MT /O2")
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /MTd /Z7 /Od")
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
    SET(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /MT /O2")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "/DNDEBUG /MT /O2")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/DNDEBUG /MTd /Z7 /Od")
    # specific settings for the various Visual Studio versions
    IF(VS_VERSION EQUAL 6)
      SET(CMAKE_C_FLAGS "/nologo /W3 /GX /Gy /YX")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /GX /Gy /YX /Zm500") # /Zm500 increments heap size which is needed on some system to compile templates in dcmimgle
    ENDIF(VS_VERSION EQUAL 6)
    IF(VS_VERSION EQUAL 7)
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
    ENDIF(VS_VERSION EQUAL 7)
    IF(VS_VERSION GREATER 7)
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy /EHsc")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy /EHsc")
    ENDIF(VS_VERSION GREATER 7)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio .*")

  # settings for Borland C++
  IF(CMAKE_GENERATOR MATCHES "Borland Makefiles")
    # further settings required? not tested for a very long time!
    SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  ENDIF(CMAKE_GENERATOR MATCHES "Borland Makefiles")

ENDIF(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS AND NOT BUILD_SHARED_LIBS)

IF(BUILD_SHARED_LIBS)
  SET(DCMTK_SHARED ON)
  IF(BUILD_SINGLE_SHARED_LIBRARY)
    # We can't build apps, because there is no way to tell CMake to link apps
    # against the library.
    SET(BUILD_APPS OFF CACHE BOOL "" FORCE)
    # We are building static code that can be used in a shared lib
    SET(DCMTK_BUILD_SINGLE_SHARED_LIBRARY ON)
    # Make CMake build object libraries. They are just a list of object files
    # which aren't linked together yet.
    SET(DCMTK_LIBRARY_TYPE OBJECT)
    # Static and shared libraries can have dependencies in CMake. Object
    # libraries cannot. Since CMake saves dependencies in its cache, we have to
    # make sure that it doesn't get confused when a "normal" library turns into
    # an object library. Do this via a suffix.
    SET(DCMTK_LIBRARY_SUFFIX _obj)
    # This uses object libraries which are new in CMake 2.8.8
    CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
  ENDIF(BUILD_SINGLE_SHARED_LIBRARY)

  OPTION(USE_COMPILER_HIDDEN_VISIBILITY
      "Use hidden visibility support if available" ON)
  MARK_AS_ADVANCED(USE_COMPILER_HIDDEN_VISIBILITY)

  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" GXX_SUPPORTS_VISIBILITY)
  # This "NOT WIN32" is needed due to a CMake bug that was fixed in
  # CMake 2.8.x. CHECK_CXX_COMPILER_FLAG() always says "ok" with MSC.
  IF(GXX_SUPPORTS_VISIBILITY AND USE_COMPILER_HIDDEN_VISIBILITY AND NOT WIN32)
    SET(HAVE_HIDDEN_VISIBILITY ON)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
  ELSE(GXX_SUPPORTS_VISIBILITY AND USE_COMPILER_HIDDEN_VISIBILITY AND NOT WIN32)
    SET(HAVE_HIDDEN_VISIBILITY)
  ENDIF(GXX_SUPPORTS_VISIBILITY AND USE_COMPILER_HIDDEN_VISIBILITY AND NOT WIN32)
ENDIF(BUILD_SHARED_LIBS)

IF(WIN32)   # special handling for Windows systems

  IF(MINGW)
    # Avoid auto-importing warnings on MinGW
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
  ELSE(MINGW)
    # On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security warnings.
    IF(NOT BORLAND)
      IF(NOT CYGWIN)
        IF(NOT DCMTK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
          ADD_DEFINITIONS(
            -D_CRT_FAR_MAPPINGS_NO_DEPRECATE
            -D_CRT_IS_WCTYPE_NO_DEPRECATE
            -D_CRT_MANAGED_FP_NO_DEPRECATE
            -D_CRT_NONSTDC_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE
            -D_CRT_SECURE_NO_DEPRECATE_GLOBALS
            -D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
            -D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
            -D_CRT_VCCLRIT_NO_DEPRECATE
            -D_SCL_SECURE_NO_DEPRECATE
            )
        ENDIF(NOT DCMTK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
      ENDIF(NOT CYGWIN)
    ENDIF(NOT BORLAND)
  ENDIF(MINGW)

ELSE(WIN32)   # ... for non-Windows systems

  # Compiler flags for Mac OS X
  IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_DARWIN_C_SOURCE")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_DARWIN_C_SOURCE")
  ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # FreeBSD fails with these flags
    IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
      # Compiler flags for all other non-Windows systems
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
    ENDIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  OPTION(DCMTK_FORCE_FPIC_ON_UNIX "Add -fPIC compiler flag on unix 64 bit machines to allow linking from dynamic libraries even if DCMTK is built statically" OFF)
  MARK_AS_ADVANCED(DCMTK_FORCE_FPIC_ON_UNIX)

  # Setting for IA64 / x86_64 which needs -fPIC compiler flag required for shared library build on these platforms
  IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND UNIX AND DCMTK_FORCE_FPIC_ON_UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND UNIX AND DCMTK_FORCE_FPIC_ON_UNIX)

ENDIF(WIN32)

# define libraries and object files that must be linked to most Windows applications
IF(WIN32)
  SET(WIN32_STD_LIBRARIES ws2_32 netapi32 wsock32)
  IF(NOT DEFINED MINGW)
    # additional object file needed for wildcard expansion; for wchar_t* support, use 'wsetargv'
    SET(WIN32_STD_OBJECTS setargv)
  ENDIF(NOT DEFINED MINGW)
  # settings for Borland C++
  IF(CMAKE_CXX_COMPILER MATCHES bcc32)
    # to be checked: further settings required?
    SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  ENDIF(CMAKE_CXX_COMPILER MATCHES bcc32)
ENDIF(WIN32)

# add definition of "DEBUG" to debug mode (since CMake does not do this automatically)
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

#-----------------------------------------------------------------------------
# Third party libraries
#-----------------------------------------------------------------------------

INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/3rdparty.cmake)

#-----------------------------------------------------------------------------
# DCMTK libraries
#-----------------------------------------------------------------------------

INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/dcmtkMacros.cmake)
INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/GenerateDCMTKConfigure.cmake)

#-----------------------------------------------------------------------------
# Dart configuration (disabled per default)
#-----------------------------------------------------------------------------

# Includes build targets Experimental, Nightly and Continuous which are the standard
# groups pre-configured in Dashboard. In CTest these groups are called "Tracks".
#
# INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
# IF(BUILD_TESTING)
#   ENABLE_TESTING()
# ENDIF(BUILD_TESTING)

#-----------------------------------------------------------------------------
# Thread support
#-----------------------------------------------------------------------------

# See dcmtk/config/configure.in
IF(WITH_THREADS)
  ADD_DEFINITIONS(-D_REENTRANT)
  IF(HAVE_PTHREAD_RWLOCK)
    IF(APPLE)
      ADD_DEFINITIONS(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE)
    ENDIF(APPLE)
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "^IRIX")
      ADD_DEFINITIONS(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT)
    ENDIF("${CMAKE_SYSTEM_NAME}" MATCHES "^IRIX")
  ENDIF(HAVE_PTHREAD_RWLOCK)

  IF(HAVE_PTHREAD_H)
    CHECK_LIBRARY_EXISTS(pthread pthread_key_create "" HAVE_LIBPTHREAD)
    IF(HAVE_LIBPTHREAD)
      SET(THREAD_LIBS pthread)
    ENDIF(HAVE_LIBPTHREAD)
    CHECK_LIBRARY_EXISTS(rt sem_init "" HAVE_LIBRT)
    IF(HAVE_LIBRT)
      SET(THREAD_LIBS ${THREAD_LIBS} rt)
    ENDIF(HAVE_LIBRT)
  ENDIF(HAVE_PTHREAD_H)
ENDIF(WITH_THREADS)
