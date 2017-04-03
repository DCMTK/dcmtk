# declare additional include directories
INCLUDE_DIRECTORIES("${dcmjpls_SOURCE_DIR}/libcharls" "${ofstd_SOURCE_DIR}/include")

# create library from source files
DCMTK_ADD_LIBRARY(charls header intrface jpegls)

DCMTK_TARGET_LINK_MODULES(charls ofstd oflog)
