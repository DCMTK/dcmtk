# declare executables
DCMTK_ADD_EXECUTABLE(dcmfg_tests
  tests.cc
  t_concatenation_creator.cc
  t_concatenation_loader.cc
  t_ct_acquisition_details.cc
  t_ct_acquisition_type.cc
  t_ct_image_frame_type.cc
  t_ct_position.cc
  t_ct_table_dynamics.cc
  t_deriv_image.cc
  t_fg_base.cc
  t_frame_content.cc
  t_irradiation_event_identification.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmfg_tests dcmfg dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmfg)
