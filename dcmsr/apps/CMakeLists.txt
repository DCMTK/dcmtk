# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${dcmimage_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${LIBXML_LIBDIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS dsr2html dsr2xml dsrdump xml2dsr DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd ${LIBTIFF_LIBS} ${LIBPNG_LIBS} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${LIBXML_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
  IF(WIN32)
    SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES LINK_FLAGS ${WIN32_STD_OBJECTS})
  ENDIF(WIN32)
ENDFOREACH(PROGRAM)
