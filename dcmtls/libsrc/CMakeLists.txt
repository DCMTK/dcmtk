# create library from source files
ADD_LIBRARY(dcmtls ${STATIC_ON_WINDOWS} tlslayer tlstrans tlsscu)

# declare installation files
INSTALL(TARGETS dcmtls DESTINATION ${DCMTK_INSTALL_LIBDIR} COMPONENT lib)
