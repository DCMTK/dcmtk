# declare project
PROJECT(dcmdata)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${dcmdata_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc libi2d apps include docs data tests)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
