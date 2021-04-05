# create library from source files
DCMTK_ADD_LIBRARY(dcmtls
    tlsciphr.cc
    tlscond.cc
    tlslayer.cc
    tlsopt.cc
    tlsscu.cc
    tlstrans.cc
)


DCMTK_TARGET_LINK_MODULES(dcmtls ofstd dcmdata dcmnet)
DCMTK_TARGET_LINK_LIBRARIES(dcmtls ${OPENSSL_LIBS})
set_property(TARGET dcmtls PROPERTY OUTPUT_NAME "dcmtls${DCMTK_TLS_LIBRARY_POSTFIX}")
