# declare installation files
INSTALL(DIRECTORY dcmtk/dcmpmap DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dcmtk" COMPONENT include FILES_MATCHING PATTERN "*.h")
