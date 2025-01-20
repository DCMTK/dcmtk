# declare executables
foreach(PROGRAM dcm2img)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcm2img dcmjpeg dcmjpls)
