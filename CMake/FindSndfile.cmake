# Find sndfile library
#
# Released under BSD license
#
#  Will define cmake target Sndfile::Sndfile
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

# Look for the library
set(SNDFILE_LIBS_DEBUG sndfiled)
find_library(SNDFILE_LIBRARY_DEBUG NAMES ${SNDFILE_LIBS_DEBUG})
mark_as_advanced(SNDFILE_LIBRARY_DEBUG)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SNDFILE REQUIRED_VARS SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR)

# Copy the result to output variables
if(SNDFILE_FOUND)
  set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})

  if(SNDFILE_INCLUDE_DIRS AND SNDFILE_LIBRARIES)
    add_library(Sndfile::Sndfile UNKNOWN IMPORTED)
    set_target_properties(Sndfile::Sndfile PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SNDFILE_INCLUDE_DIR}")

    if(SNDFILE_LIBRARIES AND SNDFILE_LIBRARIES_DEBUG)
      set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARIES})
      set_property(TARGET Sndfile::Sndfile APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE)

      set_target_properties(Sndfile::Sndfile PROPERTIES
        IMPORTED_LOCATION_RELEASE "${SNDFILE_LIBRARIES}")
    endif()

    if(SNDFILE_LIBRARIES_DEBUG)
      set_property(TARGET Sndfile::Sndfile APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG)

      set_target_properties(Sndfile::Sndfile PROPERTIES
        IMPORTED_LOCATION_DEBUG "${SNDFILE_LIBRARIES_DEBUG}")
    endif()

    if(SNDFILE_LIBRARIES AND NOT SNDFILE_LIBRARIES_DEBUG)
      set_property(TARGET Sndfile::Sndfile APPEND PROPERTY
      IMPORTED_LOCATION "${SNDFILE_LIBRARIES}")
    endif()
  endif()
else()
  set(SNDFILE_LIBS)
  set(SNDFILE_LIBRARY)
  set(SNDFILE_LIBRARIES)
  set(SNDFILE_INCLUDE_DIR)
  set(SNDFILE_INCLUDE_DIRS)
endif()
