# declare executables
DCMTK_ADD_TEST_EXECUTABLE(wltest wltest.cc)
DCMTK_ADD_TEST_EXECUTABLE(dcmwlm_tests tests.cc twlaetval.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(wltest dcmwlm dcmtls)
DCMTK_TARGET_LINK_MODULES(dcmwlm_tests dcmwlm)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmwlm)
