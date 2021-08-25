# declare executables
DCMTK_ADD_EXECUTABLE(mkreport mkreport.cc)
DCMTK_ADD_EXECUTABLE(dcmsr_tests
  tests.cc
  tsrcmr.cc
  tsrcodvl.cc
  tsrdoc.cc
  tsrdoctr.cc
  tsrlist.cc
  tsrnumvl.cc
  tsrtpl.cc
  tsrtree.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(mkreport dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
DCMTK_TARGET_LINK_MODULES(dcmsr_tests cmr dcmsr dcmimage dcmimgle dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmsr)
