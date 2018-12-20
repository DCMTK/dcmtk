# Find wrap library
#
# Released under BSD license
#
#  Will define cmake target Wrap::Wrap
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

set(WRAP_LIBS_DEBUG wrapd)
find_library(WRAP_LIBRARY_DEBUG NAMES ${WRAP_LIBS_DEBUG})
mark_as_advanced(WRAP_LIBRARY_DEBUG)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WRAP REQUIRED_VARS WRAP_LIBRARY WRAP_INCLUDE_DIR)

# Copy the result to output variables
if(WRAP_FOUND)
  set(WRAP_LIBRARIES ${WRAP_LIBRARY})
  set(WRAP_INCLUDE_DIRS ${WRAP_INCLUDE_DIR})

  if(WRAP_INCLUDE_DIRS AND WRAP_LIBRARIES)
    add_library(Wrap::Wrap UNKNOWN IMPORTED)
    set_target_properties(Wrap::Wrap PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${WRAP_INCLUDE_DIR}")

    if(WRAP_LIBRARIES AND WRAP_LIBRARIES_DEBUG)
      set(WRAP_LIBRARIES ${WRAP_LIBRARIES})
      set_property(TARGET Wrap::Wrap APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)

      set_target_properties(Wrap::Wrap PROPERTIES
        IMPORTED_LOCATION_RELEASE "${WRAP_LIBRARIES}")
    endif()

    if(WRAP_LIBRARIES_DEBUG)
      set_property(TARGET Wrap::Wrap APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)

      set_target_properties(Wrap::Wrap PROPERTIES
        IMPORTED_LOCATION_DEBUG "${WRAP_LIBRARIES_DEBUG}")
    endif()

    if(WRAP_LIBRARIES AND NOT WRAP_LIBRARIES_DEBUG)
      set_property(TARGET Wrap::Wrap APPEND PROPERTY
      IMPORTED_LOCATION "${WRAP_LIBRARIES}")
    endif()
  endif()
else()
  set(WRAP_LIBS)
  set(WRAP_LIBRARY)
  set(WRAP_LIBRARIES)
  set(WRAP_INCLUDE_DIR)
  set(WRAP_INCLUDE_DIRS)
endif()

