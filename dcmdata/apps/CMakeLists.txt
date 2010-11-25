# declare additional include directories
INCLUDE_DIRECTORIES(${LIBXML_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)
ADD_EXECUTABLE(dcmodify dcmodify mdfconen mdfdsman)

# declare installation files
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmdata oflog ofstd ${ZLIB_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
ENDFOREACH(PROGRAM)

TARGET_LINK_LIBRARIES(dcmftest ofstd ${ZLIB_LIBS} ${THREAD_LIBS})
TARGET_LINK_LIBRARIES(xml2dcm ${TARGET_LINK_LIBRARIES} ${LIBXML_LIBS})
TARGET_LINK_LIBRARIES(img2dcm ${TARGET_LINK_LIBRARIES} libi2d dcmdata)
