# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmiod_tests
  tchecks.cc
  tcielabutil.cc
  tcodes.cc
  ticcprofile.cc
  timagepixel.cc
  tmacro.cc
  tpalette.cc
  tests.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmiod_tests dcmimgle dcmiod dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmiod)
