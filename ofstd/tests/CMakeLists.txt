# declare directories containing used libraries
LINK_DIRECTORIES(${ofstd_BINARY_DIR} ${LIBICONV_LIBDIR})

# declare executables
ADD_EXECUTABLE(ofstd_tests tests tatof tmap tvec tftoa tthread tbase64 tstring tlist tstack tofdatim tofstd tmarkup tchrenc)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(ofstd_tests ofstd ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(ofstd)
