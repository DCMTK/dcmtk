# Set up the build environment.
# This should be run before the individual modules are created.

# This file should only run once
IF(DEFINED DCMTK_CONFIGURATION_DONE)
  RETURN()
ENDIF(DEFINED DCMTK_CONFIGURATION_DONE)
SET(DCMTK_CONFIGURATION_DONE true)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 3.7.2)
  SET(CMAKE_BACKWARDS_COMPATIBILITY 3.7.2 CACHE STRING "Latest version of CMake when this project was released." FORCE)
ENDIF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 3.7.2)

# CMAKE_BUILD_TYPE is set to value "Release" if none is specified by the
# selected build file generator. For those generators that support multiple
# configuration types (e.g. Debug, Release), CMAKE_CONFIGURATION_TYPES holds
# possible values.  For other generators this value is empty, and for those
# generators the build type is controlled at CMake time by CMAKE_BUILD_TYPE.
# See http://www.cmake.org/pipermail/cmake/2006-January/008065.html for
# details.
#
# To disable the CMAKE_BUILD_TYPE default value, set CMAKE_BUILD_TYPE to value
# "None" during CMake configuration, e.g. use "-DCMAKE_BUILD_TYPE:STRING=None"
# on the command line.  This may be useful if the compiler flags should be
# controlled manually (e.g. as defined in environment variables like CXXFLAGS)
# and no CMake defaults related to the selected configuration type kick in.
IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  MESSAGE(STATUS "Setting build type to 'Release' as none was specified.")
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
ENDIF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)


# Basic version information
# (Starting with version 3.5.5, an odd number at the last position indicates
#  a development snapshot and an even number indicates an official release.)
SET(DCMTK_MAJOR_VERSION 3)
SET(DCMTK_MINOR_VERSION 6)
SET(DCMTK_BUILD_VERSION 1)
# The ABI is not guaranteed to be stable between different snapshots/releases,
# so this particular version number is increased for each snapshot or release.
SET(DCMTK_ABI_VERSION 11)

# Package "release" settings (some are currently unused and, therefore, disabled)
SET(DCMTK_PACKAGE_NAME "dcmtk")
SET(DCMTK_PACKAGE_DATE "DEV")
SET(DCMTK_PACKAGE_VERSION "${DCMTK_MAJOR_VERSION}.${DCMTK_MINOR_VERSION}.${DCMTK_BUILD_VERSION}")
SET(DCMTK_PACKAGE_VERSION_NUMBER ${DCMTK_MAJOR_VERSION}${DCMTK_MINOR_VERSION}${DCMTK_BUILD_VERSION})
SET(DCMTK_PACKAGE_VERSION_SUFFIX "")
#SET(DCMTK_PACKAGE_TARNAME "dcmtk-${DCMTK_PACKAGE_VERSION}")
#SET(DCMTK_PACKAGE_STRING "dcmtk ${DCMTK_PACKAGE_VERSION}")
#SET(DCMTK_PACKAGE_BUGREPORT "bugs@dcmtk.org")
#SET(DCMTK_PACKAGE_URL "http://www.dcmtk.org/")

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
OPTION(DCMTK_WITH_ICU "Configure DCMTK with support for ICU." ON)
IF(NOT WIN32)
  OPTION(DCMTK_WITH_WRAP "Configure DCMTK with support for WRAP." ON)
ENDIF(NOT WIN32)
OPTION(DCMTK_ENABLE_PRIVATE_TAGS "Configure DCMTK with support for DICOM private tags coming with DCMTK." OFF)
OPTION(DCMTK_WITH_THREADS "Configure DCMTK with support for multi-threading." ON)
OPTION(DCMTK_WITH_DOXYGEN "Build API documentation with DOXYGEN." ON)
OPTION(DCMTK_GENERATE_DOXYGEN_TAGFILE "Generate a tag file with DOXYGEN." OFF)
OPTION(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS "Build with wide char file I/O functions." OFF)
OPTION(DCMTK_WIDE_CHAR_MAIN_FUNCTION "Build command line tools with wide char main function." OFF)

# Built-in (compiled-in) dictionary enabled on Windows per default, otherwise
# disabled. Loading of external dictionary via run-time is, per default,
# configured the the opposite way since most users won't be interested in using
# the external default dictionary if it is already compiled in.
IF(WIN32 OR MINGW)
  OPTION(DCMTK_ENABLE_BUILTIN_DICTIONARY "Configure DCMTK with compiled-in data dictionary." ON)
  OPTION(DCMTK_ENABLE_EXTERNAL_DICTIONARY "Configure DCMTK to load external dictionary from default path on startup." OFF)
ELSE(WIN32 or MINGW) # built-in dictionary turned off on Unix per default
  OPTION(DCMTK_ENABLE_BUILTIN_DICTIONARY "Configure DCMTK with compiled-in data dictionary." OFF)
  OPTION(DCMTK_ENABLE_EXTERNAL_DICTIONARY "Configure DCMTK to load external dictionary from default path on startup." ON)
