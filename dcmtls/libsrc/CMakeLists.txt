# create library from source files
DCMTK_ADD_LIBRARY(dcmtls tlslayer tlstrans tlsscu)

TARGET_LINK_LIBRARIES(dcmtls ofstd dcmdata dcmnet ${OPENSSL_LIBS})
