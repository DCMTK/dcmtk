# declare project
PROJECT(ofstd)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${ofstd_SOURCE_DIR}/include" ${LIBICONV_INCDIR})

# recurse into subdirectories
FOREACH(SUBDIR libsrc include tests)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
