# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBXML_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
DCMTK_ADD_EXECUTABLE(mkreport mkreport)
DCMTK_ADD_EXECUTABLE(dcmsr_tests tests tsrtree)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM mkreport dcmsr_tests)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${LIBXML_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmsr)
