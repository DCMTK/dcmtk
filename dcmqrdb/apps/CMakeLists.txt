# declare directories containing used libraries
LINK_DIRECTORIES(${dcmqrdb_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmnet_BINARY_DIR} ${ZLIB_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  TARGET_LINK_LIBRARIES(${PROGRAM} netapi32 wsock32 ofstd dcmdata dcmnet dcmqrdb ${ZLIB_LIBS})
ENDFOREACH(PROGRAM)
