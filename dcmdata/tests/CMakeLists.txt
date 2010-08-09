# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
FOREACH(PROGRAM i2dbmp tstpath tstchval tstpread tvrdatim)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM tstpath tstchval tstpread tvrdatim)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmdata oflog ofstd ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
ENDFOREACH(PROGRAM)

TARGET_LINK_LIBRARIES(i2dbmp libi2d dcmdata oflog ofstd ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

# declare installation files
#FOREACH(PROGRAM i2dbmp tstpath tstchval tstpread tvrdatim)
#  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
#ENDFOREACH(PROGRAM)