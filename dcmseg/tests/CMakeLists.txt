# declare executables
DCMTK_ADD_EXECUTABLE(dcmseg_tests tests tpack)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmseg_tests dcmseg ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmseg)
