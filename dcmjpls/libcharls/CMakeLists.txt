# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/libcharls ${ofstd_SOURCE_DIR}/include)

# create library from source files
DCMTK_ADD_LIBRARY(charls header intrface jpegls)

# declare installation files
INSTALL(TARGETS charls
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
