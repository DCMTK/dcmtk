# create library from source files
DCMTK_ADD_LIBRARY(ofstd ofchrenc ofcmdln ofconapp ofcond ofconfig ofconsol ofcrc32 ofdate ofdatime offile offname oflist ofstd ofstring ofthread oftime oftimer oftempf ofxml ofuuid)

DCMTK_TARGET_LINK_LIBRARIES(ofstd ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
