# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/libcharls ${ofstd_SOURCE_DIR}/include)

# create library from source files
ADD_LIBRARY(charls header intrface jpegls)

# set proper version information for shared library
IF(BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(charls PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
ENDIF(BUILD_SHARED_LIBS)

# declare installation files
INSTALL(TARGETS charls
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
