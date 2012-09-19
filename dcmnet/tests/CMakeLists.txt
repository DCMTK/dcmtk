# declare directories containing used libraries
LINK_DIRECTORIES(${dcmnet_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
ADD_EXECUTABLE(dcmnet_tests tests tdump)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(dcmnet_tests dcmnet)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmnet)
