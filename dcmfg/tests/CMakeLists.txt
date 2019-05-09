# declare executables
DCMTK_ADD_EXECUTABLE(dcmfg_tests tests t_deriv_image.cc t_frame_content.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmfg_tests dcmfg dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmfg)
