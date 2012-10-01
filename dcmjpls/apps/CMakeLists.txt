# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmjpls_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpls charls dcmimage dcmimgle dcmdata oflog ofstd ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
ENDFOREACH(PROGRAM)
