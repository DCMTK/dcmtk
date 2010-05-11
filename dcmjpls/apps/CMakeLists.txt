# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${ZLIB_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmjpls_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmcjpls dcmdjpls)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
FOREACH(PROGRAM dcmcjpls dcmdjpls)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpls dcmdjpls)
  TARGET_LINK_LIBRARIES(${PROGRAM} netapi32 wsock32 ofstd oflog dcmdata dcmimgle dcmimage dcmjpls charls ${ZLIB_LIBS})
ENDFOREACH(PROGRAM)
