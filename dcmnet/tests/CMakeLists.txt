# declare executables
DCMTK_ADD_EXECUTABLE(dcmnet_tests tests tdump tpool tscuscp)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmnet_tests dcmnet)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmnet)
