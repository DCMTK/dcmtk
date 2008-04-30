# create configuration file (expand variables)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cfwin32.h.in ${CMAKE_CURRENT_SOURCE_DIR}/cfwin32.h ESCAPE_QUOTES @ONLY)

# declare installation files
INSTALL_FILES(${INSTALL_INCDIR}/dcmtk/config FILES osconfig.h cfwin32.h)
