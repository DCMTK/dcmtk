# create library from source files
DCMTK_ADD_LIBRARY(dcmpmap
    dpmmodparametricmapimage.cc
    dpmmodparametricmapseries.cc
    dpmparametricmapbase.cc
    dpmparametricmapiod.cc
    dpmtypes.cc
)

DCMTK_TARGET_LINK_MODULES(dcmpmap dcmfg dcmiod dcmdata ofstd oflog)
