# declare directories containing used libraries
LINK_DIRECTORIES(${dcmsign_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR})
IF(WITH_ZLIB)
  LINK_DIRECTORIES(${WITH_ZLIBINC}/lib)
ENDIF(WITH_ZLIB)
IF(WITH_OPENSSL)
  LINK_DIRECTORIES(${WITH_OPENSSLINC}/lib)
ENDIF(WITH_OPENSSL)

# declare executables
ADD_EXECUTABLE(dcmsign dcmsign)

# make sure executables are linked to the corresponding libraries
# dcmsign
IF(WITH_ZLIB)
  IF(WITH_OPENSSL)
    TARGET_LINK_LIBRARIES(dcmsign netapi32 wsock32 ofstd dcmdata dcmdsig debug zlib_d optimized zlib_o debug ssleay32_d optimized ssleay32_o debug libeay32_d optimized libeay32_o)
  ELSE(WITH_OPENSSL)
    TARGET_LINK_LIBRARIES(dcmsign netapi32 wsock32 ofstd dcmdata dcmdsig debug zlib_d optimized zlib_o)
  ENDIF(WITH_OPENSSL)
ELSE(WITH_ZLIB)
  IF(WITH_OPENSSL)
    TARGET_LINK_LIBRARIES(dcmsign netapi32 wsock32 ofstd dcmdata dcmdsig debug ssleay32_d optimized ssleay32_o debug libeay32_d optimized libeay32_o)
  ELSE(WITH_OPENSSL)
    TARGET_LINK_LIBRARIES(dcmsign netapi32 wsock32 ofstd dcmdata dcmdsig)
  ENDIF(WITH_OPENSSL)
ENDIF(WITH_ZLIB)
