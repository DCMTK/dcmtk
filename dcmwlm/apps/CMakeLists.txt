# declare additional include directories
INCLUDE_DIRECTORIES(${dcmtls_SOURCE_DIR}/include)

# declare directories containing used libraries
LINK_DIRECTORIES(${dcmwlm_BINARY_DIR} ${ofstd_BINARY_DIR} ${dcmdata_BINARY_DIR} ${dcmtls_BINARY_DIR} ${dcmnet_BINARY_DIR})
IF(WITH_ZLIB)
  LINK_DIRECTORIES(${WITH_ZLIBINC}/lib)
ENDIF(WITH_ZLIB)

# declare executables
ADD_EXECUTABLE(wlmscpfs wlmscpfs wlcefs)

# make sure executables are linked to the corresponding libraries
# wlmscpfs
IF(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(wlmscpfs netapi32 wsock32 ofstd dcmdata dcmtls dcmnet dcmwlm debug zlib_d optimized zlib_o)
ELSE(WITH_ZLIB)
  TARGET_LINK_LIBRARIES(wlmscpfs netapi32 wsock32 ofstd dcmdata dcmtls dcmnet dcmwlm)
ENDIF(WITH_ZLIB)
