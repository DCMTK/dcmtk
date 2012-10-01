# create library from source files
DCMTK_ADD_LIBRARY(ofstd ofchrenc ofcmdln ofconapp ofcond ofconfig ofconsol ofcrc32 ofdate ofdatime offile offname oflist ofstd ofstring ofthread oftime oftimer oftempf ofxml ofuuid)

TARGET_LINK_LIBRARIES(ofstd ${LIBICONV_LIBS} ${THREAD_LIBS} ${WIN32_STD_LIBRARIES})

# declare installation files
INSTALL(TARGETS ofstd
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
