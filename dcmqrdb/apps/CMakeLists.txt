# declare directories containing used libraries
LINK_DIRECTORIES(${dcmqrdb_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmnet_BINARY_DIR} ${ZLIB_LIBDIR} ${OPENSSL_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS dcmqrscp dcmqridx dcmqrti DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmqrdb dcmnet dcmdata oflog ofstd ${OPENSSL_LIBS} ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
