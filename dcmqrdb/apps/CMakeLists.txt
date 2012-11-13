# declare executables
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmqrdb dcmnet dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
