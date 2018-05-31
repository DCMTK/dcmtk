# Find sndfile library
#
# Released under BSD license
#
#  SNDFILE_INCLUDE_DIRS - where to find sndfile.h, etc
#  SNDFILE_LIBRARIES    - Lists of libraries when using sndfile
#  SNDFILE_FOUND        - True if sndfile found

include(FindPackageHandleStandardArgs)

# Look for the header file
find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h)
mark_as_advanced(SNDFILE_INCLUDE_DIR)

# Look for the library
set(SNDFILE_LIBS sndfile)
find_library(SNDFILE_LIBRARY NAMES ${SNDFILE_LIBS})
mark_as_advanced(SNDFILE_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SNDFILE REQUIRED_VARS SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)

# Copy the result to output variables
if(SNDFILE_FOUND)
  set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
else()
  set(SNDFILE_LIBS)
  set(SNDFILE_LIBRARY)
  set(SNDFILE_LIBRARIES)
  set(SNDFILE_INCLUDE_DIR)
  set(SNDFILE_INCLUDE_DIRS)
endif()
