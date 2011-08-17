# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR})

# declare executables
ADD_EXECUTABLE(dcmdata_tests tests tpread ti2dbmp tchval tpath tvrdatim telemlen tparser tdict)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(dcmdata_tests i2d dcmdata oflog ofstd ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmdata)
