# Find iconv library
#
# Released under BSD license
#
#  LIBICONV_INCLUDE_DIRS - where to find iconv.h, etc
#  LIBICONV_LIBRARIES    - Lists of libraries when using iconv
#  LIBICONV_FOUND        - True if iconv found

include(FindPackageHandleStandardArgs)
include(dcmtkTryCompile)

# Look for the header file
find_path(LIBICONV_INCLUDE_DIR NAMES iconv.h)
mark_as_advanced(LIBICONV_INCLUDE_DIR)

# Look for the library
set(LIBICONV_LIBS iconv)
find_library(LIBICONV_LIBRARY NAMES ${LIBICONV_LIBS})
mark_as_advanced(LIBICONV_LIBRARY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBICONV REQUIRED_VARS LIBICONV_LIBRARY LIBICONV_INCLUDE_DIR)

# Copy the result to output variables
if(LIBICONV_FOUND)
  set(LIBICONV_LIBRARIES ${LIBICONV_LIBRARY})
  set(LIBICONV_INCLUDE_DIRS ${LIBICONV_INCLUDE_DIR})
else()
  set(LIBICONV_LIBS)
  set(LIBICONV_LIBRARY)
  set(LIBICONV_LIBRARIES)
  set(LIBICONV_INCLUDE_DIR)
  set(LIBICONV_INCLUDE_DIRS)
endif()
