# create library from source files
DCMTK_ADD_LIBRARY(ofstd ofchrenc ofcmdln ofconapp ofcond ofconfig ofconsol ofcrc32 ofdate ofdatime oferror offile offilsys offname oflist ofstd ofstring ofstrutl ofthread oftime oftimer oftempf ofxml ofuuid ofmath ofsockad ofrand)

DCMTK_TARGET_LINK_LIBRARIES(ofstd ${CHARSET_CONVERSION_LIBS} ${SOCKET_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})
