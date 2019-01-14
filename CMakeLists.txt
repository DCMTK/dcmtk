# Minimum CMake version required
cmake_minimum_required(VERSION 2.8.5 FATAL_ERROR)

# As of 2018-12-26 DCMTK has been validated to build with CMake 3.13.2 policies.
set(DCMTK_MAX_CMAKE_POLICY_VERSION 3.13.2)

# Set and use the newest CMake policies that are validated to work
# (VERSION_LESS comparison is only defined for CMake 3 and newer)
if(CMAKE_MAJOR_VERSION LESS 3)
    set(DCMTK_CMAKE_POLICY_VERSION "${CMAKE_VERSION}")
elseif(CMAKE_VERSION VERSION_LESS DCMTK_MAX_CMAKE_POLICY_VERSION)
    set(DCMTK_CMAKE_POLICY_VERSION "${CMAKE_VERSION}")
else()
    set(DCMTK_CMAKE_POLICY_VERSION "${DCMTK_MAX_CMAKE_POLICY_VERSION}")
endif()
cmake_policy(VERSION "${DCMTK_CMAKE_POLICY_VERSION}")

# Declare project
project(DCMTK)

# Check the build system
include(CMake/dcmtkPrepare.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

# Modules to be built

set(DCMTK_MODULES ofstd oflog dcmdata dcmimgle
  dcmimage dcmjpeg dcmjpls dcmtls dcmnet dcmsr
  dcmsign dcmwlm dcmqrdb dcmpstat dcmrt dcmiod dcmfg dcmseg dcmtract dcmpmap
  CACHE STRING "List of modules that should be built.")

#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------

set(DCMTK_INCLUDE_DIR "${DCMTK_BINARY_DIR}/config/include")
foreach(inc ${DCMTK_MODULES})
  list(APPEND DCMTK_INCLUDE_DIR "${DCMTK_SOURCE_DIR}/${inc}/include")
endforeach()

include_directories(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# add the osconfig.h.in file
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
install(FILES ANNOUNCE CHANGES COPYRIGHT CREDITS FAQ HISTORY VERSION
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT doc)
install(DIRECTORY docs/ DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        COMPONENT doc FILES_MATCHING PATTERN "CHANGES.???")

#-----------------------------------------------------------------------------
# Create an install configuration files for external projects
#-----------------------------------------------------------------------------
#
# DCMTKTargets.cmake will contain list of executables and libraries produced
# DCMTKConfigVersion.cmake will contain DCMTK version information
# DCMTKConfig.cmake will contain options used to build DCMTK
#
# All three files are created within the build tree's main directory (handled in
# CMake/GenerateCMakeExports.cmake, and are installed to locations (OS-specific
# under the main install dir (handled directly below).

# Only create fully-fledged CMake export files if we have the related commands
include(CMake/CheckCMakeCommandExists.cmake)
include(CMakePackageConfigHelpers OPTIONAL)
CHECK_CMAKE_COMMAND_EXISTS("CONFIGURE_PACKAGE_CONFIG_FILE")
CHECK_CMAKE_COMMAND_EXISTS("WRITE_BASIC_PACKAGE_VERSION_FILE")

if(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

  # Create and configure CMake export files
  include(CMake/GenerateCMakeExports.cmake)

  # ${DCMTK_INSTALL_CONFIG} and ${DCMTK_CONFIG_VERSION} are
  # defined within CMake/GenerateCMakeExports.cmake.
  # Install DCMTKTargets.cmake to install tree
  install(EXPORT DCMTKTargets FILE DCMTKTargets.cmake
          DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

  # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
  install(FILES "${DCMTK_INSTALL_CONFIG}" "${DCMTK_CONFIG_VERSION}"
          DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

else()

  # Warning that we use old "configure_file" command
  message(STATUS "Warning: Using old configure_file() mechanism to produce DCMTKConfig.cmake")

  # Actually configure file
  configure_file("${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.old_cmake.in"
                 "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" @ONLY)

  # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
  install(FILES "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" "${DCMTK_BINARY_DIR}/DCMTKConfigVersion.cmake"
          DESTINATION "${DCMTK_INSTALL_CMKDIR}"
          COMPONENT cmake)

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
    # nothing to do
  endif()
else()
  string(REPLACE ";" "${ENVIRONMENT_PATH_SEPARATOR}" DCMDICTPATH "${DCMTK_DICOM_DICTIONARIES}")
  configure_file("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRun.cmake.in"
    "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
  )
endif()
