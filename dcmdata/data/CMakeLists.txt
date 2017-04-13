# declare installation files
INSTALL(FILES dicom.dic acrnema.dic private.dic diconde.dic dcm2xml.dtd dumppat.txt SC.dump VLP.dump DESTINATION "${DCMTK_INSTALL_DATDIR}" COMPONENT data)

# add paths to the dictionaries to a CMake variable for being used by the unit tests
SET(DCMTK_DICOM_DICTIONARIES
  "${CMAKE_CURRENT_SOURCE_DIR}/dicom.dic"
  "${CMAKE_CURRENT_SOURCE_DIR}/private.dic"
  CACHE INTERNAL "paths to the DICOM data dictionaries"
)