ENDIF(WIN32 OR MINGW)
if (NOT DCMTK_ENABLE_EXTERNAL_DICTIONARY AND NOT DCMTK_ENABLE_BUILTIN_DICTIONARY)
  MESSAGE(WARNING "Either external or built-in dictionary should be enabled, otherwise dictionary must be loaded manually on startup!")
ENDIF(NOT DCMTK_ENABLE_EXTERNAL_DICTIONARY AND NOT DCMTK_ENABLE_BUILTIN_DICTIONARY)

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
# Include appropriate modules and set required variables for cross compiling
#-----------------------------------------------------------------------------

IF(CMAKE_CROSSCOMPILING)
  IF(WIN32)
    INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/dcmtkUseWine.cmake)
    DCMTK_SETUP_WINE()
  ELSEIF(ANDROID)
    SET(DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS "-DANDROID_TOOLCHAIN_CONFIG_FILE:INTERNAL=${ANDROID_TOOLCHAIN_CONFIG_FILE}")
    INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/dcmtkUseAndroidSDK.cmake)
    IF(NOT DCMTK_ANDROID_TOOLCHAIN_VERIFIED)
      # Ensure the configuration variables for the Android device emulator exist in the cache.
      DCMTK_SETUP_ANDROID_EMULATOR()
      SET(DCMTK_ANDROID_TOOLCHAIN_VERIFIED TRUE CACHE INTERNAL "Set to TRUE to prevent aborting configuration on first Android toolchain run")
      MESSAGE(FATAL_ERROR
        "Please verify your Android toolchain configuration (e.g.\"ANDROID_NATIVE_API_LEVEL\") is correct before configuring DCMTK.\n"
        "Hit \"Configure\" again to resume configuration when you are done.\n"
        "NOTE: set \"DCMTK_ANDROID_TOOLCHAIN_VERIFIED\" to \"TRUE\" to avoid this check (e.g. to allow automated builds), you may use"
        "something like\n    \"cmake -DDCMTK_ANDROID_TOOLCHAIN_VERIFIED=TRUE ...\"\nto achieve that."
      )
    ENDIF()
  ENDIF()
ENDIF(CMAKE_CROSSCOMPILING)

#-----------------------------------------------------------------------------
# Generic utilities used for configuring DCMTK
#-----------------------------------------------------------------------------

INCLUDE(${DCMTK_CMAKE_INCLUDE}CMake/dcmtkMacros.cmake)

#-----------------------------------------------------------------------------
# Prepare external dependencies for cross compiling
# (i.e. start the emulator if required)
#-----------------------------------------------------------------------------

IF(CMAKE_CROSSCOMPILING)
  UNSET(DCMTK_UNIT_TESTS_UNSUPPORTED_WARN_ONCE CACHE)
  IF(ANDROID)
    UNSET(DCMTK_TRY_RUN_ANDROID_RUNTIME_INSTALLED CACHE)
    DCMTK_ANDROID_START_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)
    DCMTK_ATEXIT(DCMTK_ANDROID_STOP_EMULATOR DCMTK_ANDROID_EMULATOR_INSTANCE)
  ENDIF()
ENDIF(CMAKE_CROSSCOMPILING)

#-----------------------------------------------------------------------------
# Installation sub-directories
#-----------------------------------------------------------------------------

SET(DCMTK_INSTALL_BINDIR "bin" CACHE STRING "Installation sub-directory for binary executables.")
SET(DCMTK_INSTALL_INCDIR "include" CACHE STRING "Installation sub-directory for header files.")
SET(DCMTK_INSTALL_LIBDIR "lib" CACHE STRING "Installation sub-directory for object code libraries.")
# CMake's files (DCMTKTarget.cmake, DCMTKConfigVersion.cmake and DCMTKConfig.cmake) are installed
# to different installation paths under Unix- and Windows-based systems
IF(UNIX)
  SET(DCMTK_INSTALL_CMKDIR "lib/cmake/dcmtk" CACHE STRING "Installation sub-directory for CMake files.")
ELSEIF(WIN32)
  SET(DCMTK_INSTALL_CMKDIR "cmake" CACHE STRING "Installation sub-directory for CMake files.")
ENDIF(UNIX)
SET(DCMTK_INSTALL_ETCDIR "etc/dcmtk" CACHE STRING "Installation sub-directory for configuration files.")
SET(DCMTK_INSTALL_DATDIR "share/dcmtk" CACHE STRING "Installation sub-directory for sample files and the like.")
SET(DCMTK_INSTALL_DOCDIR "share/doc/dcmtk" CACHE STRING "Installation sub-directory for general documentation.")
SET(DCMTK_INSTALL_HTMDIR "share/doc/dcmtk/html" CACHE STRING "Installation sub-directory for HTML documentation.")
SET(DCMTK_INSTALL_MANDIR "share/man" CACHE STRING "Installation sub-directory for man pages.")

MARK_AS_ADVANCED(DCMTK_INSTALL_BINDIR DCMTK_INSTALL_INCDIR DCMTK_INSTALL_LIBDIR DCMTK_INSTALL_CMKDIR
                 DCMTK_INSTALL_ETCDIR DCMTK_INSTALL_DATDIR DCMTK_INSTALL_DOCDIR DCMTK_INSTALL_HTMDIR
                 DCMTK_INSTALL_MANDIR)

