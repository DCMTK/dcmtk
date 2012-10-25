# Declare project
PROJECT(DCMTK)

# Minimum CMake version required
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Check the build system
INCLUDE(CMake/dcmtkPrepare.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# General project settings to configure DCMTK build process
#-----------------------------------------------------------------------------

# Modules to be built
SET(DCMTK_MODULES config ofstd oflog dcmdata dcmimgle dcmimage dcmjpeg dcmjpls dcmtls dcmnet dcmsr dcmsign dcmwlm dcmqrdb dcmpstat dcmrt doxygen
    CACHE STRING "List of modules that should be built.")
MARK_AS_ADVANCED(DCMTK_MODULES)

#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------

SET(DCMTK_INCLUDE_DIR
  ${DCMTK_BINARY_DIR}/config/include
  ${DCMTK_SOURCE_DIR}/ofstd/include
  ${DCMTK_SOURCE_DIR}/oflog/include
  ${DCMTK_SOURCE_DIR}/dcmdata/include
  ${DCMTK_SOURCE_DIR}/dcmimgle/include
  ${DCMTK_SOURCE_DIR}/dcmimage/include
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg8
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg12
  ${DCMTK_SOURCE_DIR}/dcmjpeg/libijg16
  ${DCMTK_SOURCE_DIR}/dcmjpeg/include
  ${DCMTK_SOURCE_DIR}/dcmjpls/include
  ${DCMTK_SOURCE_DIR}/dcmnet/include
  ${DCMTK_SOURCE_DIR}/dcmpstat/include
  ${DCMTK_SOURCE_DIR}/dcmqrdb/include
  ${DCMTK_SOURCE_DIR}/dcmsign/include
  ${DCMTK_SOURCE_DIR}/dcmsr/include
  ${DCMTK_SOURCE_DIR}/dcmtls/include
  ${DCMTK_SOURCE_DIR}/dcmwlm/include
)
INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Prepare osconfig.h
#-----------------------------------------------------------------------------

# add the osconfig.h.in file
CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/osconfig.h.in
               ${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h)

#-----------------------------------------------------------------------------
# Write current configuration to file (for external projects)
#-----------------------------------------------------------------------------

CONFIGURE_FILE(${DCMTK_SOURCE_DIR}/CMake/DCMTKConfig.cmake.in
               ${DCMTK_BINARY_DIR}/DCMTKConfig.cmake @ONLY)

#-----------------------------------------------------------------------------
# Start actual compilation tasks
#-----------------------------------------------------------------------------

# Recurse into subdirectories
FOREACH(module ${DCMTK_MODULES})
  ADD_SUBDIRECTORY(${module})
ENDFOREACH(module)
INCLUDE(CMake/dcmtkAfterModules.cmake NO_POLICY_SCOPE)

#-----------------------------------------------------------------------------
# Installation tasks
#-----------------------------------------------------------------------------

# Install global headers
INSTALL(FILES ${DCMTK_BINARY_DIR}/config/include/dcmtk/config/osconfig.h
        DESTINATION ${DCMTK_INSTALL_INCDIR}/dcmtk/config
        COMPONENT include)

# Install DCMTK's general documentation files
INSTALL(FILES ANNOUNCE.${DCMTK_PACKAGE_VERSION_NUMBER} CHANGES.${DCMTK_PACKAGE_VERSION_NUMBER} COPYRIGHT FAQ HISTORY VERSION
        DESTINATION ${DCMTK_INSTALL_DOCDIR}
        COMPONENT doc)

# Install DCMTK's CMake configuration file
INSTALL(FILES ${DCMTK_BINARY_DIR}/DCMTKConfig.cmake
        DESTINATION ${DCMTK_INSTALL_DATDIR}
        COMPONENT data)
