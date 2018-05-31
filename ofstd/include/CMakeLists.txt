# declare installation files
install(DIRECTORY dcmtk/ofstd DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dcmtk" COMPONENT include FILES_MATCHING PATTERN "*.h" PATTERN "*.def")
