# Declare project
PROJECT(DCMTK)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

# Disables a warning emitted by CMake 3.7.2. The same setting is performed
# again in CMake/dcmtkPrepare.cmake (included below), but the warning is still
# emitted if it is not set here (it only goes away if the policy is set in
# both files).
# We do not entirely understand this behavior, perhaps it is a bug in CMake?
IF(POLICY CMP0017)
    CMAKE_POLICY(SET CMP0017 NEW)
ENDIF()

# Check the build system
INCLUDE(CMake/dcmtkPrepare.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

# Modules to be built

SET(DCMTK_MODULES ofstd oflog dcmdata dcmimgle
  dcmimage dcmjpeg dcmjpls dcmtls dcmnet dcmsr
  dcmsign dcmwlm dcmqrdb dcmpstat dcmrt dcmiod dcmfg dcmseg dcmtract dcmpmap
  CACHE STRING "List of modules that should be built.")

#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------

SET(DCMTK_INCLUDE_DIR "${DCMTK_BINARY_DIR}/config/include")
FOREACH(inc ${DCMTK_MODULES})
  LIST(APPEND DCMTK_INCLUDE_DIR "${DCMTK_SOURCE_DIR}/${inc}/include")
ENDFOREACH(inc)

INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# add the osconfig.h.in file
CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/osconfig.h.in"
               "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h")

#-----------------------------------------------------------------------------
# Prepare arith.h
#-----------------------------------------------------------------------------

INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES()

# ----------------------------------------------------------------------------
# Unit test related configuration/setup
# ----------------------------------------------------------------------------

