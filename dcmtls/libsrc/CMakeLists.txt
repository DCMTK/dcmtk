# create library from source files
ADD_LIBRARY(dcmtls tlslayer tlstrans tlsscu)

TARGET_LINK_LIBRARIES(dcmtls ofstd dcmdata)

# declare installation files
INSTALL(TARGETS dcmtls
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
