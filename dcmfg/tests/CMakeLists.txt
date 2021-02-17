# declare executables
DCMTK_ADD_EXECUTABLE(dcmfg_tests
                     tests
                     t_concatenation_creator
                     t_concatenation_loader
                     t_ct_acquisition_details
                     t_ct_acquisition_type
                     t_ct_image_frame_type
                     t_ct_position
                     t_ct_table_dynamics
                     t_deriv_image
                     t_frame_content
                     t_irradiation_event_identification)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmfg_tests dcmfg dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmfg)
