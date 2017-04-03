# declare project
PROJECT(oflog)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES("${oflog_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include")

# recurse into subdirectories
FOREACH(SUBDIR libsrc include etc)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
