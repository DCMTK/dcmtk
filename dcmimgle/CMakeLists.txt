# declare project
project(dcmimgle)

# declare include directories which hold for all subdirectories
include_directories("${dcmimgle_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc apps include data)
  add_subdirectory(${SUBDIR})
endforeach()
