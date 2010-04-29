# create library from source files
ADD_LIBRARY(dcmtls tlslayer tlstrans tlsscu)

# declare installation files
INSTALL_TARGETS(${INSTALL_LIBDIR} dcmtls)
