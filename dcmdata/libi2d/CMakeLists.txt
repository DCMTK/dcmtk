include_directories(${LIBXML_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(i2d
  i2d.cc
  i2dplvlp.cc
  i2djpgs.cc
  i2dbmps.cc
  i2dplsc.cc
  i2dplnsc.cc
  i2dplop.cc
  i2doutpl.cc
)

DCMTK_TARGET_LINK_MODULES(i2d dcmxml dcmdata)
