# Find OpenJPEG library
#
# Released under BSD license
#
#  OPENJPEG_INCLUDE_DIRS - where to find openjpeg.h, etc.
#  OPENJPEG_LIBRARIES    - Lists of libraries when using OpenJPEG
#  OPENJPEG_FOUND        - True if OpenJPEG found

# Use pkg_check_modules to determine the paths for OpenJPEG
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(OPENJPEG_PKGCONF libopenjp2)
endif()

# Look for the header file
find_path(OPENJPEG_INCLUDE_DIR NAMES openjpeg.h HINTS ${OPENJPEG_PKGCONF_INCLUDE_DIRS})
mark_as_advanced(OPENJPEG_INCLUDE_DIR)

# Look for the library
set(OPENJPEG_LIBS openjp2)
find_library(OPENJPEG_LIBRARY NAMES ${OPENJPEG_LIBS} HINTS ${OPENJPEG_PKGCONF_LIBRARY_DIRS})
mark_as_advanced(OPENJPEG_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENJPEG REQUIRED_VARS OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)

# Copy the result to output variables
if(OPENJPEG_FOUND)
  set(OPENJPEG_LIBRARIES ${OPENJPEG_LIBRARY})
  set(OPENJPEG_INCLUDE_DIRS ${OPENJPEG_INCLUDE_DIR})
else()
  set(OPENJPEG_LIBS)
  set(OPENJPEG_LIBRARY)
  set(OPENJPEG_LIBRARIES)
  set(OPENJPEG_INCLUDE_DIR)
  set(OPENJPEG_INCLUDE_DIRS)
endif()
