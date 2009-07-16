# create library from source files
ADD_LIBRARY(libi2d i2d i2dplvlp i2djpgs i2dbmps i2dplsc i2dplnsc)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} libi2d)
