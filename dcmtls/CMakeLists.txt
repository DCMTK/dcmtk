# declare project
PROJECT(dcmtls)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${dcmtls_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmnet_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${OPENSSL_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc include docs)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
