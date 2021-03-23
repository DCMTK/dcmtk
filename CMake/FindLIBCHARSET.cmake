# Find charset library
#
#  LIBCHARSET_INCLUDE_DIRS - where to find localcharset.h, etc
#  LIBCHARSET_LIBRARIES    - Lists of libraries when using charset
#  LIBCHARSET_FOUND        - True if charset found

include(FindPackageHandleStandardArgs)

# Look for the header file
find_path(LIBCHARSET_INCLUDE_DIR NAMES localcharset.h)
mark_as_advanced(LIBCHARSET_INCLUDE_DIR)

set(LIBCHARSET_LIBS charset)
find_library(LIBCHARSET_LIBRARY NAMES ${LIBCHARSET_LIBS})
mark_as_advanced(LIBCHARSET_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBCHARSET REQUIRED_VARS LIBCHARSET_LIBRARY LIBCHARSET_INCLUDE_DIR)

if(LIBCHARSET_FOUND)
  set(LIBCHARSET_INCLUDE_DIRS ${LIBCHARSET_INCLUDE_DIR})
  set(LIBCHARSET_LIBRARIES ${LIBCHARSET_LIBRARY})
endif()
