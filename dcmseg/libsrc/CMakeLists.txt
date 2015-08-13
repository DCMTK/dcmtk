# create library from source files
DCMTK_ADD_LIBRARY(dcmseg segdoc segment segtypes segutils)

DCMTK_TARGET_LINK_MODULES(dcmseg dcmfg dcmsr dcmimage dcmimgle dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmseg ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES})
