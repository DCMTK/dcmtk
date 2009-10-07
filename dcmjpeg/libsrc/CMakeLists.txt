# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpeg_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${dcmjpeg_SOURCE_DIR}/libijg8 ${dcmjpeg_SOURCE_DIR}/libijg12 ${dcmjpeg_SOURCE_DIR}/libijg16 ${ZLIB_INCDIR})

# create library from source files
ADD_LIBRARY(dcmjpeg ddpiimpl dipijpeg djcodecd djcodece djcparam djdecbas djdecext djdeclol djdecode djdecpro djdecsps djdecsv1 djdijg12 djdijg8 djdijg16 djeijg12 djeijg8 djeijg16 djencbas djencext djenclol djencode djencpro djencsps djencsv1 djrplol djrploss djutils)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmjpeg)
