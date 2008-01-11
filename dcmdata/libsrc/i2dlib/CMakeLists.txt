# create library from source files
ADD_LIBRARY(i2dlib i2d i2dplvlp i2djpgs i2dplsc i2dplnsc)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} i2dlib)
