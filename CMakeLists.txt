# declare project
PROJECT(dcmtk)

# check for Windows operating system
IF(NOT WIN32)
  MESSAGE(FATAL_ERROR "Please use GNU autoconf for Unix systems.")
ENDIF(NOT WIN32)

# check required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
IF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.0.6)
  SET(CMAKE_BACKWARDS_COMPATIBILITY 2.0.6 CACHE STRING "Latest version of CMake when this project was released." FORCE)
ENDIF(CMAKE_BACKWARDS_COMPATIBILITY GREATER 2.0.6)

# default build type
SET(CMAKE_BUILD_TYPE RELEASE)

# various CMake-specific settings
SET(CMAKE_USE_RELATIVE_PATHS ON)
SET(CMAKE_SUPPRESS_REGENERATION ON)
MARK_AS_ADVANCED(FORCE EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

# define user configurable options and paths to external libraries which might be used

# installation path prefix
SET(INSTALL_PREFIX "${dcmtk_SOURCE_DIR}/../dcmtk-3.5.4-win32-i386/" CACHE PATH "Installation path prefix.")

# shared libraries (option not yet available)
#OPTION(WITH_SHARED_LIBS "Build with shared libraries." OFF)
SET(WITH_SHARED_LIBS OFF)
SET(BUILD_SHARED_LIBS ${WITH_SHARED_LIBS})

# doxygen support
FIND_PROGRAM(DOXYGEN_EXECUTABLE "doxygen")

# libxml support
OPTION(WITH_LIBXML "Build with libxml support." OFF)
FIND_PATH(WITH_LIBXMLINC "include/libxml/parser.h" "${dcmtk_SOURCE_DIR}/../libxml2-2.6.26")
# libpng support
OPTION(WITH_LIBPNG "Build with libpng support." ON)
FIND_PATH(WITH_LIBPNGINC "include/png.h" "${dcmtk_SOURCE_DIR}/../libpng-1.2.8")
# libtiff support
OPTION(WITH_LIBTIFF "Build with libtiff support." ON)
FIND_PATH(WITH_LIBTIFFINC "include/tiff.h" "${dcmtk_SOURCE_DIR}/../tiff-3.7.4")
# openssl support
OPTION(WITH_OPENSSL "Build with openssl support." OFF)
FIND_PATH(WITH_OPENSSLINC "include/openssl/ssl.h" "${dcmtk_SOURCE_DIR}/../openssl-0.9.8a")
# zlib support
OPTION(WITH_ZLIB "Build with zlib support." ON)
FIND_PATH(WITH_ZLIBINC "include/zlib.h" "${dcmtk_SOURCE_DIR}/../zlib-1.2.3")

# set project wide flags for compiler and linker

# settings for Microsoft Visual C
IF(CMAKE_C_COMPILER MATCHES cl)
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
    IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
      SET(CMAKE_C_FLAGS "/nologo /W3 /Gy")
      SET(CMAKE_C_FLAGS_DEBUG "/MTd /Z7 /Od")
      SET(CMAKE_C_FLAGS_RELEASE "/MT /O2")
      SET(CMAKE_C_FLAGS_MINSIZEREL "/MT /O2")
      SET(CMAKE_C_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
  ENDIF(NOT WITH_SHARED_LIBS)
ENDIF(CMAKE_C_COMPILER MATCHES cl)

# settings for Microsoft Visual C++
IF(CMAKE_CXX_COMPILER MATCHES cl)
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
    IF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
      SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
      SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
      SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
      SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
      SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
    ENDIF(CMAKE_GENERATOR STREQUAL "Visual Studio 8 2005")
  ENDIF(NOT WITH_SHARED_LIBS)
ENDIF(CMAKE_CXX_COMPILER MATCHES cl)

# settings for Borland C++
IF(CMAKE_CXX_COMPILER MATCHES bcc32)
  # further settings required?
  SET(CMAKE_STANDARD_LIBRARIES "import32.lib cw32mt.lib")
ENDIF(CMAKE_CXX_COMPILER MATCHES bcc32)

# set general compiler flags
ADD_DEFINITIONS(-D_REENTRANT)

# set precompiler switches, directories and libraries
# according to optional settings

# libxml support
IF(WITH_LIBXML AND WITH_LIBXMLINC)
  ADD_DEFINITIONS(-DWITH_LIBXML)
  SET(LIBXML_INCDIR ${WITH_LIBXMLINC}/include)
  SET(LIBXML_LIBDIR ${WITH_LIBXMLINC}/lib)
  SET(LIBXML_LIBS debug libxml2_d optimized libxml2_o debug iconv_d optimized iconv_o)
ENDIF(WITH_LIBXML AND WITH_LIBXMLINC)
# libpng support
IF(WITH_LIBPNG AND WITH_LIBPNGINC)
  ADD_DEFINITIONS(-DWITH_LIBPNG)
  SET(LIBPNG_INCDIR ${WITH_LIBPNGINC}/include)
  SET(LIBPNG_LIBDIR ${WITH_LIBPNGINC}/lib)
  SET(LIBPNG_LIBS debug libpng_d optimized libpng_o)
ENDIF(WITH_LIBPNG AND WITH_LIBPNGINC)
# libtiff support
IF(WITH_LIBTIFF AND WITH_LIBTIFFINC)
  ADD_DEFINITIONS(-DWITH_LIBTIFF)
  SET(LIBTIFF_INCDIR ${WITH_LIBTIFFINC}/include)
  SET(LIBTIFF_LIBDIR ${WITH_LIBTIFFINC}/lib)
  SET(LIBTIFF_LIBS debug libtiff_d optimized libtiff_o)
ENDIF(WITH_LIBTIFF AND WITH_LIBTIFFINC)
# openssl support
IF(WITH_OPENSSL AND WITH_OPENSSLINC)
  ADD_DEFINITIONS(-DWITH_OPENSSL)
  SET(OPENSSL_BINDIR ${WITH_OPENSSLINC}/bin)
  SET(OPENSSL_INCDIR ${WITH_OPENSSLINC}/include)
  SET(OPENSSL_LIBDIR ${WITH_OPENSSLINC}/lib)
  SET(OPENSSL_LIBS debug dcmtkssl_d.lib optimized dcmtkssl_o.lib debug dcmtkeay_d.lib optimized dcmtkeay_o.lib)
ENDIF(WITH_OPENSSL AND WITH_OPENSSLINC)
# zlib support
IF(WITH_ZLIB AND WITH_ZLIBINC)
  ADD_DEFINITIONS(-DWITH_ZLIB)
  SET(ZLIB_INCDIR ${WITH_ZLIBINC}/include)
  SET(ZLIB_LIBDIR ${WITH_ZLIBINC}/lib)
  SET(ZLIB_LIBS debug zlib_d optimized zlib_o)
ENDIF(WITH_ZLIB AND WITH_ZLIBINC)

# specify installation prefix
SET(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})

# specify installation sub-directories
SET(INSTALL_BINDIR "/bin")
SET(INSTALL_ETCDIR "/etc")
SET(INSTALL_DATDIR "/share/data")
SET(INSTALL_DOCDIR "/share/doc")
SET(INSTALL_HTMDIR "/share/doc/html")
SET(INSTALL_INCDIR "/include")
SET(INSTALL_LIBDIR "/lib")
SET(INSTALL_MANDIR "/man")

# declare installation files
INSTALL_FILES(${INSTALL_DOCDIR} FILES COPYRIGHT FAQ HISTORY)

# declare include directories which hold for all subdirectories and subsubdirectories
INCLUDE_DIRECTORIES(${dcmtk_SOURCE_DIR}/config/include)

# recurse into subdirectories
SUBDIRS(config ofstd dcmdata dcmtls dcmnet dcmqrdb dcmimgle dcmimage dcmjpeg dcmsign dcmsr dcmpstat dcmwlm doxygen)
# add non-public modules
#SUBDIRS(dcmdbsup dcmwlmxl dcmpps dcmppswm dcmjp2k dcmqrdbx dcmcheck dcmprint dcmavi dcmmisc)
