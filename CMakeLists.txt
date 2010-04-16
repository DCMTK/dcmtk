# declare project
PROJECT(dcmtk)

# check for Windows operating system
IF(NOT WIN32)
  MESSAGE(FATAL_ERROR "Please use GNU autoconf for Unix systems.")
ENDIF(NOT WIN32)

# check for Microsoft compiler
IF(NOT MSVC)
  MESSAGE("Warning: This package has only been tested with Microsoft compilers.")
ENDIF(NOT MSVC)

# check required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.8)
IF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.1)
  SET(CMAKE_BACKWARDS_COMPATIBILITY 2.8.1 CACHE STRING "Latest version of CMake when this project was released." FORCE)
ENDIF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.8.1)

IF(COMMAND CMAKE_POLICY)
  # Works around warnings about libraries linked against that don't have absolute paths
  CMAKE_POLICY(SET CMP0003 OLD)
  # Works around warnings about escaped quotes in ADD_DEFINITIONS statements
  CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(COMMAND CMAKE_POLICY)

# package information
SET(PACKAGE_NAME "dcmtk")
SET(PACKAGE_DATE "CVS")
SET(PACKAGE_VERSION "3.5.5")
SET(PACKAGE_VERSION_NUMBER "355")
SET(PACKAGE_VERSION_SUFFIX "")
SET(PACKAGE_TARNAME "${PACKAGE_NAME}-${PACKAGE_VERSION}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
SET(PACKAGE_BUGREPORT "dicom-bugs@offis.de")

# default build type
SET(CMAKE_BUILD_TYPE Release)

# various CMake-specific settings
SET(CMAKE_USE_RELATIVE_PATHS ON)
SET(CMAKE_SUPPRESS_REGENERATION ON)
MARK_AS_ADVANCED(FORCE EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# define user configurable options and paths to external libraries which might be used

# installation path prefix
GET_FILENAME_COMPONENT(INSTALL_DIR "${dcmtk_SOURCE_DIR}/../${PACKAGE_TARNAME}-win32-i386" ABSOLUTE)
SET(INSTALL_PREFIX "${INSTALL_DIR}" CACHE PATH "Installation path prefix.")

# shared libraries (option not yet available)
#OPTION(WITH_SHARED_LIBS "Build with shared libraries." OFF)
SET(WITH_SHARED_LIBS OFF)
SET(BUILD_SHARED_LIBS ${WITH_SHARED_LIBS})

# doxygen support
FIND_PROGRAM(DOXYGEN_EXECUTABLE "doxygen")

# private tag dictionary
OPTION(WITH_PRIVATE_TAGS "Build with private tag dictionary." OFF)

# libxml support
OPTION(WITH_LIBXML "Build with libxml support." OFF)
GET_FILENAME_COMPONENT(LIBXML_DIR "${dcmtk_SOURCE_DIR}/../libxml2-2.6.26" ABSOLUTE)
FIND_PATH(WITH_LIBXMLINC "include/libxml/parser.h" "${LIBXML_DIR}" NO_DEFAULT_PATH)
# libpng support
OPTION(WITH_LIBPNG "Build with libpng support." ON)
GET_FILENAME_COMPONENT(LIBPNG_DIR "${dcmtk_SOURCE_DIR}/../libpng-1.2.8" ABSOLUTE)
FIND_PATH(WITH_LIBPNGINC "include/png.h" "${LIBPNG_DIR}" NO_DEFAULT_PATH)
# libtiff support
OPTION(WITH_LIBTIFF "Build with libtiff support." ON)
GET_FILENAME_COMPONENT(LIBTIFF_DIR "${dcmtk_SOURCE_DIR}/../tiff-3.8.2" ABSOLUTE)
FIND_PATH(WITH_LIBTIFFINC "include/tiff.h" "${LIBTIFF_DIR}" NO_DEFAULT_PATH)
# OpenSSL support
OPTION(WITH_OPENSSL "Build with OpenSSL support." OFF)
GET_FILENAME_COMPONENT(OPENSSL_DIR "${dcmtk_SOURCE_DIR}/../openssl-0.9.8a" ABSOLUTE)
FIND_PATH(WITH_OPENSSLINC "include/openssl/ssl.h" "${OPENSSL_DIR}" NO_DEFAULT_PATH)
# zlib support
OPTION(WITH_ZLIB "Build with zlib support." ON)
GET_FILENAME_COMPONENT(ZLIB_DIR "${dcmtk_SOURCE_DIR}/../zlib-1.2.3" ABSOLUTE)
FIND_PATH(WITH_ZLIBINC "include/zlib.h" "${ZLIB_DIR}" NO_DEFAULT_PATH)
# sndfile support
OPTION(WITH_SNDFILE "Build with sndfile support." ON)
GET_FILENAME_COMPONENT(SNDFILE_DIR "${dcmtk_SOURCE_DIR}/../libsndfile-1.0.17" ABSOLUTE)
FIND_PATH(WITH_SNDFILEINC "sndfile.h" "${SNDFILE_DIR}" NO_DEFAULT_PATH)

# set project wide flags for compiler and linker

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
    IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008")
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy /EHsc")
      SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
      SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
      SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
      SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008")
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
    IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy /EHsc")
      SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
      SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
      SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
      SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005" OR CMAKE_GENERATOR STREQUAL "Visual Studio 9 2008")
  ENDIF(NOT WITH_SHARED_LIBS)
ENDIF(CMAKE_CXX_COMPILER MATCHES "CL|cl")

# settings for Borland C++
IF(CMAKE_CXX_COMPILER MATCHES "BCC32|bcc32")
  # further settings required?
  SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
ENDIF(CMAKE_CXX_COMPILER MATCHES "BCC32|bcc32")

# set general compiler flags
ADD_DEFINITIONS(-D_REENTRANT)

# pass optional build date to compiler
#ADD_DEFINITIONS("-DDCMTK_BUILD_DATE=\\\"YYYY-MM-DD\\\"")

# set precompiler switches, directories and libraries
# according to optional settings

# libxml support
IF(WITH_LIBXML AND WITH_LIBXMLINC)
  SET(LIBXML_INCDIR ${WITH_LIBXMLINC}/include)
  SET(LIBXML_LIBDIR ${WITH_LIBXMLINC}/lib)
  SET(LIBXML_LIBS debug libxml2_d optimized libxml2_o debug iconv_d optimized iconv_o)
ELSE(WITH_LIBXML AND WITH_LIBXMLINC)
  SET(WITH_LIBXML OFF)
ENDIF(WITH_LIBXML AND WITH_LIBXMLINC)
# libpng support
IF(WITH_LIBPNG AND WITH_LIBPNGINC)
  SET(LIBPNG_INCDIR ${WITH_LIBPNGINC}/include)
  SET(LIBPNG_LIBDIR ${WITH_LIBPNGINC}/lib)
  SET(LIBPNG_LIBS debug libpng_d optimized libpng_o)
ELSE(WITH_LIBPNG AND WITH_LIBPNGINC)
  SET(WITH_LIBPNG OFF)
ENDIF(WITH_LIBPNG AND WITH_LIBPNGINC)
# libtiff support
IF(WITH_LIBTIFF AND WITH_LIBTIFFINC)
  SET(LIBTIFF_INCDIR ${WITH_LIBTIFFINC}/include)
  SET(LIBTIFF_LIBDIR ${WITH_LIBTIFFINC}/lib)
  SET(LIBTIFF_LIBS debug libtiff_d optimized libtiff_o)
ELSE(WITH_LIBTIFF AND WITH_LIBTIFFINC)
  SET(WITH_LIBTIFF OFF)
ENDIF(WITH_LIBTIFF AND WITH_LIBTIFFINC)
# OpenSSL support
IF(WITH_OPENSSL AND WITH_OPENSSLINC)
  SET(OPENSSL_BINDIR ${WITH_OPENSSLINC}/bin)
  SET(OPENSSL_INCDIR ${WITH_OPENSSLINC}/include)
  SET(OPENSSL_LIBDIR ${WITH_OPENSSLINC}/lib)
  SET(OPENSSL_LIBS debug dcmtkssl_d.lib optimized dcmtkssl_o.lib debug dcmtkeay_d.lib optimized dcmtkeay_o.lib)
ELSE(WITH_OPENSSL AND WITH_OPENSSLINC)
  SET(WITH_OPENSSL OFF)
ENDIF(WITH_OPENSSL AND WITH_OPENSSLINC)
# zlib support
IF(WITH_ZLIB AND WITH_ZLIBINC)
  SET(ZLIB_INCDIR ${WITH_ZLIBINC}/include)
  SET(ZLIB_LIBDIR ${WITH_ZLIBINC}/lib)
  SET(ZLIB_LIBS debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB AND WITH_ZLIBINC)
  SET(WITH_ZLIB OFF)
ENDIF(WITH_ZLIB AND WITH_ZLIBINC)
# sndfile support
IF(WITH_SNDFILE AND WITH_SNDFILEINC)
  SET(SNDFILE_INCDIR ${WITH_SNDFILEINC}/include)
  SET(SNDFILE_LIBDIR ${WITH_SNDFILEINC})
  SET(SNDFILE_LIBS debug libsndfile-1 optimized libsndfile-1)
ELSE(WITH_SNDFILE AND WITH_SNDFILEINC)
  SET(WITH_SNDFILE OFF)
ENDIF(WITH_SNDFILE AND WITH_SNDFILEINC)

# specify installation prefix
SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
MARK_AS_ADVANCED(FORCE CMAKE_INSTALL_PREFIX)

# specify installation sub-directories
SET(INSTALL_BINDIR "/bin")
SET(INSTALL_ETCDIR "/etc/dcmtk")
SET(INSTALL_DATDIR "/share/dcmtk")
SET(INSTALL_DOCDIR "/share/doc/dcmtk")
SET(INSTALL_HTMDIR "/share/doc/dcmtk/html")
SET(INSTALL_INCDIR "/include")
SET(INSTALL_LIBDIR "/lib")
SET(INSTALL_MANDIR "/man")

# generate native Windows paths
STRING(REPLACE "/" "\\\\" NATIVE_INSTALL_PREFIX ${INSTALL_PREFIX})
STRING(REPLACE "/" "\\\\" NATIVE_INSTALL_ETCDIR ${INSTALL_ETCDIR})
STRING(REPLACE "/" "\\\\" NATIVE_INSTALL_DATDIR ${INSTALL_DATDIR})

# specify default data dictionaries
SET(DEFAULT_DATADICT_PATH "@NATIVE_INSTALL_PREFIX@@NATIVE_INSTALL_DATDIR@\\\\dicom.dic")
IF(WITH_PRIVATE_TAGS)
  SET(DEFAULT_DATADICT_PATH ${DEFAULT_DATADICT_PATH} "@NATIVE_INSTALL_PREFIX@@NATIVE_INSTALL_DATDIR@\\\\private.dic")
ENDIF(WITH_PRIVATE_TAGS)

# declare installation files
INSTALL_FILES(${INSTALL_DOCDIR} FILES COPYRIGHT FAQ HISTORY VERSION ANNOUNCE.355 CHANGES.355)

# declare include directories which hold for all subdirectories and subsubdirectories
INCLUDE_DIRECTORIES(${dcmtk_SOURCE_DIR}/config/include)

# recurse into subdirectories
SUBDIRS(config ofstd oflog dcmdata dcmtls dcmnet dcmqrdb dcmimgle dcmimage dcmjpeg dcmjpls dcmsign dcmsr dcmpstat dcmwlm doxygen)
# add non-public modules
#SUBDIRS(dcmdbsup dcmwlmxl dcmpps dcmppswm dcmjp2k dcmloco dcmqrdbx dcmcheck dcmprint dcmprscp dcmavi dcmmisc dcmppscu dcmrt dcmwave)
