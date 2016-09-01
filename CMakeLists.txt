# Declare project
PROJECT(DCMTK)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

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

SET(DCMTK_INCLUDE_DIR ${DCMTK_BINARY_DIR}/config/include)
FOREACH(inc ${DCMTK_MODULES})
  LIST(APPEND DCMTK_INCLUDE_DIR ${DCMTK_SOURCE_DIR}/${inc}/include)
ENDFOREACH(inc)

INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# add the osconfig.h.in file
CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/osconfig.h.in
               ${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h)

#-----------------------------------------------------------------------------
# Prepare arith.h
#-----------------------------------------------------------------------------

INSPECT_FUNDAMENTAL_ARITHMETIC_TYPES()

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
INSTALL(FILES ${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h
              ${DCMTK_BINARY_DIR}/config/include/dcmtk/config/arith.h
        DESTINATION ${DCMTK_INSTALL_INCDIR}/dcmtk/config
        COMPONENT include)

# Install DCMTK's general documentation files
INSTALL(FILES ANNOUNCE CHANGES COPYRIGHT CREDITS FAQ HISTORY VERSION
        DESTINATION ${DCMTK_INSTALL_DOCDIR}
        COMPONENT doc)

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

  # ${dcmtk_install_config} and ${dcmtk_config_version} are
  # defined within CMake/GenerateCMakeExports.cmake.
  # Install DCMTKTargets.cmake to install tree
  INSTALL(EXPORT DCMTKTargets FILE DCMTKTargets.cmake
          DESTINATION ${DCMTK_INSTALL_CMKDIR} COMPONENT cmake)

  # Install DCMTKConfigVersion.cmake
  INSTALL(FILES ${dcmtk_install_config} ${dcmtk_config_version}
          DESTINATION ${DCMTK_INSTALL_CMKDIR} COMPONENT cmake)

ELSE(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

  # Warning that we use old "configure_file" command
  MESSAGE(STATUS "Warning: Using old CONFIGURE_FILE() mechanism to produce DCMTKConfig.cmake")

  # Actually configure file
  CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.cmake26.in
                 ${DCMTK_BINARY_DIR}/DCMTKConfig.cmake @ONLY)

  # Install DCMTK's CMake configuration file
  INSTALL(FILES ${DCMTK_BINARY_DIR}/DCMTKConfig.cmake
          DESTINATION ${DCMTK_INSTALL_CMKDIR}
          COMPONENT cmake)

ENDIF(HAVE_CONFIGURE_PACKAGE_CONFIG_FILE AND HAVE_WRITE_BASIC_PACKAGE_VERSION_FILE)

#-----------------------------------------------------------------------------
# Configure files needed for cross compiling (if any)
#-----------------------------------------------------------------------------

IF(CMAKE_CROSSCOMPILING)
  IF(ANDROID)
    # setup the CTestCustom.cmake file as appropriate for android
    FILE(COPY ${DCMTK_SOURCE_DIR}/CMake/CTest/CTestCustom.cmake
        DESTINATION ${CMAKE_BINARY_DIR}
    )
    # prepare the configuration for unit testing
    CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/CTest/dcmtkCTestConfig.cmake.in
        ${CMAKE_BINARY_DIR}/dcmtkCTestConfig.cmake @ONLY
    )
  ENDIF()
ENDIF(CMAKE_CROSSCOMPILING)
