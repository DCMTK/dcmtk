# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmqrdb_tests
  tests.cc
  taccess.cc
  tqrlevel.cc
  tdispatch.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmqrdb_tests dcmqrdb dcmtls dcmnet dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmqrdb)
