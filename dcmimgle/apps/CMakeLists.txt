# declare executables
foreach(PROGRAM dcmdspfn dcod2lum dconvlum)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcmdspfn dcod2lum dconvlum)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmimgle dcmdata oflog ofstd)
endforeach()
