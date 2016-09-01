# create library from source files
DCMTK_ADD_LIBRARY(dcmpmap dpmtypes dpmmodparametricmapseries dpmmodparametricmapimage dpmparametricmapbase dpmparametricmapiod)

DCMTK_TARGET_LINK_MODULES(dcmpmap dcmfg dcmiod dcmdata ofstd oflog)
DCMTK_TARGET_LINK_LIBRARIES(dcmpmap ${ZLIB_LIBS} ${WIN32_STD_LIBRARIES})
