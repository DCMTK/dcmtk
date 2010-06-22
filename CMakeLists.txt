# Declare project
PROJECT(DCMTK)

# Minimum CMake version required
cmake_minimum_required(VERSION 2.6)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

#  Modules to be built (default: build all public modules)
SET(DCMTK_MODULES ofstd oflog dcmdata dcmimgle dcmjpeg dcmimage dcmtls dcmnet dcmwlm dcmqrdb dcmsr dcmpstat dcmsign dcmjpls)

# Package "release" settings
SET(DCMTK_PACKAGE_NAME "DCMTK")
SET(DCMTK_PACKAGE_VERSION "3.5.5")
SET(DCMTK_PACKAGE_VERSION_NUMBER "355")
SET(DCMTK_PACKAGE_VERSION_SUFFIX "")
SET(DCMTK_PACKAGE_DATE "20100226")
SET(DCMTK_DEFAULT_CONFIGURATION_DIR "")
SET(DCMTK_DEFAULT_SUPPORT_DATA_DIR "")

# DCMTK build options
OPTION(DCMTK_WITH_TIFF "Support for TIFF" ON)
OPTION(DCMTK_WITH_PNG "Support for PNG" ON)
OPTION(DCMTK_WITH_XML "Support for XML" OFF)
OPTION(DCMTK_WITH_ZLIB "Support for ZLIB" ON)
OPTION(DCMTK_WITH_OPENSSL "Support for OPENSSL" OFF)
OPTION(DCMTK_WITH_SNDFILE "Support for SNDFILE" OFF)
MARK_AS_ADVANCED(SNDFILE_DIR DCMTK_WITH_SNDFILE) # not yet needed in public DCMTK
OPTION(DCMTK_WITH_PRIVATE_TAGS "Support for DICOM private tags" OFF)
OPTION(DCMTK_WITH_THREADS "Support for multi-threading" ON)


#SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})


#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------

SET(DCMTK_INCLUDE_DIR
  ${DCMTK_BINARY_DIR}/include
  #${DCMTK_SOURCE_DIR}/config/include
  ${DCMTK_SOURCE_DIR}/dcmimgle/include
  ${DCMTK_SOURCE_DIR}/ofstd/include
  ${DCMTK_SOURCE_DIR}/oflog/include
  ${DCMTK_SOURCE_DIR}/dcmdata/include
  ${DCMTK_SOURCE_DIR}/dcmimage/include
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg8
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg12
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg16
  ${DCMTK_SOURCE_DIR}/dcmjpeg/include
  ${DCMTK_SOURCE_DIR}/dcmnet/include
  ${DCMTK_SOURCE_DIR}/dcmpstat/include
  ${DCMTK_SOURCE_DIR}/dcmsr/include
  ${DCMTK_SOURCE_DIR}/dcmqrdb/include
  ${DCMTK_SOURCE_DIR}/dcmsign/include
  ${DCMTK_SOURCE_DIR}/dcmwlm/include
  ${DCMTK_SOURCE_DIR}/dcmtls/include
  ${DCMTK_SOURCE_DIR}/dcmnet/include
  ${DCMTK_SOURCE_DIR}/dcmqrdb/include
  ${DCMTK_SOURCE_DIR}/dcmsign/include
  ${DCMTK_SOURCE_DIR}/dcmsr/include
  ${DCMTK_SOURCE_DIR}/dcmwlm/include
  ${DCMTK_SOURCE_DIR}/dcmpstat/include
)
INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})


#-----------------------------------------------------------------------------
# Installation sub-directories
#-----------------------------------------------------------------------------

SET(INSTALL_BINDIR "/bin")
SET(INSTALL_INCDIR "/include")
SET(INSTALL_LIBDIR "/lib")
SET(INSTALL_ETCDIR "/etc")
SET(INSTALL_DATDIR "/share")
SET(INSTALL_DOCDIR "/share/doc/dcmtk-${DCMTK_PACKAGE_VERSION}")
SET(INSTALL_HTMDIR "/share/doc/dcmtk-${DCMTK_PACKAGE_VERSION}/html")
SET(INSTALL_MANDIR "/share/man")

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
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /GX /Gy /YX")
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

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings
IF(WIN32)
  IF(NOT BORLAND)
    IF(NOT CYGWIN)
      IF(NOT MINGW)
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
      ENDIF(NOT MINGW)
    ENDIF(NOT CYGWIN)
  ENDIF(NOT BORLAND)
ENDIF(WIN32)

IF( WIN32 )
  SET(WIN32_STD_LIBRARIES ws2_32 netapi32 wsock32)
ENDIF( WIN32 )

# settings for Apple
IF(APPLE)
  # Need to fix ofstd/libsrc/ofthread.cxx
  MESSAGE(STATUS "APPLE - FORCE THREADS OFF") # semaphores must be named in OS X!
  SET(DCMTK_WITH_THREADS OFF CACHE BOOL "" FORCE)
ENDIF(APPLE)

# setting for IA64 / x86_64
IF(UNIX)
  IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
ENDIF(UNIX)

# settings for Borland C++
IF(CMAKE_CXX_COMPILER MATCHES bcc32)
  # further settings required?
  SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
ENDIF(CMAKE_CXX_COMPILER MATCHES bcc32)

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
# ENABLE_TESTING()
# ENDIF(BUILD_TESTING)

#-----------------------------------------------------------------------------
# Third party libraries
#-----------------------------------------------------------------------------

INCLUDE(CMake/3rdparty.cmake)

#-----------------------------------------------------------------------------
# Thread support
#-----------------------------------------------------------------------------

# See dcmtk-3.5.4/config/configure.in
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
ENDFOREACH()

#-----------------------------------------------------------------------------
# Installation tasks
#-----------------------------------------------------------------------------

# Install global headers

INSTALL_FILES(${INSTALL_INCDIR}/dcmtk/config FILES ${CMAKE_CURRENT_BINARY_DIR}/include/dcmtk/config/osconfig.h)


# Install headers and static libraries
FOREACH(lib ${DCMTK_LIBS})
  FILE(GLOB headers_to_install "${CMAKE_CURRENT_SOURCE_DIR}/${lib}/include/dcmtk/${lib}/*.h")
  INSTALL(FILES
    ${headers_to_install}
    DESTINATION ${DCMTK_INSTALL_INCLUDE_DIR}/dcmtk/${lib} COMPONENT Development
  )
ENDFOREACH()
