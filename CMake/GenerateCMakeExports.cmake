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

# Start with clean DCMTKTargets.cmake and fill it by appending
file(WRITE "${CMAKE_BINARY_DIR}/DCMTKTargets.cmake" "")

# Get and store all executable targets to DCMTKTargets.cmake within build's main dir
get_property(DCMTK_EXECUTABLE_TARGETS GLOBAL PROPERTY DCMTK_EXECUTABLE_TARGETS)
export(TARGETS ${DCMTK_EXECUTABLE_TARGETS} APPEND FILE "${CMAKE_BINARY_DIR}/DCMTKTargets.cmake" NAMESPACE DCMTK::)

# Get and store libraries to DCMTKTargets.cmake within the build's main dir
get_property(DCMTK_LIBRARY_TARGETS GLOBAL PROPERTY DCMTK_LIBRARY_TARGETS)
export(TARGETS config ${DCMTK_LIBRARY_TARGETS} APPEND FILE "${CMAKE_BINARY_DIR}/DCMTKTargets.cmake" NAMESPACE DCMTK::)

# Add interface library for conveniently linking to all libraries via DCMTK::DCMTK
export(TARGETS DCMTK APPEND FILE "${CMAKE_BINARY_DIR}/DCMTKTargets.cmake" NAMESPACE DCMTK::)

# Create DCMTKConfigVersion.cmake with basic DCMTK version information (build tree)
set(DCMTK_CONFIG_VERSION "${CMAKE_BINARY_DIR}/DCMTKConfigVersion.cmake")
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
set(DCMTK_CMKDIR_CONFIG "${CMAKE_BINARY_DIR}")
# Collect include paths
set(DCMTK_INCLUDE_DIR_CONFIGS
    "SET_AND_CHECK(DCMTK_config_INCLUDE_DIR \"${DCMTK_BINARY_DIR}/config/include/dcmtk/config\")")
foreach(module ${DCMTK_MODULES})
    set(DCMTK_INCLUDE_DIR_CONFIGS
        "${DCMTK_INCLUDE_DIR_CONFIGS}\nSET_AND_CHECK(DCMTK_${module}_INCLUDE_DIR \"${DCMTK_SOURCE_DIR}/${module}/include/dcmtk/${module}\")")
endforeach()
set(DCMTK_CONFIG_CODE "####### Expanded from \@DCMTK_CONFIG_CODE\@ #######\n")
set(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"${DCMTK_BINARY_DIR}/config/include\")\n")
foreach(module ${DCMTK_MODULES})
    set(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"${DCMTK_SOURCE_DIR}/${module}/include\")\n")
endforeach()
set(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}##################################################")
set(dcmtk_config "${CMAKE_BINARY_DIR}/DCMTKConfig.cmake")
# Actually configure file and write it to build's main directory
CONFIGURE_PACKAGE_CONFIG_FILE(
    ${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.cmake.in
    ${dcmtk_config}
    INSTALL_DESTINATION "${CMAKE_BINARY_DIR}"
    PATH_VARS DCMTK_CMKDIR_CONFIG DCMTK_INCLUDE_DIR_CONFIGS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Case 2: Configure 'DCMTKConfig.cmake' for an install tree
set(DCMTK_CMKDIR_CONFIG "${DCMTK_INSTALL_CMKDIR}")
# Collect include paths
set(DCMTK_INCLUDE_DIR_CONFIGS
    "SET_AND_CHECK(DCMTK_config_INCLUDE_DIR \"\${PACKAGE_PREFIX_DIR}/include/dcmtk/config\")")
foreach(module ${DCMTK_MODULES})
    set(DCMTK_INCLUDE_DIR_CONFIGS
        "${DCMTK_INCLUDE_DIR_CONFIGS}\nSET_AND_CHECK(DCMTK_${module}_INCLUDE_DIR \"\${PACKAGE_PREFIX_DIR}/include/dcmtk/${module}\")")
endforeach()
set(DCMTK_CONFIG_CODE "####### Expanded from \@DCMTK_CONFIG_CODE\@ #######\n")
set(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}list(APPEND DCMTK_INCLUDE_DIRS \"\${PACKAGE_PREFIX_DIR}/include\")\n")
set(DCMTK_CONFIG_CODE "${DCMTK_CONFIG_CODE}##################################################")
set(DCMTK_INSTALL_CONFIG "${CMAKE_BINARY_DIR}/CMakeFiles/DCMTKConfig.cmake")
# Actually configure file and set rule to install it to installation subdir
CONFIGURE_PACKAGE_CONFIG_FILE(
    ${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.cmake.in
    "${DCMTK_INSTALL_CONFIG}"
    INSTALL_DESTINATION "${DCMTK_INSTALL_CMKDIR}"
    PATH_VARS DCMTK_CMKDIR_CONFIG DCMTK_INCLUDE_DIR_CONFIGS
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
