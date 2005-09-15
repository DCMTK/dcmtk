# create library from source files
ADD_LIBRARY(dcmwlm wlds wldsfs wlfsim wlmactmg)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmwlm)
