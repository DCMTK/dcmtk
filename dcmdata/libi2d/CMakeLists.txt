# create library from source files
DCMTK_ADD_LIBRARY(i2d i2d i2dplvlp i2djpgs i2dbmps i2dplsc i2dplnsc)

TARGET_LINK_LIBRARIES(i2d dcmdata)
