# declare executables
DCMTK_ADD_EXECUTABLE(wltest wltest.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(wltest dcmwlm dcmnet dcmtls dcmdata oflog ofstd)
