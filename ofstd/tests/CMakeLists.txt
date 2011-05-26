# declare directories containing used libraries
LINK_DIRECTORIES(${ofstd_BINARY_DIR})

# declare executables
ADD_EXECUTABLE(ofstd_tests tests tatof tmap tvec tftoa tthread tbase64 tstring tlist tstack tofdatim tofstd)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(ofstd_tests ofstd ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

#ADD_TEST(ofstd_tests ofstd_tests)
