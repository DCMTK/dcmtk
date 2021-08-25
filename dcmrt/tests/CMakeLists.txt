# declare executables
DCMTK_ADD_EXECUTABLE(drttest drttest.cc)
DCMTK_ADD_EXECUTABLE(dcmrt_tests
  tests.cc
  tsearch.cc
  tcontour.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(drttest dcmrt dcmdata oflog ofstd)
DCMTK_TARGET_LINK_MODULES(dcmrt_tests dcmrt dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmrt)
