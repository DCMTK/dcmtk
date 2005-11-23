# declare additional include directories
INCLUDE_DIRECTORIES(${dcmtls_SOURCE_DIR}/include ${OPENSSL_INCDIR})

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmnet_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmtls_BINARY_DIR} ${ZLIB_LIBDIR} ${OPENSSL_LIBDIR})

# declare executables
FOREACH(PROGRAM echoscu findscu movescu storescp storescu termscu)
  ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
FOREACH(PROGRAM echoscu findscu movescu storescp storescu termscu)
  INSTALL_TARGETS(${INSTALL_BINDIR} ${PROGRAM})
ENDFOREACH(PROGRAM)
IF(WITH_OPENSSL)
  INSTALL_FILES(${INSTALL_BINDIR} FILES ${OPENSSL_BINDIR}/dcmtkeay.dll ${OPENSSL_BINDIR}/dcmtkssl.dll)
ENDIF(WITH_OPENSSL)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM echoscu findscu movescu storescp storescu termscu)
  TARGET_LINK_LIBRARIES(${PROGRAM} netapi32 wsock32 ofstd dcmdata dcmnet ${ZLIB_LIBS})
ENDFOREACH(PROGRAM)
FOREACH(PROGRAM echoscu findscu storescp storescu)
  TARGET_LINK_LIBRARIES(${PROGRAM} dcmtls ${OPENSSL_LIBS})
ENDFOREACH(PROGRAM)
