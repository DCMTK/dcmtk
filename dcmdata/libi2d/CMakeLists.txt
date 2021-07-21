# declare additional include directories
include_directories(${LIBXML_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(i2d i2d i2dplvlp i2djpgs i2dbmps i2dplsc i2dplnsc i2dplop i2doutpl)

DCMTK_TARGET_LINK_MODULES(i2d dcmdata)
