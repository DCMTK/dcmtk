# create library from source files
DCMTK_ADD_LIBRARY(dcmtls tlslayer tlstrans tlsscu)

DCMTK_TARGET_LINK_MODULES(dcmtls ofstd dcmdata dcmnet)
DCMTK_TARGET_LINK_LIBRARIES(dcmtls ${OPENSSL_LIBS})
