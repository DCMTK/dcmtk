# declare additional include directories
include_directories("${dcmgrok_SOURCE_DIR}/include" "${ofstd_SOURCE_DIR}/include" "${oflog_SOURCE_DIR}/include" "${dcmdata_SOURCE_DIR}/include" ${GROK_INCDIR})

# create library from source files
DCMTK_ADD_LIBRARY(dcmgrok
  gkutils.cc
  gkcparam.cc
  gkdecode.cc
  gkcodecd.cc
)

DCMTK_TARGET_LINK_MODULES(dcmgrok ofstd oflog dcmdata)
DCMTK_TARGET_LINK_LIBRARIES(dcmgrok ${GROK_LIBS})
