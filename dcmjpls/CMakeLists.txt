# declare project
PROJECT(dcmjpls)

# recurse into subdirectories
FOREACH(SUBDIR libsrc libcharls apps include)
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH(SUBDIR)
