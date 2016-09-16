# declare executables
DCMTK_ADD_EXECUTABLE(mkreport mkreport)
DCMTK_ADD_EXECUTABLE(dcmsr_tests tests tsrtree tsrdoctr tsrdoc tsrcodvl tsrnumvl tsrtpl tsrcmr tsrlist)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(mkreport dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
DCMTK_TARGET_LINK_MODULES(dcmsr_tests cmr dcmsr dcmimage dcmimgle dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmsr)
