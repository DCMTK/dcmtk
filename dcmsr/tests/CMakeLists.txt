# declare executables
DCMTK_ADD_EXECUTABLE(mkreport mkreport)
DCMTK_ADD_EXECUTABLE(dcmsr_tests tests tsrtree)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM mkreport dcmsr_tests)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmsr)
