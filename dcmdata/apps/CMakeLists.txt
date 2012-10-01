# declare additional include directories
INCLUDE_DIRECTORIES(${LIBXML_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmdata_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)
DCMTK_ADD_EXECUTABLE(dcmodify dcmodify mdfconen mdfdsman)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm dcm2pdf img2dcm)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
ENDFOREACH(PROGRAM)

DCMTK_TARGET_LINK_MODULES(dcmftest ofstd)
DCMTK_TARGET_LINK_LIBRARIES(dcmftest ${LIBICONV_LIBS} ${THREAD_LIBS})
DCMTK_TARGET_LINK_LIBRARIES(xml2dcm ${LIBXML_LIBS})
DCMTK_TARGET_LINK_MODULES(img2dcm i2d dcmdata)