#-----------------------------------------------------------------------------
# Build directories
#-----------------------------------------------------------------------------
SET(DCMTK_BUILD_CMKDIR ${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Start with clean DCMTKTargets.cmake, filled in GenerateCMakeExports.cmake
#-----------------------------------------------------------------------------
FILE(WRITE ${DCMTK_BUILD_CMKDIR}/DCMTKTargets.cmake "")

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
#ADD_DEFINITIONS(-DDCMTK_BUILD_DATE=\\\"YYYY-MM-DD\\\")

# make OFString(NULL) safe by default
ADD_DEFINITIONS(-DUSE_NULL_SAFE_OFSTRING)

# tell the DCMTK that we are building the DCMTK
ADD_DEFINITIONS(-DDCMTK_BUILD_IN_PROGRESS)

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
    IF(NOT BORLAND)
      IF(NOT CYGWIN)
        # Disable min() and max() macros pre-defined by Microsoft. We define our own
        # version in oflimits.h and on Windows that could result in name clashes in
        # Visual Studio.
        ADD_DEFINITIONS(-DNOMINMAX)
        # On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security warnings.
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
  # FreeBSD and newer versions of OpenBSD fail with these flags
  ELSEIF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" AND (NOT ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" OR ${CMAKE_SYSTEM_VERSION} VERSION_LESS 4))
    # Compiler flags for all other non-Windows systems
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
  ENDIF()

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
# C++11 support
#-----------------------------------------------------------------------------

MACRO(GET_CXX_VERSION VERSION)
  EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE ${VERSION})
  STRING(REGEX REPLACE "^.*[ ]([0-9]\\.[0-9]\\.[0-9]).*$" "\\1" ${VERSION} "${${VERSION}}")
ENDMACRO(GET_CXX_VERSION VERSION)

MACRO(ALLOW_CXX11_SUPPORT FLAGS)
  IF(NOT DCMTK_CXX11_FLAGS)
    SET(DCMTK_CXX11_FLAGS ${FLAGS} CACHE STRING "Flags used to enable C++11 support.")
  ENDIF()
  IF(DCMTK_USE_CXX11_STL)
    MESSAGE(STATUS "Info: Configured DCMTK to use native C++11 features.")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DCMTK_CXX11_FLAGS}")
    SET(DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS} "-DCMAKE_CXX_FLAGS:STRING=${DCMTK_CXX11_FLAGS}")
  ELSE()
    MESSAGE(STATUS "Info: Your compiler supports C++11. You may enable C++11 features via \"DCMTK_USE_CXX11_STL\" to create a C++11 build of DCMTK.")
    SET(DCMTK_USE_CXX11_STL OFF CACHE BOOL "Enable or disable C++11 while building DCMTK.")
  ENDIF()
ENDMACRO(ALLOW_CXX11_SUPPORT)

MACRO(DENY_CXX11_SUPPORT)
  IF(DCMTK_USE_CXX11_STL)
    IF(DCMTK_CXX11_FLAGS AND NOT DCMTK_CXX11_FLAGS STREQUAL "unknown")
      MESSAGE(STATUS "Info: Enabling C++11 support with custom flags \"${DCMTK_CXX11_FLAGS}\" for a potentially unsupported compiler.")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DCMTK_CXX11_FLAGS}")
      SET(DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS ${DCMTK_TRY_COMPILE_REQUIRED_CMAKE_FLAGS} "-DCMAKE_CXX_FLAGS:STRING=${DCMTK_CXX11_FLAGS}")
    ELSE()
      MESSAGE(WARNING "DCMTK has been configured to use the C++11 STL, but the compiler does not seem to support C++11. Override this warning by setting DCMTK_CXX11_FLAGS to tell DCMTK how to enable C++11 support for your compiler.")
      SET(DCMTK_CXX11_FLAGS "unknown" CACHE STRING "Flags used to enable C++11 support.")
    ENDIF()
  ENDIF()
ENDMACRO(DENY_CXX11_SUPPORT)

IF(CMAKE_COMPILER_IS_GNUCXX)
  GET_CXX_VERSION(GXX_VERSION)
  IF(GXX_VERSION VERSION_GREATER 4.8.1 OR GXX_VERSION VERSION_EQUAL 4.8.1)
    ALLOW_CXX11_SUPPORT("-std=c++11")
  ELSE()
    DENY_CXX11_SUPPORT()
  ENDIF()
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  GET_CXX_VERSION(CLANG_VERSION)
  IF(CLANG_VERSION VERSION_GREATER 3.3 OR CLANG_VERSION VERSION_EQUAL 3.3)
    ALLOW_CXX11_SUPPORT("-std=c++11")
  ELSE()
    DENY_CXX11_SUPPORT()
  ENDIF()
ELSE()
  DENY_CXX11_SUPPORT()
ENDIF()

#-----------------------------------------------------------------------------
# DCMTK libraries
#-----------------------------------------------------------------------------

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
