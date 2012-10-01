# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpeg_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmjpeg_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpeg ijg8 ijg12 ijg16 dcmimage dcmimgle dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
