# declare directories containing used libraries
# declare executables
DCMTK_ADD_EXECUTABLE(dcmsign dcmsign)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmsign dcmdsig dcmdata oflog ofstd)
