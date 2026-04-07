# declare executables
DCMTK_ADD_TEST_EXECUTABLE(dcmdata_tests
  tbytestr.cc
  tchval.cc
  tcond.cc
  tdict.cc
  telemlen.cc
  tfrmsiz.cc
  tests.cc
  tfilter.cc
  tgenuid.cc
  ti2dbmp.cc
  titem.cc
  tmatch.cc
  tnesting.cc
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
  txfer.cc
  # dcmodify app sources needed for condition check tests (tcond.cc)
  ${CMAKE_CURRENT_SOURCE_DIR}/../apps/mdfdsman.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../apps/mdfconen.cc
)

# include path for dcmodify app headers (mdfdsman.h, mdfconen.h)
target_include_directories(dcmdata_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../apps)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmdata_tests i2d)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmdata)
