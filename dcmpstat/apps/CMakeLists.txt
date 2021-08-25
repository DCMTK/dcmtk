# declare additional include directories
include_directories(${dcmtls_SOURCE_DIR}/include ${LIBXML_INCDIR})

# declare executables
foreach(PROGRAM dcmmkcrv dcmmklut dcmp2pgm dcmprscp dcmprscu dcmpschk dcmpsmk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcmp2pgm dcmprscp dcmprscu dcmpsmk dcmpschk dcmpsprt dcmpsrcv dcmpssnd)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmpstat dcmdsig dcmsr dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog ofstd)
endforeach()

foreach(PROGRAM dcmmkcrv dcmmklut)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdsig dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
endforeach()
