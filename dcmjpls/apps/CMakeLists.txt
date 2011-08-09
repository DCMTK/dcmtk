# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmjpls_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS dcmcjpls dcmdjpls dcml2pnm DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpls dcmdjpls dcml2pnm)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmjpls charls dcmimage dcmimgle dcmdata oflog ofstd ofstd ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
ENDFOREACH(PROGRAM)
