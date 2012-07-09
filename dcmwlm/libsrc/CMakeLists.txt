# create library from source files
ADD_LIBRARY(dcmwlm wlds wldsfs wlfsim wlmactmg)

TARGET_LINK_LIBRARIES(dcmwlm ofstd dcmdata dcmnet)
DCMTK_SETUP_LIBRARY(dcmwlm)

# declare installation files
INSTALL(TARGETS dcmwlm
        COMPONENT lib
        RUNTIME DESTINATION ${DCMTK_INSTALL_BINDIR}
        LIBRARY DESTINATION ${DCMTK_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${DCMTK_INSTALL_LIBDIR})
