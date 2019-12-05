# declare executables
DCMTK_ADD_EXECUTABLE(dcmect_tests
                     tests
                     t_huge_concat
                     t_roundtrip)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmect_tests dcmect dcmfg dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmect)
