# declare project
project(dcmtract)

# declare include directories which hold for all subdirectories
include_directories("${dcmtract_SOURCE_DIR}/include" "${dcmiod_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc include)
  add_subdirectory(${SUBDIR})
endforeach()
