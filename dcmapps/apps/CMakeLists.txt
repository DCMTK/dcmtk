# declare executables
foreach(PROGRAM dcm2img)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcm2img)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmpstat dcmdsig dcmsr dcmjpeg dcmjpls dcmimage dcmimgle dcmqrdb dcmnet dcmtls dcmdata oflog oficonv ofstd)
endforeach()
