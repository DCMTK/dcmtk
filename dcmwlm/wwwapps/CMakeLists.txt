# declare directories containing used libraries
LINK_DIRECTORIES(${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR})

# declare executables
FOREACH(PROGRAM readoviw readwlst writwlst)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM readoviw readwlst writwlst)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)

# command line tools that do not (yet) work under Windows OS
IF(NOT WIN32)
  DCMTK_ADD_EXECUTABLE(preplock preplock)
  DCMTK_TARGET_LINK_LIBRARIES(preplock ${WIN32_STD_LIBRARIES})
ENDIF(NOT WIN32)
