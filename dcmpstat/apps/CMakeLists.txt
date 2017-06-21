# declare additional include directories
INCLUDE_DIRECTORIES(${dcmtls_SOURCE_DIR}/include ${LIBXML_INCDIR})

# declare executables
FOREACH(PROGRAM dcmmkcrv dcmmklut dcmp2pgm dcmprscp dcmprscu dcmpschk dcmpsmk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmp2pgm dcmprscp dcmprscu dcmpsmk dcmpschk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmpstat dcmdsig dcmsr dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

FOREACH(PROGRAM dcmmkcrv dcmmklut)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdsig dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)
