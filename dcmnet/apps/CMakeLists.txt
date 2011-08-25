
# declare directories containing used libraries
LINK_DIRECTORIES(${dcmnet_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmtls_BINARY_DIR} ${ZLIB_LIBDIR} ${OPENSSL_LIBDIR})

# declare executables
FOREACH(PROGRAM getscu echoscu findscu movescu storescp storescu termscu)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
INSTALL(TARGETS getscu echoscu findscu movescu storescp storescu termscu DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)
IF(WITH_OPENSSL)
  IF(WIN32)
    INSTALL(FILES ${OPENSSL_BINDIR}/dcmtkeay.dll ${OPENSSL_BINDIR}/dcmtkssl.dll DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)
  ENDIF(WIN32)
ENDIF(WITH_OPENSSL)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM getscu echoscu findscu movescu storescp storescu termscu)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmnet dcmdata oflog ofstd ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
FOREACH(PROGRAM echoscu findscu storescp storescu)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmtls ${OPENSSL_LIBS})
ENDFOREACH(PROGRAM)