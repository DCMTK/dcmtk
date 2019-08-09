# declare project
project(dcmwlm)

# declare include directories which hold for all subdirectories
include_directories("${dcmwlm_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmnet_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc apps include docs data tests)
  add_subdirectory(${SUBDIR})
endforeach()
