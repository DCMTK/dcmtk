# Minimum CMake version required
cmake_minimum_required(VERSION 3.0.0...3.13.2 FATAL_ERROR)

# Declare project
project(DCMTK)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

# Modules to be built
set(DCMTK_MODULES ofstd oflog dcmdata dcmimgle
  dcmimage dcmjpeg dcmjpls dcmtls dcmnet dcmsr
  dcmsign dcmwlm dcmqrdb dcmpstat dcmrt dcmiod dcmfg
  dcmseg dcmtract dcmpmap dcmect
  CACHE STRING "List of modules that should be built.")

# Provide an interface target to elegantly include the config directory
add_library(config INTERFACE)
target_include_directories(config INTERFACE
  $<BUILD_INTERFACE:${DCMTK_BINARY_DIR}/config/include>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS config EXPORT DCMTKTargets)

# Include directories
set(DCMTK_INCLUDE_DIR "${DCMTK_BINARY_DIR}/config/include")
foreach(inc ${DCMTK_MODULES})
  list(APPEND DCMTK_INCLUDE_DIR "${DCMTK_SOURCE_DIR}/${inc}/include")
endforeach()

include_directories(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Check the build system
#-----------------------------------------------------------------------------

include(CMake/dcmtkPrepare.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# Add the osconfig.h.in file
configure_file("${DCMTK_SOURCE_DIR}/CMake/osconfig.h.in"
               "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h")

#-----------------------------------------------------------------------------
# Prepare arith.h
#-----------------------------------------------------------------------------

INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES()

# ----------------------------------------------------------------------------
# Unit test related configuration/setup
# ----------------------------------------------------------------------------

if(CMAKE_CROSSCOMPILING)
  if(WIN32)
    set(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunWine.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test as a detached Wine process in the prepared wineprefix")
  elseif(ANDROID)
    set(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunAndroid.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test via the android emulator")
  else()
    message(WARNING "Emulation for your target platform is not available, CTest will not be able to execute the unit tests!")
  endif()
else()
  set(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRun.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test")
endif()
# Add a target to run the unit tests in exhaustive mode
add_custom_target("test-exhaustive"
    COMMAND "${CMAKE_COMMAND}" "-DCONFIG=${DCMTK_CONFIG_GENERATOR_EXPRESSION}" "-P"
        "${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunExhaustive.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

#-----------------------------------------------------------------------------
# Start actual compilation tasks
#-----------------------------------------------------------------------------

# Recurse into subdirectories
foreach(module config doxygen ${DCMTK_MODULES})
  add_subdirectory(${module})
endforeach()
include(CMake/dcmtkAfterModules.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# Installation tasks
#-----------------------------------------------------------------------------

# Install global headers
install(FILES "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h"
              "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/arith.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dcmtk/config"
        COMPONENT include)

# Install DCMTK's general documentation files
install(FILES ANNOUNCE COPYRIGHT CREDITS FAQ HISTORY VERSION
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT doc)
install(DIRECTORY docs/ DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT doc FILES_MATCHING PATTERN "CHANGES.???")

#-----------------------------------------------------------------------------
# Create an install configuration files for external projects
#-----------------------------------------------------------------------------

if(NOT DCMTK_NO_EXPORT)
    dcmtk_create_install_exports()
endif()

#-----------------------------------------------------------------------------
# Configure files needed for running the unit tests and cleanup
#-----------------------------------------------------------------------------

if(CMAKE_CROSSCOMPILING)
  if(WIN32)
    string(REPLACE ";" "${ENVIRONMENT_PATH_SEPARATOR}" DCMDICTPATH "${DCMTK_DICOM_DICTIONARIES}")
    configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/CTestCustomWine.cmake.in"
      "${DCMTK_BINARY_DIR}/CTestCustom.cmake" ESCAPE_QUOTES @ONLY
    )
    configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunWine.cmake.in"
      "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
    )
  elseif(ANDROID)
    DCMTK_ANDROID_STOP_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)
    # Prepare setting environment variable DCMDICTPATH
    set(DICTIONARIES ${DCMTK_DICOM_DICTIONARIES})
    list(GET DICTIONARIES 0 DCMDICTPATH)
    list(REMOVE_AT DICTIONARIES 0)
    get_filename_component(DCMDICTPATH "${DCMDICTPATH}" NAME)
    set(DCMDICTPATH "${ANDROID_TEMPORARY_FILES_LOCATION}/${DCMDICTPATH}")
    foreach(DICTIONARY ${DICTIONARIES})
        get_filename_component(FILE "${DICTIONARY}" NAME)
        set(DCMDICTPATH "${DCMDICTPATH}:${ANDROID_TEMPORARY_FILES_LOCATION}/${FILE}")
    endforeach()
    configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/CTestCustomAndroid.cmake.in"
      "${DCMTK_BINARY_DIR}/CTestCustom.cmake" ESCAPE_QUOTES @ONLY
    )
    configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunAndroid.cmake.in"
      "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
    )
  else()
    # Nothing to do
  endif()
else()
  string(REPLACE ";" "${ENVIRONMENT_PATH_SEPARATOR}" DCMDICTPATH "${DCMTK_DICOM_DICTIONARIES}")
  configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRun.cmake.in"
    "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
  )
endif()

# Generate pkgconfig
if(DCMTK_WITH_OPENSSL)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} libssl")
endif()

if(DCMTK_WITH_LIBXML)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} libxml-2.0")
endif()

if(DCMTK_WITH_LIBPNG)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} libpng16")
endif()

if(DCMTK_WITH_ZLIB)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} zlib")
endif()

if(DCMTK_WITH_SNDFILE)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} sndfile")
endif()

if(DCMTK_WITH_OPENJPEG)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} libjpeg")
endif()

if(DCMTK_WITH_ICU)
	set(PKGCONF_REQ_PRIV "${PKGCONF_REQ_PRIV} icu-uc")
endif()

get_property(DCMTK_LIBRARY_TARGETS GLOBAL PROPERTY DCMTK_LIBRARY_TARGETS)
foreach(T ${DCMTK_LIBRARY_TARGETS})
	set(PKGCONF_LIBS "${PKGCONF_LIBS} -l${T}")
endforeach()

configure_file(
	${DCMTK_SOURCE_DIR}/dcmtk.pc.in
	${DCMTK_BINARY_DIR}/dcmtk.pc
	@ONLY
)

install(FILES "${DCMTK_BINARY_DIR}/dcmtk.pc"
	DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
