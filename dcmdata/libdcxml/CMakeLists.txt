# create library from source files

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${LIBXML_INCDIR})

DCMTK_ADD_LIBRARY(dcmxml
  xml2dcm.cc
)

DCMTK_TARGET_LINK_MODULES(dcmxml dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmxml ${ZLIB_LIBS} ${LIBXML_LIBS})
