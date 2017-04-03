# declare project
PROJECT(dcmpmaps)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${dcmpmaps_SOURCE_DIR}/include" "${dcmfg_SOURCE_DIR}/include" "${dcmiod_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" ${ZLIB_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc include)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
