# create library from source files
ADD_LIBRARY(ofstd ofcmdln ofconapp ofcond ofconfig ofconsol ofcrc32 ofdate ofdatime offname oflist ofstd ofstring ofthread oftime)

# declare installation files
INSTALL(TARGETS ofstd DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT lib)
