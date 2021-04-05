# create library from source files
DCMTK_ADD_LIBRARY(dcmseg
    segdoc.cc
    segment.cc
    segtypes.cc
    segutils.cc
)

DCMTK_TARGET_LINK_MODULES(dcmseg dcmfg dcmiod dcmdata ofstd oflog)
