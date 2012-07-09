# create library from source files
ADD_LIBRARY(dcmqrdb dcmqrcbf dcmqrcbg dcmqrcbm dcmqrcbs dcmqrcnf dcmqrdbi dcmqrdbs dcmqropt dcmqrptb dcmqrsrv dcmqrtis)

TARGET_LINK_LIBRARIES(dcmqrdb ofstd dcmdata dcmnet)
DCMTK_SETUP_LIBRARY(dcmqrdb)

# declare installation files
INSTALL(TARGETS dcmqrdb
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
