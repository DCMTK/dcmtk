# declare project
project(dcmdata)

# declare include directories which hold for all subdirectories
include_directories("${dcmdata_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
foreach(SUBDIR libsrc libi2d libdcxml apps include docs data tests)
  add_subdirectory(${SUBDIR})
endforeach()
