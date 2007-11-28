# declare directories to be installed
INSTALL(DIRECTORY wlistdb wlistqry DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_DATDIR} PATTERN "CVS" EXCLUDE)
