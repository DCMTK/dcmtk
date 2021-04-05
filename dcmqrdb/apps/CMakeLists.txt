# declare executables
foreach(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcmqrscp dcmqridx dcmqrti)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmqrdb dcmnet dcmdata oflog ofstd)
endforeach()
