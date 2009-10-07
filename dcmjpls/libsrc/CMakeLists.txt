# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${dcmjpls_SOURCE_DIR}/libcharls ${ZLIB_INCDIR})

# create library from source files
ADD_LIBRARY(dcmjpls djcparam djdecode djencode djrparam djcodecd djutils djcodece)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmjpls)
