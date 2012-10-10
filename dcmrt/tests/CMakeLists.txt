# declare directories containing used libraries
LINK_DIRECTORIES(${dcmrt_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(drttest drttest)
DCMTK_ADD_EXECUTABLE(dcmrt_tests tests tsearch)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(drttest dcmrt dcmdata oflog ofstd)
DCMTK_TARGET_LINK_MODULES(dcmrt_tests dcmrt dcmdata oflog ofstd)
DCMTK_TARGET_LINK_LIBRARIES(drttest ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmrt)
