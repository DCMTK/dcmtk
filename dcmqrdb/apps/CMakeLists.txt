# declare directories containing used libraries
LINK_DIRECTORIES(${dcmqrdb_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmnet_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${OPENSSL_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmqrdb dcmnet dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${OPENSSL_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
