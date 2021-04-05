# declare executables
foreach(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
endforeach()
