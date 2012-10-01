# create library from source files
DCMTK_ADD_LIBRARY(dcmwlm wlds wldsfs wlfsim wlmactmg)

TARGET_LINK_LIBRARIES(dcmwlm ofstd dcmdata dcmnet)

# declare installation files
INSTALL(TARGETS dcmwlm
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
