# This script creates files DCMTKTargets.cmake' and 'DCMTKConfigVersion.cmake'
# and 'DCMTKConfig.cmake'. These files are created within the build tree
# but also are installed with DCMTK.
# The files can be utilized (no matter whether they are installed or just
# located within the build tree) by external projects in order to use
# the related DCMTK build.
# In order to accomplish that, the files provide hints to the external project
# which libraries and executables are available, and which build options have
# been used to build them. The external project then uses the CMake
# find_package command in "CONFIG" mode in order to import the DCMTK package
# with help of these files.
#
# DCMTKTargets.cmake will contain list of available DCMTK executables and libs
# DCMTKConfigVersion.cmake provides checking of DCMTK version compatibility
# DCMTKConfig.cmake will contain options used to build this DCMTK package

# Get and store all executable targets to DCMTKTargets.cmake within build's main dir
GET_PROPERTY(DCMTK_EXECUTABLE_TARGETS GLOBAL PROPERTY DCMTK_EXECUTABLE_TARGETS)
EXPORT(TARGETS ${DCMTK_EXECUTABLE_TARGETS} APPEND FILE ${DCMTK_BUILD_CMKDIR}/DCMTKTargets.cmake)

# Get and store libraries to DCMTKTargets.cmake within the build's main dir
GET_PROPERTY(DCMTK_LIBRARY_TARGETS GLOBAL PROPERTY DCMTK_LIBRARY_TARGETS)
EXPORT(TARGETS ${DCMTK_LIBRARY_TARGETS} APPEND FILE ${DCMTK_BUILD_CMKDIR}/DCMTKTargets.cmake)

# Create DCMTConfigVersion.cmake with basic DCMTK version information (build tree)
SET(DCMTK_CONFIG_VERSION ${DCMTK_BUILD_CMKDIR}/DCMTKConfigVersion.cmake)
WRITE_BASIC_PACKAGE_VERSION_FILE(
    ${DCMTK_CONFIG_VERSION}
    VERSION ${DCMTK_MAJOR_VERSION}.${DCMTK_MINOR_VERSION}.${DCMTK_BUILD_VERSION}
    COMPATIBILITY SameMajorVersion)

# Depending on whether we want to create the build tree or install tree
# version of DCMTKConfig.cmake, we need to use different paths (install
# destination but also include paths set within DCMTKConfig.cmake) since
# DCMTKConfig.cmake should only contain relative paths which differ for
# build and install tree versions of the file.

# Case 1: Configure 'DCMTKConfig.cmake' for a build tree.
SET(DCMTK_CMKDIR_CONFIG ${DCMTK_BUILD_CMKDIR})
# Collect include paths
SET(DCMTK_INCLUDE_DIR_CONFIGS
    "SET_AND_CHECK(DCMTK_config_INCLUDE_DIR \"${DCMTK_BINARY_DIR}/config/include/dcmtk/config\")")
FOREACH(module ${DCMTK_MODULES})
    SET(DCMTK_INCLUDE_DIR_CONFIGS
        "${DCMTK_INCLUDE_DIR_CONFIGS}\nSET_AND_CHECK(DCMTK_${module}_INCLUDE_DIR \"${DCMTK_SOURCE_DIR}/${module}/include/dcmtk/${module}\")")
ENDFOREACH(module)
SET(DCMTK_CONFIG_CODE "####### Expanded from \@DCMTK_CONFIG_CODE\@ #######\n")
SET(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"${DCMTK_BINARY_DIR}/config/include\")\n")
FOREACH(module ${DCMTK_MODULES})
    SET(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"${DCMTK_SOURCE_DIR}/${module}/include\")\n")
ENDFOREACH(module)
SET(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}##################################################")
SET(dcmtk_config ${DCMTK_BUILD_CMKDIR}/DCMTKConfig.cmake)
# Actually configure file and write it to build's main directory
CONFIGURE_PACKAGE_CONFIG_FILE(
    CMake/DCMTKConfig.cmake.in
    ${dcmtk_config}
    INSTALL_DESTINATION ${CMAKE_BINARY_DIR}
    PATH_VARS DCMTK_CMKDIR_CONFIG DCMTK_INCLUDE_DIR_CONFIGS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Case 2: Configure 'DCMTKConfig.cmake' for an install tree
SET(DCMTK_CMKDIR_CONFIG ${DCMTK_INSTALL_CMKDIR})
# Collect include paths
SET(DCMTK_INCLUDE_DIR_CONFIGS
    "SET_AND_CHECK(DCMTK_config_INCLUDE_DIR \"\${PACKAGE_PREFIX_DIR}/include/dcmtk/config\")")
FOREACH(module ${DCMTK_MODULES})
    SET(DCMTK_INCLUDE_DIR_CONFIGS
        "${DCMTK_INCLUDE_DIR_CONFIGS}\nSET_AND_CHECK(DCMTK_${module}_INCLUDE_DIR \"\${PACKAGE_PREFIX_DIR}/include/dcmtk/${module}\")")
ENDFOREACH(module)
SET(DCMTK_CONFIG_CODE "####### Expanded from \@DCMTK_CONFIG_CODE\@ #######\n")
SET(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"\${PACKAGE_PREFIX_DIR}/include\")\n")
SET(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}##################################################")
SET(DCMTK_INSTALL_CONFIG ${CMAKE_BINARY_DIR}/CMakeFiles/DCMTKConfig.cmake)
# Actually configure file and set rule to install it to installation subdir
CONFIGURE_PACKAGE_CONFIG_FILE(
    CMake/DCMTKConfig.cmake.in
    ${DCMTK_INSTALL_CONFIG}
    INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/${DCMTK_INSTALL_CMKDIR}
    PATH_VARS DCMTK_CMKDIR_CONFIG DCMTK_INCLUDE_DIR_CONFIGS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
