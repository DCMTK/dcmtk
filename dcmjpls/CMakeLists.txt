# declare project
project(dcmjpls)

# recurse into subdirectories
foreach(SUBDIR libsrc libcharls apps include)
  add_subdirectory(${SUBDIR})
endforeach()
