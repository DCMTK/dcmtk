# declare additional include directories
include_directories(${LIBXML_INCDIR})

# declare executables
foreach(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm stl2dcm pdf2dcm dcm2pdf img2dcm dcm2json cda2dcm dcm2cda)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM}.cc)
endforeach()
DCMTK_ADD_EXECUTABLE(dcmodify
  dcmodify.cc
  mdfconen.cc
  mdfdsman.cc)

# make sure executables are linked to the corresponding libraries
foreach(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm stl2dcm pdf2dcm dcm2pdf img2dcm dcm2json cda2dcm dcm2cda)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdata oflog ofstd)
endforeach()

DCMTK_TARGET_LINK_MODULES(dcmftest ofstd)
DCMTK_TARGET_LINK_MODULES(img2dcm i2d dcmdata)
DCMTK_TARGET_LINK_MODULES(xml2dcm dcmxml dcmdata oflog ofstd)
