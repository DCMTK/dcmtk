# declare executables
DCMTK_ADD_EXECUTABLE(dcmiod_tests tests tcielabutil timagepixel)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmiod_tests dcmiod dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmiod)
