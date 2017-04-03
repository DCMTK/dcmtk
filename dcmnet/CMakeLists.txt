# declare project
PROJECT(dcmnet)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${dcmnet_SOURCE_DIR}/include" "${dcmtls_SOURCE_DIR}/include" ${OPENSSL_INCDIR} "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc apps include docs etc tests)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
