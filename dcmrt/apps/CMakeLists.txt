# declare executables
DCMTK_ADD_EXECUTABLE(drtdump drtdump.cc)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(drtdump dcmrt dcmdata oflog ofstd)
