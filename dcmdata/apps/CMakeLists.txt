# declare additional include directories
INCLUDE_DIRECTORIES(${LIBXML_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm pdf2dcm)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)
ADD_EXECUTABLE(dcmodify dcmodify mdfconen mdfdsman)

# declare installation files
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)
INSTALL_FILES(${INSTALL_DATDIR} FILES dumppat.txt)
INSTALL_FILES(${INSTALL_LIBDIR} FILES dcm2xml.dtd)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm)
  TARGET_LINK_LIBRARIES(${PROGRAM} netapi32 wsock32 ofstd dcmdata ${ZLIB_LIBS})
ENDFOREACH(PROGRAM)
TARGET_LINK_LIBRARIES(dcmftest ofstd ${ZLIB_LIBS})
TARGET_LINK_LIBRARIES(xml2dcm ${LIBXML_LIBS})
