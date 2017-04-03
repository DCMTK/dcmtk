# declare project
PROJECT(dcmseg)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${dcmseg_SOURCE_DIR}/include" "${dcmfg_SOURCE_DIR}/include" "${dcmiod_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc include tests)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
