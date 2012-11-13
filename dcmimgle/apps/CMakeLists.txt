# declare executables
FOREACH(PROGRAM dcmdspfn dcod2lum dconvlum)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmdspfn dcod2lum dconvlum)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
