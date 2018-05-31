# declare project
project(dcmpstat)

# declare include directories which hold for all subdirectories
include_directories("${dcmpstat_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmnet_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmqrdb_SOURCE_DIR}/include" "${dcmsr_SOURCE_DIR}/include" "${dcmsign_SOURCE_DIR}/include" "${dcmtls_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${OPENSSL_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc apps include data etc tests)
  add_subdirectory(${SUBDIR})
endforeach()
