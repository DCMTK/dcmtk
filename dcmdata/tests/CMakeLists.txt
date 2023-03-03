# declare executables
DCMTK_ADD_EXECUTABLE(dcmdata_tests
  tchval.cc
  tdict.cc
  telemlen.cc
  tests.cc
  tfilter.cc
  tgenuid.cc
  ti2dbmp.cc
  titem.cc
  tmatch.cc
  tnewdcme.cc
  tparent.cc
  tparser.cc
  tpath.cc
  tpread.cc
  tsequen.cc
  tspchrs.cc
  tstrval.cc
  ttag.cc
  tvrcomp.cc
  tvrdatim.cc
  tvrds.cc
  tvrfd.cc
  tvrol.cc
  tvrov.cc
  tvrpn.cc
  tvrsv.cc
  tvrui.cc
  tvruv.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmdata_tests i2d dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmdata)
