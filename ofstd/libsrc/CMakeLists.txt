# create library from source files
DCMTK_ADD_LIBRARY(ofstd ofchrenc ofcmdln ofconapp ofcond ofconfig ofconsol ofcrc32 ofdate ofdatime oferror offile offname oflist ofstd ofstring ofthread oftime oftimer oftempf ofxml ofuuid ofmath)

DCMTK_TARGET_LINK_LIBRARIES(ofstd ${CHARSET_CONVERSION_LIBS} ${SOCKET_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
