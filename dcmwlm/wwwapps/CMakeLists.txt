# declare executables
FOREACH(PROGRAM readoviw readwlst writwlst)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM readoviw readwlst writwlst)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

# command line tools that do not (yet) work under Windows OS
IF(NOT WIN32)
  DCMTK_ADD_EXECUTABLE(preplock preplock)
  DCMTK_TARGET_LINK_LIBRARIES(preplock ${WIN32_STD_LIBRARIES})
ENDIF(NOT WIN32)
