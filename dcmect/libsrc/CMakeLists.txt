# create library from source files
DCMTK_ADD_LIBRARY(dcmect enhanced_ct types)

DCMTK_TARGET_LINK_MODULES(dcmect dcmfg dcmiod dcmdata ofstd oflog)
