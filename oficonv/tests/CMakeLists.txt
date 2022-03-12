# declare executables
DCMTK_ADD_EXECUTABLE(oficonv_tests
  tests.cc
  oficonv_tests.cc
  ticonv.cc
  ticonv_iso8859.cc
  ticonv_chinese.cc
  ticonv_japanese.cc
  ticonv_korean.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(oficonv_tests ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(oficonv)
