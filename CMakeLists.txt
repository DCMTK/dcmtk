# Declare project
PROJECT(DCMTK)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.3)
  SET(CMAKE_BACKWARDS_COMPATIBILITY 2.8.3 CACHE STRING "Latest version of CMake when this project was released." FORCE)
ENDIF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.3)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

# Modules to be built (default: build all public modules)
SET(DCMTK_MODULES config ofstd oflog dcmdata dcmimgle dcmjpeg dcmimage dcmtls dcmnet dcmwlm dcmqrdb dcmsr dcmpstat dcmsign dcmjpls doxygen)
# Add non-public modules
#SET(DCMTK_MODULES ${DCMTK_MODULES} dcmdbsup dcmwlmxl dcmpps dcmppswm dcmjp2k dcmloco dcmqrdbx dcmcheck dcmprint dcmprscp dcmavi dcmmisc dcmppscu dcmstcom dcmrt dcmwave dcmeval)

# Package "release" settings (some are currently unused)
SET(DCMTK_PACKAGE_NAME "dcmtk")
SET(DCMTK_PACKAGE_DATE "2011-01-06")
#SET(DCMTK_PACKAGE_TARNAME "dcmtk-3.6.0")
SET(DCMTK_PACKAGE_VERSION "3.6.0")
SET(DCMTK_PACKAGE_VERSION_NUMBER "360")
SET(DCMTK_PACKAGE_VERSION_SUFFIX "")
#SET(DCMTK_PACKAGE_STRING "dcmtk 3.6.0")
#SET(DCMTK_PACKAGE_BUGREPORT "dicom-bugs@offis.de")

# DCMTK build options
OPTION(DCMTK_WITH_TIFF "Configure DCMTK with support for TIFF" ON)
OPTION(DCMTK_WITH_PNG "Configure DCMTK with support for PNG" ON)
OPTION(DCMTK_WITH_XML "Configure DCMTK with support for XML" OFF)
OPTION(DCMTK_WITH_ZLIB "Configure DCMTK with support for ZLIB" ON)
OPTION(DCMTK_WITH_OPENSSL "Configure DCMTK with support for OPENSSL" OFF)
OPTION(DCMTK_WITH_SNDFILE "Configure DCMTK with support for SNDFILE" OFF)
MARK_AS_ADVANCED(SNDFILE_DIR DCMTK_WITH_SNDFILE) # not yet needed in public DCMTK
OPTION(DCMTK_WITH_PRIVATE_TAGS "Configure DCMTK with support for DICOM private tags coming with DCMTK" OFF)
OPTION(DCMTK_WITH_THREADS "Configure DCMTK with support for multi-threading" ON)
OPTION(DCMTK_WITH_DOXYGEN "Build API documentation with doxygen" ON)

#SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------

SET(DCMTK_INCLUDE_DIR
  ${DCMTK_BINARY_DIR}/include
  #${DCMTK_SOURCE_DIR}/config/include
  ${DCMTK_SOURCE_DIR}/ofstd/include
  ${DCMTK_SOURCE_DIR}/oflog/include
  ${DCMTK_SOURCE_DIR}/dcmdata/include
  ${DCMTK_SOURCE_DIR}/dcmimgle/include
  ${DCMTK_SOURCE_DIR}/dcmimage/include
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg8
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg12
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg16
  ${DCMTK_SOURCE_DIR}/dcmjpeg/include
  ${DCMTK_SOURCE_DIR}/dcmjpls/include
  ${DCMTK_SOURCE_DIR}/dcmnet/include
  ${DCMTK_SOURCE_DIR}/dcmpstat/include
  ${DCMTK_SOURCE_DIR}/dcmqrdb/include
  ${DCMTK_SOURCE_DIR}/dcmsign/include
  ${DCMTK_SOURCE_DIR}/dcmsr/include
  ${DCMTK_SOURCE_DIR}/dcmtls/include
  ${DCMTK_SOURCE_DIR}/dcmwlm/include
)
INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Installation sub-directories
#-----------------------------------------------------------------------------

SET(INSTALL_BINDIR "/bin")
SET(INSTALL_INCDIR "/include")
SET(INSTALL_LIBDIR "/lib")
SET(INSTALL_ETCDIR "/etc/dcmtk")
SET(INSTALL_DATDIR "/share/dcmtk")
SET(INSTALL_DOCDIR "/share/doc/dcmtk")
SET(INSTALL_HTMDIR "/share/doc/dcmtk/html")
SET(INSTALL_MANDIR "/share/man")

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

