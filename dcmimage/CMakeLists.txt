# declare project
project(dcmimage)

# declare include directories which hold for all subdirectories
include_directories("${dcmimage_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${dcmimgle_SOURCE_DIR}/include" ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc apps include)
  add_subdirectory(${SUBDIR})
endforeach()
