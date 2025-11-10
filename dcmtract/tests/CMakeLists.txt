# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmtract_tests
  tcreate.cc
  tests.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmtract_tests dcmtract)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmtract)
