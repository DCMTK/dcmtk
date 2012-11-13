# declare executables
FOREACH(PROGRAM dcm2pnm dcmquant dcmscale)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2pnm dcmquant dcmscale)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
