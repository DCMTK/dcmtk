# declare project
project(oficonv)

# declare include directories which hold for all subdirectories
include_directories("${oficonv_SOURCE_DIR}/include")

# recurse into subdirectories
foreach(SUBDIR apps datasrc data libsrc include tests)
  add_subdirectory(${SUBDIR})
endforeach()
