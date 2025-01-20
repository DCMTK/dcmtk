# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmtls_tests tests.cc tscuscptls.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmtls_tests dcmtls)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmtls)
