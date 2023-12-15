# Set up the build environment.
# This should be run before the individual modules are created.

# This file should only run once
if(DEFINED DCMTK_CONFIGURATION_DONE)
  return()
endif()
set(DCMTK_CONFIGURATION_DONE true)

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
set(DCMTK_BUILD_VERSION 8)
# The ABI is not guaranteed to be stable between different snapshots/releases,
# so this particular version number is increased for each snapshot or release.
set(DCMTK_ABI_VERSION 18)

# Package "release" settings (some are currently unused and, therefore, disabled)
set(DCMTK_PACKAGE_NAME "dcmtk")
set(DCMTK_PACKAGE_DATE "2023-12-19")
set(DCMTK_PACKAGE_VERSION "${DCMTK_MAJOR_VERSION}.${DCMTK_MINOR_VERSION}.${DCMTK_BUILD_VERSION}")
set(DCMTK_PACKAGE_VERSION_NUMBER ${DCMTK_MAJOR_VERSION}${DCMTK_MINOR_VERSION}${DCMTK_BUILD_VERSION})
set(DCMTK_PACKAGE_VERSION_SUFFIX "")
#set(DCMTK_PACKAGE_TARNAME "dcmtk-${DCMTK_PACKAGE_VERSION}")
#set(DCMTK_PACKAGE_STRING "dcmtk ${DCMTK_PACKAGE_VERSION}")
#set(DCMTK_PACKAGE_BUGREPORT "bugs@dcmtk.org")
#set(DCMTK_PACKAGE_URL "http://www.dcmtk.org/")

