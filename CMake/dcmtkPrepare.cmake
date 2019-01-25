# Set up the build environment.
# This should be run before the individual modules are created.

# This file should only run once
if(DEFINED DCMTK_CONFIGURATION_DONE)
  return()
endif()
set(DCMTK_CONFIGURATION_DONE true)

# Minimum CMake version required
if(CMAKE_BACKWARDS_COMPATIBILITY GREATER 3.12.3)
  set(CMAKE_BACKWARDS_COMPATIBILITY 3.12.3 CACHE STRING "Latest version of CMake when this project was released." FORCE)
endif()

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
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# Basic version information
# (Starting with version 3.5.5, an odd number at the last position indicates
#  a development snapshot and an even number indicates an official release.)
set(DCMTK_MAJOR_VERSION 3)
set(DCMTK_MINOR_VERSION 6)
set(DCMTK_BUILD_VERSION 4)
# The ABI is not guaranteed to be stable between different snapshots/releases,
# so this particular version number is increased for each snapshot or release.
set(DCMTK_ABI_VERSION 14)

# Package "release" settings (some are currently unused and, therefore, disabled)
set(DCMTK_PACKAGE_NAME "dcmtk")
set(DCMTK_PACKAGE_DATE "DEV")
set(DCMTK_PACKAGE_VERSION "${DCMTK_MAJOR_VERSION}.${DCMTK_MINOR_VERSION}.${DCMTK_BUILD_VERSION}")
set(DCMTK_PACKAGE_VERSION_NUMBER ${DCMTK_MAJOR_VERSION}${DCMTK_MINOR_VERSION}${DCMTK_BUILD_VERSION})
set(DCMTK_PACKAGE_VERSION_SUFFIX "+")
#set(DCMTK_PACKAGE_TARNAME "dcmtk-${DCMTK_PACKAGE_VERSION}")
#set(DCMTK_PACKAGE_STRING "dcmtk ${DCMTK_PACKAGE_VERSION}")
#set(DCMTK_PACKAGE_BUGREPORT "bugs@dcmtk.org")
#set(DCMTK_PACKAGE_URL "http://www.dcmtk.org/")

# Shared library version information
SET(DCMTK_LIBRARY_PROPERTIES VERSION "${DCMTK_ABI_VERSION}.${DCMTK_PACKAGE_VERSION}" SOVERSION "${DCMTK_ABI_VERSION}")

# Gather information about the employed CMake version's behavior
set(DCMTK_CMAKE_HAS_CXX_STANDARD FALSE)
if(NOT CMAKE_MAJOR_VERSION LESS 3) # CMake versions prior to 3 don't understand VERSION_LESS etc.
  if(NOT CMAKE_VERSION VERSION_LESS "3.1.3")
    set(DCMTK_CMAKE_HAS_CXX_STANDARD TRUE)
  endif()
endif()
define_property(GLOBAL PROPERTY DCMTK_CMAKE_HAS_CXX_STANDARD
  BRIEF_DOCS "TRUE iff the CXX_STANDARD property exists."
  FULL_DOCS "TRUE for CMake versions since 3.1.3 that evaluate the CXX_STANDARD property and CMAKE_CXX_STANDARD variable."
)
set_property(GLOBAL PROPERTY DCMTK_CMAKE_HAS_CXX_STANDARD ${DCMTK_CMAKE_HAS_CXX_STANDARD})

# General build options and settings
option(BUILD_APPS "Build command line applications and test programs." ON)
option(BUILD_SHARED_LIBS "Build with shared libraries." OFF)
option(BUILD_SINGLE_SHARED_LIBRARY "Build a single DCMTK library." OFF)
mark_as_advanced(BUILD_SINGLE_SHARED_LIBRARY)
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Library postfix for debug builds. Usually left blank.")
# add our CMake modules to the module path, but prefer the ones from CMake.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules" "${CMAKE_CURRENT_SOURCE_DIR}/${DCMTK_CMAKE_INCLUDE}/CMake/")
if(BUILD_SINGLE_SHARED_LIBRARY)
  # When we are building a single shared lib, we are building shared libs :-)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
