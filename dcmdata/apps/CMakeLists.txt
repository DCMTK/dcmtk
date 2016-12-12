# declare additional include directories
INCLUDE_DIRECTORIES(${LIBXML_INCDIR})

# declare executables
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm dcm2json)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)
DCMTK_ADD_EXECUTABLE(dcmodify dcmodify mdfconen mdfdsman)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm dcm2json)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

DCMTK_TARGET_LINK_MODULES(dcmftest ofstd)
DCMTK_TARGET_LINK_LIBRARIES(xml2dcm ${LIBXML_LIBS})
DCMTK_TARGET_LINK_MODULES(img2dcm i2d dcmdata)
