# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${dcmjpls_SOURCE_DIR}/libcharls ${ZLIB_INCDIR})

# create library from source files
ADD_LIBRARY(dcmjpls djcparam djdecode djencode djrparam djcodecd djutils djcodece)

TARGET_LINK_LIBRARIES(dcmjpls ofstd oflog dcmdata dcmimgle dcmimage charls)

# set proper version information for shared library
IF(BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(dcmjpls PROPERTIES ${DCMTK_LIBRARY_PROPERTIES})
ENDIF(BUILD_SHARED_LIBS)

# declare installation files
INSTALL(TARGETS dcmjpls
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
