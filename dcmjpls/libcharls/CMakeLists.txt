# declare additional include directories
include_directories("${dcmjpls_SOURCE_DIR}/libcharls" "${ofstd_SOURCE_DIR}/include")

# create library from source files
DCMTK_ADD_LIBRARY(dcmtkcharls
    header.cc
    intrface.cc
    jpegls.cc
)

DCMTK_TARGET_LINK_MODULES(dcmtkcharls ofstd oflog)
