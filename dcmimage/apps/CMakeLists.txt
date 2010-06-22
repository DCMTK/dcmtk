# declare directories containing used libraries
LINK_DIRECTORIES(${dcmimage_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcm2pnm dcmquant dcmscale)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
FOREACH(PROGRAM dcm2pnm dcmquant dcmscale)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2pnm dcmquant dcmscale)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmimage dcmimgle dcmdata oflog ofstd ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
TARGET_LINK_LIBRARIES(dcm2pnm ${LIBTIFF_LIBS} ${LIBPNG_LIBS})