#-----------------------------------------------------------------------------
# Platform-specific settings
#-----------------------------------------------------------------------------

# set project wide flags for compiler and linker

IF(WIN32)
  OPTION(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS "Overwrite compiler flags with DCMTK's WIN32 package default values." ON)
ENDIF(WIN32)

IF(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS)

  # settings for Microsoft Visual C
  IF(CMAKE_C_COMPILER MATCHES "CL|cl")
    IF(NOT WITH_SHARED_LIBS)
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 6")
        SET(CMAKE_C_FLAGS "/nologo /W3 /GX /Gy /YX")
        SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 6")
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 7" OR CMAKE_GENERATOR STREQUAL "Visual Studio 7 .NET 2003")
        SET(CMAKE_C_FLAGS "/nologo /W3 /Gy")
        SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 7" OR CMAKE_GENERATOR STREQUAL "Visual Studio 7 .NET 2003")
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008" OR CMAKE_GENERATOR STREQUAL "Visual Studio 10")
        SET(CMAKE_C_FLAGS "/nologo /W3 /Gy /EHsc")
        SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008" OR CMAKE_GENERATOR STREQUAL "Visual Studio 10")
    ENDIF(NOT WITH_SHARED_LIBS)
  ENDIF(CMAKE_C_COMPILER MATCHES "CL|cl")

  # settings for Microsoft Visual C++
  IF(CMAKE_CXX_COMPILER MATCHES "CL|cl")
    IF(NOT WITH_SHARED_LIBS)
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 6")
        SET(CMAKE_CXX_FLAGS "/nologo /W3 /GX /Gy /YX /Zm500") # /Zm500 increments heap size which is needed on some system to compile templates in dcmimgle
        SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 6")
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 7" OR CMAKE_GENERATOR STREQUAL "Visual Studio 7 .NET 2003")
        SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
        SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 7" OR CMAKE_GENERATOR STREQUAL "Visual Studio 7 .NET 2003")
      IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008" OR CMAKE_GENERATOR STREQUAL "Visual Studio 10")
        SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy /EHsc")
        SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
        SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
        SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
      ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008" OR CMAKE_GENERATOR STREQUAL "Visual Studio 10")
    ENDIF(NOT WITH_SHARED_LIBS)
  ENDIF(CMAKE_CXX_COMPILER MATCHES "CL|cl")

  # settings for Borland C++
  IF(CMAKE_CXX_COMPILER MATCHES "BCC32|bcc32")
    # further settings required?
    SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
  ENDIF(CMAKE_CXX_COMPILER MATCHES "BCC32|bcc32")

ENDIF(DCMTK_OVERWRITE_WIN32_COMPILER_FLAGS)

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

ENDIF(WIN32)

# define libraries that must be linked to most Windows applications
IF(WIN32)
  SET(WIN32_STD_LIBRARIES ws2_32 netapi32 wsock32)
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

INCLUDE(CMake/3rdparty.cmake)

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# add the osconfig.h.in file
INCLUDE(${DCMTK_SOURCE_DIR}/CMake/GenerateDCMTKConfigure.cmake)
CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/osconfig.h.in
               ${DCMTK_BINARY_DIR}/include/dcmtk/config/osconfig.h)

#-----------------------------------------------------------------------------
# Dart configuration (disabled per default)
#-----------------------------------------------------------------------------

# Includes build targets Experimental, Nightly and Continous which are the standard
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

#-----------------------------------------------------------------------------
# Start actual compilation tasks
#-----------------------------------------------------------------------------

# Recurse into subdirectories
FOREACH(module ${DCMTK_MODULES})
  ADD_SUBDIRECTORY(${module})
ENDFOREACH(module)

#-----------------------------------------------------------------------------
# Installation tasks
#-----------------------------------------------------------------------------

# Install global headers
INSTALL_FILES(${INSTALL_INCDIR}/dcmtk/config FILES ${CMAKE_CURRENT_BINARY_DIR}/include/dcmtk/config/osconfig.h)

# Install DCMTK's general documentation files
INSTALL_FILES(${INSTALL_DOCDIR} FILES ANNOUNCE.${DCMTK_PACKAGE_VERSION_NUMBER} CHANGES.${DCMTK_PACKAGE_VERSION_NUMBER} COPYRIGHT FAQ HISTORY VERSION)