option(DCMTK_LINK_STATIC "Statically link all libraries and tools with the runtime and third party libraries." OFF)
# Modify linker flags and libraries for static builds if enabled by the user
if(DCMTK_LINK_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    # remove "-Wl,-Bdynamic"
    list(REMOVE_ITEM CMAKE_EXE_LINK_DYNAMIC_C_FLAGS "-Wl,-Bdynamic")
    list(REMOVE_ITEM CMAKE_EXE_LINK_DYNAMIC_CXX_FLAGS "-Wl,-Bdynamic")
    # remove -fPIC
    list(REMOVE_ITEM CMAKE_SHARED_LIBRARY_C_FLAGS "-fPIC")
    list(REMOVE_ITEM CMAKE_SHARED_LIBRARY_CXX_FLAGS "-fPIC")
    # remove -rdynamic
    list(REMOVE_ITEM CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-rdynamic")
    list(REMOVE_ITEM CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-rdynamic")
    # not sure about this one, maybe static libraries don't need it:
    set(DCMTK_LIBRARY_PROPERTIES VERSION "${DCMTK_ABI_VERSION}.${DCMTK_PACKAGE_VERSION}")
else()
    # Shared library version information
    set(DCMTK_LIBRARY_PROPERTIES VERSION "${DCMTK_ABI_VERSION}.${DCMTK_PACKAGE_VERSION}" SOVERSION "${DCMTK_ABI_VERSION}")
endif()

option(DCMTK_PORTABLE_LINUX_BINARIES "Create ELF binaries while statically linking all third party libraries and libstdc++." OFF)
if(DCMTK_PORTABLE_LINUX_BINARIES)
    if (DCMTK_LINK_STATIC)
        message(FATAL_ERROR "Options DCMTK_LINK_STATIC and DCMTK_PORTABLE_LINUX_BINARIES are mutually exclusive.")
    endif()
    # only use static versions of third party libraries
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

    # When using gcc and clang, use the static version of libgcc/libstdc++.
    if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "XLClang"))
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif()

function(get_static_library STATIC_NAME LIBRARYNAME)
    find_library("${STATIC_NAME}" "${LIBRARYNAME}")
    if("${${STATIC_NAME}}" STREQUAL "${STATIC_NAME}-NOTFOUND")
        # If the library is not found we probably do not need is as dependency. In the worst case it
        # needs to be installed, but then it should also be found.
        set("${STATIC_NAME}" "" PARENT_SCOPE)
    else()
        set("${STATIC_NAME}" "${${STATIC_NAME}}" PARENT_SCOPE)
    endif()
endfunction()

# Define additional library dependencies for statically linking the third-party libraries.
# This is difficult and error-prone, because we cannot know which extra libraries were used
# during compilation on a specific operating system.
# This should possibly be enhanced by using find_package() at some point. The best solution
# would probably be to compile all third-party libraries ourself.
if(DCMTK_LINK_STATIC OR DCMTK_PORTABLE_LINUX_BINARIES)
    get_static_library("STATIC_DL" "libdl.a")
    set(ICU_EXTRA_LIBS_STATIC "${STATIC_DL}")
    get_static_library("STATIC_LZMA" "liblzma.a")
    get_static_library("STATIC_ZLIB" "libz.a")
    get_static_library("STATIC_ICUUC" "libicuuc.a")
    get_static_library("STATIC_ICUDATA" "libicudata.a")
    set(LIBXML2_EXTRA_LIBS_STATIC "${STATIC_LZMA}" "${STATIC_ZLIB}" "${STATIC_ICUUC}" "${STATIC_ICUDATA}" "${STATIC_DL}")
    get_static_library("STATIC_PTHREAD" "libpthread.a")
    set(OPENJPEG_EXTRA_LIBS_STATIC "${STATIC_PTHREAD}")
    set(OPENSSL_EXTRA_LIBS_STATIC "${STATIC_DL}")
    get_static_library("STATIC_OGG" "libogg.a")
    get_static_library("STATIC_VORBIS" "libvorbis.a")
    get_static_library("STATIC_VORBISENC" "libvorbisenc.a")
    get_static_library("STATIC_OPUS" "libopus.a")
    get_static_library("STATIC_FLAC" "libFLAC.a")
    set(SNDFILE_EXTRA_LIBS_STATIC "${STATIC_VORBIS}" "${STATIC_OGG}" "${STATIC_VORBISENC}" "${STATIC_OPUS}" "${STATIC_FLAC}")
    get_static_library("STATIC_WEBP" "libwebp.a")
    get_static_library("STATIC_ZSTD" "libzstd.a")
    get_static_library("STATIC_JBIG" "libjbig.a")
    get_static_library("STATIC_JPEG" "libjpeg.a")
    get_static_library("STATIC_DEFLATE" "libdeflate.a")
    set(TIFF_EXTRA_LIBS_STATIC "${STATIC_WEBP}" "${STATIC_ZSTD}" "${STATIC_LZMA}" "${STATIC_JBIG}" "${STATIC_JBIG}" "${STATIC_DEFLATE}" "${STATIC_ZLIB}")
    get_static_library("STATIC_NSL" "libnsl.a")
    set(WRAP_EXTRA_LIBS_STATIC "${STATIC_NSL}")
else()
    set(ICU_EXTRA_LIBS_STATIC)
    set(LIBXML2_EXTRA_LIBS_STATIC)
    set(OPENJPEG_EXTRA_LIBS_STATIC)
    set(OPENSSL_EXTRA_LIBS_STATIC)
    set(SNDFILE_EXTRA_LIBS_STATIC)
    set(TIFF_EXTRA_LIBS_STATIC)
    set(WRAP_EXTRA_LIBS_STATIC)
endif()

# Gather information about the employed CMake version's behavior
set(DCMTK_CMAKE_HAS_CXX_STANDARD FALSE)

if(NOT CMAKE_VERSION VERSION_LESS "3.1.3")
  set(DCMTK_CMAKE_HAS_CXX_STANDARD TRUE)
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
set(DCMTK_TLS_LIBRARY_POSTFIX "" CACHE STRING "Postfix for libraries that change their ABI when using OpenSSL.")
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

# On Windows, the built-in dictionary is default, on Unix the external one.
# It is not possible to use both, built-in plus external default dictionary.
if(NOT DEFINED DCMTK_DEFAULT_DICT)
  if(WIN32 OR MINGW)
    set(DCMTK_DEFAULT_DICT_DEFAULT "builtin")
  else() # built-in dictionary turned off on Unix per default
    set(DCMTK_DEFAULT_DICT_DEFAULT "external")
  endif()
else()
  # prefer user specified one:
  set(DCMTK_DEFAULT_DICT_DEFAULT "${DCMTK_DEFAULT_DICT}")
endif()
set(DCMTK_DEFAULT_DICT "${DCMTK_DEFAULT_DICT_DEFAULT}" CACHE STRING "Denotes whether DCMTK will use built-in (compiled-in), external (file), or no default dictionary on startup")
set_property(CACHE DCMTK_DEFAULT_DICT PROPERTY STRINGS builtin external none)
if (DCMTK_DEFAULT_DICT EQUAL "none")
  message(WARNING "Denotes whether DCMTK will use built-in (compiled-in), external (file), or no default dictionary on startup")
endif()

# Per default, we allow users to load user-defined dictionaries pointed to via
# environment variable DCMDICTPATH.
option(DCMTK_USE_DCMDICTPATH "Enable reading dictionary that is defined through DCMDICTPATH environment variable." ON)

# Declare the option DCMTK_ENABLE_BUILTIN_OFICONV_DATA, which by default is ON when
# we are compiling shared libraries.
if (BUILD_SHARED_LIBS)
  option(DCMTK_ENABLE_BUILTIN_OFICONV_DATA "Embed oficonv data files into oficonv library" ON)
else()
  option(DCMTK_ENABLE_BUILTIN_OFICONV_DATA "Embed oficonv data files into oficonv library" OFF)
endif()

# evaluate the option DCMTK_ENABLE_BUILTIN_OFICONV_DATA
if (DCMTK_ENABLE_BUILTIN_OFICONV_DATA)
  add_definitions(-DDCMTK_ENABLE_BUILTIN_OFICONV_DATA)
endif()

# Mark various settings as "advanced"
mark_as_advanced(DCMTK_USE_DCMDICTPATH)
mark_as_advanced(CMAKE_DEBUG_POSTFIX)
mark_as_advanced(FORCE EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)
mark_as_advanced(SNDFILE_DIR DCMTK_WITH_SNDFILE) # not yet needed in public DCMTK
mark_as_advanced(DCMTK_GENERATE_DOXYGEN_TAGFILE)
mark_as_advanced(DCMTK_WITH_OPENJPEG) # only needed by DCMJP2K module
mark_as_advanced(DCMTK_TLS_LIBRARY_POSTFIX)

if(NOT WIN32)
  # support for wide char file I/O functions is currently Windows-specific
  mark_as_advanced(DCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS)
  # support for wide char main function is Windows-specific
  mark_as_advanced(DCMTK_WIDE_CHAR_MAIN_FUNCTION)
endif()

# Enable manpages only for non-Windows systems by default
set(DCMTK_ENABLE_MANPAGES_DOCSTRING "Enable building/installing of manpages.")
if(WIN32)
  option(DCMTK_ENABLE_MANPAGES "${DCMTK_ENABLE_MANPAGES_DOCSTRING}" OFF)
else()
  option(DCMTK_ENABLE_MANPAGES "${DCMTK_ENABLE_MANPAGES_DOCSTRING}" ON)
endif()
mark_as_advanced(DCMTK_ENABLE_MANPAGES)

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
# Interface target to collect all DCMTK libraries
#-----------------------------------------------------------------------------
add_library(DCMTK INTERFACE)
install(TARGETS DCMTK EXPORT DCMTKTargets)
target_link_libraries(DCMTK INTERFACE config)

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
  option(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS  "Modify the default compiler flags selected by CMake." ON)
  option(DCMTK_COMPILE_WIN32_MULTITHREADED_DLL "Compile DCMTK using the Multithreaded DLL runtime library." OFF)
  if (BUILD_SHARED_LIBS)
    set(DCMTK_COMPILE_WIN32_MULTITHREADED_DLL ON)
  endif()
else()
  # these settings play no role on other platforms
  set(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS OFF)
  set(DCMTK_COMPILE_WIN32_MULTITHREADED_DLL OFF)
endif()

if(WIN32 AND CMAKE_GENERATOR MATCHES "Visual Studio .*|NMake .*")
  if (POLICY CMP0091)
    # CMake 3.15 and newer use CMAKE_MSVC_RUNTIME_LIBRARY to select
    # the MSVC runtime library
    if(DCMTK_COMPILE_WIN32_MULTITHREADED_DLL OR BUILD_SHARED_LIBS)
       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
  else()
    # CMake 3.14 and older automatically set the MSVC runtime
    # library to a built-in default.
    # Evaluate the DCMTK_COMPILE_WIN32_MULTITHREADED_DLL option and adjust
    # the runtime library setting (/MT or /MD) accordingly
    set(CompilerFlags
          CMAKE_CXX_FLAGS
          CMAKE_CXX_FLAGS_DEBUG
          CMAKE_CXX_FLAGS_RELEASE
          CMAKE_CXX_FLAGS_MINSIZEREL
          CMAKE_CXX_FLAGS_RELWITHDEBINFO
          CMAKE_C_FLAGS
          CMAKE_C_FLAGS_DEBUG
          CMAKE_C_FLAGS_RELEASE
          CMAKE_C_FLAGS_MINSIZEREL
          CMAKE_C_FLAGS_RELWITHDEBINFO
          )

    if(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS OR BUILD_SHARED_LIBS)
      if(DCMTK_COMPILE_WIN32_MULTITHREADED_DLL OR BUILD_SHARED_LIBS)
        # Convert any /MT or /MTd option to /MD or /MDd
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
            set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
        endforeach()
      else()
        # Convert any /MD or /MDd option to /MT or /MTd
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
        endforeach()
      endif()
    endif()
  endif() # Policy CMP0091
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
          # suppress linker warning LNK4099 about missing .pdb files when compiling in debug mode
          set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
          set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
        endif()
        # Enable various warnings
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
  # Compiler flags for DragonFly BSD
  elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly")
    # On DragonFly BSD, we don't set any of the feature macros because they are not needed and
    # actually cause problems such as strlcpy() not being declared in <cstring>.
  # Solaris, FreeBSD and newer versions of OpenBSD fail with these flags
  elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
    # On Android, we don't set any of the feature macros
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

  # When compiling with IBM xlC, add flags to suppress some noisy C++ warnings
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qsuppress=1500-029:1500-030")
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
# Enable various warnings by default
#-----------------------------------------------------------------------------

if(MSVC)
    # This code removes existing warning flags to prevent MSVC warning D9025.
    # Remove it once our minimum CMake version is >= 3.15, since those newer
    # CMake versions no longer add those flags by default.
    foreach(FLAG C CXX)
        string(REGEX REPLACE "[ \t\r\n]+/[wW][0-9]" "" "CMAKE_${FLAG}_FLAGS" "${CMAKE_${FLAG}_FLAGS}")
        string(REGEX REPLACE "/[wW][0-9][ \t\r\n]*" "" "CMAKE_${FLAG}_FLAGS" "${CMAKE_${FLAG}_FLAGS}")
    endforeach()
    add_compile_options("/W4")
else()
    # Add -Wall to the compiler flags if we are compiling with gcc or Clang.
    if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang") OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "XLClang"))
        add_compile_options("-Wall")
    endif()
endif()

#-----------------------------------------------------------------------------
# Third party libraries
#-----------------------------------------------------------------------------

include(${DCMTK_CMAKE_INCLUDE}CMake/3rdparty.cmake)

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
# DCMTK libraries
#-----------------------------------------------------------------------------

include(${DCMTK_CMAKE_INCLUDE}CMake/GenerateDCMTKConfigure.cmake)

#-----------------------------------------------------------------------------
# Thread support
#-----------------------------------------------------------------------------

# Compile reentrant code when WITH_THREADS is active
if(WITH_THREADS)
  add_definitions(-D_REENTRANT)
endif()

# add feature macros needed for libpthread
if(HAVE_PTHREAD_RWLOCK)
  if(APPLE)
    add_definitions(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT -D_OSF_SOURCE)
  endif()
  if(CMAKE_SYSTEM_NAME MATCHES "^IRIX")
    add_definitions(-D_XOPEN_SOURCE_EXTENDED -D_BSD_SOURCE -D_BSD_COMPAT)
  endif()
endif()

# add libpthread to THREAD_LIBS. This is needed in WITH_THREADS mode,
# but also when linking against OpenSSL
if(HAVE_PTHREAD_H)
  CHECK_LIBRARY_EXISTS(pthread pthread_key_create "" HAVE_LIBPTHREAD)
  if(HAVE_LIBPTHREAD)
    set(THREAD_LIBS pthread)
  endif()
endif()

# Add librt to THREAD_LIBS even if we are not compiling WITH_THREADS,
# since OFIPCMessageQueueServer also needs it
CHECK_LIBRARY_EXISTS(rt mq_open "" HAVE_LIBRT)
if(HAVE_LIBRT)
  set(THREAD_LIBS ${THREAD_LIBS} rt)
endif()

#-----------------------------------------------------------------------------
# OpenSSL functions (these tests must be run after THREAD_LIBS is set)
#-----------------------------------------------------------------------------

# Run the OpenSSL feature tests only if OpenSSL was detected and is enabled
if(DCMTK_WITH_OPENSSL)
  # save previous value of CMAKE_REQUIRED_LIBRARIES
  set(CMAKE_REQUIRED_LIBRARIES_TEMP ${CMAKE_REQUIRED_LIBRARIES})

  # add OpenSSL libraries
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${OPENSSL_LIBS} ${THREAD_LIBS})

  # check whether we have <openssl/provider.h>
  CHECK_INCLUDE_FILE_CXX("openssl/provider.h" HAVE_OPENSSL_PROVIDER_H)

  # test presence of functions, constants and macros needed for the dcmtls module
  CHECK_FUNCTIONWITHHEADER_EXISTS("DH_bits" "openssl/dh.h" HAVE_OPENSSL_PROTOTYPE_DH_BITS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("EVP_PKEY_RSA_PSS" "openssl/evp.h" HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_RSA_PSS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("EVP_PKEY_base_id" "openssl/evp.h" HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_BASE_ID)
  CHECK_FUNCTIONWITHHEADER_EXISTS("NID_dsa_with_SHA512" "openssl/obj_mac.h" HAVE_OPENSSL_PROTOTYPE_NID_DSA_WITH_SHA512)
  CHECK_FUNCTIONWITHHEADER_EXISTS("NID_ecdsa_with_SHA3_256" "openssl/obj_mac.h" HAVE_OPENSSL_PROTOTYPE_NID_ECDSA_WITH_SHA3_256)
  CHECK_FUNCTIONWITHHEADER_EXISTS("NID_sha512_256WithRSAEncryption" "openssl/obj_mac.h" HAVE_OPENSSL_PROTOTYPE_NID_SHA512_256WITHRSAENCRYPTION)
  CHECK_FUNCTIONWITHHEADER_EXISTS("RAND_egd" "openssl/rand.h" HAVE_OPENSSL_PROTOTYPE_RAND_EGD)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_get0_param" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET0_PARAM)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_get_cert_store" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CERT_STORE)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_get_ciphers" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_GET_CIPHERS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set0_tmp_dh_pkey" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET0_TMP_DH_PKEY)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set1_curves(0,0,0)" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_CURVES)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set1_sigalgs" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET1_SIGALGS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set_ecdh_auto(0,0)" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_ECDH_AUTO)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set_max_proto_version(0,0)" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_MAX_PROTO_VERSION)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_CTX_set_security_level" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_CTX_SET_SECURITY_LEVEL)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_ERROR_WANT_ASYNC" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_ERROR_WANT_ASYNC_JOB" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_ASYNC_JOB)
  CHECK_FUNCTIONWITHHEADER_EXISTS("SSL_ERROR_WANT_CLIENT_HELLO_CB" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_SSL_ERROR_WANT_CLIENT_HELLO_CB)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_3_RFC_AES_128_CCM_8_SHA256" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_128_CCM_8_SHA256)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_3_RFC_AES_256_GCM_SHA384" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_AES_256_GCM_SHA384)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_3_RFC_CHACHA20_POLY1305_SHA256" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_3_RFC_CHACHA20_POLY1305_SHA256)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_AES_256_CCM_8)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS1_TXT_ECDHE_RSA_WITH_CHACHA20_POLY1305)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TLS_method" "openssl/ssl.h" HAVE_OPENSSL_PROTOTYPE_TLS_METHOD)
  CHECK_FUNCTIONWITHHEADER_EXISTS("X509_STORE_CTX_get0_cert" "openssl/x509_vfy.h" HAVE_OPENSSL_PROTOTYPE_X509_STORE_CTX_GET0_CERT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("X509_STORE_get0_param" "openssl/x509.h" HAVE_OPENSSL_PROTOTYPE_X509_STORE_GET0_PARAM)
  CHECK_FUNCTIONWITHHEADER_EXISTS("X509_get_signature_nid" "openssl/x509.h" HAVE_OPENSSL_PROTOTYPE_X509_GET_SIGNATURE_NID)

  # test presence of functions, constants and macros needed for the dcmsign module
  CHECK_FUNCTIONWITHHEADER_EXISTS("ASN1_STRING_get0_data" "openssl/asn1.h" HAVE_OPENSSL_PROTOTYPE_ASN1_STRING_GET0_DATA)
  CHECK_FUNCTIONWITHHEADER_EXISTS("EVP_PKEY_get0_EC_KEY" "openssl/evp.h" HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET0_EC_KEY)
  CHECK_FUNCTIONWITHHEADER_EXISTS("EVP_PKEY_get_group_name" "openssl/evp.h" HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_GET_GROUP_NAME)
  CHECK_FUNCTIONWITHHEADER_EXISTS("EVP_PKEY_id" "openssl/evp.h" HAVE_OPENSSL_PROTOTYPE_EVP_PKEY_ID)
  CHECK_FUNCTIONWITHHEADER_EXISTS("OSSL_PROVIDER_load" "openssl/provider.h" HAVE_OPENSSL_PROTOTYPE_OSSL_PROVIDER_LOAD)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_STATUS_INFO_get0_failure_info" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_FAILURE_INFO)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_STATUS_INFO_get0_status" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_STATUS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_STATUS_INFO_get0_text" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_STATUS_INFO_GET0_TEXT)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_VERIFY_CTS_set_certs(0,0)" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTS_SET_CERTS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_VERIFY_CTX_set_data" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_DATA)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_VERIFY_CTX_set_flags" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_FLAGS)
  CHECK_FUNCTIONWITHHEADER_EXISTS("TS_VERIFY_CTX_set_store" "openssl/ts.h" HAVE_OPENSSL_PROTOTYPE_TS_VERIFY_CTX_SET_STORE)
  CHECK_FUNCTIONWITHHEADER_EXISTS("X509_get0_notAfter" "openssl/x509.h" HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTAFTER)
  CHECK_FUNCTIONWITHHEADER_EXISTS("X509_get0_notBefore" "openssl/x509.h" HAVE_OPENSSL_PROTOTYPE_X509_GET0_NOTBEFORE)

  # check if type EVP_MD_CTX is defined as typedef for "struct evp_md_ctx_st" (new) or "struct env_md_ctx_st" (old)
  CHECK_FUNCTIONWITHHEADER_EXISTS("struct evp_md_ctx_st *a; EVP_MD_CTX *b=a" "openssl/evp.h" HAVE_OPENSSL_DECLARATION_NEW_EVP_MD_CTX)

  # check if the first parameter passed to X509_ALGOR_get0() should be "const ASN1_OBJECT **" (new) or "ASN1_OBJECT **" (old)
  CHECK_FUNCTIONWITHHEADER_EXISTS("const ASN1_OBJECT *a; X509_ALGOR_get0(&a,0,0,0)" "openssl/x509.h" HAVE_OPENSSL_X509_ALGOR_GET0_CONST_PARAM)

  # restore previous value of CMAKE_REQUIRED_LIBRARIES
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_TEMP})
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
# generator expression $<CONFIG>
#-----------------------------------------------------------------------------
set(DCMTK_CONFIG_GENERATOR_EXPRESSION "$<CONFIG>" CACHE INTERNAL "the generator expression to use for retrieving the current config")
