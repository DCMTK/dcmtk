# declare executables
DCMTK_ADD_EXECUTABLE(ofstd_tests tests tatof tmap tvec tftoa tthread tbase64 tstring tlist tstack tofdatim tofstd tmarkup tchrenc txml tuuid toffile tmem)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(ofstd_tests ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(ofstd)
