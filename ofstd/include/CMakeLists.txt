# declare installation files
INSTALL(DIRECTORY dcmtk/ofstd DESTINATION ${DCMTK_INSTALL_INCDIR}/dcmtk COMPONENT include FILES_MATCHING PATTERN "*.h" PATTERN "*.def")
