# create library from source files
DCMTK_ADD_LIBRARY(dcmtls tlscond tlslayer tlsopt tlstrans tlsscu tlsciphr)

DCMTK_TARGET_LINK_MODULES(dcmtls ofstd dcmdata dcmnet)
DCMTK_TARGET_LINK_LIBRARIES(dcmtls ${OPENSSL_LIBS})
set_property(TARGET dcmtls PROPERTY OUTPUT_NAME "dcmtls${DCMTK_TLS_LIBRARY_POSTFIX}")
