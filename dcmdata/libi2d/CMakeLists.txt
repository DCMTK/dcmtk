# create library from source files
DCMTK_ADD_LIBRARY(i2d
    i2dbmps.cc
    i2d.cc
    i2djpgs.cc
    i2dplnsc.cc
    i2dplsc.cc
    i2dplvlp.cc
)

DCMTK_TARGET_LINK_MODULES(i2d dcmdata)
