# create library from source files
DCMTK_ADD_LIBRARY(dcmseg segdoc segment segtypes segutils)

DCMTK_TARGET_LINK_MODULES(dcmseg dcmfg dcmiod dcmdata ofstd oflog)
