# declare executables
DCMTK_ADD_EXECUTABLE(
    segdigest
    segdigest.cc
)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(segdigest dcmseg dcmfg dcmiod dcmdata oflog ofstd)
