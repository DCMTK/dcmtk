# declare project
PROJECT(dcmjpeg)

# recurse into subdirectories
FOREACH(SUBDIR libsrc libijg8 libijg12 libijg16 apps include)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
