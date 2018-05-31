# declare project
project(dcmjpeg)

# recurse into subdirectories
foreach(SUBDIR libsrc libijg8 libijg12 libijg16 apps include)
  add_subdirectory(${SUBDIR})
endforeach()
