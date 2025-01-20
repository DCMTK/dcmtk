# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmect_tests
  tests.cc
  t_huge_concat.cc
  t_overflow.cc
  t_roundtrip.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmect_tests dcmect dcmfg)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmect)
