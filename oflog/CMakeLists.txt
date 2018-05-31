# declare project
project(oflog)

# declare include directories which hold for all subdirectories
include_directories("${oflog_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include")

# recurse into subdirectories
foreach(SUBDIR libsrc include etc)
  add_subdirectory(${SUBDIR})
endforeach()