endif()

# DCMTK build options
option(DCMTK_WITH_TIFF "Configure DCMTK with support for TIFF." ON)
option(DCMTK_WITH_PNG "Configure DCMTK with support for PNG." ON)
option(DCMTK_WITH_XML "Configure DCMTK with support for XML." ON)
option(DCMTK_WITH_ZLIB "Configure DCMTK with support for ZLIB." ON)
option(DCMTK_WITH_OPENSSL "Configure DCMTK with support for OPENSSL." ON)
option(DCMTK_WITH_SNDFILE "Configure DCMTK with support for SNDFILE." ON)
option(DCMTK_WITH_ICONV "Configure DCMTK with support for ICONV." ON)
option(DCMTK_WITH_ICU "Configure DCMTK with support for ICU." ON)
if(NOT WIN32)
  option(DCMTK_WITH_WRAP "Configure DCMTK with support for WRAP." ON)
endif()
option(DCMTK_WITH_OPENJPEG "Configure DCMTK with support for OPENJPEG." ON)
option(DCMTK_ENABLE_PRIVATE_TAGS "Configure DCMTK with support for DICOM private tags coming with DCMTK." OFF)
option(DCMTK_WITH_THREADS "Configure DCMTK with support for multi-threading." ON)
option(DCMTK_WITH_DOXYGEN "Build API documentation with DOXYGEN." ON)
option(DCMTK_GENERATE_DOXYGEN_TAGFILE "Generate a tag file with DOXYGEN." OFF)
option(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS "Build with wide char file I/O functions." OFF)
option(DCMTK_WIDE_CHAR_MAIN_FUNCTION "Build command line tools with wide char main function." OFF)
option(DCMTK_ENABLE_STL "Enable use of native STL classes and algorithms instead of DCMTK's own implementations." OFF)

macro(DCMTK_INFERABLE_OPTION OPTION DESCRIPTION)
  set("${OPTION}" INFERRED CACHE STRING "${DESCRIPTION}")
  set_property(CACHE "${OPTION}" PROPERTY STRINGS "INFERRED" "ON" "OFF")
  # currently, all inferable options are advanced options
  mark_as_advanced("${OPTION}")
endmacro()

DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_VECTOR "Enable use of STL vector.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_ALGORITHM "Enable use of STL algorithm.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_LIMITS "Enable use of STL limit.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_LIST "Enable use of STL list.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_MAP "Enable use of STL map.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_MEMORY "Enable use of STL memory.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_STACK "Enable use of STL stack.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_STRING "Enable use of STL string.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_TYPE_TRAITS "Enable use of STL type traits.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_TUPLE "Enable use of STL tuple.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_STL_SYSTEM_ERROR "Enable use of STL system_error.")
DCMTK_INFERABLE_OPTION(DCMTK_ENABLE_CXX11 "Enable use of native C++11 features (eg. move semantics).")

# Built-in (compiled-in) dictionary enabled on Windows per default, otherwise
# disabled. Loading of external dictionary via run-time is, per default,
# configured the the opposite way since most users won't be interested in using
# the external default dictionary if it is already compiled in.
if(WIN32 OR MINGW)
  option(DCMTK_ENABLE_BUILTIN_DICTIONARY "Configure DCMTK with compiled-in data dictionary." ON)
  option(DCMTK_ENABLE_EXTERNAL_DICTIONARY "Configure DCMTK to load external dictionary from default path on startup." ON)
else() # built-in dictionary turned off on Unix per default
  option(DCMTK_ENABLE_BUILTIN_DICTIONARY "Configure DCMTK with compiled-in data dictionary." OFF)
  option(DCMTK_ENABLE_EXTERNAL_DICTIONARY "Configure DCMTK to load external dictionary from default path on startup." ON)
endif()
if (NOT DCMTK_ENABLE_EXTERNAL_DICTIONARY AND NOT DCMTK_ENABLE_BUILTIN_DICTIONARY)
  message(WARNING "Either external or built-in dictionary should be enabled, otherwise dictionary must be loaded manually on startup!")
endif()

