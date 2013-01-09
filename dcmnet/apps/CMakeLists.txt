# declare additional include directories needed for compression support
INCLUDE_DIRECTORIES(${dcmjpls_SOURCE_DIR}/include ${dcmjpeg_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include)

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmnet_BINARY_DIR} ${ofstd_BINARY_DIR} ${oflog_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmtls_BINARY_DIR} ${ZLIB_LIBDIR} ${LIBICONV_LIBDIR} ${OPENSSL_LIBDIR})
# declare additional library directories needed for compression support
LINK_DIRECTORIES(${dcmjpls_BINARY_DIR} ${dcmjpeg_BINARY_DIR} ${dcmimage_BINARY_DIR} ${dcmimgle_BINARY_DIR} ${LIBTIFF_LIBDIR} ${LIBPNG_LIBDIR})

# declare executables
FOREACH(PROGRAM dcmsend echoscu findscu getscu movescu storescp storescu termscu)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# declare installation files
IF(WITH_OPENSSL)
  IF(WIN32)
    INSTALL(FILES ${OPENSSL_BINDIR}/dcmtkeay.dll ${OPENSSL_BINDIR}/dcmtkssl.dll DESTINATION ${DCMTK_INSTALL_BINDIR} COMPONENT bin)
  ENDIF(WIN32)
ENDIF(WITH_OPENSSL)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmsend echoscu findscu getscu movescu storescp storescu termscu)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmnet dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${ZLIB_LIBS} ${LIBICONV_LIBS} ${WIN32_STD_LIBRARIES} ${THREAD_LIBS})
ENDFOREACH(PROGRAM)
FOREACH(PROGRAM echoscu findscu storescp storescu)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmtls)
  DCMTK_TARGET_LINK_LIBRARIES(${PROGRAM} ${OPENSSL_LIBS})
ENDFOREACH(PROGRAM)

# uncomment the following line if "storescu" is compiled with ON_THE_FLY_COMPRESSION defined
#DCMTK_TARGET_LINK_MODULES(storescu dcmjpls dcmjpeg dcmimage)

# "dcmsend" always needs compression support
DCMTK_TARGET_LINK_MODULES(dcmsend dcmjpls dcmjpeg dcmimage)
