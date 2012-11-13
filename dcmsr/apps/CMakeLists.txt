# declare executables
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