IF(CMAKE_CROSSCOMPILING)
  IF(WIN32)
    SET(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunWine.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test as a detached Wine process in the prepared wineprefix")
  ELSEIF(ANDROID)
    SET(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRunAndroid.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test via the android emulator")
  ELSE()
    MESSAGE(WARNING "Emulation for your target platform is not available, CTest will not be able to execute the unit tests!")
  ENDIF()
ELSE(CMAKE_CROSSCOMPILING)
  SET(DCMTK_RUN_CTEST_SCRIPT "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dcmtkCTestRun.cmake" CACHE INTERNAL "path to the CMake script for launching a unit test")
ENDIF(CMAKE_CROSSCOMPILING)
# Add a target to run the unit tests in exhaustive mode
ADD_CUSTOM_TARGET("test-exhaustive"
    COMMAND "${CMAKE_COMMAND}" "-P"
        "${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunExhaustive.cmake"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

#-----------------------------------------------------------------------------
# Start actual compilation tasks
#-----------------------------------------------------------------------------

# Recurse into subdirectories
FOREACH(module config doxygen ${DCMTK_MODULES})
  ADD_SUBDIRECTORY(${module})
ENDFOREACH(module)
INCLUDE(CMake/dcmtkAfterModules.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# Installation tasks
#-----------------------------------------------------------------------------

# Install global headers
INSTALL(FILES "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h"
              "${DCMTK_BINARY_DIR}/config/include/dcmtk/config/arith.h"
        DESTINATION "${DCMTK_INSTALL_INCDIR}/dcmtk/config"
        COMPONENT include)

# Install DCMTK's general documentation files
INSTALL(FILES ANNOUNCE CHANGES COPYRIGHT CREDITS FAQ HISTORY VERSION
        DESTINATION "${DCMTK_INSTALL_DOCDIR}"
        COMPONENT doc)
INSTALL(DIRECTORY docs/ DESTINATION "${DCMTK_INSTALL_DOCDIR}"
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
INCLUDE(CMake/CheckCMakeCommandExists.cmake)
INCLUDE(CMakePackageConfigHelpers OPTIONAL)
CHECK_CMAKE_COMMAND_EXISTS("CONFIGURE_PACKAGE_CONFIG_FILE")
CHECK_CMAKE_COMMAND_EXISTS("WRITE_BASIC_PACKAGE_VERSION_FILE")

IF(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

  # Create and configure CMake export files
  INCLUDE(CMake/GenerateCMakeExports.cmake)

  # ${DCMTK_INSTALL_CONFIG} and ${DCMTK_CONFIG_VERSION} are
  # defined within CMake/GenerateCMakeExports.cmake.
  # Install DCMTKTargets.cmake to install tree
  INSTALL(EXPORT DCMTKTargets FILE DCMTKTargets.cmake
          DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

  # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
  INSTALL(FILES "${DCMTK_INSTALL_CONFIG}" "${DCMTK_CONFIG_VERSION}"
          DESTINATION "${DCMTK_INSTALL_CMKDIR}" COMPONENT cmake)

ELSE(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

  # Warning that we use old "configure_file" command
  MESSAGE(STATUS "Warning: Using old CONFIGURE_FILE() mechanism to produce DCMTKConfig.cmake")

  # Actually configure file
  CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.cmake26.in"
                 "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" @ONLY)

  # Install DCMTKConfig.cmake and DCMTKConfigVersion.cmake
  INSTALL(FILES "${DCMTK_BINARY_DIR}/DCMTKConfig.cmake" "${DCMTK_BINARY_DIR}/DCMTKConfigVersion.cmake"
          DESTINATION "${DCMTK_INSTALL_CMKDIR}"
          COMPONENT cmake)

ENDIF(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

#-----------------------------------------------------------------------------
# Configure files needed for running the unit tests and cleanup
#-----------------------------------------------------------------------------

IF(CMAKE_CROSSCOMPILING)
  IF(WIN32)
    STRING(REPLACE ";" "${ENVIRONMENT_PATH_SEPARATOR}" DCMDICTPATH "${DCMTK_DICOM_DICTIONARIES}")
    CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/CTest/CTestCustomWine.cmake.in"
      "${DCMTK_BINARY_DIR}/CTestCustom.cmake" ESCAPE_QUOTES @ONLY
    )
    CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunWine.cmake.in"
      "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
    )
  ELSEIF(ANDROID)
    DCMTK_ANDROID_STOP_EMULATOR(DCMTK_ANDROID_EMULATOR_INSTANCE)
    # Prepare setting environment variable DCMDICTPATH
    SET(DICTIONARIES ${DCMTK_DICOM_DICTIONARIES})
    LIST(GET DICTIONARIES 0 DCMDICTPATH)
    LIST(REMOVE_AT DICTIONARIES 0)
    GET_FILENAME_COMPONENT(DCMDICTPATH "${DCMDICTPATH}" NAME)
    SET(DCMDICTPATH "${ANDROID_TEMPORARY_FILES_LOCATION}/${DCMDICTPATH}")
    FOREACH(DICTIONARY ${DICTIONARIES})
        GET_FILENAME_COMPONENT(FILE "${DICTIONARY}" NAME)
        SET(DCMDICTPATH "${DCMDICTPATH}:${ANDROID_TEMPORARY_FILES_LOCATION}/${FILE}")
    ENDFOREACH()
    CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/CTest/CTestCustomAndroid.cmake.in"
      "${DCMTK_BINARY_DIR}/CTestCustom.cmake" ESCAPE_QUOTES @ONLY
    )
    CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRunAndroid.cmake.in"
      "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
    )
  ELSE()
    # nothing to do
  ENDIF()
ELSE(CMAKE_CROSSCOMPILING)
  STRING(REPLACE ";" "${ENVIRONMENT_PATH_SEPARATOR}" DCMDICTPATH "${DCMTK_DICOM_DICTIONARIES}")
  CONFIGURE_FILE("${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestRun.cmake.in"
    "${DCMTK_RUN_CTEST_SCRIPT}" ESCAPE_QUOTES @ONLY
  )
ENDIF(CMAKE_CROSSCOMPILING)
