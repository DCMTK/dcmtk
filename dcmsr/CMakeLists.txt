# declare project
project(dcmsr)

# declare include directories which hold for all subdirectories
include_directories("${dcmsr_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" "${dcmimage_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${LIBXML_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc libcmr apps tests include data)
  add_subdirectory(${SUBDIR})
endforeach()
