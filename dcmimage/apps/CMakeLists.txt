# declare executables
foreach(PROGRAM dcm2pnm dcmquant dcmscale dcmicmp)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcm2pnm dcmquant dcmscale dcmicmp)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmimage dcmimgle dcmdata oflog ofstd)
endforeach()
