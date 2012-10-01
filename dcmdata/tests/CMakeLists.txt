# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(dcmdata_tests tests tpread ti2dbmp tchval tpath tvrdatim telemlen tparser tdict tvrds tvrui tstrval tspchrs tvrpn tparent tfilter)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmdata_tests i2d dcmdata oflog ofstd)
DCMTK_TARGET_LINK_LIBRARIES(dcmdata_tests ${ZLIB_LIBS} ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmdata)
