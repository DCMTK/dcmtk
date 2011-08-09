# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsr_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBXML_LIBDIR})

# declare executables
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS dsr2html dsr2xml dsrdump xml2dsr DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmsr dcmdata oflog ofstd ${ZLIB_LIBS} ${LIBXML_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