# Mark various settings as "advanced"
mark_as_advanced(CMAKE_DEBUG_POSTFIX)
mark_as_advanced(FORCE EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)
mark_as_advanced(SNDFILE_DIR DCMTK_WITH_SNDFILE) # not yet needed in public DCMTK
mark_as_advanced(DCMTK_GENERATE_DOXYGEN_TAGFILE)
mark_as_advanced(DCMTK_WITH_OPENJPEG) # only needed by DCMJP2K module

if(NOT WIN32)
  # support for wide char file I/O functions is currently Windows-specific
  mark_as_advanced(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  # support for wide char main function is Windows-specific
  mark_as_advanced(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
endif()

enable_testing()

#-----------------------------------------------------------------------------
# Include appropriate modules and set required variables for cross compiling
#-----------------------------------------------------------------------------

if(CMAKE_CROSSCOMPILING)
  if(WIN32)
    include("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkUseWine.cmake")
    DCMTK_SETUP_WINE()
  elseif(ANDROID)
    include("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkUseAndroidSDK.cmake")
    # Ensure the configuration variables for the Android device emulator exist in the cache.
    DCMTK_SETUP_ANDROID_EMULATOR()
  endif()
endif()

#-----------------------------------------------------------------------------
# Generic utilities used for configuring DCMTK
#-----------------------------------------------------------------------------

include("${DCMTK_CMAKE_INCLUDE}CMake/dcmtkMacros.cmake")

#-----------------------------------------------------------------------------
# Prepare external dependencies for cross compiling
# (i.e. start the emulator if required)
#-----------------------------------------------------------------------------

if(CMAKE_CROSSCOMPILING)
  unset(DCMTK_UNIT_TESTS_UNSUPPORTED_WARN_ONCE CACHE)
  if(ANDROID)
    unset(DCMTK_TRY_RUN_ANDROID_RUNTIME_INSTALLED CACHE)
    DCMTK_ANDROID_START_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)
  endif()
endif()

#-----------------------------------------------------------------------------
# Installation sub-directories
#-----------------------------------------------------------------------------

# Set project name variable to package name for GnuInstallDirs
set(PROJECT_NAME "${DCMTK_PACKAGE_NAME}")
# Provides CMake cache variables with reasonable defaults to create a GNU style installation
# directory structure
include(GNUInstallDirs)
# CMake's files (DCMTKTarget.cmake, DCMTKConfigVersion.cmake and DCMTKConfig.cmake) are installed
# to different installation paths under Unix- and Windows-based systems
if(UNIX)
  set(DCMTK_INSTALL_CMKDIR "${CMAKE_INSTALL_LIBDIR}/cmake/dcmtk")
elseif(WIN32)
  set(DCMTK_INSTALL_CMKDIR "cmake")
endif()

#-----------------------------------------------------------------------------
# Build directories
#-----------------------------------------------------------------------------
set(DCMTK_BUILD_CMKDIR "${CMAKE_BINARY_DIR}")

#-----------------------------------------------------------------------------
# Start with clean DCMTKTargets.cmake, filled in GenerateCMakeExports.cmake
#-----------------------------------------------------------------------------
file(WRITE "${DCMTK_BUILD_CMKDIR}/DCMTKTargets.cmake" "")

#-----------------------------------------------------------------------------
# Platform-independent settings
#-----------------------------------------------------------------------------

# pass optional build date to compiler
#set(DCMTK_BUILD_DATE "\\\"YYYY-MM-DD\\\"")
if(DCMTK_BUILD_DATE)

    if(COMMAND CMAKE_POLICY)
        # Works around warnings about escaped quotes in ADD_DEFINITIONS statements
        cmake_policy(SET CMP0005 OLD)
    endif()

    # Xcode needs one escaping layer more than (as far as we know) everyone else - we gotta go deeper!
    if(CMAKE_GENERATOR MATCHES Xcode)
        string(REPLACE "\\" "\\\\" DCMTK_BUILD_DATE "${DCMTK_BUILD_DATE}")
    endif()
    add_definitions(-DDCMTK_BUILD_DATE=${DCMTK_BUILD_DATE})
endif()

# make OFString(NULL) safe by default
add_definitions(-DUSE_NULL_SAFE_OFSTRING)

# tell the DCMTK that we are building the DCMTK
add_definitions(-DDCMTK_BUILD_IN_PROGRESS)

# build output files in these directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#-----------------------------------------------------------------------------
# Platform-specific settings
#-----------------------------------------------------------------------------

# set project wide flags for compiler and linker

if(WIN32)
  option(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS "Overwrite compiler flags with DCMTK's WIN32 package default values." ON)
else()
  set(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS OFF)
endif()

if(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS AND NOT BUILD_SHARED_LIBS)

  # settings for Microsoft Visual Studio
  if(CMAKE_GENERATOR MATCHES "Visual Studio .*")
    # get Visual Studio Version
    string(REGEX REPLACE "Visual Studio ([0-9]+).*" "\\1" VS_VERSION "${CMAKE_GENERATOR}")
    # these settings never change even for C or C++
    set(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
    set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /MT /O2")
    set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /MT /O2")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /MTd /Z7 /Od")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /MT /O2")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/DNDEBUG /MT /O2")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/DNDEBUG /MTd /Z7 /Od")
    # specific settings for the various Visual Studio versions
    if(VS_VERSION EQUAL 6)
      set(CMAKE_C_FLAGS "/nologo /W3 /GX /Gy /YX")
      set(CMAKE_CXX_FLAGS "/nologo /W3 /GX /Gy /YX /Zm500") # /Zm500 increments heap size which is needed on some system to compile templates in dcmimgle
    endif()
    if(VS_VERSION EQUAL 7)
      set(CMAKE_C_FLAGS "/nologo /W3 /Gy")
      set(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
    endif()
    if(VS_VERSION GREATER 7)
      set(CMAKE_C_FLAGS "/nologo /W3 /Gy /EHsc")
      set(CMAKE_CXX_FLAGS "/nologo /W3 /Gy /EHsc")
    endif()
  endif()

  # settings for Borland C++
  if(CMAKE_GENERATOR MATCHES "Borland Makefiles")
    # further settings required? not tested for a very long time!
    set(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  endif()

endif()

if(BUILD_SHARED_LIBS)
  set(DCMTK_SHARED ON)
  if(BUILD_SINGLE_SHARED_LIBRARY)
    # We can't build apps, because there is no way to tell CMake to link apps
    # against the library.
    set(BUILD_APPS OFF CACHE BOOL "" FORCE)
    # We are building static code that can be used in a shared lib
    set(DCMTK_BUILD_SINGLE_SHARED_LIBRARY ON)
    # Make CMake build object libraries. They are just a list of object files
    # which aren't linked together yet.
    set(DCMTK_LIBRARY_TYPE OBJECT)
    # Static and shared libraries can have dependencies in CMake. Object
    # libraries cannot. Since CMake saves dependencies in its cache, we have to
    # make sure that it doesn't get confused when a "normal" library turns into
    # an object library. Do this via a suffix.
    set(DCMTK_LIBRARY_SUFFIX _obj)
    # This uses object libraries which are new in CMake 2.8.8
    cmake_minimum_required(VERSION 2.8.8)
  endif()

  option(USE_COMPILER_HIDDEN_VISIBILITY
      "Use hidden visibility support if available" ON)
  mark_as_advanced(USE_COMPILER_HIDDEN_VISIBILITY)

  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" GXX_SUPPORTS_VISIBILITY)
  # This "NOT WIN32" is needed due to a CMake bug that was fixed in
  # CMake 2.8.x. CHECK_CXX_COMPILER_FLAG() always says "ok" with MSC.
  if(GXX_SUPPORTS_VISIBILITY AND USE_COMPILER_HIDDEN_VISIBILITY AND NOT WIN32)
    set(HAVE_HIDDEN_VISIBILITY ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
  else()
    set(HAVE_HIDDEN_VISIBILITY)
  endif()
endif()

if(WIN32)   # special handling for Windows systems

  if(MINGW)
    # Avoid auto-importing warnings on MinGW
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
  else()
    if(NOT BORLAND)
      if(NOT CYGWIN)
        # Disable min() and max() macros pre-defined by Microsoft. We define our own
        # version in oflimits.h and on Windows that could result in name clashes in
        # Visual Studio.
        add_definitions(-DNOMINMAX)
        # On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security warnings.
        if(NOT DCMTK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
          add_definitions(
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
        endif()
      endif()
    endif()
  endif()

else()   # ... for non-Windows systems

  # Compiler flags for Mac OS X
  if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_DARWIN_C_SOURCE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_DARWIN_C_SOURCE")
  # Compiler flags for NetBSD
  elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_NETBSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_NETBSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
  # Solaris, FreeBSD and newer versions of OpenBSD fail with these flags
  elseif(NOT CMAKE_SYSTEM_NAME MATCHES "SunOS" AND NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD" AND (NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD" OR CMAKE_SYSTEM_VERSION VERSION_LESS 4))
    # Compiler flags for all other non-Windows systems
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE_EXTENDED -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE -D_POSIX_C_SOURCE=199506L")
  endif()

  option(DCMTK_FORCE_FPIC_ON_UNIX "Add -fPIC compiler flag on unix 64 bit machines to allow linking from dynamic libraries even if DCMTK is built statically" OFF)
  mark_as_advanced(DCMTK_FORCE_FPIC_ON_UNIX)

  # Setting for IA64 / x86_64 which needs -fPIC compiler flag required for shared library build on these platforms
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND UNIX AND DCMTK_FORCE_FPIC_ON_UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  endif()

endif()

# define libraries and object files that must be linked to most Windows applications
if(WIN32)
  set(WIN32_STD_LIBRARIES iphlpapi ws2_32 netapi32 wsock32)
  if(NOT DEFINED MINGW)
    # additional object file needed for wildcard expansion; for wchar_t* support, use 'wsetargv'
    set(WIN32_STD_OBJECTS setargv)
  endif()
  # settings for Borland C++
  if(CMAKE_CXX_COMPILER MATCHES bcc32)
    # to be checked: further settings required?
    set(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  endif()
endif()

# add definition of "DEBUG" to debug mode (since CMake does not do this automatically)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# handle CMAKE_CXX_STANDARD and related variables
if(DCMTK_CMAKE_HAS_CXX_STANDARD)
  if(NOT DEFINED CMAKE_CXX_STANDARD)
    if(DCMTK_ENABLE_CXX11 AND NOT DCMTK_ENABLE_CXX11 STREQUAL "INFERRED")
      set(CMAKE_CXX_STANDARD 11)
    endif()
  endif()
  if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD MATCHES "^9[0-9]?$")
    set(DCMTK_MODERN_CXX_STANDARD FALSE)
  else()
    set(DCMTK_MODERN_CXX_STANDARD TRUE)
  endif()
  define_property(GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARD
    BRIEF_DOCS "TRUE when compiling C++11 (or newer) code."
    FULL_DOCS "TRUE when the compiler does support and is configured for C++11 or a later C++ standard."
  )
  set_property(GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARD ${DCMTK_MODERN_CXX_STANDARD})
  if(DEFINED DCMTK_CXX11_FLAGS)
    message(WARNING "Legacy variable DCMTK_CXX11_FLAGS will be ignored since CMake now sets the flags based on the CMAKE_CXX_STANDARD variable automatically.")
  endif()
elseif(NOT DEFINED DCMTK_CXX11_FLAGS)
  # determine which flags are required to enable C++11 features (if any)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(DCMTK_CXX11_FLAGS "-std=c++11")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    if(CMAKE_HOST_WIN32)
      set(DCMTK_CXX11_FLAGS "/Qstd=c++11")
    else()
      set(DCMTK_CXX11_FLAGS "-std=c++11")
    endif()
  else()
    set(DCMTK_CXX11_FLAGS "")
  endif()
  set(DCMTK_CXX11_FLAGS "${DCMTK_CXX11_FLAGS}" CACHE STRING "The flags to add to CMAKE_CXX_FLAGS for enabling C++11 (if any).")
  mark_as_advanced(DCMTK_CXX11_FLAGS)
endif()
define_property(GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARDS
  BRIEF_DOCS "Modern C++ standards DCMTK knows about."
  FULL_DOCS "The list of C++ standards since C++11 that DCMTK currently has configuration tests for. "
)
set_property(GLOBAL PROPERTY DCMTK_MODERN_CXX_STANDARDS 11 14 17)

#-----------------------------------------------------------------------------
# Third party libraries
#-----------------------------------------------------------------------------

include(${DCMTK_CMAKE_INCLUDE}CMake/3rdparty.cmake)

#-----------------------------------------------------------------------------
# DCMTK libraries
#-----------------------------------------------------------------------------

include(${DCMTK_CMAKE_INCLUDE}CMake/GenerateDCMTKConfigure.cmake)

#-----------------------------------------------------------------------------
# Dart configuration (disabled per default)
#-----------------------------------------------------------------------------

# Includes build targets Experimental, Nightly and Continuous which are the standard
# groups pre-configured in Dashboard. In CTest these groups are called "Tracks".
#
# include(${CMAKE_ROOT}/Modules/Dart.cmake)
# if(BUILD_TESTING)
#   enable_testing()
# endif()

#-----------------------------------------------------------------------------
# Thread support
#-----------------------------------------------------------------------------

# See dcmtk/config/configure.in
if(WITH_THREADS)
  add_definitions(-D_REENTRANT)
  if(HAVE_PTHREAD_RWLOCK)
    if(APPLE)
      add_definitions(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE)
    endif()
    if(CMAKE_SYSTEM_NAME MATCHES "^IRIX")
      add_definitions(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT)
    endif()
  endif()

  if(HAVE_PTHREAD_H)
    CHECK_LIBRARY_EXISTS(pthread pthread_key_create "" HAVE_LIBPTHREAD)
    if(HAVE_LIBPTHREAD)
      set(THREAD_LIBS pthread)
    endif()
    CHECK_LIBRARY_EXISTS(rt sem_init "" HAVE_LIBRT)
    if(HAVE_LIBRT)
      set(THREAD_LIBS ${THREAD_LIBS} rt)
    endif()
  endif()
endif()

#-----------------------------------------------------------------------------
# Test for socket libraries if needed (Solaris)
#-----------------------------------------------------------------------------

set(SOCKET_LIBS)

function(DCMTK_TEST_SOCKET_LIBRARY NAME SYMBOL)
  string(TOUPPER "${NAME}" VARNAME)
  CHECK_LIBRARY_EXISTS("${NAME}" "main" "" "HAVE_LIB${VARNAME}_MAIN")
  if(NOT HAVE_LIB${VARNAME}_MAIN)
    CHECK_LIBRARY_EXISTS("${NAME}" "${SYMBOL}" "" "HAVE_LIB${VARNAME}")
  endif()
  if(HAVE_LIB${VARNAME} OR HAVE_LIB${VARNAME}_MAIN)
    list(APPEND SOCKET_LIBS "${NAME}")
    set(SOCKET_LIBS "${SOCKET_LIBS}" PARENT_SCOPE)
  endif()
endfunction()

DCMTK_TEST_SOCKET_LIBRARY(nsl "gethostbyname")
DCMTK_TEST_SOCKET_LIBRARY(socket "socket")

#-----------------------------------------------------------------------------
# Test if SunPro compiler and add features
#-----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL SunPro)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -features=tmplrefstatic")
endif()

#-----------------------------------------------------------------------------
# workaround for using the deprecated generator expression $<CONFIGURATION>
# with old CMake versions that do not understand $<CONFIG>
#-----------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_LESS 3.0.0)
  set(DCMTK_CONFIG_GENERATOR_EXPRESSION "$<CONFIGURATION>" CACHE INTERNAL "the generator expression to use for retriving the current config")
else()
  set(DCMTK_CONFIG_GENERATOR_EXPRESSION "$<CONFIG>" CACHE INTERNAL "the generator expression to use for retriving the current config")
endif()
