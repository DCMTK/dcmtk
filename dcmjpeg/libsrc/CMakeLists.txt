# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpeg_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${dcmjpeg_SOURCE_DIR}/libijg8 ${dcmjpeg_SOURCE_DIR}/libijg12 ${dcmjpeg_SOURCE_DIR}/libijg16 ${ZLIB_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(dcmjpeg ddpiimpl dipijpeg djcodecd djcodece djcparam djdecbas djdecext djdeclol djdecode djdecpro djdecsps djdecsv1 djdijg12 djdijg8 djdijg16 djeijg12 djeijg8 djeijg16 djencbas djencext djenclol djencode djencpro djencsps djencsv1 djrplol djrploss djutils)

TARGET_LINK_LIBRARIES(dcmjpeg ofstd oflog dcmdata dcmimgle dcmimage ijg8 ijg12 ijg16)

# declare installation files
INSTALL(TARGETS dcmjpeg
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
