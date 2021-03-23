# Find wrap library
#
# Released under BSD license
#
#  WRAP_INCLUDE_DIRS - where to find tcpd.h, etc
#  WRAP_LIBRARIES    - Lists of libraries when using libwrap
#  WRAP_FOUND        - True if wrap found

include(FindPackageHandleStandardArgs)

# Look for the header file
find_path(WRAP_INCLUDE_DIR NAMES tcpd.h)
mark_as_advanced(WRAP_INCLUDE_DIR)

# Look for the library
set(WRAP_LIBS wrap)
find_library(WRAP_LIBRARY NAMES ${WRAP_LIBS})
mark_as_advanced(WRAP_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WRAP REQUIRED_VARS WRAP_LIBRARY WRAP_INCLUDE_DIR)

# Copy the result to output variables
if(WRAP_FOUND)
  set(WRAP_LIBRARIES ${WRAP_LIBRARY})
  set(WRAP_INCLUDE_DIRS ${WRAP_INCLUDE_DIR})
else()
  set(WRAP_LIBS)
  set(WRAP_LIBRARY)
  set(WRAP_LIBRARIES)
  set(WRAP_INCLUDE_DIR)
  set(WRAP_INCLUDE_DIRS)
endif()
