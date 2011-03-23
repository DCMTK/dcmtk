# create library from source files
ADD_LIBRARY(i2d i2d i2dplvlp i2djpgs i2dbmps i2dplsc i2dplnsc)

# declare installation files
INSTALL_TARGETS(${DCMTK_INSTALL_LIBDIR} i2d)
