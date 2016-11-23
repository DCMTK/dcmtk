# create library from source files
DCMTK_ADD_LIBRARY(dcmpmap dpmtypes dpmmodparametricmapseries dpmmodparametricmapimage dpmparametricmapbase dpmparametricmapiod)

DCMTK_TARGET_LINK_MODULES(dcmpmap dcmfg dcmiod dcmdata ofstd oflog)
