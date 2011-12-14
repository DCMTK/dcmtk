# create library from source files
ADD_LIBRARY(dcmwlm ${STATIC_ON_WINDOWS} wlds wldsfs wlfsim wlmactmg)

# declare installation files
INSTALL(TARGETS dcmwlm DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT lib)
