# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmpstat_tests tests.cc tgl.cc)
DCMTK_ADD_TEST_EXECUTABLE(msgserv msgserv.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmpstat_tests dcmpstat dcmdsig dcmsr dcmimage dcmiod dcmdata oflog ofstd)
DCMTK_TARGET_LINK_MODULES(msgserv dcmpstat dcmdsig dcmsr dcmimage)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmpstat)
