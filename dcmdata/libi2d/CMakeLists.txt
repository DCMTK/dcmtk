# create library from source files
ADD_LIBRARY(i2d i2d i2dplvlp i2djpgs i2dbmps i2dplsc i2dplnsc)

# declare installation files
INSTALL(TARGETS i2d DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT lib)
