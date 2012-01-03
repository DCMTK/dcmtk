# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
ADD_EXECUTABLE(mkreport mkreport)

# make sure executables are linked to the corresponding libraries
TARGET_LINK_LIBRARIES(mkreport dcmsr dcmimage dcmimgle dcmdata oflog ofstd ${ZLIB_LIBS} ${LIBICONV_LIBS} ${LIBXML_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
