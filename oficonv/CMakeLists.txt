# declare project
project(oficonv)

# declare include directories which hold for all subdirectories
include_directories("${oficonv_SOURCE_DIR}/include")

# recurse into subdirectories
foreach(SUBDIR libsrc include)
  add_subdirectory(${SUBDIR})
endforeach()
