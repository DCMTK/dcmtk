# declare project
PROJECT(dcmtk)

# define user configurable options and paths to external libraries which might be used
# libxml support
OPTION(WITH_LIBXML "Turn support for libxml on/off" OFF)
FIND_PATH(WITH_LIBXMLINC "include/libxml/parser.h" ${dcmtk_SOURCE_DIR}/../libxml2-2.5.7)
# libpng support
OPTION(WITH_LIBPNG "Turn support for libpng on/off" ON)
FIND_PATH(WITH_LIBPNGINC "include/png.h" ${dcmtk_SOURCE_DIR}/../libpng-1.2.5)
# libtiff support
OPTION(WITH_LIBTIFF "Turn support for libtiff on/off" ON)
FIND_PATH(WITH_LIBTIFFINC "include/tiff.h" ${dcmtk_SOURCE_DIR}/../tiff-3.6.1)
# openssl support
OPTION(WITH_OPENSSL "Turn support for openssl on/off" OFF)
FIND_PATH(WITH_OPENSSLINC "include/openssl/ssl.h" ${dcmtk_SOURCE_DIR}/../openssl-0.9.7d)
# zlib support
OPTION(WITH_ZLIB "Turn support for zlib on/off" ON)
FIND_PATH(WITH_ZLIBINC "include/zlib.h" ${dcmtk_SOURCE_DIR}/../zlib-1.2.1)

# set certain project wide flags
SET(CMAKE_C_FLAGS "" CACHE STRING "Flags for C compiler." FORCE)
SET(CMAKE_C_FLAGS_DEBUG "/nologo /MTd /W3 /GX /Z7 /Od /GR- /YX" CACHE STRING "Win32 Debug Options C Code" FORCE)
SET(CMAKE_C_FLAGS_RELEASE "/nologo /MT /W3 /GX /O2 /GR- /YX" CACHE STRING "Win32 Release Options C Code" FORCE)
SET(CMAKE_CXX_FLAGS "" CACHE STRING "Flags used by the compiler during all build types." FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "/nologo /MTd /W3 /GX /Z7 /Od /GR- /YX" CACHE STRING "Win32 Debug Options C++ Code" FORCE)
SET(CMAKE_CXX_FLAGS_RELEASE "/nologo /MT /W3 /GX /O2 /GR- /YX" CACHE STRING "Win32 Release Options C++ Code" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL:NO" CACHE STRING "Win32 Linker Flags" FORCE)

# set _REENTRANT flag
ADD_DEFINITIONS(-D_REENTRANT)

# set precompiler switches according to settings
# libxml support
IF(WITH_LIBXML)
  ADD_DEFINITIONS(-DWITH_LIBXML)
ENDIF(WITH_LIBXML)
# libpng support
IF(WITH_LIBPNG)
  ADD_DEFINITIONS(-DWITH_LIBPNG)
ENDIF(WITH_LIBPNG)
# libtiff support
IF(WITH_LIBTIFF)
  ADD_DEFINITIONS(-DWITH_LIBTIFF)
ENDIF(WITH_LIBTIFF)
# openssl support
IF(WITH_OPENSSL)
  ADD_DEFINITIONS(-DWITH_OPENSSL)
ENDIF(WITH_OPENSSL)
# zlib support
IF(WITH_ZLIB)
  ADD_DEFINITIONS(-DWITH_ZLIB)
ENDIF(WITH_ZLIB)

# declare include directories which hold for all subdirectories and subsubdirectories
INCLUDE_DIRECTORIES(${dcmtk_SOURCE_DIR}/config/include)

# recurse into subdirectories
SUBDIRS(ofstd dcmdata dcmtls dcmnet imagectn dcmimgle dcmimage dcmjpeg dcmsign dcmsr dcmpstat dcmwlm)
