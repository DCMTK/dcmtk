# declare additional include directories
include_directories("${dcmjpls_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" "${dcmjpls_SOURCE_DIR}/libcharls" ${ZLIB_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(dcmjpls
    djcodecd.cc
    djcodece.cc
    djcparam.cc
    djdecode.cc
    djencode.cc
    djrparam.cc
    djutils.cc
)

DCMTK_TARGET_LINK_MODULES(dcmjpls ofstd oflog dcmdata dcmimgle dcmimage dcmtkcharls)
