# declare directories containing used libraries
LINK_DIRECTORIES(${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR})

# declare executables
FOREACH(PROGRAM readoviw readwlst writwlst)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS readoviw readwlst writwlst DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM readoviw readwlst writwlst)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmdata oflog ofstd ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)

# command line tools that do not (yet) work under Windows OS
IF(NOT WIN32)
  ADD_EXECUTABLE(preplock preplock)
  INSTALL(TARGETS preplock DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)
  TARGET_LINK_LIBRARIES(preplock ${WIN32_STD_LIBRARIES})
ENDIF(NOT WIN32)
